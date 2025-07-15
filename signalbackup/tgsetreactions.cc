/*
  Copyright (C) 2025  Selwin van Dijk

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

bool SignalBackup::tgSetReactions(std::string const &reactionsjson, long long int new_msg_id,
                                  std::vector<std::pair<std::vector<std::string>, long long int>> const &contactmap,
                                  bool custom) const
{
  if (reactionsjson.empty()) [[likely]]
    return true;

  if (!d_database.containsTable("reaction")) [[unlikely]]
  {
    Logger::warning("Input database does not have reaction table (too old?)");
    return false;
  }

  // get recipient id for conversation
  auto find_in_contactmap = [&contactmap](std::string const &identifier) -> long long int
  {
    for (unsigned int i = 0; i < contactmap.size(); ++i)
      for (unsigned int j = 0; j < contactmap[i].first.size(); ++j)
        if (contactmap[i].first[j] == identifier)
          return contactmap[i].second;
    return -1;
  };

  //std::cout << reactionsjson << std::endl;

  std::string_view query(custom ?
                         "SELECT "
                         "json_extract(value, '$.author') AS author,"
                         "json_extract(value, '$.emoji') AS emoji,"
                         "json_extract(value, '$.timestamp') AS timestamp "
                         "FROM json_each(?)" :
                         "SELECT "
                         "json_extract(L2.value, '$.from_id') AS author, "
                         "json_extract(L1.value, '$.emoji') AS emoji, "
                         "UNIXEPOCH(json_extract(L2.value, '$.date'), 'utc') AS timestamp "
                         "FROM json_each(?) L1, json_each(json_extract(L1.value, '$.recent')) L2 "
                         "WHERE json_extract(L1.value, '$.type') = 'emoji'");

  SqliteDB::QueryResults reactions_results;
  if (!d_database.exec(query, reactionsjson, &reactions_results)) [[unlikely]]
    return false;

  //reactions_results.prettyPrint(false);

  for (unsigned int i = 0; i < reactions_results.rows(); ++i)
  {
    long long int authorid = find_in_contactmap(reactions_results(i, "author"));
    if (authorid == -1)
    {
      Logger::error("Failed to map reaction author '", reactions_results(i, "author"), "' to id in Android backup");
      return false;
    }

    std::string emoji = reactions_results(i, "emoji");
    if (emoji.empty()) [[unlikely]]
    {
      Logger::error("Failed to retrieve emoji from json-reaction");
      return false;
    }

    long long int timestamp = reactions_results.valueAsInt(i, "timestamp", -1);
    if (timestamp == -1)
    {
      Logger::error("failed to get timestamp for reaction");
      return false;
    }
    if (timestamp < 100000000000) // only 11 digits (max), timestamp is likely in seconds instead of milliseconds
      timestamp *= 1000;

    if (!insertRow("reaction",
                   {{"author_id", authorid},
                    {"emoji", emoji},
                    {"date_sent", timestamp},
                    {"date_received", timestamp},
                    {"message_id", new_msg_id}}))
      return false;
  }

  return true;
}
