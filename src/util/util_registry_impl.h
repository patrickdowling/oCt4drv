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

#ifndef UTIL_REGISTRY_IMPL_H_
#define UTIL_REGISTRY_IMPL_H_

template <typename TBase, size_t num_entries>
typename StaticTypeRegistry<TBase, num_entries>::Registry &
StaticTypeRegistry<TBase, num_entries>::get_registry()
{
  static typename StaticTypeRegistry<TBase, num_entries>::Registry registry;
  return registry;
}

template <typename TBase, size_t num_entries>
void StaticTypeRegistry<TBase, num_entries>::Register(
    const FourCC fourcc, TBase *instance,
    typename StaticTypeRegistry<TBase, num_entries>::FactoryFn factory_fn)
{
  auto &registry = get_registry();
  auto e = std::find_if(registry.begin(), registry.end(),
                        [](const auto &entry) { return !entry.fourcc; });
  if (e != registry.end()) {
    e->fourcc = fourcc.value;
    e->instance = instance;
    e->factory = factory_fn;
  }
}

template <typename TBase, size_t num_entries>
TBase *StaticTypeRegistry<TBase, num_entries>::GetInstance(const FourCC fourcc)
{
  const auto &registry = get_registry();
  auto e = std::find_if(registry.begin(), registry.end(),
                        [fourcc](const auto &entry) { return fourcc == entry.fourcc; });

  return e != registry.end() ? e->instance : nullptr;
}

template <typename TBase, size_t num_entries>
void StaticTypeRegistry<TBase, num_entries>::PrintRegistry()
{
  const auto &registry = get_registry();
  for (auto &entry : registry)
    PrintRegistryEntry({TBase::registry_fourcc}, {entry.fourcc}, entry.instance,
                       (const void *)entry.factory);
}

#endif  // UTIL_REGISTRY_IMPL_H_
