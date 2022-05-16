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

#ifndef OCT4_SPI_H_
#define OCT4_SPI_H_

#include <cstddef>

#include "system/drivers/oct4_gpio.h"
#include "util/util_macros.h"

// The original o_C SPI driver was a bit of mish-mash of various implementations,
// along with some custom bits. Since the DAC/screen share the same port it was a bit
// messy and in some ways worked "by accident"; the DAC part used the hardware CS
// selection from one implementation and the screen part did it manually, which makes
// the whole thing a bit fragile.
//
// This is really a Transmit-only master since we don't care about MISO at all.

namespace oct4 {
namespace drivers {

class SPI {
public:
  SPI() = default;
  DELETE_COPY_ASSIGN(SPI);

  void Init(uint32_t clock);

  enum PCSMode { PCS_DISABLE, PCS_ENABLE };
  enum FlushMode { NOFLUSH, FLUSH };

  template <PCSMode pcs_mode, uint8_t frame_size>
  void SetMode()
  {
    static_assert(0 == frame_size % 2, "...2 bits");
    static_assert(frame_size <= 32, "TODO");  // So we can avoid using CONT for now
    UpdateTCR(pcs_mode, frame_size);
  }

  // Yeah, we can template these eventually
  void Send(const uint8_t *data, size_t count, FlushMode flush_mode = FLUSH);
  void Send(const uint32_t *data, size_t count, FlushMode flush_mode = FLUSH);

  void Flush();

  // Begin async tranfer
  void AsyncTransferBegin(const uint8_t *data, size_t num_bytes);

  // Wait for async transfer to complete (assuming one was started)a
  void AsyncTransferWait();

private:
  uint32_t tcr_ = 0;
  bool dma_active_ = false;

  void UpdateTCR(PCSMode pcs_mode, uint8_t frame_size);
};

}  // namespace drivers
}  // namespace oct4

#endif  // OCT4_SPI_H_
