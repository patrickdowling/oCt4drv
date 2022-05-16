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

#ifndef NULLMENU_H_
#define NULLMENU_H_

#include "api/api_menu.h"

namespace oct4 {

class NullMenu : public api::Menu {
public:
  static constexpr util::FourCC::value_type fourcc = "NULL"_4CCV;

  // MENU
  util::FourCC menu_type() const final { return {fourcc}; }
  void HandleMenuEvent(api::MenuEvent) final{};
  void Tick() final{};
  void HandleEvent(const UI::Event &) final {}
  void Draw(weegfx::Graphics &) const final {}
};

}  // namespace oct4

#endif  // NULLMENU_H_
