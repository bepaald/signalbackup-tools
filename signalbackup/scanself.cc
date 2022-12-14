/*
  Copyright (C) 2021-2022  Selwin van Dijk

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

long long int SignalBackup::scanSelf() const
{

  // only 'works' on 'newer' versions
  if (!d_database.containsTable("recipient") ||
      !d_database.tableContainsColumn("thread", "thread_recipient_id") ||
      !d_database.tableContainsColumn("mms", "quote_author") ||
      (!d_database.tableContainsColumn("mms", "reactions") || !d_database.containsTable("reaction")))
    return -1;

  SqliteDB::QueryResults res;
  // get thread ids of all 1-on-1 conversations

  if (!d_database.exec("SELECT _id, thread_recipient_id FROM thread WHERE thread_recipient_id IN (SELECT _id FROM recipient WHERE group_id IS NULL)", &res))
    return -1;
  //res.prettyPrint();

  std::set<long long int> options;

  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int tid = res.getValueAs<long long int>(i, "_id");
    long long int rid = res.getValueAs<long long int>(i, "thread_recipient_id");
    //std::cout << "Dealing with thread: " << tid << std::endl;

    // try to find other recipient in thread
    SqliteDB::QueryResults res2;
    if (!d_database.exec("SELECT DISTINCT quote_author FROM mms "
                         "WHERE thread_id IS ? AND quote_author IS NOT NULL AND quote_author IS NOT ?", {tid, rid}, &res2))
      continue;
    for (uint j = 0; j < res2.rows(); ++j)
    {
      //std::cout << "  From quote:" << res2.valueAsString(j, "quote_author") << std::endl;
      options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "quote_author")));
    }

    if (d_database.tableContainsColumn("sms", "reactions"))
    {
      if (!d_database.exec("SELECT reactions FROM sms WHERE thread_id IS ? AND reactions IS NOT NULL", tid, &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        ReactionList reactions(res2.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(j, "reactions"));
        for (uint k = 0; k < reactions.numReactions(); ++k)
        {
          if (reactions.getAuthor(k) != static_cast<uint64_t>(rid))
          {
            //std::cout << "  From reaction (old): " << reactions.getAuthor(k) << std::endl;
            options.insert(reactions.getAuthor(k));
          }
        }
      }
    }
    if (d_database.tableContainsColumn("mms", "reactions"))
    {
      if (!d_database.exec("SELECT reactions FROM mms WHERE thread_id IS ? AND reactions IS NOT NULL", tid, &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        ReactionList reactions(res2.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(j, "reactions"));
        for (uint k = 0; k < reactions.numReactions(); ++k)
        {
          if (reactions.getAuthor(k) != static_cast<uint64_t>(rid))
          {
            //std::cout << "  From reaction (old): " << reactions.getAuthor(k) << std::endl;
            options.insert(reactions.getAuthor(k));
          }
        }
      }
    }
    if (d_database.containsTable("reaction"))
    {
      if (!d_database.exec("SELECT DISTINCT author_id FROM reaction WHERE is_mms IS 0 AND author_id IS NOT ? AND message_id IN (SELECT DISTINCT _id FROM sms WHERE thread_id = ?)", {rid, tid}, &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        //std::cout << "  From reaction (new):" << res2.valueAsString(j, "author_id") << std::endl;
        options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "author_id")));
      }

      if (!d_database.exec("SELECT DISTINCT author_id FROM reaction WHERE is_mms IS 1 AND author_id IS NOT ? AND message_id IN (SELECT DISTINCT _id FROM mms WHERE thread_id = ?)", {rid, tid} , &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        //std::cout << "  From reaction (new):" << res2.valueAsString(j, "author_id") << std::endl;
        options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "author_id")));
      }
    }
  }

  // get thread ids of all group conversations
  if (!d_database.exec("SELECT _id,active FROM groups", &res))
    return -1;
  //res.prettyPrint();

  // for each group-thread
  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int gid = res.getValueAs<long long int>(i, "_id");
    long long int active = res.getValueAs<long long int>(i, "active");

    if (active == 0)
    {
      //std::cout << "Skipping group: " << gid << " (inactive)" << std::endl;
      continue;
    }

    SqliteDB::QueryResults res2;
    // skip groups without thread
    if (!d_database.exec("SELECT * from thread WHERE thread_recipient_id IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id from groups WHERE _id IS ?))", gid, &res2))
      continue;
    if (res2.rows() == 0)
    {
      //std::cout << "Skipping group: " << gid << " (no thread)" << std::endl;
      continue;
    }

    //std::cout << "Dealing with group: " << gid << std::endl;

    // this prints all group members that never appear as recipient in a message (in groups, the recipient ('address') is always the sender, except for self, who has the groups id as address)
    if (!d_database.exec("WITH split(word, str) AS (SELECT '',members||',' FROM groups WHERE _id IS ? UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT word FROM split WHERE word!='' AND word NOT IN (SELECT DISTINCT " + d_mms_recipient_id +" FROM mms WHERE thread_id IS (SELECT _id FROM thread WHERE thread_recipient_id IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id FROM groups WHERE _id IS ?)))) AND word NOT IN (SELECT DISTINCT " + d_sms_recipient_id + " FROM sms WHERE thread_id IS (SELECT _id FROM thread WHERE thread_recipient_id IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id FROM groups WHERE _id IS ?))))", {gid, gid, gid}, &res2))
      continue;

    for (uint j = 0; j < res2.rows(); ++j)
    {
      //std::cout << "  From group membership:" << res2.valueAsString(j, "word") << std::endl;
      options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "word")));
    }
  }

  if (options.size() == 1)
    return *options.begin();

  return -1;
}
