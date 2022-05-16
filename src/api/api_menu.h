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

#ifndef OCF4_UI_MENU_H_
#define OCF4_UI_MENU_H_

#include "system/ui/ui_event.h"
#include "util/util_fourcc.h"
#include "util/util_registry.h"
#include "weegfx/weegfx.h"

namespace oct4 {
namespace api {

class Debuggable {
public:
  virtual ~Debuggable() {}
  // virtual void DebugView(Display::Frame &frame) const = 0;
};

enum MenuEvent { MENU_DEACTIVATE, MENU_ACTIVATE };

class Menu : public util::StaticTypeRegistry<Menu, 32> {
public:
  static constexpr util::FourCC::value_type registry_fourcc = "MENU"_4CCV;

  virtual ~Menu() {}

  virtual util::FourCC menu_type() const = 0;
  virtual void HandleMenuEvent(MenuEvent) = 0;
  virtual void Tick() = 0;
  virtual void HandleEvent(const UI::Event &) = 0;
  virtual void Draw(weegfx::Graphics &gfx) const = 0;
  // virtual void SerialCommand(uint8_t c) = 0;

  template <typename T>
  using MenuRegistrar = util::StaticTypeRegistry<Menu, 32>::Registrar<T>;
};

}  // namespace api
}  // namespace oct4

#endif  // OCF4_UI_MENU_H_
