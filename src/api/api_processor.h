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

#ifndef API_PROCESSOR_H_
#define API_PROCESSOR_H_

#include <array>
#include <cstdint>

#include "system/drivers/dac8565.h"
#include "system/drivers/oct4_analog_inputs.h"
#include "system/drivers/oct4_digital_inputs.h"
#include "util/util_registry.h"

// This is base abstraction for "processing", i.e. transforming inputs into outputs.
// At this level it just uses the "raw" values so it's probably even just a precursor to what an
// "app" might look like.

// TODO Where to declare num channels and array types? Including drivers seems inelegant.

namespace oct4 {
namespace api {

class Processor : public util::StaticTypeRegistry<Processor, 32> {
public:
  static constexpr util::FourCC::value_type registry_fourcc = "PROC"_4CCV;

  struct Inputs {
    drivers::AnalogInputs::ValueType analog_inputs;
    drivers::DigitalInputs::ValueType digital_inputs;
  };

  struct Outputs {
    drivers::DAC8565::ValueType dac;
  };

  virtual ~Processor() {}
  virtual util::FourCC processor_type() const = 0;
  virtual void Process(uint32_t ticks, const Inputs &inputs, Outputs &output) = 0;

  template <typename T>
  using ProcRegistrar = util::StaticTypeRegistry<Processor, 32>::Registrar<T>;
};

}  // namespace api
}  // namespace oct4

#endif  // API_PROCESSOR_H_
