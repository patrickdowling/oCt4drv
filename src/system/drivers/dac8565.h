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

#ifndef DRIVERS_DAC8565_H_
#define DRIVERS_DAC8565_H_

#include <array>
#include <cstddef>
#include <cstdint>

#include "util/util_macros.h"

namespace oct4 {
namespace drivers {

class SPI;

class DAC8565 {
public:
  DELETE_COPY_ASSIGN(DAC8565);

  static constexpr size_t kNumChannels = 4;
  static constexpr uint32_t kMaxValue = 65535U;
  static constexpr uint16_t kInitialValue = 0x8000;
  using ValueType = std::array<uint16_t, kNumChannels>;

  enum Command : uint32_t {
    WRITE_BUFFER = 0x00 << 16,
    WRITE_LOAD = 0x10 << 16,
    WRITE_LOAD_ALL = 0x20 << 16,
  };

  explicit DAC8565(SPI &spi) : spi_{spi} {}

  void Init();

  void Update(const ValueType &values);

private:
  SPI &spi_;
};

}  // namespace drivers
}  // namespace oct4

#endif  // DRIVERS_DAC8565_H_
