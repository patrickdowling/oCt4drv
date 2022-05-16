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

#ifndef UTIL_TIMER_H_
#define UTIL_TIMER_H_

namespace util {

class Timer {
public:
  int32_t id() const { return id_; }
  bool enabled() const { return timeout_ > 0; }
  bool elapsed() const { return elapsed_; }

  void Arm(int32_t id, uint32_t start, uint32_t timeout)
  {
    id_ = id;
    start_ = start;
    timeout_ = timeout;
    elapsed_ = false;
  }

  void Update(uint32_t now)
  {
    uint32_t timeout = timeout_;
    if (timeout) {
      if (now - start_ > timeout) {
        elapsed_ = true;
        timeout_ = 0;
      }
    }
  }

  void Reset()
  {
    id_ = 0;
    timeout_ = 0;
    elapsed_ = false;
  }

private:
  int32_t id_ = 0;
  uint32_t start_ = 0;
  uint32_t timeout_ = 0;
  bool elapsed_ = false;
};

}  // namespace util

#endif  // UTIL_TIMER_H_
