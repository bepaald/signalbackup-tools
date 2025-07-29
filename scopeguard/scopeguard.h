/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

// a _very_ simple (limited) scope guard

template <typename F>
class ScopeGuard
{
  F d_function;
 public:
  explicit ScopeGuard(F &&fn)
    :
    d_function(std::forward<F>(fn))
  {}

  ~ScopeGuard()
  {
    d_function();
  }
};

#endif
