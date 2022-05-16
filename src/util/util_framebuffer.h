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

#ifndef UTIL_FRAMEBUFFER_H_
#define UTIL_FRAMEBUFFER_H_

#include <cstddef>

#include "util/util_macros.h"
#include "util/util_ringbuffer.h"

namespace util {

// - This could be specialized for frames == 2 (i.e. double-buffer)
// - Takes some short-cuts so assumes correct order of calls
//
// Initial version used a different ring-buffer implementation that used
// frames - 1 elements to be able to distinguish between empty/full, but
// which also meant that it wasn't properly double-buffering.
// This uses an alternate approach that relies on unsigned integer wrap,
// but allows a new frame to be written while the old one is being
// transferred.
// See https://gist.github.com/patrickdowling/0029f58fb20e63d7db9d

template <size_t frame_size, size_t num_frames>
class FrameBuffer : public RingBufferBase<num_frames> {
public:
  static constexpr size_t kFrameSize = frame_size;
  static constexpr size_t kNumFrames = num_frames;
  static constexpr size_t kBufferSize = frame_size * num_frames;

  FrameBuffer() = delete;
  explicit FrameBuffer(uint8_t *buffer) : frame_memory_{buffer}
  {
    memset(frame_memory_, 0, kBufferSize);
    for (size_t f = 0; f < kNumFrames; ++f) frame_buffers_[f] = frame_memory_ + kFrameSize * f;
  }

  // @return readable frame, nullptr if none available
  // tail is in our "thread", but we're consuming head
  const uint8_t *readable_frame() const
  {
    auto tail = this->tail_relaxed();
    if (this->head_.load(std::memory_order_acquire) - tail)
      return frame_buffers_[tail % kNumFrames];
    else
      return nullptr;
  }

  // @return next writeable frame, nullptr if none availbe
  // head is in our "thread", but we're consuming tail
  uint8_t *writeable_frame()
  {
    auto head = this->head_relaxed();
    if ((head - this->tail_.load(std::memory_order_acquire)) < kNumFrames)
      return frame_buffers_[head % kNumFrames];
    else
      return nullptr;
  }

  inline void frame_written()
  {
    this->head_.store(this->head_relaxed() + 1, std::memory_order_release);
  }

  inline void frame_read()
  {
    this->tail_.store(this->tail_relaxed() + 1, std::memory_order_release);
  }

private:
  uint8_t *frame_memory_ = nullptr;
  uint8_t *frame_buffers_[kNumFrames];

  DELETE_COPY_ASSIGN(FrameBuffer);
};

}  // namespace util

#endif  // UTIL_FRAMEBUFFER_H_
