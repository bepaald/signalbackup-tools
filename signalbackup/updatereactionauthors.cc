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

// update (old-style)reaction authors
// current (and future) databases do not have reactions in the [s|m]ms tables,
// but in their own table called 'reaction'.
void SignalBackup::updateReactionAuthors(long long int id1, long long int id2) const // if id2 == -1, id1 is an offset
{                                                                                    // else, change id1 into id2
  for (auto const &msgtable : {"sms"s, d_mms_table})
  {
    if (d_database.tableContainsColumn(msgtable, "reactions"))
    {
      int changedcount = 0;
      SqliteDB::QueryResults results;
      d_database.exec("SELECT _id, reactions FROM "s + msgtable + " WHERE reactions IS NOT NULL", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        bool changed = false;
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
        {
          //std::cout << "Updating reaction author (" << msgtable << ") : " << reactions.getAuthor(j) << "..." << std::endl;
          if (id2 == -1)
          {
            reactions.setAuthor(j, reactions.getAuthor(j) + id1);
            ++changedcount;
            changed = true;
          }
          else if (reactions.getAuthor(j) == static_cast<uint64_t>(id1))
          {
            reactions.setAuthor(j, id2);
            ++changedcount;
            changed = true;
          }
        }
        if (changed)
          d_database.exec("UPDATE "s + msgtable + " SET reactions = ? WHERE _id = ?",
                          {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
                           results.getValueAs<long long int>(i, "_id")});
      }
      if (d_verbose) [[unlikely]]
        Logger::message("     Updated ", changedcount, " ", msgtable, ".reaction authors");
    }
  }
}
