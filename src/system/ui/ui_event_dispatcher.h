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

#ifndef UI_EVENT_DISPATCHER_H_
#define UI_EVENT_DISPATCHER_H_

#include "ui_event.h"

namespace oct4 {
namespace UI {

template <typename T>
class EventDispatch {
protected:
  void DispatchEvent(const UI::Event &event)
  {
    auto event_handlers = static_cast<const T *>(this)->get_event_handlers();
    while (event_handlers->fn) {
      if (event_handlers->event_type == event.type && event_handlers->control == event.control) {
        (static_cast<T *>(this)->*event_handlers->fn)(event.type, event.value);
        break;
      }
      ++event_handlers;
    }
  }

  using EventHandlerFn = void (T::*)(UI::ControlEvent, int32_t);

  struct EventHandler {
    const ControlEvent event_type = EVENT_NONE;
    const ControlID control = CONTROL_NONE;
    const EventHandlerFn fn = nullptr;
  };
};

}  // namespace UI
}  // namespace oct4

#define EVENT_DISPATCH_DECLARE_HANDLER(fn) void fn(UI::ControlEvent, int32_t)
#define EVENT_DISPATCH_DEFINE_HANDLER(cls, fn) \
  void cls::fn(UI::ControlEvent event_type, int32_t event_value)
#define EVENT_DISPATCH_HANDLER_STUB() \
  {                                   \
    do {                              \
      (void)event_type;               \
      (void)event_value;              \
    } while (0);                      \
  }

#define EVENT_DISPATCH_DEFAULT()               \
  friend class EventDispatch;                  \
  static const EventHandler event_handlers_[]; \
  const EventHandler *get_event_handlers() const { return event_handlers_; }

#define EVENT_DISPATCH_DEFAULT_DEFINE(cls) const cls::EventHandler cls::event_handlers_[] =

#define EVENT_DISPATCH_HANDLERS(cls, name) const cls::EventHandler cls::name[] =

#endif  // UI_EVENT_DISPATCHER_H_
