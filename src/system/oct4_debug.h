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

#ifndef OCT4_DEBUG_H_
#define OCT4_DEBUG_H_

#include <cstdint>

#include "system/system_profiling.h"
#include "system/system_serial.h"

namespace oct4 {

struct Debug {
  struct {
    uint32_t event_queue_overflow = 0;
    uint32_t event_count = 0;
  } ui;

  struct {
    uint32_t adc_wait_count = 0;
  } drivers;

  struct {
    profiling::AverageCycleCount core_isr_cycles;
    profiling::AverageCycleCount core_isr_period;
  } perf;
};

extern Debug debug;

}  // namespace oct4

#define DEBUG_CRASH_REPORT()       \
  do                               \
    if (CrashReport) {             \
      Serial.println(CrashReport); \
      CrashReport.clear();         \
    }                              \
  while (0)

#define DEBUG_BUFFER(name, p, sz) \
  SERIAL_DEBUG("[%p - %p] " name, (const void *)p, (const void *)((const uint8_t *)p + sz))

#ifdef OCT4_ENABLE_DEBUG
#define OCT4_DEBUG_INC(x) \
  do {                    \
    ++x;                  \
  } while (0)
#else
#define OCT4_DEBUG_INC(x) \
  do {                    \
  } while (0)
#endif

#ifdef OCT4_SERIAL_WAIT
#define SYSTEM_SERIAL_INIT() serial::wait_available(OCT4_SERIAL_WAIT)
#else
#define SYSTEM_SERIAL_INIT() \
  do {                       \
  } while (0)
#endif

#endif  // OCT4_DEBUG_H_
