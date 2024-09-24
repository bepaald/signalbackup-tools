/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#endif

bool SignalBackup::insertRow(std::string const &table, std::vector<std::pair<std::string, std::any>> data,
                             std::string const &returnfield, std::any *returnvalue) const
{
  // check if columns exist...
  for (auto it = data.begin(); it != data.end();)
  {
    if (it->first.empty())
      it = data.erase(it);
    else if (!d_database.tableContainsColumn(table, it->first))
    {
      Logger::warning("Table '", table, "' does not contain any column '", it->first, "'. Removing");
      it = data.erase(it);
    }
    else
      ++it;
  }

  std::string query = "INSERT INTO " + table + " (";
  for (unsigned int i = 0; i < data.size(); ++i)
    query += data[i].first + (i < data.size() -1 ? ", " : ") ");
  query += "VALUES (";
  for (unsigned int i = 0; i < data.size(); ++i)
    query += "?"s + (i < data.size() -1 ? ", " : ")");
#if SQLITE_VERSION_NUMBER >= 3035000 // RETURNING was not available prior to 3.35.0
  if (!returnfield.empty() && returnvalue)
    query += " RETURNING " + returnfield;
#endif

  SqliteDB::QueryResults res;
#if __cpp_lib_ranges >= 201911L
  bool ret = d_database.exec(query, std::views::values(data), &res, d_verbose);
#else
  std::vector<std::any> values;
  std::transform(data.begin(), data.end(), std::back_inserter(values), [](auto const &pair){ return pair.second; });
  bool ret = d_database.exec(query, values, &res, d_verbose);
#endif

#if SQLITE_VERSION_NUMBER < 3035000 // RETURNING was not available prior to 3.35.0
  if (ret && !returnfield.empty() && returnvalue)
  {
    long long int lastid = d_database.lastId();
    ret = d_database.exec("SELECT " + returnfield + " FROM " + table + " WHERE rowid = ?", lastid, &res, d_verbose);
  }
#endif

  if (ret && !returnfield.empty() && returnvalue && res.rows() && res.columns())
  {
    if (res.rows() > 1 || res.columns() > 1)
      Logger::warning("Requested return of '", returnfield, "', "
                      "but query returned multiple results. Returning first.");
    *returnvalue = res.value(0, 0);
  }

  if (d_verbose) [[unlikely]]
    Logger::message("Inserted new row into table '", table, "'. New _id: ", d_database.lastId());

  return ret;
}
