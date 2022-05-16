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

#ifndef T4_H_
#define T4_H_

// Gah
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <ADC.h>
#include <Arduino.h>
#include <DMAChannel.h>
#include <IntervalTimer.h>
#include <SPI.h>
#pragma GCC diagnostic pop

#include <arm_math.h>

// So cache/DMA mem
// We like to flush things from the cache before they get sent via DMA
// Difficulty: They have to be 32byte aligned, but the DMAMEM macro only puts them in the .dma
// section
#define DMAMEM_ALIGN32 DMAMEM __attribute__((aligned(32)))

namespace t4 {

class XBAR1 {
public:
  static void Enable() { CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON); }

  template <unsigned input, unsigned output>
  static void Connect()
  {
    static_assert(input < 88, "Invalid input");
    static_assert(output < 132, "Input output");

    volatile uint16_t *xbar_sel = &XBARA1_SEL0 + (output >> 1);
    uint16_t val = *xbar_sel;
    if (!(output & 1)) {
      val = (val & 0xFF00) | input;
    } else {
      val = (val & 0x00FF) | (input << 8);
    }

    *xbar_sel = val;
  }
};

};  // namespace t4

#endif  // T4_H_
