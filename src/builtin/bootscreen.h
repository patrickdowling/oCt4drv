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

#ifndef BUILTIN_BOOTSCREEN_H_
#define BUILTIN_BOOTSCREEN_H_

#include "api/api_menu.h"

namespace oct4 {

class Bootscreen : public api::Menu {
public:
  static constexpr util::FourCC::value_type fourcc = "BOOT"_4CCV;

  static constexpr uint32_t kDisplayOnTimeoutMs = 100;
  static constexpr uint32_t kBootscreenTimeoutMs = 128 * 8;  // ca. 1s assuming 1ms updates

  // MENU
  util::FourCC menu_type() const final { return {fourcc}; }
  void HandleMenuEvent(api::MenuEvent menu_event) final;
  void Tick() final;
  void HandleEvent(const UI::Event &event) final;
  void Draw(weegfx::Graphics &gfx) const final;

private:
  uint32_t ticks_ = 0;
  int state_ = 0;
};

}  // namespace oct4

#endif  // BUILTIN_BOOTSCREEN_H_
