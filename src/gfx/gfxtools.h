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

#ifndef GFXTOOLS_H_
#define GFXTOOLS_H_

#include "weegfx/weegfx.h"

namespace gfxtools {

struct PrintLines {
  PrintLines(weegfx::Graphics &gfx) : gfx_{gfx} { gfx.setPrintPos(x, y); }

  weegfx::coord_t x = 0;
  weegfx::coord_t y = 12;

  void nl()
  {
    y += 10;
    gfx_.setPrintPos(x, y);
  }

  weegfx::Graphics &gfx_;
};
}  // namespace gfxtools

#endif  // GFXTOOLS_H_
