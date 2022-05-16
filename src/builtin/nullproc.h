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

#ifndef BUILTIN_NULLPROC_H_
#define BUILTIN_NULLPROC_H_

#include "api/api_processor.h"

namespace oct4 {

class NullProcessor : public api::Processor {
public:
  static constexpr util::FourCC::value_type fourcc = "NULL"_4CCV;

  util::FourCC processor_type() const final { return {fourcc}; }
  void Process(uint32_t ticks, const api::Processor::Inputs &inputs,
               api::Processor::Outputs &outputs) final;
};

}  // namespace oct4

#endif  // BUILTIN_NULLPROC_H_
