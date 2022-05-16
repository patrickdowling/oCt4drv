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

#ifndef OCT4_CONTROLS_H_
#define OCT4_CONTROLS_H_

#include "util/util_encoder.h"
#include "util/util_macros.h"
#include "util/util_switchstate.h"

namespace oct4 {

// TODO Better solution for these mappings
// We have to different things
// - Physical switches
// - Control IDs, which for o_C switches an be used as bits in a mask

enum SwitchID : unsigned {
  SWITCH_UP,
  SWITCH_DOWN,
  SWITCH_ENCODER_L,
  SWITCH_ENCODER_R,
  SWITCH_LAST
};

enum EncoderID : unsigned { ENCODER_L, ENCODER_R, ENCODER_LAST };

class Controls {
public:
  void Init();
  void Poll();

  util::SwitchState switches[SWITCH_LAST];
  util::Encoder encoders[ENCODER_LAST];
};

}  // namespace oct4

#endif  // OCT4_CONTROLS_H_
