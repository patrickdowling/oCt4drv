// oCt4drv -- Basic drivers for O_C + T4.0
// Copyright (C) 2022 Patrick Dowling (pld@gurkenkiste.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "system/drivers/oct4_digital_inputs.h"

#include "system/drivers/oct4_gpio.h"
#include "system/system_serial.h"

// NOTES
// So the original o_C digital inputs used an interrupt on the falling edge of the pins (because the
// inputs are inverted). This generally works ok-ish but
// a) the read & reset mechanism wasn't atomic (solveable)
// b) interrupts have some overhead so hammering the four inputs with triggers was measureable.
// That's not necessarily important in the grand scheme of things but still.
//
// So what we're (trying?) to do here is connecting the GPIOs to timers and checking if the counter
// has changed since last time. None (?) of the used pins seesm to align the AF but using XBAR we
// should be able to connect things. This should also pave the way to the frequency counter
// eventually.
//
// One thing to note though is that once we've enabled the XBAR AF, the GPIO can't be read using the
// "normal" means.
//
// 12.5.4.4 Fields
// NOTE: The IOMUXC must be configured to GPIO mode for GPIO_PSR to reflect the state of the
// corresponding signal.
//
// But we can get the (filtered and inverted) status from each channels SCTRL register. Other
// alternatives might be
// - Count both edges and determine state from even/odd (will that even work without a known state?)
// - Just poll the status and assume > 60us-ish pulses
// - ???

namespace oct4 {
namespace drivers {

// TR1 = 0 = IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_03 -> XBAR1_INOUT17
// TR2 = 1 = IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_02 -> XBAR1_INOUT16
// TR3 = 2 = IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04 -> XBAR1_INOUT06
// TR4 = 3 = IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05 -> XBAR1_INOUT07

static constexpr uint8_t kFilterSampleCount = 0;  // actual value 3 + kFilterSampleCount
static constexpr uint8_t kFilterSamplePeriod = 0;

static void init_qtimer_ch(unsigned channel)
{
  auto &ch = IMXRT_TMR4.CH[channel];
  ch.CTRL = 0;
  ch.CNTR = 0;
  ch.LOAD = 0;

  // ch.FILT = 0;
  ch.FILT = TMR_FILT_FILT_CNT(kFilterSampleCount) | TMR_FILT_FILT_PER(kFilterSamplePeriod);

  ch.SCTRL = TMR_SCTRL_IPS;  // invert signal polarity
  // CM(1)    : 001 Count rising edges of primary source
  // PCS(n)   : Counter n input pin
  // LENGTH(0): Count until roll over at $FFFF and continue from $0000
  ch.CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(channel) | TMR_CTRL_SCS(channel);
}

void DigitalInputs::Init()
{
  GPIO::TR1::Init();
  GPIO::TR2::Init();
  GPIO::TR3::Init();
  GPIO::TR4::Init();

  std::fill(last_cntr_.begin(), last_cntr_.end(), 0xffff);

  SERIAL_DEBUG(" TMR_FILT_FILT_CNT=%u TMR_FILT_FILT_PER=%u", kFilterSampleCount,
               kFilterSamplePeriod);

  // 4 channels of QTIMER
  CCM_CCGR6 |= CCM_CCGR6_QTIMER4(CCM_CCGR_ON);
  init_qtimer_ch(0);
  init_qtimer_ch(1);
  init_qtimer_ch(2);
  init_qtimer_ch(3);

  // MUX and XBAR setup to connect GPIOs with timers, since they aren't the native AF
  t4::XBAR1::Enable();

  // TODO parametrize all this? Else FLIP180 becomes a PITA (unless the resort happens elsewhere)
  /*CORE_PIN0_CONFIG*/ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_03 = 0x1;
  /*CORE_PIN1_CONFIG*/ IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_02 = 0x1;
  /*CORE_PIN2_CONFIG*/ IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04 = 0x3;
  /*CORE_PIN3_CONFIG*/ IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05 = 0x3;

  IOMUXC_XBAR1_IN17_SELECT_INPUT = 1;  // GPIO_AD_B0_03_ALT1
  IOMUXC_XBAR1_IN16_SELECT_INPUT = 0;  // GPIO_AD_B0_02_ALT1
  IOMUXC_XBAR1_IN06_SELECT_INPUT = 0;  // GPIO_EMC_04_ALT3
  IOMUXC_XBAR1_IN07_SELECT_INPUT = 0;  // GPIO_EMC_05_ALT3

  IOMUXC_GPR_GPR6 |= IOMUXC_GPR_GPR6_QTIMER4_TRM0_INPUT_SEL;
  IOMUXC_GPR_GPR6 |= IOMUXC_GPR_GPR6_QTIMER4_TRM1_INPUT_SEL;
  IOMUXC_GPR_GPR6 |= IOMUXC_GPR_GPR6_QTIMER4_TRM2_INPUT_SEL;
  IOMUXC_GPR_GPR6 |= IOMUXC_GPR_GPR6_QTIMER4_TRM3_INPUT_SEL;

  t4::XBAR1::Connect<XBARA1_IN_IOMUX_XBAR_INOUT17, XBARA1_OUT_QTIMER4_TIMER0>();
  t4::XBAR1::Connect<XBARA1_IN_IOMUX_XBAR_INOUT16, XBARA1_OUT_QTIMER4_TIMER1>();
  t4::XBAR1::Connect<XBARA1_IN_IOMUX_XBAR_INOUT06, XBARA1_OUT_QTIMER4_TIMER2>();
  t4::XBAR1::Connect<XBARA1_IN_IOMUX_XBAR_INOUT07, XBARA1_OUT_QTIMER4_TIMER3>();
}

template <size_t ch, typename gpio>
static InputState read_input_state(std::array<uint16_t, DigitalInputs::kNumChannels> &last_cntr)
{
  uint16_t cntr = IMXRT_TMR4.CH[ch].CNTR;
  uint16_t flags = (cntr != last_cntr[ch]) ? InputState::INPUT_STATE_TRIGGERED : 0;
  if (IMXRT_TMR4.CH[ch].SCTRL & TMR_SCTRL_INPUT) flags |= InputState::INPUT_STATE_RAISED;
  // Pin can't be read because we're using a different AF than GPIOx_xxxx
  // if (gpio::Read()) flags |= InputState::INPUT_STATE_RAISED;

  last_cntr[ch] = cntr;
  return {cntr, flags};
}

void DigitalInputs::Read(ValueType &input_states)
{
  input_states[0] = read_input_state<0, GPIO::TR1>(last_cntr_);
  input_states[1] = read_input_state<1, GPIO::TR2>(last_cntr_);
  input_states[2] = read_input_state<2, GPIO::TR3>(last_cntr_);
  input_states[3] = read_input_state<3, GPIO::TR4>(last_cntr_);
}

}  // namespace drivers
}  // namespace oct4
