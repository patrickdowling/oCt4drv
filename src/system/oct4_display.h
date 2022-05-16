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

#ifndef OCT4_DISPLAY_H_
#define OCT4_DISPLAY_H_

// #include "../../util/util_debugpins.h"
#include "extern/weegfx/weegfx.h"
#include "util/util_framebuffer.h"
#include "util/util_macros.h"

namespace oct4 {

extern weegfx::Graphics graphics;

template <typename driver_type>
class Display {
public:
  using FrameBuffer = util::FrameBuffer<driver_type::kFrameSize, 2>;

  Display(driver_type &driver, uint8_t *buffer) : driver_{driver}, frame_buffer_{buffer} {}
  DELETE_COPY_ASSIGN(Display);

  inline void Flush()
  {
    if (driver_.frame_valid() &&
        driver_type::TransferStatus::FRAME_COMPLETE == driver_.AsyncTransferWait())
      frame_buffer_.frame_read();
  }

  inline void Update()
  {
    if (driver_.frame_valid()) {
      driver_.AsyncTransferBegin();
    } else {
      // TODO we need to ensure that if frame is valid, the transfer has started. This isn't how
      // things worked for OG o_C but otherwise the AsyncTransferBegin/AsyncTransferWait tick/tock
      // gets out of sync here.
      auto buffer = frame_buffer_.readable_frame();
      if (buffer) {
        driver_.SetupFrame(buffer);
        driver_.AsyncTransferBegin();
      }
    }
  }

  struct Frame {
    Frame(uint8_t *buffer, Display *owner) : buffer_{buffer}, owner_{owner} {}
    ~Frame()
    {
      if (owner_) owner_->EndFrame(this);
    }

    inline bool valid() const { return buffer_; }

    uint8_t *buffer() { return buffer_; }

  private:
    uint8_t *buffer_ = nullptr;
    Display *owner_ = nullptr;
  };

  inline Frame BeginFrame(bool wait)
  {
    uint8_t *buffer = nullptr;
    do {
      buffer = frame_buffer_.writeable_frame();
    } while (!buffer && wait);

    return {buffer, this};
  }

  driver_type &driver() { return driver_; }

private:
  driver_type &driver_;
  FrameBuffer frame_buffer_;

  void EndFrame(const Frame *frame)
  {
    if (frame->valid()) { frame_buffer_.frame_written(); }
  }
};

}  // namespace oct4

#endif  // OCT4_DISPLAY_H_
