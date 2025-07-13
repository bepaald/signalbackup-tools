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
                                  std::vector<std::pair<std::vector<std::string>, long long int>> const &contactmap) const
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

  long long int numreactions = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$')", reactionsjson, -1);
  if (numreactions == -1) [[unlikely]]
  {
    Logger::error("Failed to get number of reactions from json string");
    return false;
  }

  if (numreactions == 0) [[unlikely]] // is this even possible?
    return true;

  for (unsigned int i = 0; i < numreactions; ++i)
  {
    SqliteDB::QueryResults reaction_results;
    if (!d_database.exec("SELECT "
                         "json_extract(?1, '$[' || ?2 || '].author') AS author,"
                         "json_extract(?1, '$[' || ?2 || '].timestamp') AS timestamp,"
                         "json_extract(?1, '$[' || ?2 || '].emoji') AS emoji",
                         {reactionsjson, i}, &reaction_results) ||
        reaction_results.rows() != 1)
      return false;

    //reaction_results.prettyPrint(false);

    long long int authorid = find_in_contactmap(reaction_results("author"));
    if (authorid == -1)
    {
      Logger::error("Failed to map reaction author '", reaction_results("author"), "' to id in Android backup");
      return false;
    }

    long long int timestamp = reaction_results.valueAsInt(0, "timestamp", -1);
    if (timestamp == -1)
    {
      Logger::error("failed to get timestamp for reaction");
      return false;
    }
    if (timestamp < 100000000000) // only 11 digits (max), timestamp is likely in seconds instead of milliseconds
      timestamp *= 1000;

    if (!insertRow("reaction",
                   {{"author_id", authorid},
                    {"emoji", reaction_results.value(0, "emoji")},
                    {"date_sent", timestamp},
                    {"date_received", timestamp},
                    {"message_id", new_msg_id}}))
      return false;
  }

  return true;
}
