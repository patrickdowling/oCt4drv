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

#include "system/drivers/oct4_analog_inputs.h"

#include <algorithm>

#include "system/drivers/oct4_gpio.h"
#include "system/oct4_debug.h"
#include "system/system_serial.h"

namespace oct4 {
namespace drivers {

// This mimicks the method from T3.2 hardware that has a DMA channel that writes into the ADC
// register, so it's basically a DMA-triggered software conversion. It's a bit awkward to use if we
// want to use both ADCs because we now need 4 DMA channels but oh well.
//
// TODO We should be able to fixup something with the XBAR to the ADC triggers, although this may
// eat some timers.
//
// For now it's mostly based around the existing ADC library.
//
// It's not free-running, instead we should StartConversion and Read, GOTO 10.
// If necessary we can use some faster settings, or both ADCs, and get more samples but then we'll
// actually need a better way to measure (or calculate) the times to fit within the main loop.

static constexpr ADC_REFERENCE kAdcReference = ADC_REFERENCE::REF_3V3;

// TODO verify
static constexpr struct {
  uint8_t averaging;
  uint8_t resolution;
  ADC_SAMPLING_SPEED sampling_speed;
  ADC_CONVERSION_SPEED conversion_speed;
} kAdcParams[AnalogInputs::ADC_MODE_LAST] = {
    // These values seem to result in ca. 53us sample time (for 4 channels)
    {8, 12, ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_CONVERSION_SPEED::HIGH_SPEED},
    // Best guess
    {4, 12, ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_CONVERSION_SPEED::VERY_HIGH_SPEED},
};

static ADC adc;

static constexpr size_t kNumSamples = AnalogInputs::kNumChannels;

static DMAChannel dma_channel_adc = {false};
static DMAChannel dma_channel_mux = {false};

static DMAMEM_ALIGN32 uint16_t adc0_buffer_rx[kNumSamples];
static DMAMEM_ALIGN32 uint16_t adc0_buffer_mux[AnalogInputs::kNumChannels];

template <typename T>
void Configure(T *adcn, AnalogInputs::AdcMode adc_mode, bool first_time)
{
  const auto params = kAdcParams[adc_mode];
  SERIAL_DEBUG("resolution=%u averaging=%u conversion=%02x sampling=%02x", params.resolution,
               params.averaging, (unsigned)params.conversion_speed,
               (unsigned)params.sampling_speed);

  if (first_time) adcn->setReference(kAdcReference);

  adcn->setResolution(params.resolution);
  adcn->setAveraging(params.averaging);
  adcn->setConversionSpeed(params.conversion_speed);
  adcn->setSamplingSpeed(params.sampling_speed);
  adcn->recalibrate();
}

// From cores/teensy4/analog.c
// NOTE order is off-by-one
static constexpr uint16_t kPinChannelMap[] = {
    6,   // 18/A4  AD_B1_01
    15,  // 20/A6  AD_B1_10
    11,  // 17/A3  AD_B1_06
    5,   // 19/A5  AD_B1_00
};

FLASHMEM void AnalogInputs::Init(AdcMode adc_mode)
{
  GPIO::CV1::Init();
  GPIO::CV2::Init();
  GPIO::CV3::Init();
  GPIO::CV4::Init();

  Configure(adc.adc0, adc_mode, true);
  adc.adc0->enableDMA();

  DEBUG_BUFFER("adc0_buffer_rx", adc0_buffer_rx, sizeof(adc0_buffer_rx));
  DEBUG_BUFFER("adc0_buffer_mux", adc0_buffer_mux, sizeof(adc0_buffer_mux));

  dma_channel_adc.begin(true);
  SERIAL_DEBUG("dma_channel_adc.channel=%x", dma_channel_adc.channel);
  dma_channel_adc.disable();
  dma_channel_adc.source(*(uint16_t *)&ADC1_R0);
  dma_channel_adc.destinationBuffer(adc0_buffer_rx, sizeof(adc0_buffer_rx));
  dma_channel_adc.triggerAtHardwareEvent(DMAMUX_SOURCE_ADC1);
  dma_channel_adc.disableOnCompletion();

  dma_channel_mux.begin(true);
  SERIAL_DEBUG("dma_channel_mux.channel=%x", dma_channel_mux.channel);
  dma_channel_mux.disable();
  dma_channel_mux.sourceCircular(adc0_buffer_mux, sizeof(adc0_buffer_mux));
  dma_channel_mux.destination(*(uint16_t *)&ADC1_HC0);
  dma_channel_mux.triggerAtTransfersOf(dma_channel_adc);
  dma_channel_mux.triggerAtCompletionOf(dma_channel_adc);

  std::fill(values_.begin(), values_.end(), 0);
  std::fill(adc0_buffer_rx, adc0_buffer_rx + kNumSamples, 0);
  std::copy(kPinChannelMap, kPinChannelMap + 4, adc0_buffer_mux);

  arm_dcache_flush_delete(adc0_buffer_mux, sizeof(adc0_buffer_mux));
  dma_channel_mux.enable();
  dma_channel_adc.enable();
}

void AnalogInputs::ChangeMode(AdcMode adc_mode)
{
  Configure(adc.adc0, adc_mode, false);
}

FASTRUN void AnalogInputs::StartConversion()
{
  if (dma_channel_adc.complete()) {
    dma_channel_adc.clearComplete();
    dma_channel_adc.TCD->DADDR = adc0_buffer_rx;
    dma_channel_adc.enable();
    // __DMB();
  } else {
    OCT4_DEBUG_INC(debug.drivers.adc_wait_count);
  }
}
void AnalogInputs::Read(ValueType &values) const
{
  std::copy(adc0_buffer_rx, adc0_buffer_rx + kNumSamples, values.begin());
  arm_dcache_delete(adc0_buffer_rx, sizeof(adc0_buffer_rx));  // TODO where does this go?
}

}  // namespace drivers
}  // namespace oct4
