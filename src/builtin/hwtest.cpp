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

#include "hwtest.h"

#include "resources/oct4_strings.h"
#include "system/oct4_system_core.h"

namespace oct4 {

static HWTestApp instance;
api::Menu::MenuRegistrar<HWTestApp> register_menu(&instance);
api::Processor::ProcRegistrar<HWTestApp> register_proc(&instance);

static constexpr const char *to_string(HWTestApp::Mode mode)
{
  switch (mode) {
    case HWTestApp::MODE_INFO: return "INFO";
    case HWTestApp::MODE_CORE: return "CORE";
    case HWTestApp::MODE_GFX: return "GFX";
    case HWTestApp::MODE_ADC: return "ADC";
    case HWTestApp::MODE_TRx: return "TRx";
    case HWTestApp::MODE_DEBUG: return "DEBUG";
    case HWTestApp::MODE_LAST: break;
  };
  return "?";
}

void HWTestApp::Process(uint32_t ticks, const api::Processor::Inputs &inputs,
                        api::Processor::Outputs &outputs)
{
  inputs_ = inputs;

  // Ramp
  outputs.dac[0] += 1;
  outputs.dac[1] += 2;

  // Tri
  auto value = outputs.dac[2] + dir_;
  if (value == 0xffff)
    dir_ = -1;
  else if (!value)
    dir_ = 1;
  outputs.dac[2] = value;

  // ca. 2KHz square
  auto t = ticks & 7;
  outputs.dac[3] = t < 4 ? 0xffff : 0;
}

void HWTestApp::HandleMenuEvent(api::MenuEvent menu_event)
{
  switch (menu_event) {
    case api::MENU_ACTIVATE: SystemCore::Execute(this, core_freq_); break;
    case api::MENU_DEACTIVATE: break;
  }
}

void HWTestApp::HandleEvent(const UI::Event &event)
{
  DEBUG_EVENT(HWTestApp, event);
  DispatchEvent(event);
}

static const uint8_t unit_khz_8[] = {0x00, 0x7c, 0x10, 0x68, 0x00, 0x7e, 0x08,
                                     0x08, 0x7e, 0x00, 0x48, 0x68, 0x58};

void HWTestApp::Draw(weegfx::Graphics &gfx) const
{
  gfx.setPrintPos(0, 0);
  gfx.printf("oCT4/%s", to_string(mode_));

  auto core_freq = static_cast<float>(SystemCore::current_core_freq()) / 1000.f;
  gfx.setPrintPos(128 - 13 - 24, 0);
  gfx.printf("%.1f", (double)core_freq);
  gfx.writeBitmap8(128 - 13, 0, sizeof(unit_khz_8), unit_khz_8);

  gfx.drawHLinePattern(0, 10, gfx.kWidth, 2);

  switch (mode_) {
    case MODE_INFO: DrawInfo(gfx); break;
    case MODE_CORE: DrawCore(gfx); break;
    case MODE_GFX: DrawGfx(gfx); break;
    case MODE_ADC: DrawADC(gfx); break;
    case MODE_TRx: DrawTRx(gfx); break;
    case MODE_DEBUG: DrawDebug(gfx); break;
    default: break;
  }
}

struct PrintLines {
  PrintLines(weegfx::Graphics &gfx) : gfx_{gfx} { gfx.setPrintPos(x, y); }

  weegfx::coord_t x = 0;
  weegfx::coord_t y = 16;

  void nl()
  {
    y += 8;
    gfx_.setPrintPos(x, y);
  }

  weegfx::Graphics &gfx_;
};

void HWTestApp::DrawInfo(weegfx::Graphics &gfx) const
{
  PrintLines lines{gfx};

  gfx.printf("F_CPU=%luMHz", F_CPU / 1000LU / 1000LU);
  lines.nl();

  gfx.printf("%s", strings::BUILD_VERSION);
  lines.nl();
  gfx.printf("%s", strings::BUILD_TAG);
  lines.nl();

#ifdef USB_SERIAL
  gfx.printf("USB_SERIAL");
  lines.nl();
#endif
#ifdef OCT4_ENABLE_DEBUG
  gfx.printf("OCT4_ENABLE_DEBUG");
  lines.nl();
#endif
}

void HWTestApp::DrawCore(weegfx::Graphics &gfx) const
{
  PrintLines lines{gfx};
  gfx.printf("Temp            %.1fC", (double)SystemCore::read_temperature());
  lines.nl();

  auto core_isr_cycles = debug.perf.core_isr_cycles.value();
  auto core_isr_period = debug.perf.core_isr_period.value();

  gfx.printf("C %5lu       %6.2fu", core_isr_cycles,
             (double)profiling::cycles_to_us(core_isr_cycles));
  lines.nl();

  gfx.printf("P %5lu       %6.2fu", core_isr_period,
             (double)profiling::cycles_to_us(core_isr_period));
  lines.nl();

  auto load = static_cast<float>(core_isr_cycles) / static_cast<float>(core_isr_period);
  gfx.printf("              %6.2f%%", (double)(load * 100.f));

  gfx.drawFrame(0, lines.y, 72, 8);
  gfx.drawRect(0, lines.y, static_cast<weegfx::coord_t>(load * 72.f + .5f), 8);
}

void HWTestApp::DrawGfx(weegfx::Graphics &gfx) const
{
  auto t = tick_ % 256;
  weegfx::coord_t x = t < 128 ? 128 - t : t - 128;

  gfx.invertRect(x - 8, 0, 16, gfx.kHeight);
}

void HWTestApp::DrawADC(weegfx::Graphics &gfx) const
{
  weegfx::coord_t x = 0;
  int i = 0;
  for (auto v : inputs_.analog_inputs) {
    gfx.setPrintPos(x, 16);
    gfx.printf("CV%d", i);

    gfx.setPrintPos(x, 24);
    gfx.printf("%04x", v);
    gfx.setPrintPos(x, 32);
    gfx.printf("%4u", v);

    ++i;
    x += 32;
  }
}

void HWTestApp::DrawTRx(weegfx::Graphics &gfx) const
{
  weegfx::coord_t x = 0;
  int i = 0;
  for (auto di : inputs_.digital_inputs) {
    gfx.setPrintPos(x, 16);
    gfx.printf("TR%d", i);

    gfx.setPrintPos(x, 24);
    gfx.printf("%04x", di.raw);
    gfx.setPrintPos(x, 32);
    gfx.printf("%04x", di.flags);

    ++i;
    x += 32;
  }
}

void HWTestApp::DrawDebug(weegfx::Graphics &gfx) const
{
  PrintLines lines{gfx};

#ifdef OCT4_ENABLE_DEBUG
  gfx.printf("ADC wait    %8lu", debug.drivers.adc_wait_count);
  lines.nl();
  gfx.printf("UI events   %8lu", debug.ui.event_count);
  lines.nl();
  gfx.printf("UI overflow %8lu", debug.ui.event_queue_overflow);
#else
  gfx.printf("[_] OCT4_ENABLE_DEBUG");
#endif
}

EVENT_DISPATCH_DEFAULT_DEFINE(HWTestApp){
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_UP, &HWTestApp::evButtonUp},
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_DOWN, &HWTestApp::evButtonDown},
    {UI::EVENT_BUTTON_PRESS, UI::CONTROL_BUTTON_R, &HWTestApp::evButtonR},
    {UI::EVENT_ENCODER, UI::CONTROL_ENCODER_L, &HWTestApp::evEncoderL},
    {},
};

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonUp)
{
  EVENT_DISPATCH_HANDLER_STUB();

  display_on_ = !display_on_;
  SystemCore::display.driver().CmdDisplayOn(display_on_);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonDown)
{
  EVENT_DISPATCH_HANDLER_STUB();
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evButtonR)
{
  EVENT_DISPATCH_HANDLER_STUB();

  auto freq = core_freq_ + 1;
  if (freq >= SystemCore::CORE_FREQ_LAST) freq = 0;
  core_freq_ = static_cast<SystemCore::CoreFreq>(freq);

  SystemCore::Execute(this, core_freq_);
}

EVENT_DISPATCH_DEFINE_HANDLER(HWTestApp, evEncoderL)
{
  EVENT_DISPATCH_HANDLER_STUB();

  auto m = mode_ + 1;
  if (m >= MODE_LAST) m = MODE_FIRST;
  mode_ = static_cast<Mode>(m);
}
}  // namespace oct4
