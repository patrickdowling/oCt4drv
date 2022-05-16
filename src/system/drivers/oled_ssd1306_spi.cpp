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

#include "system/drivers/oled_ssd1306_spi.h"

#include <cstring>

#include "system/drivers/oct4_gpio.h"

namespace oct4 {
namespace drivers {

static const uint8_t empty_page_buffer[OledSSD1306::kPageSize] = {0};

FLASHMEM void OledSSD1306::Init(bool display_on)
{
  GPIO::OLED_CS::Init();
  GPIO::OLED_RST::Init();
  GPIO::OLED_DC::Init();

  page_header_[0] = SSD1306::SET_HIGHER_COL_START_ADDRESS /*| (offset_ >> 4)*/;
  page_header_[1] = SSD1306::SET_LOWER_COL_START_ADDRESS | kDefaultOffset;
  page_header_[2] = SSD1306::SET_PAGE_START_ADDRESS | current_page_;

  command_buffer_[0] = display_on ? SSD1306::SET_DISPLAY_ON : SSD1306::SET_DISPLAY_OFF;
  command_buffer_[1] = SSD1306::SET_CONTRAST;
  command_buffer_[2] = kDefaultContrast;

  // Reset
  GPIO::OLED_RST::Set();
  delay(1);
  GPIO::OLED_RST::Clear();
  delay(10);
  GPIO::OLED_RST::Set();

  GPIO::OLED_CS::Set();
  GPIO::OLED_DC::Clear();
  GPIO::OLED_RST::Clear();
  delay(20);
  GPIO::OLED_RST::Set();
  delay(20);

  // CS active until end of scope
  ActiveLow<GPIO::OLED_CS> cs;
  spi_.SetMode<SPI::PCS_DISABLE, 8>();

  // Startup sequence (without SET_DISPLAY_ON)
  size_t sequence_length = sizeof(SSD1306::startup_sequence) - 1;
  {
    ActiveLow<GPIO::OLED_DC> cmd_mode;
    spi_.Send(SSD1306::startup_sequence, sequence_length);
    spi_.Send(command_buffer_, kCommandBufferSize);
  }

  // Clear
  for (size_t page = 0; page < kNumPages; ++page) {
    {
      ActiveLow<GPIO::OLED_DC> cmd_mode;
      page_header_[2] = SSD1306::SET_PAGE_START_ADDRESS | page;
      spi_.Send(page_header_, kPageHeaderSize);
    }
    spi_.Send(empty_page_buffer, sizeof(empty_page_buffer));
  }

  // And finally, display on (at least if configured)
  {
    ActiveLow<GPIO::OLED_DC> cmd_mode;
    spi_.Send(command_buffer_, kCommandBufferSize);
  }
}

void OledSSD1306::SetupFrame(const uint8_t *frame)
{
  // TODO inject commands here, use member array for page_write header

  current_frame_ = frame;
  current_page_ = 0;
}

void OledSSD1306::AsyncTransferBegin()
{
  spi_.Flush();
  spi_.SetMode<SPI::PCS_DISABLE, 8>();

  GPIO::OLED_CS::Clear();
  {
    ActiveLow<GPIO::OLED_DC> cmd_mode;
    page_header_[2] = SSD1306::SET_PAGE_START_ADDRESS | current_page_;

    if (!current_page_) spi_.Send(command_buffer_, kCommandBufferSize);
    spi_.Send(page_header_, kPageHeaderSize);
  }

  spi_.AsyncTransferBegin(current_page_ptr(), kPageSize);
}

OledSSD1306::TransferStatus OledSSD1306::AsyncTransferWait()
{
  spi_.AsyncTransferWait();
  GPIO::OLED_CS::Set();

  if (current_page_ < kNumPages - 1) {
    ++current_page_;
    return TransferStatus::PAGE_COMPLETE;
  } else {
    current_frame_ = nullptr;
    current_page_ = 0;
    return TransferStatus::FRAME_COMPLETE;
  }
}

void OledSSD1306::AdjustOffset(uint8_t offset)
{
  page_header_[1] = SSD1306::SET_LOWER_COL_START_ADDRESS | (offset & 0x0f);
}

void OledSSD1306::CmdDisplayOn(bool on)
{
  command_buffer_[0] = on ? SSD1306::SET_DISPLAY_ON : SSD1306::SET_DISPLAY_OFF;
}

void OledSSD1306::CmdSetContrast(uint8_t contrast)
{
  command_buffer_[2] = contrast;
}

}  // namespace drivers
}  // namespace oct4
