/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

#include "signalbackup.ih"

long long int SignalBackup::getMinUsedId(std::string const &table)
{
  SqliteDB::QueryResults results;
  d_database.exec("SELECT MIN(_id) FROM " + table, &results);
  if (results.rows() != 1 ||
      results.columns() != 1 ||
      !results.valueHasType<long long int>(0, 0))
  {
    return 0;
  }
  return results.getValueAs<long long int>(0, 0);
}

long long int SignalBackup::getMaxUsedId(std::string const &table)
{
  SqliteDB::QueryResults results;
  d_database.exec("SELECT MAX(_id) FROM " + table, &results);
  if (results.rows() != 1 ||
      results.columns() != 1 ||
      !results.valueHasType<long long int>(0, 0))
  {
    return 0;
  }
  return results.getValueAs<long long int>(0, 0);
}
