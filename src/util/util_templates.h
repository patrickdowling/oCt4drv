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

#ifndef UTIL_TEMPLATES_H_
#define UTIL_TEMPLATES_H_

#include <array>
#include <tuple>
#include <type_traits>

namespace util {

// C++11, >= 14 have their own
template <size_t... Is>
struct index_sequence {
  template <size_t N>
  using append = index_sequence<Is..., N>;
};

template <size_t size>
struct make_index_sequence {
  using type = typename make_index_sequence<size - 1>::type::template append<size - 1>;
};

template <>
struct make_index_sequence<0U> {
  using type = index_sequence<>;
};

template <typename T, T...>
struct sum : std::integral_constant<T, 0> {};

template <typename T, T S, T... Ss>
struct sum<T, S, Ss...> : std::integral_constant<T, S + sum<T, Ss...>::value> {};

// >= C++17 has it's own
template <typename T>
constexpr const T &clamp(const T &value, const T &lo, const T &hi)
{
  return value < lo ? lo : hi < value ? hi : value;
}

// This is somewhat specific to get the addresses of all elements in the tuple.
// THe desired type of pointer means we can convert to a base class.
template <typename T, typename Tuple, size_t... Is>
std::array<T, std::tuple_size<Tuple>::value> tuple_to_ptr_array(Tuple &tuple,
                                                                util::index_sequence<Is...>)
{
  return std::array<T, std::tuple_size<Tuple>::value>{&std::get<Is>(tuple)...};
}

template <typename T, typename Tuple>
std::array<T, std::tuple_size<Tuple>::value> tuple_to_ptr_array(Tuple &tuple)
{
  return tuple_to_ptr_array<T>(
      tuple, typename util::make_index_sequence<std::tuple_size<Tuple>::value>::type());
}

}  // namespace util

#endif  // UTIL_TEMPLATES_H_
