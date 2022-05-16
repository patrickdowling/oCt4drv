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

#ifndef SYSTEM_SERIAL_H_
#define SYSTEM_SERIAL_H_

#include <cstdint>

namespace serial {

void println(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void wait_available(uint32_t ms);

}  // namespace serial

// TODO gnu specific __VA_ARGS__

#ifdef ENABLE_SERIAL_PRINTLN
#define SERIAL_PRINTLN(msg, ...)              \
  do {                                        \
    serial::println(msg "\n", ##__VA_ARGS__); \
  } while (0)

#define SERIAL_ERROR(msg, ...)                                         \
  do {                                                                 \
    serial::println("%s: ERR " msg "\n", __FUNCTION__, ##__VA_ARGS__); \
  } while (0)
#else
#define SERIAL_PRINTLN(...) \
  do {                      \
  } while (0)

#define SERIAL_ERROR(...) \
  do {                    \
  } while (0)
#endif

#ifdef ENABLE_SERIAL_DEBUG
#define SERIAL_DEBUG(msg, ...)                                     \
  do {                                                             \
    serial::println("%s: " msg "\n", __FUNCTION__, ##__VA_ARGS__); \
  } while (0)
#else
#define SERIAL_DEBUG(...) \
  do {                    \
  } while (0)
#endif

#endif  // SYSTEM_SERIAL_H_
