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

#ifndef UI_EVENT_QUEUE_H_
#define UIL_EVENT_QUEUE_H_

#include "util/util_ringbuffer.h"

namespace oct4 {
namespace UI {

// Event queue for UI events
// Meant for single producer/single consumer setting
//
// Yes, looks similar to stmlib::EventQueue, but hey, it's a queue for UI events.
template <typename OwnerType, size_t size = 16>
class EventQueue {
public:
  explicit EventQueue(OwnerType &owner) : owner_{owner} {}
  DELETE_COPY_ASSIGN(EventQueue);

  inline void Flush() { events_.Flush(); }

  inline bool available() const { return events_.readable(); }

  template <typename... Args>
  inline void PushEvent(Args &&...args)
  {
    events_.EmplaceWrite(std::forward<Args>(args)...);
    Poke();
  }

  inline typename OwnerType::EventType PopEvent() { return events_.Read(); }

  inline void Poke() { last_event_time_ = owner_.ticks(); }

  inline uint32_t idle_time() const { return owner_.ticks() - last_event_time_; }

  // More for debugging purposes
  inline bool writable() const { return events_.writable(); }

private:
  OwnerType &owner_;
  util::RingBuffer<typename OwnerType::EventType, size> events_;
  uint32_t last_event_time_ = 0;
};

}  // namespace UI
}  // namespace oct4

#endif  // UI_EVENT_QUEUE_H_
