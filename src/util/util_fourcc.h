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

#ifndef UTIL_FOURCC_H_
#define UTIL_FOURCC_H_

#include <cstddef>
#include <cstdint>

namespace util {

// FourCC are often useful as template or constexpr types so they are mostly interhangebale with
// uint32_t. Definitely needs work.

struct FourCCString {
  explicit constexpr FourCCString(uint32_t fourcc)
      : value{static_cast<char>(fourcc), static_cast<char>(fourcc >> 8),
              static_cast<char>(fourcc >> 16), static_cast<char>(fourcc >> 24), 0}
  {}

  const char value[5] = {0};
};

struct FourCC {
  using value_type = uint32_t;

  const value_type value = 0;

  bool operator!() const { return !value; }

  template <size_t N>
  static constexpr FourCC FromString(const char (&s)[N])
  {
    static_assert(N == 5, "4 chars + '\0'");
    uint32_t fourcc = static_cast<uint32_t>(s[0]) | (static_cast<uint32_t>(s[1]) << 8) |
                      (static_cast<uint32_t>(s[2]) << 16) | (static_cast<uint32_t>(s[3]) << 24);
    return {fourcc};
  }

  static constexpr FourCC FromString(const char *s, size_t len)
  {
    uint32_t fourcc = len ? static_cast<uint32_t>(s[0]) : 0;
    if (len > 1) fourcc |= static_cast<uint32_t>(s[1]) << 8;
    if (len > 2) fourcc |= static_cast<uint32_t>(s[2]) << 16;
    if (len > 3) fourcc |= static_cast<uint32_t>(s[3]) << 24;
    return {fourcc};
  }

  constexpr FourCCString str() const { return FourCCString{value}; }
};

static constexpr bool operator==(const FourCC &lhs, const FourCC &rhs)
{
  return lhs.value == rhs.value;
}

static constexpr bool operator==(const FourCC &lhs, const FourCC::value_type rhs)
{
  return lhs.value == rhs;
}

}  // namespace util

constexpr util::FourCC::value_type operator""_4CCV(const char *s, size_t len)
{
  return util::FourCC::FromString(s, len).value;
}

constexpr util::FourCC operator"" _4CC(const char *s, size_t len)
{
  return util::FourCC::FromString(s, len);
}
#endif  // UTIL_FOURCC_H_
