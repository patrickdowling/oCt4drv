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

#ifndef HWTEST_PLUGIN_H_
#define HWTEST_PLUGIN_H_

#include "api/api_processor.h"
#include "gfx/gfxtools.h"
#include "system/ui/ui_event_dispatcher.h"

namespace oct4 {

// Just a minimal-ish way to add features to the HWTest "app"
// We're not super concerned with performance here since it's mostly "FYI" or sanity checks.

class HWTestPlugin {
public:
  virtual ~HWTestPlugin() {}

  virtual void Process(const api::Processor::Inputs &inputs) { last_inputs_ = inputs; }
  virtual void Tick() {}
  virtual void Draw(weegfx::Graphics &gfx) const = 0;

  EVENT_DISPATCH_DECLARE_HANDLER(evEncoderR);

protected:
  void DrawTitlebar(weegfx::Graphics &gfx, const char *plugin_name) const;

  int32_t current_page_ = 0;
  const int32_t num_pages_;

  api::Processor::Inputs last_inputs_;

  HWTestPlugin(int32_t num_pages = 1) : num_pages_{num_pages} {}

  virtual void evPageChanged() { }
};

}  // namespace oct4

#endif  // HWTEST_PLUGIN_H_
