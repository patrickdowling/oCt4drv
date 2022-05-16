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

#ifndef UI_EVENT_H_
#define UI_EVENT_H_

#include <cstdint>

namespace oct4 {
namespace UI {

// THese are compatible with O_C codebase where each control ID is really a mask
enum ControlID : uint16_t {
  CONTROL_NONE = 0,
  CONTROL_BUTTON_UP = 0x1,
  CONTROL_BUTTON_DOWN = 0x2,
  CONTROL_BUTTON_L = 0x4,
  CONTROL_BUTTON_R = 0x8,
  CONTROL_BUTTON_MASK = 0xf,
  CONTROL_ENCODER_L = 0x10,
  CONTROL_ENCODER_R = 0x20,
  CONTROL_LAST,
};

enum ControlEvent : uint16_t {
  EVENT_NONE,
  EVENT_BUTTON_PRESS,
  EVENT_BUTTON_LONG_PRESS,
  EVENT_ENCODER,
  EVENT_TIMER,
};

// Generic and simultaneously specific wrapper for UI events.
// Perhaps it would have been better to just to drop it in the UI class.

struct Event {
  ControlEvent type = EVENT_NONE;
  ControlID control = CONTROL_NONE;
  int32_t value = 0;
  uint32_t mask = 0;
  uint32_t ticks = 0;

  Event() = default;
  Event(ControlEvent t, ControlID c, int32_t v, uint32_t m, uint32_t tk)
      : type(t), control(c), value(v), mask(m), ticks(tk)
  {}
};

}  // namespace UI
}  // namespace oct4

#define DEBUG_EVENT(ctx, e)                                                                  \
  SERIAL_DEBUG(#ctx " {type=%#02x, control=%#02x, value=%ld, ticks=%lu}", e.type, e.control, \
               e.value, e.ticks)

#endif  // UTIL_EVENT_H_
