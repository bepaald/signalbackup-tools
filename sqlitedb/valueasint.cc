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

long long int SqliteDB::QueryResults::valueAsInt(size_t row, size_t column, long long int def) const
{
  if (valueHasType<std::string>(row, column))
    return bepaald::toNumber<long long int>(getValueAs<std::string>(row, column), def);

  if (valueHasType<unsigned int>(row, column))
    return getValueAs<unsigned int>(row, column);

  if (valueHasType<unsigned long long int>(row, column))
    return getValueAs<unsigned long long int>(row, column);

  if (valueHasType<unsigned long>(row, column))
    return getValueAs<unsigned long>(row, column);

  if (valueHasType<long long int>(row, column))
    return getValueAs<long long int>(row, column);

  if (valueHasType<double>(row, column))
    return def;

  if (valueHasType<std::nullptr_t>(row, column))
    return def;

  if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(row, column))
    return def;

  else [[unlikely]]
    return def;
}

long long int SqliteDB::QueryResults::valueAsInt(size_t row, std::string const &header, long long int def) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
              << ": Column `" << header << "' not found in query results" << std::endl;
    return def;
  }
  return valueAsInt(row, i, def);
}
