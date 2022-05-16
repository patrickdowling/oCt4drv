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

#include "builtin/nullproc.h"

namespace oct4 {

static NullProcessor instance;
static api::Processor::ProcRegistrar<NullProcessor> register_proc(&instance);

void NullProcessor::Process(uint32_t ticks, const api::Processor::Inputs &inputs, api::Processor::Outputs &outputs) 
{
  (void)ticks;
  (void)inputs;
  (void)outputs;
}

}  // namespace oct4
