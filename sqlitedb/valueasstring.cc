/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

std::string SqliteDB::QueryResults::valueAsString(size_t row, size_t column) const
{  // order empirically determined
  if (valueHasType<std::nullptr_t>(row, column))
    return std::string();

  if (valueHasType<std::string>(row, column))
    return getValueAs<std::string>(row, column);

  if (valueHasType<long long int>(row, column))
    return bepaald::toString(getValueAs<long long int>(row, column));

  if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(row, column))
    return Base64::bytesToBase64String(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(row, column).first.get(),
                                       getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(row, column).second);

  if (valueHasType<unsigned int>(row, column))
    return bepaald::toString(getValueAs<unsigned int>(row, column));

  if (valueHasType<unsigned long long int>(row, column))
    return bepaald::toString(getValueAs<unsigned long long int>(row, column));

  if (valueHasType<unsigned long>(row, column))
    return bepaald::toString(getValueAs<unsigned long>(row, column));

  if (valueHasType<double>(row, column))
    return bepaald::toString(getValueAs<double>(row, column));

  else [[unlikely]]
    return "(unhandled type)";
}

std::string SqliteDB::QueryResults::valueAsString(size_t row, std::string_view header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    Logger::warning("Column `", header, "' not found in query results");
    return "(column not found)";
  }
  return valueAsString(row, i);
}
