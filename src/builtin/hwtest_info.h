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

#ifndef HWTEST_INFO_H_
#define HWTEST_INFO_H_

#include "hwtest_plugin.h"
#include "resources/oct4_strings.h"

namespace oct4 {

class PluginInfo : public HWTestPlugin {
public:
  PluginInfo() : HWTestPlugin{2} {}

  void Draw(weegfx::Graphics &gfx) const final
  {
    DrawTitlebar(gfx, "INFO");

    gfxtools::PrintLines lines{gfx};
    switch (current_page_) {
      case 0:
        gfx.printf("F_CPU=%luMHz", F_CPU / 1000LU / 1000LU);
        lines.nl();
        gfx.printf("VER %s", strings::BUILD_VERSION);
        lines.nl();
        gfx.printf("TAG %s", strings::BUILD_TAG);
        lines.nl();
        break;
      case 1:
#ifdef USB_SERIAL
        gfx.printf("USB_SERIAL");
        lines.nl();
#endif
#ifdef ENABLE_SERIAL_DEBUG
        gfx.printf("ENABLE_SERIAL_DEBUG");
        lines.nl();
#endif
#ifdef OCT4_ENABLE_DEBUG
        gfx.printf("OCT4_ENABLE_DEBUG");
        lines.nl();
#endif
        break;
      default: break;
    }
  }
};

}  // namespace oct4

#endif  // HWTEST_INFO_H_
