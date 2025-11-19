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

bool SignalBackup::handleDTPoll(long long int mms_id [[maybe_unused]],
                                long long int rid [[maybe_unused]],
                                std::string const &poll_json [[maybe_unused]]
                                /*, recipientmap, createcontacts, createcontacts_valid, generatekeys, warn*/) const
{
  // not supported yet...
  return false;

  /*
    // Poll json from desktop:

    //  - [ terminated, no single votes, quite a few removed votes (are not stored, only the the active vote(s) are). ]

    // poll = {"question":"Very very long poll title, how does this render exactly? let find out!","options":["This is the first option in the poll, it also is very long, much longer than is practical really","Now here comes the second option, it seems like the length poll option is actually limited, 87654321","Thats it, not feeling like doing more..."],"allowMultiple":false,"votes":[{"fromConversationId":"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx7874","optionIndexes":[1],"voteCount":4,"timestamp":1763135837739}],"terminatedAt":1763135995546,"terminateSendStatus":"NotInitiated"}

    //  - [ running, multiple votes allowed ]

    // poll = {"question":"Again, made on phone","options":["Opt 1","Opt 2","Opt 3"],"allowMultiple":true,"votes":[{"fromConversationId":"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx7874","optionIndexes":[1,0],"voteCount":2,"timestamp":1763135974752},{"fromConversationId":"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx5230","optionIndexes":[1],"voteCount":1,"timestamp":1763135987315}]}


  if (!d_database.containsTable("poll") ||
      !d_database.containsTable("poll_option") ||
      !d_database.containsTable("poll_vote"))
  {
    Logger::warning("Target Android database does not have 'poll'-tables");
    return false;
  }

  // insert data into 'poll'-table :  _id, author_id, message_id, question, allow_multiple_votes, end_message_id
  std::any new_poll_id;
  if (!insertRow("poll",
                 {{"auhor_id", rid},
                  {"message_id", mms_id},
                  {"question", d_database.exec("json_extract(?, '$.question')", poll_json)},
                  {"allow_multiple_votes", d_database.exec("json_extract(?, '$.allowMultiple')", poll_json)},
                  {"end_message_id", 0}}, "_id", &new_poll_id))
  {
    Logger::error("Failed to insert data into 'poll'-table");
    return false;
  }

  // insert data into 'poll-option'-table : id, poll_id, option_text, option_order
  long long int numoptions = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$.options')", poll_json, 0);
  std::map<unsigned int, long long int> poll_option_idx_to_id;
  for (unsigned int i = 0; i < numoptions; ++i)
  {
    std::any new_poll_option_id;
    if (!insertRow("poll_option",
                   {{"poll_id", new_poll_id},
                    {"option_text", d_database.getSingleResultAs<std::string>("SELECT json_extract(?, '$.options[' || ? || '])", {poll_json, i})},
                    {"option_order", i}}, "_id", &new_poll_option_id))
    {
      Logger::error("Failed to insert data into 'poll_option'-table");
      d_database.exec("DELETE FROM poll WHERE _id = ?", new_poll_id);
      d_database.exec("DELETE FROM poll_option WHERE poll_id = ?", new_poll_id);
      return false;
    }
    poll_option_idx_to_id[i] = std::any_cast<long long int>(new_poll_option_id);
  }

  // insert data into 'poll-vote'-table : _id, poll_id, poll_option_id, voter_id, vote_count, date_received, vote_state
  long long int numvotes = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$.votes')", poll_json, 0);
  for (unsigned int i = 0; i < numvotes; ++i)
  {
    std::string vote_json = d_database.getSingleResultAs<std::string>("SELECT json_extract(?, '$.votes[' || ? || '])", {poll_json, i});

    // get author of these votes -> voter_id
    // - extract fromConversationId
    // - dtCreateRecipient if not in map
    long long int voter_rid = 0;

    // extract voteCount -> vote_count
    // - (NOTE! on Android each individual vote (removed or not), has its own vote_count. On Desktop, removed votes are not saved. Additionally,
    //          when multiple votes are allowed, even if multiple are un-removed and present in the Desktop database, only the current highest
    //          voteCount is saved. I am not sure how Android would deal with this...
    //   (NOTE2 I think this is ok on Android, old votes are also removed sometimes (when the same option is added and removed multiple times),
    //          so the voteCount is not a continuum there anyway. I should probably just set the vote_count on the last aadded cote and auto-
    //          decrement from this number on any other votes (but check count > 0)

    // extract timestamp -> date_received (note, even when multiple votes are cast, only one timestamp is saved? we will just use it for all votes)

    // set vote state to 4 (ADDED). (note, while old removed votes are present in Android, they are not saved in Desktop).

    long long int numvotes_from_this_author = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$.optionIndexes')", vote_json, 0);
    if (numvotes_from_this_author == 0) [[unlikely]]
      continue;

    for (unsigned int j = 0; j < numvotes_from_this_author; ++j)
    {

      long long int option_index = d_database.getSingleResultAs<long long int>("SELECT json_extract(?, '$.optionIndexes[' || ? || ']", {vote_json, j}, -1);
      if (option_index == -1) [[unlikely]]
        continue;

      if (!insertRow("poll_votes",
                     {{"poll_id", new_poll_id},
                      {"poll_option_id", poll_option_idx_to_id[option_index]},
                      {"voter_id", voter_rid},
                      {"vote_count", d_database.getSingleResultAs<long long int>("SELECT json_extract(?, '$.voteCount)", {poll_json, i}, 0)},
                      {"date_received", d_database.getSingleResultAs<long long int>("SELECT json_extract(?, '$.timestamp)", {poll_json, i}, 0)},
                      {"vote_state", 4}}))
      {
        Logger::error("Failed to insert data into 'poll_option'-table");
        d_database.exec("DELETE FROM poll WHERE _id = ?", new_poll_id);
        d_database.exec("DELETE FROM poll_option WHERE poll_id = ?", new_poll_id);
        d_database.exec("DELETE FROM poll_vote WHERE poll_id = ?", new_poll_id);
        return false;
      }
    }
  }

  // insert message

  // save _id, date in poll-map for possible terminate message

  */
}
