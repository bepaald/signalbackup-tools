/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

bool SignalBackup::dropBadFrames()
{
  if (d_badattachments.empty())
    return true;

  Logger::message("Removing ", d_badattachments.size(), " bad frames from database...");
  for (auto it = d_badattachments.begin(); it != d_badattachments.end(); )
  {
    uint32_t rowid = it->first;

    SqliteDB::QueryResults results;
    std::string query = "SELECT " + d_part_mid + " FROM " + d_part_table
      + " WHERE _id = " + bepaald::toString(rowid);
    if (d_database.tableContainsColumn(d_part_table, "unique_id"))
      query += " AND unique_id = " + bepaald::toString(it->second);

    long long int mid = -1;
    d_database.exec(query, &results);
    for (uint i = 0; i < results.rows(); ++i)
      for (uint j = 0; j < results.columns(); ++j)
        if (results.valueHasType<long long int>(i, j))
          if (results.header(j) == d_part_mid)
          {
            mid = results.getValueAs<long long int>(i, j);
            break;
          }

    if (mid == -1)
    {
      Logger::error("Failed to remove frame :( Could not find matching 'part' entry");
      return false;
    }

    d_database.exec("DELETE FROM " + d_part_table + " WHERE " + d_part_mid + " = " + bepaald::toString(mid));
    d_badattachments.erase(it);
  }

  return true;
}
