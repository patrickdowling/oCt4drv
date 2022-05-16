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

#ifndef UTIL_SWITCHSTATE_H_
#define UTIL_SWITCHSTATE_H_

#include <stdint.h>

namespace util {

class SwitchState {
public:
  template <typename GPIO>
  void Poll()
  {
    state_ = state_ << 1 | GPIO::Read();
  }

  template <typename GPIO>
  void Poll(GPIO &gpio)
  {
    state_ = (state_ << 1) | gpio.Read();
  }

  inline bool pressed() const { return state_ == 0x00; }

  inline bool just_pressed() const { return state_ == 0x7f; }

  inline bool released() const { return state_ == 0x80; }

  template <typename GPIO>
  bool read_immediate() const
  {
    return !GPIO::Read();
  }

private:
  uint8_t state_ = 0;
};

}  // namespace util

#endif  // UTIL_SWITCHSTATE_H_
