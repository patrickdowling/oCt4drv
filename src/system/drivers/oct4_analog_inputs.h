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

#ifndef OCT4_ANALOG_INPUTS_H_
#define OCT4_ANALOG_INPUTS_H_

#include <array>
#include <cstddef>

#include "util/util_macros.h"

namespace oct4 {
namespace drivers {

class AnalogInputs {
public:
  AnalogInputs() = default;
  DELETE_COPY_ASSIGN(AnalogInputs);

  static constexpr size_t kNumChannels = 4;
  using ValueType = std::array<uint16_t, kNumChannels>;

  enum AdcMode { ADC_MODE_NORMAL, ADC_MODE_FAST, ADC_MODE_LAST };

  void Init(AdcMode adc_mode);

  void ChangeMode(AdcMode adc_mode);

  void StartConversion();

  void Read(ValueType &values) const;

private:
  ValueType values_;
};

}  // namespace drivers
}  // namespace oct4

#endif  // OCT4_ANALOG_INPUTS_H_
