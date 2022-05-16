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

#ifndef DRIVERS_SSD1306_H_
#define DRIVERS_SSD1306_H_

#include <stdint.h>

namespace SSD1306 {
enum Command : uint8_t {
  SET_LOWER_COL_START_ADDRESS = 0x00,
  SET_HIGHER_COL_START_ADDRESS = 0x10,
  SET_ADDRESSING_MODE = 0x20,
  SET_COLUMN_ADDRESS = 0x21,
  SET_PAGE_ADDRESS = 0x22,
  SET_FADE_OUT = 0x23,
  SCROLL_DEACTIVATE = 0x2E,
  SET_DISPLAY_START_LINE = 0x40,
  SET_CONTRAST = 0x81,
  CHARGE_PUMP_SETTING = 0x8D,
  SET_SEGMENT_REMAP_0SEG0 = 0xA0,
  SET_SEGMENT_REMAP_127SEG0 = 0xA1,
  DISPLAY_OUTPUT_RAM = 0xA4,
  DISPLAY_OUTPUT_FULL = 0xA5,
  SET_DISPLAY_NORMAL = 0xA6,
  SET_DISPLAY_INVERSE = 0xA7,
  SET_MULTIPLEX_RATIO = 0xA8,
  SET_DISPLAY_OFF = 0xAE,
  SET_DISPLAY_ON = 0xAF,
  SET_PAGE_START_ADDRESS = 0xB0,
  SET_COM_OUTPUT_SCAN_0_N = 0xC0,
  SET_COM_OUTPUT_SCAN_N_0 = 0xC8,
  SET_DISPLAY_OFFSET = 0xD3,
  SET_DISPLAY_CLOCK_DIVIDE_RATIO = 0xD5,
  SET_PRECHARGE_PERIOD = 0xD9,
  SET_COM_PIN_CONFIG = 0xDA,
  SET_VCOMH_DESELECT_LEVEL = 0xDB,
};

enum FadeMode : uint8_t {
  FADE_DISABLE = 0x00,
  FADE_OUT = 0x20,
  FADE_BLINK = 0x30,
};

enum AddressingMode : uint8_t {
  ADDRESSING_MODE_HORIZONTAL = 0x00,
  ADDRESSING_MODE_VERTICAL = 0x01,
  ADDRESSING_MODE_PAGE = 0x02
};

static const uint8_t startup_sequence[] = {
    SET_DISPLAY_OFF,
    SET_DISPLAY_CLOCK_DIVIDE_RATIO,
    0x80,  // [7:4] = set osc frequency
    SET_MULTIPLEX_RATIO,
    0x3f,  // [5:0]
    SET_DISPLAY_OFFSET,
    0x00,
    CHARGE_PUMP_SETTING,
    0x14,                        // 0x10 | [2] = 0b Disable, 1b Enable
    SET_DISPLAY_START_LINE | 0,  // 0
    SET_ADDRESSING_MODE,
    ADDRESSING_MODE_PAGE,
#ifdef FLIP_180
    SET_SEGMENT_REMAP_0SEG0,
    SET_COM_OUTPUT_SCAN_0_N,
#else
    SET_SEGMENT_REMAP_127SEG0,
    SET_COM_OUTPUT_SCAN_N_0,
#endif
    SET_COM_PIN_CONFIG,
    0x12,
    SET_CONTRAST,
    0xCF,
    SET_PRECHARGE_PERIOD,
    0xF1,  // A[3:0] = divide ratio, A[7:4] = FOsc
    SET_VCOMH_DESELECT_LEVEL,
    0x40,  // 0.77 x VCC
    SCROLL_DEACTIVATE,
    DISPLAY_OUTPUT_RAM,
#ifdef INVERT_DISPLAY
    SET_DISPLAY_INVERSE,
#else
    SET_DISPLAY_NORMAL,
#endif
    SET_DISPLAY_ON,
};
}  // namespace SSD1306

#endif  // DRIVERS_SSD1306_H_
