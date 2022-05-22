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

#include "hwtest.h"

#include <algorithm>

#include "builtin/hwtest_adc.h"
#include "builtin/hwtest_core.h"
#include "builtin/hwtest_debug.h"
#include "builtin/hwtest_gfx.h"
#include "builtin/hwtest_info.h"
#include "builtin/hwtest_trx.h"
#include "system/oct4_system_core.h"
#include "util/util_templates.h"

namespace oct4 {

static HWTestApp instance;
api::Menu::MenuRegistrar<HWTestApp> register_menu(&instance);
api::Processor::ProcRegistrar<HWTestApp> register_proc(&instance);

// These could also use a registry, and eventually should only be allocated if the menu is active
std::tuple<PluginInfo, PluginCore, PluginADC, PluginTRx, PluginGfx, PluginDebug> plugin_types;
auto plugins_ = util::tuple_to_ptr_array<HWTestPlugin *>(plugin_types);

void HWTestApp::Process(uint32_t ticks, const api::Processor::Inputs &inputs,
                        api::Processor::Outputs &outputs)
{
  plugins_[plugin_index_]->Process(inputs);

  // Ramp
  outputs.dac[0] += 1;
  outputs.dac[1] += 2;

  // Tri
  auto value = outputs.dac[2] + dir_;
  if (value == 0xffff)
    dir_ = -1;
  else if (!value)
    dir_ = 1;
  outputs.dac[2] = value;

  // ca. 2KHz square
  auto t = ticks & 7;
  outputs.dac[3] = t < 4 ? 0xffff : 0;
}

void HWTestApp::HandleMenuEvent(api::MenuEvent menu_event)
{
  switch (menu_event) {
    case api::MENU_ACTIVATE: SystemCore::Execute(this, core_freq_); break;
    case api::MENU_DEACTIVATE: break;
  }
}

void HWTestApp::Tick()
{
  plugins_[plugin_index_]->Tick();
}

void HWTestApp::HandleEvent(const UI::Event &event)
{
  // DEBUG_EVENT(HWTestApp, event);
  DispatchEvent(event);
}

void HWTestApp::Draw(weegfx::Graphics &gfx) const
{
  plugins_[plugin_index_]->Draw(gfx);
}

EVENT_DISPATCH_DEFAULT_DEFINE(HWTestApp){
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_UP, &HWTestApp::evButtonUp},
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_DOWN, &HWTestApp::evButtonDown},
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_R, &HWTestApp::evButtonR},
    {UI::EVENT_ENCODER, UI::CONTROL_ENCODER_L, &HWTestApp::evEncoderL},
    {UI::EVENT_ENCODER, UI::CONTROL_ENCODER_R, &HWTestApp::evEncoderR},
    {},
};

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonUp)
{
  EVENT_DISPATCH_HANDLER_STUB();

  display_on_ = !display_on_;
  SystemCore::display.driver().CmdDisplayOn(display_on_);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonDown)
{
  EVENT_DISPATCH_HANDLER_STUB();
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonR)
{
  EVENT_DISPATCH_HANDLER_STUB();

  auto freq = core_freq_ + 1;
  if (freq >= SystemCore::CORE_FREQ_LAST) freq = 0;
  core_freq_ = static_cast<SystemCore::CoreFreq>(freq);

  SystemCore::Execute(this, core_freq_);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evEncoderL)
{
  EVENT_DISPATCH_HANDLER_STUB();

  static constexpr int32_t num_modes = plugins_.size();

  plugin_index_ = util::clamp(plugin_index_ + event_value, 0L, num_modes - 1);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evEncoderR)
{
  EVENT_DISPATCH_HANDLER_STUB();
  plugins_[plugin_index_]->evEncoderR(event_type, event_value);
}

}  // namespace oct4
