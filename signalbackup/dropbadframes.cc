/*
  Copyright (C) 2019-2022  Selwin van Dijk

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

  std::cout << "Removing " << d_badattachments.size() << " bad frames from database..." << std::endl;
  for (auto it = d_badattachments.begin(); it != d_badattachments.end(); )
  {
    uint32_t rowid = it->first;
    uint64_t uniqueid = it->second;

    SqliteDB::QueryResults results;
    std::string query = "SELECT mid FROM part WHERE _id = " + bepaald::toString(rowid) + " AND unique_id = " + bepaald::toString(uniqueid);
    long long int mid = -1;
    d_database.exec(query, &results);
    for (uint i = 0; i < results.rows(); ++i)
      for (uint j = 0; j < results.columns(); ++j)
        if (results.valueHasType<long long int>(i, j))
          if (results.header(j) == "mid")
          {
            mid = results.getValueAs<long long int>(i, j);
            break;
          }

    if (mid == -1)
    {
      std::cout << "Failed to remove frame :( Could not find matching 'part' entry" << std::endl;
      return false;
    }

    d_database.exec("DELETE FROM part WHERE mid = " + bepaald::toString(mid));
    d_badattachments.erase(it);
  }

  return true;
}
