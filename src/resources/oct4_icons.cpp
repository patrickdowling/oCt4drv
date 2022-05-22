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

#include "resources/oct4_icons.h"

namespace oct4 {
namespace icons {

static const uint8_t icon_unit_khz[] = {0x00, 0x7c, 0x10, 0x68, 0x00, 0x7e, 0x08,
                                        0x08, 0x7e, 0x00, 0x48, 0x68, 0x58};

const Icon8 unit_khz = {sizeof(icon_unit_khz), icon_unit_khz};

}  // namespace icons
}  // namespace oct4
