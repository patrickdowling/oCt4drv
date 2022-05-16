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

#include "builtin/bootscreen.h"

#include "resources/oct4_strings.h"
#include "system/oct4_system_core.h"

namespace oct4 {

static Bootscreen instance;
static api::Menu::MenuRegistrar<Bootscreen> register_menu(&instance);

void Bootscreen::HandleMenuEvent(api::MenuEvent menu_event)
{
  switch (menu_event) {
    case api::MENU_ACTIVATE:
      SystemCore::event_dispatcher.AddTimer(UI::TIMER_BOOTSCREEN, kDisplayOnTimeoutMs);
      break;
    case api::MENU_DEACTIVATE: break;
  }
}

void Bootscreen::Tick()
{
  ++ticks_;
}

void Bootscreen::HandleEvent(const UI::Event &event)
{
  // DEBUG_EVENT(, event);
  if (UI::EVENT_TIMER == event.type && UI::TIMER_BOOTSCREEN == event.value) {
    switch (state_) {
      case 0:
        ++state_;
        ticks_ = 0;
        SystemCore::display.driver().CmdDisplayOn(true);
        SystemCore::event_dispatcher.AddTimer(UI::TIMER_BOOTSCREEN, kBootscreenTimeoutMs);
        break;
      case 1: SystemCore::Execute("TEST"_4CC); break;
      default: break;
    }
  }
}

void Bootscreen::Draw(weegfx::Graphics &gfx) const
{
  gfx.drawFrame(0, 0, gfx.kWidth, gfx.kHeight);
  gfx.drawHLine(0, 56, gfx.kWidth);
  gfx.drawRect(0, 56, ticks_ / 8, 8);

  gfx.drawStr(8, 8, strings::NAME);
  gfx.drawStr(9, 16, strings::VERSION);
}

}  // namespace oct4
