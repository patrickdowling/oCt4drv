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

#ifndef OCT4_UI_H_
#define OCT4_UI_H_

#include "api/api_menu.h"
#include "system/oct4_controls.h"
#include "system/oct4_debug.h"
#include "system/ui//ui_event.h"
#include "system/ui/ui_event_queue.h"
#include "util/util_timer.h"

// NOTES
// THis is a reduced verson of OC::Ui in that the UI no longer contains the calibration, debug
// menu, app menu, etc. which are now implemented using the Menu interface. It's basically reduced
// to these functions
// - Event dispatching
// - Screensaver mode handling
//
// It also does the physical control (SystemCore::controls) to event polling

namespace oct4 {
namespace UI {

enum UiMode { UI_MODE_SCREENSAVER, UI_MODE_MENU };

enum TimerID : int32_t {
  TIMER_NONE = 0,
  TIMER_BOOTSCREEN,
};

class EventDispatcher {
public:
  using EventType = Event;

  static constexpr size_t kEventQueueDepth = 16;
  static constexpr uint32_t kLongPressTicks = 1000;
  static constexpr uint32_t kXLongPressTicks = 4000;
  static constexpr size_t kNumTimers = 4;

  void Init();
  void PollControls();
  UiMode DispatchEvents(api::Menu *menu);

  void AddTimer(TimerID timer_id, uint32_t timeout);

  inline uint32_t idle_time() const { return event_queue_.idle_time(); }

  inline uint32_t ticks() const { return ticks_; }

private:
  enum EScreensaverMode { SCREENSAVER_OFF, SCREENSAVER_ACTIVE, SCREENSAVER_BLANKING };

  uint32_t ticks_ = 0;
  uint32_t screensaver_timeout_ = 0;
  uint32_t blanking_timeout_ = 0;

  EventQueue<EventDispatcher, kEventQueueDepth> event_queue_{*this};

  std::array<uint32_t, oct4::SWITCH_LAST> button_press_time_;
  std::array<util::Timer, kNumTimers> timer_slots_;

  template <typename... Args>
  inline void PushEvent(Args &&...args)
  {
    OCT4_DEBUG_INC(debug.ui.event_count);
    if (event_queue_.writable()) {
      event_queue_.PushEvent(std::forward<Args>(args)...);
    } else {
      OCT4_DEBUG_INC(debug.ui.event_queue_overflow);
    }
  }
};

}  // namespace UI
}  // namespace oct4

#endif  // OCT4_UI_H_
