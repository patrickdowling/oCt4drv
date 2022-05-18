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

#include "resources/oct4_strings.h"
#include "system/oct4_debug.h"
#include "system/oct4_system_core.h"

// Main entry point. Welcome to Arduino Land!
//
// All/most of the actual board and driver details are hidden away.

FLASHMEM void setup()
{
  SYSTEM_SERIAL_INIT();
  DEBUG_CRASH_REPORT();

  using namespace oct4;
  SERIAL_DEBUG("%s %s %s", strings::NAME, strings::BUILD_VERSION, strings::BUILD_TAG);
  SERIAL_DEBUG("Initializing...");

  api::Processor::PrintRegistry();
  api::Menu::PrintRegistry();

  SystemCore::Init();
  SystemCore::StartInterrupts(SystemCore::CORE_FREQ_OC16Khz);

  // This is a crutch. There should porbably be an "AppManager" or an "ActivityManager" or
  // Patch/State/Window/workbench/Thingy or even a framework.
  SystemCore::Execute("BOOT"_4CC);
}

FASTRUN void loop()
{
  oct4::SystemCore::MainLoop();

  // Not actually necessary, but limits redraw until there's a replacement for MENU_REDRAW?
  delay(1);
}
