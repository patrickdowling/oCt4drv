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

#ifndef HWTEST_DEBUG_H_
#define HWTEST_DEBUG_H_

#include "builtin/hwtest_plugin.h"
#include "system/oct4_system_core.h"

namespace oct4 {

class PluginDebug : public HWTestPlugin {
public:
  PluginDebug() : HWTestPlugin{1} {}

  void Draw(weegfx::Graphics &gfx) const final
  {
    DrawTitlebar(gfx, "DEBUG");

    gfxtools::PrintLines lines{gfx};
#ifdef OCT4_ENABLE_DEBUG
    gfx.printf("ADC wait    %8lu", debug.drivers.adc_wait_count);
    lines.nl();
    gfx.printf("UI events   %8lu", debug.ui.event_count);
    lines.nl();
    gfx.printf("UI overflow %8lu", debug.ui.event_queue_overflow);
#else
    gfx.printf("[_] OCT4_ENABLE_DEBUG");
#endif
  }
};
}  // namespace oct4

#endif  // HWTEST_DEBUG_H_
