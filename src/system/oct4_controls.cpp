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

#include "oct4_controls.h"

#include "drivers/oct4_gpio.h"

// TODO This init and poll scenario isn't great

namespace oct4 {
using drivers::GPIO;

void Controls::Init()
{
  GPIO::SW_UP::Init();
  GPIO::SW_DOWN::Init();
  GPIO::SW_ENCL::Init();
  GPIO::SW_ENCR::Init();
  GPIO::ENCL_A::Init();
  GPIO::ENCL_B::Init();
  GPIO::ENCR_A::Init();
  GPIO::ENCR_B::Init();
}

void Controls::Poll()
{
  switches[SWITCH_UP].Poll<GPIO::SW_UP>();
  switches[SWITCH_DOWN].Poll<GPIO::SW_DOWN>();
  switches[SWITCH_ENCODER_L].Poll<GPIO::SW_ENCL>();
  switches[SWITCH_ENCODER_R].Poll<GPIO::SW_ENCR>();

  encoders[0].Poll<GPIO::ENCL_A, GPIO::ENCL_B>();
  encoders[1].Poll<GPIO::ENCR_A, GPIO::ENCR_B>();
}

}  // namespace oct4
