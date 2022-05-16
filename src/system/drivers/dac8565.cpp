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

#include "system/drivers/dac8565.h"

#include "system/drivers/oct4_gpio.h"
#include "system/drivers/oct4_spi.h"
#include "system/system_serial.h"

// NOTE DAC_CS is hardware CS and not managed here

namespace oct4 {
namespace drivers {

static uint32_t tx_buffer[DAC8565::kNumChannels] = {0};

template <uint32_t channel>
constexpr uint32_t pack_channel()
{
#ifdef FLIP_180
  return (kNumChannels - 1 - channel) << 17;
#else
  return channel << 17;
#endif
}

static inline uint32_t PackData(uint32_t cmd, const uint16_t value)
{
  return (cmd & 0x00ff0000U) | value;
}

template <DAC8565::Command command, uint8_t channel>
static inline uint32_t PackCommand()
{
  static_assert(channel < DAC8565::kNumChannels, "");
  return command | pack_channel<channel>();
}

/*static*/
void DAC8565::Init()
{
  GPIO::DAC_RST::Init();
  delay(1);
  GPIO::DAC_RST::Set();

  tx_buffer[0] = PackCommand<WRITE_BUFFER, 0>();
  tx_buffer[1] = PackCommand<WRITE_BUFFER, 1>();
  tx_buffer[2] = PackCommand<WRITE_BUFFER, 2>();
  tx_buffer[3] = PackCommand<WRITE_LOAD_ALL, 3>();

  SERIAL_DEBUG("%08" PRIx32, tx_buffer[0]);
  SERIAL_DEBUG("%08" PRIx32, tx_buffer[1]);
  SERIAL_DEBUG("%08" PRIx32, tx_buffer[2]);
  SERIAL_DEBUG("%08" PRIx32, tx_buffer[3]);

  ValueType initial_values = {kInitialValue, kInitialValue, kInitialValue, kInitialValue};
  Update(initial_values);
  spi_.Flush();
}

// Technically we can probably interleave the array update & send but benefits are marginal
void DAC8565::Update(const ValueType &values)
{
  for (size_t i = 0; i < kNumChannels; ++i) tx_buffer[i] = PackData(tx_buffer[i], values[i]);

  spi_.SetMode<SPI::PCS_ENABLE, 24>();
  spi_.Send(tx_buffer, kNumChannels, SPI::NOFLUSH);
}

}  // namespace drivers
}  // namespace oct4
