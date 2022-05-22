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

#ifndef HWTEST_CORE_H_
#define HWTEST_CORE_H_

#include "builtin/hwtest_plugin.h"
#include "resources/oct4_strings.h"

namespace oct4 {
class PluginCore : public HWTestPlugin {
public:
  void Draw(weegfx::Graphics &gfx) const final
  {
    DrawTitlebar(gfx, "CORE");

    gfxtools::PrintLines lines{gfx};
    gfx.printf("Temp            %.1fC", (double)SystemCore::read_temperature());
    lines.nl();

    auto core_isr_cycles = debug.perf.core_isr_cycles.value();
    auto core_isr_period = debug.perf.core_isr_period.value();

    gfx.printf("C %5lu       %6.2fu", core_isr_cycles,
               (double)profiling::cycles_to_us(core_isr_cycles));
    lines.nl();

    gfx.printf("P %5lu       %6.2fu", core_isr_period,
               (double)profiling::cycles_to_us(core_isr_period));
    lines.nl();

    auto load = static_cast<float>(core_isr_cycles) / static_cast<float>(core_isr_period);
    gfx.printf("              %6.2f%%", (double)(load * 100.f));

    gfx.drawFrame(0, lines.y, 72, 8);
    gfx.drawRect(0, lines.y, static_cast<weegfx::coord_t>(load * 72.f + .5f), 8);
  }
};

}  // namespace oct4

#endif  // HWTEST_CORE_H_
