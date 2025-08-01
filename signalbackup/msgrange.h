/*
  Copyright (C) 2023-2025  Selwin van Dijk

  This file is part of signalbackup-tools.

  signalbackup-tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-tools.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MSGRANGE_H_
#define MSGRANGE_H_

// data that defines ranges in a message body to be replaced for html output
struct Range
{
  long long int start;
  long long int length;
  std::string pre;
  std::string replacement;
  std::string post;
  bool nobreak;

#if __cpp_aggregate_paren_init < 201902L
  // to allow compiling on older compilers (gcc 9, apple clang, anything pre-c++20)
  Range(long long int s, long long int l, std::string &&pr, std::string &&r, std::string &&po, bool nb)
    :
    start(s),
    length(l),
    pre(pr),
    replacement(r),
    post(po),
    nobreak(nb)
  {};
  Range(long long int s, long long int l, std::string const &pr, std::string const &r, std::string const &po, bool nb)
    :
    start(s),
    length(l),
    pre(pr),
    replacement(r),
    post(po),
    nobreak(nb)
  {};
#endif

  bool operator<(Range const &other) const
  {
    return (start < other.start) ||
      (start == other.start && start + length < other.start + other.length) ||
      (start == other.start && start + length == other.start + other.length && replacement < other.replacement);

    //return std::tie(start, start + length, replacement) <
    //  std::tie(other.start, other.start + other.length, other.replacement);

  }
};

#endif
