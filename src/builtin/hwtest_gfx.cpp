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

#include "builtin/hwtest_gfx.h"

#include "system/system_profiling.h"
#include "util/util_templates.h"

namespace oct4 {

// Numerical recipes. "can use as random: not recommended" :p
static uint32_t lcg32()
{
  static uint32_t state_ = 12345U;
  auto r = state_ * 1372383749U + 1289706010U;
  state_ = r;
  return r;
}

static void GfxTestRect(weegfx::Graphics &gfx, uint32_t tick)
{
  static constexpr unsigned kNumRects = 32;
  static constexpr weegfx::coord_t kRectSize = 32;

  weegfx::coord_t x = tick % 128;
  weegfx::coord_t y = (tick >> 8) % 64;

  unsigned n = kNumRects;
  while (n--) {
    gfx.invertRect(-16 + x, -16 + y, kRectSize, kRectSize);

    auto rnd = lcg32();
    x = (x + (rnd & 0xff)) % 128;
    y = (y + ((rnd & 0xff00) >> 8)) % 64;
  }
}

static void GfxTestFill(weegfx::Graphics &gfx, uint32_t)
{
  gfx.drawRect(0, 0, gfx.kWidth, gfx.kHeight);
}

// NOTE constexpr sin is gnu specific (until > c++20?)
static constexpr float pi = 3.14159265358979323846f;
static constexpr size_t kSinTableSize = 64;

template <weegfx::coord_t amplitude, size_t index, size_t N>
constexpr weegfx::coord_t calc_sin()
{
  return static_cast<weegfx::coord_t>(
      static_cast<float>(amplitude) *
      sinf(2.f * pi * static_cast<float>(index) / static_cast<float>(N)));
}

// C++17 fold expressions would be way simpler
template <weegfx::coord_t amplitude, size_t... Is>
constexpr std::array<weegfx::coord_t, sizeof...(Is)> generate_sin_table(util::index_sequence<Is...>)
{
  return {calc_sin<amplitude, Is, sizeof...(Is)>()...};
}

static constexpr auto sin_table =
    generate_sin_table<16>(util::make_index_sequence<kSinTableSize>::type());

static void GfxTestHLine(weegfx::Graphics &gfx, uint32_t ticks)
{
  auto i = ticks >> 2;

  for (weegfx::coord_t y = 0; y < gfx.kHeight; ++y, ++i) {
    gfx.drawHLine(16 + sin_table[i % kSinTableSize], y, 128 - 32);
  }
}

static void GfxTestVLine(weegfx::Graphics &gfx, uint32_t ticks)
{
  auto i = ticks >> 1;

  for (weegfx::coord_t x = 0; x < gfx.kWidth; ++x, ++i) {
    gfx.drawVLine(x, 8 + sin_table[i % kSinTableSize] / 2, 48);
  }
}

static constexpr struct {
  const char *const name;
  void (*test_fn)(weegfx::Graphics &, uint32_t);
} gfx_test_modes[] = {
    {"RECT", GfxTestRect},
    {"HLIN", GfxTestHLine},
    {"VLIN", GfxTestVLine},
    {"FILL", GfxTestFill},
};

static constexpr int kNumTestModes = sizeof(gfx_test_modes) / sizeof(gfx_test_modes[0]);
static profiling::AverageCycleCount cycle_count_;

PluginGfx::PluginGfx() : HWTestPlugin{kNumTestModes} {}

void PluginGfx::Tick()
{
  ++tick_;
}

void PluginGfx::Draw(weegfx::Graphics &gfx) const
{
  auto mode = gfx_test_modes[current_page_];

  {
    profiling::ScopedCycleCount cc{cycle_count_};
    mode.test_fn(gfx, tick_);
  }

  gfx.clearRect(0, 56, 10 * weegfx::kFixedFontW, 8);
  gfx.setPrintPos(0, 56);
  gfx.printf("%s %3uus", mode.name, (unsigned)profiling::cycles_to_us(cycle_count_.value()));
}

}  // namespace oct4
