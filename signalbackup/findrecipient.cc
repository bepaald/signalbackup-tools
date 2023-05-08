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

#include "signalbackup.ih"

bool SignalBackup::findRecipient(long long int id) const
{
  for (auto const &dbl : d_databaselinks)
  {
    if (dbl.table != "recipient")
      continue;

    for (auto const &c : dbl.connections)
    {
      if (d_database.containsTable(c.table) && d_database.tableContainsColumn(c.table, c.column))
      {
        SqliteDB::QueryResults res;
        if (!d_database.exec("SELECT COUNT(*) AS 'count' FROM " + c.table + " WHERE " + c.column + " IS ?",
                             id, &res) ||
            res.rows() != 1)
        {
          std::cout << "Error querying database." << std::endl;
          return false;
        }
        long long int count = res.getValueAs<long long int>(0, "count");
        if (count)
        {
          std::cout << "Found recipient " << id << " referenced in '"
                    << c.table << "." << c.column << "' (" << count << " times)" << std::endl;
        }
      }
    }
  }

  // now check the other thigns (uuid in group updates?)


  return false;
}
