/*
  Copyright (C) 2023  Selwin van Dijk

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

long long int SqliteDB::QueryResults::valueAsInt(size_t row, size_t column) const
{
  if (valueHasType<std::string>(row, column))
    return bepaald::toNumber<long long int>(getValueAs<std::string>(row, column));

  if (valueHasType<unsigned int>(row, column))
    return getValueAs<unsigned int>(row, column);

  if (valueHasType<unsigned long long int>(row, column))
    return getValueAs<unsigned long long int>(row, column);

  if (valueHasType<unsigned long>(row, column))
    return getValueAs<unsigned long>(row, column);

  if (valueHasType<long long int>(row, column))
    return getValueAs<long long int>(row, column);

  if (valueHasType<double>(row, column))
    return -1;

  if (valueHasType<std::nullptr_t>(row, column))
    return -1;

  if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(row, column))
    return -1;

  else [[unlikely]]
    return -1;
}

long long int SqliteDB::QueryResults::valueAsInt(size_t row, std::string const &header) const
{
  int i = idxOfHeader(header);
  if (i > -1) [[likely]]
    return valueAsInt(row, i);
  return -1;
}
