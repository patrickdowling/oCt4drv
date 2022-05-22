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

#ifndef HWTEST_ADC_H_
#define HWTEST_ADC_H_

namespace oct4 {

class PluginADC : public HWTestPlugin {
public:
  PluginADC() : HWTestPlugin{1} {}

  void Draw(weegfx::Graphics &gfx) const final
  {
    DrawTitlebar(gfx, "ADC");

    weegfx::coord_t x = 0;
    int i = 0;
    for (auto v : last_inputs_.analog_inputs) {
      gfx.setPrintPos(x, 16);
      gfx.printf("CV%d", i);

      gfx.setPrintPos(x, 24);
      gfx.printf("%04x", v);
      gfx.setPrintPos(x, 32);
      gfx.printf("%4u", v);

      ++i;
      x += 32;
    }
  }
};

}  // namespace oct4

#endif  // HWTEST_ADC_H_
