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

#ifndef SYSTEM_PROFILING_H_
#define SYSTEM_PROFILING_H_

#include <cstdint>
#include <limits>

#include "system/t4.h"

namespace profiling {

class CycleCount {
public:
  uint32_t elapsed() const { return ARM_DWT_CYCCNT - start_; }

  uint32_t elapsed_restart()
  {
    auto now = ARM_DWT_CYCCNT;
    auto d = now - start_;
    start_ = ARM_DWT_CYCCNT;
    return d;
  }

  static void Init()
  {
    ARM_DEMCR |= ARM_DEMCR_TRCENA;
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
  }

private:
  uint32_t start_ = ARM_DWT_CYCCNT;
};

// We live in the future, so we have floats
static inline float cycles_to_us(uint32_t cycles)
{
  static constexpr float div = 1.f / (static_cast<float>(F_CPU) / 1000000.f);
  return static_cast<float>(cycles) * div;
}

#if 0
static inline uint32_t cycles_to_us(uint32_t cycles)
{
  return multiply_u32xu32_rshift32(cycles, (1ULL << 32) / (F_CPU / 1000000));
}
#endif

struct AverageCycleCount {
  static constexpr uint32_t kSmoothing = 32;

  AverageCycleCount() : value_(0), min_(0xffffffff), max_(0) {}

  uint32_t value_ = 0;
  uint32_t min_ = std::numeric_limits<uint32_t>::max();
  uint32_t max_ = std::numeric_limits<uint32_t>::min();

  uint32_t value() const { return value_; }

  uint32_t min_value() const { return min_; }

  uint32_t max_value() const { return max_; }

  void Reset()
  {
    min_ = 0xffffffff;
    max_ = 0;
  }

  void push(uint32_t value)
  {
    if (value < min_) min_ = value;
    if (value > max_) max_ = value;
    value_ = (value_ * (kSmoothing - 1) + value) / kSmoothing;
  }
};

class ScopedCycleCount {
public:
  explicit ScopedCycleCount(AverageCycleCount &dest) : dest_(dest), cycles_() {}
  ~ScopedCycleCount() { dest_.push(cycles_.elapsed()); }

private:
  AverageCycleCount &dest_;
  CycleCount cycles_;
};

};  // namespace profiling

#endif  // SYSTEM_PROFILING_H_
