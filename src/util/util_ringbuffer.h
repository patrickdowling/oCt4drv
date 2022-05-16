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

#ifndef UTIL_RINGBUFFER_H_
#define UTIL_RINGBUFFER_H_

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "util/util_macros.h"

namespace util {

// The assumption here is that atomics are mostly free (because we're using size_t/u32).
// But we can use the barrier/memory_order instead of the platform-specific __DMB. Generally the
// ring buffer is written in one "thread" (an ISR) and consumed in another (main loop). So we have
// to take some care to ensure memory ordering, especially now there's a cache involved.
//
// https://developer.arm.com/documentation/dui0646/a/The-Cortex-M7-Instruction-Set/Miscellaneous-instructions/DSB,
template <size_t size>
class RingBufferBase {
protected:
  volatile std::atomic<size_t> head_;
  volatile std::atomic<size_t> tail_;

  size_t head_relaxed() const { return head_.load(std::memory_order_relaxed); }
  size_t tail_relaxed() const { return tail_.load(std::memory_order_relaxed); }
};

// There seem to be two types of common ring buffer implementations:
// One that doesn't use the last item in order to distinguish between "empty"
// and "full" states (e.g. MI stmlib/utils/ring_buffer.h). The other relies on
// wrapping read/write heads and appears to use all available items.
// This implements the latter.
// - Pretty much assumes single producer/consumer
// - Assume size is pow2
template <typename T, size_t size>
class RingBuffer : public RingBufferBase<size> {
public:
  RingBuffer() = default;
  DELETE_COPY_ASSIGN(RingBuffer);

  inline size_t readable() const { return this->head_relaxed() - this->tail_relaxed(); }

  inline size_t writable() const { return size - readable(); }

  // Tail is used in this context, head_ is written elsewhere
  inline T Read()
  {
    auto tail = this->tail_.load(std::memory_order_acquire);
    T value = buffer_[tail & (size - 1)];
    this->tail_.store(tail + 1, std::memory_order_release);
    return value;
  }

  // Head is in this context, tail_ written elsewhere
  inline void Write(T value)
  {
    auto head = this->head_relaxed();
    buffer_[head & (size - 1)] = value;
    this->head_.store(head + 1, std::memory_order_release);
  }

  template <typename... Args>
  inline void EmplaceWrite(Args&&... args)
  {
    auto head = this->head_relaxed();
    buffer_[head & (size - 1)] = T{std::forward<Args>(args)...};
    this->head_.store(head + 1, std::memory_order_release);
  }

  // inline void Flush() { write_ptr_ = read_ptr_ = 0; }

private:
  T buffer_[size];
};

};  // namespace util

#endif  // UTIL_RINGBUFFER_H_
