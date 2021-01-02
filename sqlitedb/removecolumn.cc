/*
    Copyright (C) 2020-2021  Selwin van Dijk

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

#include "sqlitedb.ih"

bool SqliteDB::QueryResults::removeColumn(uint idx)
{
  if (idx >= d_headers.size())
    return false;
  for (auto const &v : d_values)
    if (idx >= v.size())
      return false;

  d_headers.erase(d_headers.begin() + idx);
  for (auto &v : d_values)
    v.erase(v.begin() + idx);

  return true;
}
