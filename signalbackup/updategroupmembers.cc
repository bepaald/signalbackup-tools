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

void SignalBackup::updateGroupMembers(long long int id1, long long int id2) const // if id2 == -1, id1 is an offset
{                                                                                 // else, change id1 into id2
  for (auto const &members : {"members"s, d_groups_v1_members})
  {
    if (!d_database.tableContainsColumn("groups", members))
      continue;

    // get group members
    SqliteDB::QueryResults results;
    bool changed = false;
    d_database.exec("SELECT _id,"s + members + " FROM groups WHERE " + members + " IS NOT NULL", &results);
    //d_database.prettyPrint("SELECT _id,members FROM groups");
    for (uint i = 0; i < results.rows(); ++i)
    {
      long long int gid = results.getValueAs<long long int>(i, "_id");
      std::string membersstr = results.getValueAs<std::string>(i, members);
      std::vector<int> membersvec;
      std::stringstream ss(membersstr);
      while (ss.good())
      {
        std::string substr;
        std::getline(ss, substr, ',');
        membersvec.emplace_back(bepaald::toNumber<int>(substr));
      }

      std::string newmembers;
      for (uint m = 0; m < membersvec.size(); ++m)
      {
        if (m > 0)
          newmembers += ",";
        newmembers += bepaald::toString((id2 != -1 && membersvec[m] == id1) ? id2 : ((id2 == -1) ? membersvec[m] + id1 : membersvec[m]));
      }

      if (membersstr != newmembers)
      {
        changed = true;
        d_database.exec("UPDATE groups SET "s + members + " = ? WHERE _id == ?", {newmembers, gid});
        if (d_verbose)
          Logger::message("    Updated groups.", members, ", changed: ", membersstr, " -> ", newmembers);
      }
      if (d_verbose && changed)
        Logger::message("    Updated groups.", members, ", changed: 0");
    }
  }
}
