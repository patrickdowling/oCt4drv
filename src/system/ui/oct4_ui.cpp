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

#include "system/ui/oct4_ui.h"

#include "api/api_menu.h"
#include "system/oct4_system_core.h"

// extern uint_fast8_t MENU_REDRAW;

namespace oct4 {
namespace UI {

void EventDispatcher::Init()
{
  std::fill(button_press_time_.begin(), button_press_time_.end(), 0);
}

// TODO Move the ID into encoder/switch implementation?

static constexpr ControlID SwitchIDToControlID[] = {
    CONTROL_BUTTON_UP,
    CONTROL_BUTTON_DOWN,
    CONTROL_BUTTON_L,
    CONTROL_BUTTON_R,
};

static constexpr ControlID EncoderIDToControlID[] = {CONTROL_ENCODER_L, CONTROL_ENCODER_R};

void FASTRUN EventDispatcher::PollControls()
{
  SystemCore::controls.Poll();

  auto now = ++ticks_;
  uint32_t button_state = 0;
#if 0
  for (size_t i = 0; i < CONTROL_BUTTON_LAST; ++i) {
    if (buttons_[i].Poll())
      button_state |= control_mask(i);
  }
#endif

  for (unsigned i = 0; i < oct4::SWITCH_LAST; ++i) {
    auto &button = oct4::SystemCore::controls.switches[i];
    if (button.just_pressed()) {
      button_press_time_[i] = now;
    } else if (button.released()) {
      auto t = now - button_press_time_[i];
      if (t < kLongPressTicks)
        PushEvent(EVENT_BUTTON_PRESS, SwitchIDToControlID[i], 0, button_state, t);
      else
        PushEvent(EVENT_BUTTON_LONG_PRESS, SwitchIDToControlID[i], 0, button_state, t);
    }
  }

  for (unsigned i = 0; i < oct4::ENCODER_LAST; ++i) {
    int32_t increment = oct4::SystemCore::controls.encoders[i].Read();
    if (increment) PushEvent(EVENT_ENCODER, EncoderIDToControlID[i], increment, button_state, 0U);
  }

  // button_state_ = button_state;
}

UiMode EventDispatcher::DispatchEvents(api::Menu *menu)
{
  // We could do this in PollControls but that may involve more sync
  auto ticks = ticks_;
  for (auto &timer : timer_slots_) {
    if (timer.enabled()) {
      timer.Update(ticks);
      if (timer.elapsed()) {
        auto id = timer.id();
        timer.Reset();
        menu->HandleEvent({EVENT_TIMER, CONTROL_NONE, id, 0, 0});
      }
    }
  }

  while (event_queue_.available()) {
    const auto event = event_queue_.PopEvent();
    menu->HandleEvent(event);
  }

  return UI_MODE_MENU;
}

void EventDispatcher::AddTimer(TimerID id, uint32_t timeout)
{
  for (auto &timer : timer_slots_) {
    if (!timer.enabled()) {
      timer.Arm(static_cast<int32_t>(id), ticks_, timeout);
      break;
    }
  }
}

}  // namespace UI
}  // namespace oct4
