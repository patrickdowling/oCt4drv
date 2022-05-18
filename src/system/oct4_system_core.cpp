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

#include "oct4_system_core.h"

#include "system/oct4_debug.h"
#include "system/system_profiling.h"
#include "system/system_serial.h"
#include "system/t4.h"

namespace oct4 {
#define CORE_TIMER_PARAMS(freq, e, adc)                            \
  {                                                                \
    freq, (1000000UL / freq), e, drivers::AnalogInputs::adc, #freq \
  }

static constexpr struct {
  unsigned freq;
  unsigned period;
  bool enable_subpages;
  drivers::AnalogInputs::AdcMode adc_mode;
  const char *const str;
} kCoreTimerParams[SystemCore::CORE_FREQ_LAST] = {
    CORE_TIMER_PARAMS(16666, false, ADC_MODE_NORMAL),
    CORE_TIMER_PARAMS(32000, true, ADC_MODE_FAST),
};

static constexpr unsigned OC_UI_TIMER_PERIOD = 1000U;

static constexpr uint32_t OCT4_SPI_CLOCK = 30'000'000;

static constexpr uint8_t OC_CORE_TIMER_PRIO = 80;  // higher
static constexpr uint8_t OC_UI_TIMER_PRIO = 128;   // default

/*static*/
DMAMEM_ALIGN32 uint8_t SystemCore::framebuffer[SystemCore::kFrameBufferSize];

/*static*/ SystemCore::Drivers SystemCore::drivers;
/*static*/ Controls SystemCore::controls;
/*static*/ UI::EventDispatcher SystemCore::event_dispatcher;
/*static*/ SystemCore::display_type SystemCore::display{SystemCore::drivers.oled_ssd1306,
                                                        framebuffer};

/*static*/ volatile uint32_t SystemCore::ticks_ = 0;

Debug debug;
weegfx::Graphics graphics;

static IntervalTimer core_interrupt_timer;
static IntervalTimer ui_interrupt_timer;

static api::Processor::Inputs inputs;
static api::Processor::Outputs outputs;

static api::Processor *processor_ = nullptr;
/*static*/ SystemCore::CoreFreq SystemCore::core_freq_ = SystemCore::CORE_FREQ_LAST;
static api::Menu *menu_ = nullptr;

static void FASTRUN ui_interrupt_handler()
{
  SystemCore::event_dispatcher.PollControls();
}

static profiling::CycleCount core_interrupt_period;

static void FASTRUN core_interrupt_handler()
{
  debug.perf.core_isr_period.push(core_interrupt_period.elapsed_restart());
  profiling::ScopedCycleCount _{debug.perf.core_isr_cycles};

  // The DAC update isn't using DMA, but since we have a FIFO and hardware CS it effectively runs
  // async while other things are done. Care needs to be taken to ensure it has been flushed before
  // the display driver does anything.
  SystemCore::display.Flush();
  SystemCore::drivers.dac8565.Update(outputs.dac);
  SystemCore::drivers.digital_inputs.Read(inputs.digital_inputs);
  SystemCore::drivers.analog_inputs.Read(inputs.analog_inputs);
  SystemCore::display.Update();
  // DMA -> screen begins, all other processing can start here.
  SystemCore::drivers.analog_inputs.StartConversion();
  uint32_t ticks = SystemCore::Tick();

  auto proc = processor_;
  proc->Process(ticks, inputs, outputs);
}

FLASHMEM void SystemCore::Init()
{
  DEBUG_BUFFER("framebuffer", framebuffer, sizeof(framebuffer));

  profiling::CycleCount::Init();

  drivers.spi.Init(OCT4_SPI_CLOCK);
  drivers.dac8565.Init();
  drivers.oled_ssd1306.Init(false);
  drivers.analog_inputs.Init(drivers::AnalogInputs::ADC_MODE_NORMAL);
  drivers.digital_inputs.Init();

  controls.Init();
  event_dispatcher.Init();

  menu_ = api::Menu::GetInstance("NULL"_4CC);
  if (!menu_) SERIAL_ERROR("MENU/NULL not found!");

  processor_ = api::Processor::GetInstance("NULL"_4CC);
  if (!processor_) SERIAL_ERROR("PROC/NULL not found!");

  tempmon_Start();  // ?
}

FLASHMEM void SystemCore::StartInterrupts(CoreFreq core_freq)
{
  // Ensure there's a valid frame and the first SPI transfer is started
  DISPLAY_BEGIN_FRAME(true);
  DISPLAY_END_FRAME();

  // Start ADC as well since it's dependent on DMA completion as well
  drivers.analog_inputs.StartConversion();
  delay(1);

  ui_interrupt_timer.begin(ui_interrupt_handler, OC_UI_TIMER_PERIOD);
  ui_interrupt_timer.priority(OC_UI_TIMER_PRIO);

  StartCoreInterupt(core_freq);
}

void SystemCore::StopCoreInterrupt()
{
  // FIXME This is pretty brute force. Really we might want to wait for
  // - display frame to complete. This is 480us for 8 pages or x4 for subpage mode
  // - Other?
  delay(4);
  core_interrupt_timer.end();
}

void SystemCore::StartCoreInterupt(CoreFreq core_freq)
{
  const auto params = kCoreTimerParams[core_freq];

  display.enable_subpages(params.enable_subpages);
  drivers.analog_inputs.ChangeMode(params.adc_mode);
  delay(1);

  core_interrupt_timer.begin(core_interrupt_handler, params.period);
  core_interrupt_timer.priority(OC_CORE_TIMER_PRIO);
}

FLASHMEM void SystemCore::MainLoop()
{
  auto menu = menu_;

  SystemCore::event_dispatcher.DispatchEvents(menu);
  menu->Tick();  // Temporary

  DISPLAY_BEGIN_FRAME(true);
  menu->Draw(graphics);
  DISPLAY_END_FRAME();
}

void SystemCore::Execute(const util::FourCC menu_fourcc)
{
  auto menu = api::Menu::GetInstance(menu_fourcc);
  if (!menu) menu = api::Menu::GetInstance("NULL"_4CC);

  if (menu_ != menu) {
    SERIAL_DEBUG("MENU/%s %p", menu->menu_type().str().value, menu);
    menu_->HandleMenuEvent(api::MENU_DEACTIVATE);
    menu->HandleMenuEvent(api::MENU_ACTIVATE);
    menu_ = menu;
  }
}

void SystemCore::Execute(api::Processor *processor, CoreFreq core_freq)
{
  if (!processor) processor = api::Processor::GetInstance("NULL"_4CC);

  if (processor_ != processor || core_freq != core_freq_) {
    SERIAL_DEBUG("PROC/%s %p timing=%s", processor->processor_type().str().value, processor,
                 kCoreTimerParams[core_freq].str);

    StopCoreInterrupt();
    core_freq_ = core_freq;
    processor_ = processor;
    StartCoreInterupt(core_freq);
  }
}

unsigned SystemCore::current_core_freq()
{
  return kCoreTimerParams[core_freq_].freq;
}

}  // namespace oct4
