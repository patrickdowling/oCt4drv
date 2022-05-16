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

#ifndef UTIL_REGISTRY_H_
#define UTIL_REGISTRY_H_

#include <algorithm>
#include <array>
#include <cstddef>

#include "util/util_fourcc.h"

namespace util {

template <typename TBase, size_t num_entries>
class StaticTypeRegistry {
public:
  static constexpr size_t kNumEntries = num_entries;

  using FactoryFn = TBase *(*)();

  static void Register(const util::FourCC fourcc, TBase *instance, FactoryFn factory_fn);
  static TBase *GetInstance(const util::FourCC fourcc);
  static void PrintRegistry();

  template <typename T>
  struct Registrar {
    explicit Registrar(T *instance) { Register({T::fourcc}, instance, nullptr); }
  };

private:
  struct Entry {
    util::FourCC::value_type fourcc = 0;
    TBase *instance = nullptr;
    FactoryFn factory = nullptr;
  };
  using Registry = std::array<Entry, kNumEntries>;

  static Registry &get_registry();
};

void PrintRegistryEntry(const FourCC registry_fourcc, const FourCC fourcc, const void *instance,
                        const void *factory);

#include "util_registry_impl.h"

}  // namespace util

#endif  // UTIL_REGISTRY_H_
