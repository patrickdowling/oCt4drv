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

#ifndef OCT4_GPIO_PINS_H_
#define OCT4_GPIO_PINS_H_

// NOTE don't implement FLIP_180 here because of MUX/XBAR etc.

#define PIN_DAC_RST 9
#define PIN_DAC_CS 10  // NOTE hardware CS

#define PIN_OLED_DC 6
#define PIN_OLED_RST 7
#define PIN_OLED_CS 8

#define PIN_CV1 PIN_A5  // 19 // A5
#define PIN_CV2 PIN_A4  // 18 // A4
#define PIN_CV3 PIN_A6  // 20 // A6
#define PIN_CV4 PIN_A3  // 17 // A3

#define PIN_TR1 0
#define PIN_TR2 1
#define PIN_TR3 2
#define PIN_TR4 3

#define PIN_SW_UP 5
#define PIN_SW_DOWN 4
#define PIN_SW_ENCR 14
#define PIN_SW_ENCL 23

#define PIN_ENCR_A 16
#define PIN_ENCR_B 15

#define PIN_ENCL_A 22
#define PIN_ENCL_B 21

#endif  // OCT4_GPIO_PINS_H_
