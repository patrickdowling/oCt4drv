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

#include "builtin/hwtest_plugin.h"

#include "resources/oct4_icons.h"
#include "system/oct4_system_core.h"
#include "util/util_templates.h"

namespace oct4 {

void HWTestPlugin::DrawTitlebar(weegfx::Graphics &gfx, const char *plugin_name) const
{
  gfx.setPrintPos(0, 0);
  gfx.printf("oCT4/%s", plugin_name);
  if (num_pages_ > 1) gfx.printf(" %ld/%ld", current_page_ + 1, num_pages_);

  gfx.drawHLinePattern(0, 9, gfx.kWidth, 2);

  auto core_freq = static_cast<float>(SystemCore::current_core_freq()) / 1000.f;
  gfx.setPrintPos(128 - 13 - 24, 0);
  gfx.printf("%.1f", (double)core_freq);
  gfx.writeBitmap8(128 - 13, 0, icons::unit_khz.w, icons::unit_khz.data);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestPlugin, evEncoderR)
{
  EVENT_DISPATCH_HANDLER_STUB();

  auto page = util::clamp(current_page_ + event_value, 0L, num_pages_ - 1);
  if (page != current_page_) {
    current_page_ = page;
    evPageChanged();
  }
}

}  // namespace oct4
