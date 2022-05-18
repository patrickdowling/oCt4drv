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

#ifndef OCT4_SYSTEM_H_
#define OCT4_SYSTEM_H_

#include "api/api_processor.h"
#include "drivers/dac8565.h"
#include "drivers/oct4_analog_inputs.h"
#include "drivers/oct4_digital_inputs.h"
#include "drivers/oct4_spi.h"
#include "drivers/oled_ssd1306_spi.h"
#include "system/oct4_controls.h"
#include "system/oct4_display.h"
#include "system/ui/oct4_ui.h"
#include "util/util_macros.h"

namespace oct4 {

class SystemCore {
public:
  SystemCore() = default;
  DELETE_COPY_ASSIGN(SystemCore);

  enum CoreFreq { CORE_FREQ_OC16Khz, CORE_FREQ_32KHz, CORE_FREQ_LAST };

  static void Init();

  static void StartInterrupts(CoreFreq core_freq);

  static void MainLoop();

  static void Execute(const util::FourCC menu_fourcc);

  static void Execute(api::Processor *processor, CoreFreq core_freq);

  static struct Drivers {
    drivers::SPI spi;
    drivers::DAC8565 dac8565{spi};
    drivers::OledSSD1306 oled_ssd1306{spi};
    drivers::AnalogInputs analog_inputs;
    drivers::DigitalInputs digital_inputs;
  } drivers;

  static Controls controls;
  static UI::EventDispatcher event_dispatcher;

  using display_type = Display<drivers::OledSSD1306>;
  static display_type display;

  static uint32_t ticks() { return ticks_; }

  static uint32_t Tick() { return ticks_++; }

  static float read_temperature() { return tempmonGetTemp(); }

  static unsigned current_core_freq();

private:
  static constexpr size_t kFrameBufferSize = display_type::FrameBuffer::kBufferSize;
  static uint8_t framebuffer[kFrameBufferSize];

  static volatile uint32_t ticks_;
  static CoreFreq core_freq_;

  static void StopCoreInterrupt();
  static void StartCoreInterupt(CoreFreq core_freq);
};

extern weegfx::Graphics graphics;

}  // namespace oct4

#define DISPLAY_BEGIN_FRAME(wait)                                 \
  do {                                                            \
    auto frame = SystemCore::display.BeginFrame(wait);            \
    if (frame.valid()) {                                          \
      graphics.Begin(frame.buffer(), weegfx::CLEAR_FRAME_ENABLE); \
      do {                                                        \
    } while (0)

#define DISPLAY_END_FRAME() \
  graphics.End();           \
  }                         \
  }                         \
  while (0)

#endif  // OCT4_SYSTEM_H_
