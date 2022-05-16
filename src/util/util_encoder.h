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

#ifndef UTIL_ENCODER_H_
#define UTIL_ENCODER_H_

#include <cstdint>

#include "util/util_macros.h"

namespace util {

class Encoder {
public:
  static const int32_t kAccelerationDec = 16;
  static const int32_t kAccelerationInc = 208;
  static const int32_t kAccelerationMax = 16 << 8;

  // For debouncing of pins, use 0x0f (b00001111) and 0x0c (b00001100) etc.
  static const uint8_t kPinMask = 0x03;
  static const uint8_t kPinEdge = 0x02;

  Encoder() = default;
  DELETE_COPY_ASSIGN(Encoder);

  template <typename PINA, typename PINB>
  void Poll()
  {
    pin_state_[0] = (pin_state_[0] << 1) | PINA::Read();
    pin_state_[1] = (pin_state_[1] << 1) | PINB::Read();
  }

  void enable_acceleration(bool b)
  {
    if (b != acceleration_enabled_) {
      acceleration_enabled_ = b;
      acceleration_ = 0;
    }
  }

  void reverse(bool reversed) { reversed_ = reversed; }

  inline int32_t Read()
  {
    int32_t acceleration = acceleration_;
    if (acceleration_enabled_ && acceleration) {
      acceleration -= kAccelerationDec;
      if (acceleration < 0) acceleration = 0;
    }

    // Find direction by detecting state change and evaluating the other pin.
    // 0x02 == b10 == rising edge on pin
    // Should also work just checking for falling edge on PINA and checking
    // PINB state.
    int32_t i = 0;
    const uint8_t a = pin_state_[0] & kPinMask;
    const uint8_t b = pin_state_[1] & kPinMask;
    if (a == kPinEdge && b == 0x00) {
      i = 1;
    } else if (b == kPinEdge && a == 0x00) {
      i = -1;
    }

    if (i) {
      if (reversed_) i = -i;
      if (acceleration_enabled_) {
        if (i != last_dir_) {
          // We've stored the pre-acceleration value so don't need to actually check the signs.
          // 1001 ways to check if sign bit is different are left as an exercise for the reader ;)
          acceleration = 0;
        } else {
          acceleration += kAccelerationInc;
          if (acceleration > kAccelerationMax) acceleration = kAccelerationMax;
        }
      } else {
        acceleration = 0;
      }

      last_dir_ = i;
      i += i * (acceleration >> 8);
    }

    acceleration_ = acceleration;
    return i;
  }

private:
  bool acceleration_enabled_ = true;
  bool reversed_ = false;
  int32_t last_dir_ = 0;
  int32_t acceleration_ = 0;
  uint8_t pin_state_[2] = {0, 0};
};

};  // namespace util

#endif  // UTIL_ENCODER_H_
