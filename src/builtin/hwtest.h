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

#ifndef BUILTIN_HWTEST_H_
#define BUILTIN_HWTEST_H_

#include "api/api_menu.h"
#include "api/api_processor.h"
#include "system/ui/ui_event_dispatcher.h"

namespace oct4 {

class HWTestApp : public api::Processor, public api::Menu, public UI::EventDispatch<HWTestApp> {
public:
  static constexpr util::FourCC::value_type fourcc = "TEST"_4CCV;

  // PROCESSOR
  util::FourCC processor_type() const final { return {fourcc}; }
  void Process(uint32_t ticks, const api::Processor::Inputs &inputs,
               api::Processor::Outputs &outputs) final;

  // MENU
  util::FourCC menu_type() const final { return {fourcc}; }
  void HandleMenuEvent(api::MenuEvent menu_event) final;
  void Tick() final { ++tick_; };
  void HandleEvent(const UI::Event &event) final;
  void Draw(weegfx::Graphics &gfx) const final;

  enum Mode {
    MODE_INFO,
    MODE_CORE,
    MODE_ADC,
    MODE_TRx,
    MODE_DEBUG,
    MODE_LAST,
    MODE_FIRST = MODE_INFO
  };

protected:
  Mode mode_ = MODE_FIRST;
  uint32_t tick_ = 0;

  int dir_ = 1;

  void DrawInfo(weegfx::Graphics &gfx) const;
  void DrawCore(weegfx::Graphics &gfx) const;
  void DrawADC(weegfx::Graphics &gfx) const;
  void DrawTRx(weegfx::Graphics &gfx) const;
  void DrawDebug(weegfx::Graphics &gfx) const;

  EVENT_DISPATCH_DEFAULT();
  EVENT_DISPATCH_DECLARE_HANDLER(evDisplayOn);
  EVENT_DISPATCH_DECLARE_HANDLER(evDisplayOff);
  EVENT_DISPATCH_DECLARE_HANDLER(evEncoderL);

  Processor::Inputs inputs_;
};

}  // namespace oct4

#endif  // BUILTIN_HWTEST_H_
