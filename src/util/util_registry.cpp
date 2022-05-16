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

#include "util/util_registry.h"

#include "system/system_serial.h"

namespace util {

void PrintRegistryEntry(const FourCC registry_fourcc, const FourCC fourcc, const void *instance,
                        const void *factory)
{
  (void)registry_fourcc;
  (void)fourcc;
  (void)instance;
  (void)factory;

  if (fourcc.value) {
    SERIAL_DEBUG("%s { %s, %p, %p }", registry_fourcc.str().value, fourcc.str().value, instance,
                 factory);
  }
}

}  // namespace util
