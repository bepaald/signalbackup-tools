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

void SignalBackup::getDTReactions(SqliteDB const &ddb, long long int rowid, long long int numreactions, std::vector<std::vector<std::string>> *reactions) const
{
  SqliteDB::QueryResults results_emoji_reactions;
  //if (numreactions)
  //  std::cout << "  " << numreactions << " reactions." << std::endl;
  for (uint k = 0; k < numreactions; ++k)
  {
    if (!ddb.exec("SELECT "
                  "json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].emoji') AS emoji,"

                  // not present in android database
                  //"json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].remove') AS remove,"

                  // THIS IS THE AUTHOR OF THE MESSAGE THATS REACTED TO
                  //"json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].targetAuthorUuid') AS target_author_uuid,"

                  //timestamp of message that reaction belongs to, dont know why this exists
                  //"json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].targetTimestamp') AS target_timestamp,"

                  "json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].timestamp') AS timestamp,"

                  // THE ID OF THE CONVERSATION OF THE REACTION AUTHOR (conversation somewhat doubles android's recipient table)
                  // ON OLDER DATABASES THIS IS PHONE NUMBER OF THE ACTUAL AUTHOR
                  "json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].fromId') AS from_id,"

                  //"json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].source') AS source" // ???
                  "conversations." + d_dt_c_uuid + " AS uuid,"
                  "conversations.e164 AS phone"
                  " FROM messages LEFT JOIN conversations ON"
                  " (conversations.id IS json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].fromId')"
                  " OR "
                  "conversations.e164 IS json_extract(messages.json, '$.reactions[" + bepaald::toString(k) + "].fromId'))"
                  " WHERE rowid = ?", rowid, &results_emoji_reactions))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to get reaction data from desktop database. Skipping." << std::endl;
      continue;
    }
    //std::cout << "  Reaction " << k + 1 << "/" << numreactions << std::endl;
    //results_emoji_reactions.print(false);

    // DEBUG
    if (results_emoji_reactions.valueAsString(0, "uuid").empty() &&
        results_emoji_reactions.valueAsString(0, "phone").empty()) [[unlikely]]
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " : Got empty author uuid, here is some additional info:" << std::endl;
      ddb.print("SELECT json_extract(json, '$.reactions') FROM messages WHERE rowid = ?", rowid);
      results_emoji_reactions.printLineMode();
    }

    reactions->emplace_back(std::vector{results_emoji_reactions.valueAsString(0, "emoji"),
                                        results_emoji_reactions.valueAsString(0, "timestamp"),
                                        results_emoji_reactions.valueAsString(0, "uuid"),
                                        results_emoji_reactions.valueAsString(0, "phone")});
  }
}
