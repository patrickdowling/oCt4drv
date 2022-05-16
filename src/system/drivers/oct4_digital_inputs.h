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

#ifndef OCT4_DIGITAL_INPUTS_H_
#define OCT4_DIGITAL_INPUTS_H_

#include <array>
#include <cstddef>

#include "util/util_macros.h"

namespace oct4 {
namespace drivers {

struct InputState {
  uint16_t raw = 0;
  uint16_t flags = 0;

  enum Flags : uint16_t { INPUT_STATE_TRIGGERED = 0x1, INPUT_STATE_RAISED = 0x2 };

  bool triggered() const { return flags & INPUT_STATE_TRIGGERED; }
  bool raised() const { return flags & INPUT_STATE_RAISED; }
};

class DigitalInputs {
public:
  static constexpr size_t kNumChannels = 4;
  using ValueType = std::array<InputState, kNumChannels>;

  DigitalInputs() = default;
  DELETE_COPY_ASSIGN(DigitalInputs);

  void Init();

  void Read(ValueType &input_states);

private:
  std::array<uint16_t, kNumChannels> last_cntr_;
};

}  // namespace drivers
}  // namespace oct4

#endif  // OCT4_DIGITAL_INPUTS_H_
