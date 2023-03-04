/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

#if __cpp_lib_ranges >= 201911L && !defined(__clang__) // ranges does not seem to work woith clang ATM
#include <ranges>
#endif

bool SignalBackup::insertRow(std::string const &table, std::vector<std::pair<std::string, std::any>> data,
                             std::string const &returnfield, std::any *returnvalue) const
{
  using namespace std::string_literals;

  // check if columns exist...
  for (auto it = data.begin(); it != data.end();)
  {
    if (!d_database.tableContainsColumn(table, it->first))
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                << ": Table '" << table << "' does not contain any column '" << it->first
                << "'. Removing"  << std::endl;
      it = data.erase(it);
    }
    else
      ++it;
  }

  std::string query = "INSERT INTO " + table + "(";
  for (uint i = 0; i < data.size(); ++i)
    query += data[i].first + (i < data.size() -1 ? ", " : ") ");
  query += "VALUES (";
  for (uint i = 0; i < data.size(); ++i)
    query += "?"s + (i < data.size() -1 ? ", " : ")");
  if (!returnfield.empty() && returnvalue)
    query += " RETURNING " + returnfield;


  SqliteDB::QueryResults res;
#if __cpp_lib_ranges >= 201911L && !defined(__clang__)
  bool ret = d_database.exec(query, std::views::values(data), &res);
#else
  std::vector<std::any> values;
  std::transform(data.begin(), data.end(), std::back_inserter(values), [](auto const &pair){ return pair.second; });
  bool ret = d_database.exec(query, values, &res);
#endif
  if (ret && !returnfield.empty() && returnvalue && res.rows() && res.columns())
  {
    if (res.rows() > 1 || res.columns() > 1)
    {
      std::cout << "WARNING: Requested return of '" << returnfield
                << "', but query returned multiple results. Returning first." << std::endl;
    }
    *returnvalue = res.value(0, 0);
  }
  return ret;
}
