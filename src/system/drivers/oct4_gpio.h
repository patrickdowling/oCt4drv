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

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "oct4_gpio_pins.h"
#include "system/t4.h"

namespace oct4 {
namespace drivers {

template <uint8_t pin>
struct Pin {
  static constexpr uint8_t pin_number() { return pin; }
};
template <uint8_t pin, uint8_t initial_value>
struct OutputPin : public Pin<pin> {
  static void Init()
  {
    pinMode(pin, OUTPUT);
    digitalWriteFast(pin, initial_value);
  }

  static inline void Set() { digitalWriteFast(pin, HIGH); }
  static inline void Clear() { digitalWriteFast(pin, LOW); }
};

template <uint8_t pin>
struct DigitalInputPin : public Pin<pin> {
  static void Init() { pinMode(pin, INPUT_PULLUP); }

  static bool Read() { return !digitalReadFast(pin); }
};

template <uint8_t pin>
struct AnalogInputPin : public Pin<pin> {
  // Note to self: according to source in core, INPUT doesn't set PKE (keeper enable)
  // Which is what we want.
  static void Init() { pinMode(pin, INPUT); }
};

class GPIO {
public:
  using DAC_CS = OutputPin<PIN_DAC_CS, HIGH>;
  using DAC_RST = OutputPin<PIN_DAC_RST, LOW>;

  using OLED_CS = OutputPin<PIN_OLED_CS, HIGH>;
  using OLED_RST = OutputPin<PIN_OLED_RST, HIGH>;
  using OLED_DC = OutputPin<PIN_OLED_DC, HIGH>;

  using CV1 = AnalogInputPin<PIN_CV1>;
  using CV2 = AnalogInputPin<PIN_CV2>;
  using CV3 = AnalogInputPin<PIN_CV3>;
  using CV4 = AnalogInputPin<PIN_CV4>;

  using TR1 = DigitalInputPin<PIN_TR1>;
  using TR2 = DigitalInputPin<PIN_TR2>;
  using TR3 = DigitalInputPin<PIN_TR3>;
  using TR4 = DigitalInputPin<PIN_TR4>;

  using SW_UP = DigitalInputPin<PIN_SW_UP>;
  using SW_DOWN = DigitalInputPin<PIN_SW_DOWN>;
  using SW_ENCR = DigitalInputPin<PIN_SW_ENCR>;
  using SW_ENCL = DigitalInputPin<PIN_SW_ENCL>;

  using ENCR_A = DigitalInputPin<PIN_ENCR_A>;
  using ENCR_B = DigitalInputPin<PIN_ENCR_B>;

  using ENCL_A = DigitalInputPin<PIN_ENCL_A>;
  using ENCL_B = DigitalInputPin<PIN_ENCL_B>;

#if 0  // C++17
  template <typename... Pins>
  static void InitPins()
  {
    (Pins::Init(), ...);
  }
#endif
};

template <typename Pin>
struct ActiveLow {
  ActiveLow() { Pin::Clear(); }
  ~ActiveLow() { Pin::Set(); }
};

template <typename Pin>
struct ActiveHigh {
  ActiveHigh() { Pin::Set(); }
  ~ActiveHigh() { Pin::Clear(); }
};

}  // namespace drivers
}  // namespace oct4

#endif  // DRIVERS_GPIO_H_
