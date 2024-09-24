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

bool SignalBackup::mergeRecipients(std::vector<std::string> const &addresses/*, bool editgroupmembers*/) // addresses is list of phone numbers
{
  Logger::message(__FUNCTION__);

  if (addresses.size() != 2)
  {
    Logger::error("Need exactly two recipient ID's");
    return false;
  }

  std::vector<std::string> r_ids = addresses;
  std::vector<std::string> phonenumbers = addresses;

  // for database version >= 24, addresses = recipient_ids, for db version < 24 addresses = recipient.phone
  // so convert to recipient._ids
  if (d_databaseversion >= 24)
  {
    for (unsigned int i = 0; i < r_ids.size(); ++i)
    {
      SqliteDB::QueryResults res;
      d_database.exec("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", r_ids[i], &res);
      if (res.rows() != 1 || res.columns() != 1 ||
          !res.valueHasType<long long int>(0, 0))
      {
        Logger::error("Failed to find recipient._id matching phone in target database: '", r_ids[i], "'");
        return false;
      }
      r_ids[i] = bepaald::toString(res.getValueAs<long long int>(0, 0));
    }
  }

  std::string target_rid = r_ids.back();
  std::string targetphone = phonenumbers.back();

  // deal with one-on-one conversations:
  // get thread of target address
  long long int tid = getThreadIdFromRecipient(target_rid);

  // update all messages from this r_ids[i] to belong to that same thread and change address in new number
  for (unsigned int i = 0; i < r_ids.size() - 1; ++i)
  {
    Logger::message("Dealing with recipient: ", r_ids[i]);

    long long int oldtid = getThreadIdFromRecipient(r_ids[i]);
    // update thread info:
    if (tid != -1 && oldtid != -1)
    {
      // update thread_id in sms table
      if (d_database.containsTable("sms"))
      {
        d_database.exec("UPDATE sms SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
        Logger::message("Updated ", d_database.changed(), " thread_ids in 'sms'");
      }

      // update thread_id in message table
      d_database.exec("UPDATE " + d_mms_table + " SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
      Logger::message("Updated ", d_database.changed(), " thread_ids in '", d_mms_table, "'");

      // update thread_id in mention
      if (d_database.containsTable("mention"))
      {
        d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
        Logger::message("Updated ", d_database.changed(), " thread_ids in 'mention'");
      }
    }

    // Update recipient_ids
    if (d_database.containsTable("sms"))
    {
      d_database.exec("UPDATE sms SET " + d_sms_recipient_id + " = ? WHERE " + d_sms_recipient_id + " = ?",
                      {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " recipients in 'sms' table");
    }

    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id")) // < dbv 185
    {
      d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_recipient_id + " = ? WHERE " + d_mms_recipient_id + " = ?",
                      {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " recipients in '", d_mms_table, "' table");
    }
    else
    {
      int count = 0;
      d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_recipient_id + " = ? WHERE " + d_mms_recipient_id + " = ?",
                      {target_rid, r_ids[i]});
      count += d_database.changed();
      d_database.exec("UPDATE " + d_mms_table + " SET to_recipient_id = ? WHERE to_recipient_id = ?",
                      {target_rid, r_ids[i]});
      Logger::message("Updated ", count + d_database.changed(), " recipients in '", d_mms_table, "' table");
    }

    // change quote author
    if (d_database.tableContainsColumn(d_mms_table, "quote_author"))
    {
      d_database.exec("UPDATE " + d_mms_table + " SET quote_author = ? WHERE quote_author = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " quote_authors");
    }

    // change msl_recipient
    if (d_database.containsTable("msl_recipient"))
    {
      d_database.exec("UPDATE msl_recipient SET recipient_id = ? WHERE recipient_id = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " msl_recipients");
    }

    if (d_database.containsTable("mention"))
    {
      d_database.exec("UPDATE mention SET recipient_id = ? WHERE recipient_id = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " entries in 'mention' table");
    }

    if (d_database.containsTable("reaction"))
    {
      d_database.exec("UPDATE reaction SET author_id = ? WHERE author_id = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " reaction authors table");
    }

    if (d_database.containsTable("call"))
    {
      d_database.exec("UPDATE call SET peer = ? WHERE peer = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " recipients in 'call' table");
    }

    if (d_database.containsTable("notification_profile_allowed_members"))
    {
      d_database.exec("UPDATE notification_profile_allowed_members SET recipient_id = ? WHERE recipient_id = ?", {target_rid, r_ids[i]});
      Logger::message("Updated ", d_database.changed(), " entries in 'notification_profile_allowed_members' table");
    }

    // delete old thread, lets make sure it has no messages
    if (tid != -1 && oldtid != -1)
    {
      if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM " + d_mms_table + " WHERE thread_id = ?", oldtid, -1) != 0)
      {
        Logger::error("Something went wrong: moved all messages to new thread, but old thread is not empty...");
        return false;
      }
      else
      {
        d_database.exec("DELETE FROM thread WHERE _id = ?", oldtid);
        Logger::message("Deleted ", d_database.changed(), " empty thread(s) from database");
      }
    }
  }

  // OLD STYLE REACTIONS (currently reactions are in their own table, before they were in a column in the message tables
  for (auto const &t : {"sms"s, d_mms_table})
  {
    // update reaction authors
    if (d_database.tableContainsColumn(t, "reactions"))
    {
      SqliteDB::QueryResults results;
      d_database.exec("SELECT _id, reactions FROM " + t + " WHERE reactions IS NOT NULL", &results);
      bool changed = false;
      for (unsigned int i = 0; i < results.rows(); ++i)
      {
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (unsigned int k = 0; k < reactions.numReactions(); ++k)
        {
          for (unsigned int j = 0; j < r_ids.size() - 1; ++j)
            if (reactions.getAuthor(k) == bepaald::toNumber<uint64_t>(r_ids[j]))
            {
              reactions.setAuthor(k, bepaald::toNumber<uint64_t>(target_rid));
              changed = true;
            }
          if (changed)
            d_database.exec("UPDATE " + t + " SET reactions = ? WHERE _id = ?",
                            {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
                             results.getValueAs<long long int>(i, "_id")});
        }
      }
    }
  }

  // // deal with groups
  // SqliteDB::QueryResults results;
  // d_database.exec("SELECT group_id,members,title FROM groups", &results); // get id,members and title from all groups
  // for (unsigned int i = 0; i < results.rows(); ++i)
  // {
  //   if (results.columns() != 3 ||
  //       !results.valueHasType<std::string>(i, 0) ||
  //       !results.valueHasType<std::string>(i, 1) ||
  //       !results.valueHasType<std::string>(i, 2))
  //   {
  //     Logger::error(":(");
  //     continue;
  //   }

  //   std::string id = results.getValueAs<std::string>(i, 0);
  //   std::string members = results.getValueAs<std::string>(i, 1);
  //   std::string title = results.getValueAs<std::string>(i, 2);

  //   Logger::message("Dealing with group: ", id, " (title: '", title, "', members: ", members, ")");

  //   std::string recipient_id = id;
  //   if (d_databaseversion >= 24)
  //   {
  //     SqliteDB::QueryResults res;
  //     d_database.exec("SELECT _id FROM recipient WHERE group_id = ?", id, &res);
  //     if (res.rows() != 1 || res.columns() != 1 ||
  //         !res.valueHasType<long long int>(0, 0))
  //     {
  //       Logger::error("Failed to find recipient._id matching phone/group_id in target database");
  //       return false;
  //     }
  //     recipient_id = bepaald::toString(res.getValueAs<long long int>(0, 0));
  //   }

  //   // get thread id for this group:
  //   tid = getThreadIdFromRecipient(recipient_id);
  //   if (tid == -1)
  //   {
  //     Logger::error("Failed to find thread for groupchat");
  //     continue;
  //   }

  //   // for all incoming messages of this group(= this thread), if the (originating) address = oldaddress, change it to target
  //   for (unsigned int j = 0; j < r_ids.size() - 1; ++j)
  //   {
  //     if (d_database.containsTable("sms"))
  //     {
  //       d_database.exec("UPDATE sms SET " + d_sms_recipient_id + " = ? "
  //                       "WHERE " + d_sms_recipient_id + " = ? AND thread_id = ?", {target_rid, r_ids[j], tid});
  //       Logger::message("Updated ", d_database.changed(), " entries in 'sms' table");
  //     }
  //     d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_recipient_id + " = ? "
  //                     "WHERE " + d_mms_recipient_id + " = ? AND thread_id = ?", {target_rid, r_ids[j], tid});
  //     Logger::message("Updated ", d_database.changed(), " entries in '" + d_mms_table + "' table");
  //   }

  //   // if (editgroupmembers)
  //   // {
  //   //   // maybe former_v1_members needs to be adjusted similarly?
  //   //   for (unsigned int j = 0; j < r_ids.size() - 1; ++j)
  //   //   {
  //   //     // change current member list in group database:
  //   //     //std::cout << "  GROUP MEMBERS BEFORE: " << members << std::endl;
  //   //     std::string::size_type pos = std::string::npos;
  //   //     if ((pos = members.find(r_ids[j])) != std::string::npos)
  //   //     {
  //   //       Logger::message("  GROUP MEMBERS BEFORE: ", members);
  //   //       //std::cout << "  FOUND ADDRESS TO CHANGE" << std::endl;
  //   //       // remove address
  //   //       members.erase(pos, r_ids[j].length());
  //   //       // remove left over comma
  //   //       if (members[0] == ',') // if removed was first
  //   //         members.erase(0, 1);
  //   //       else if (members.back() == ',') // if removed was last
  //   //         members.erase(members.size() - 1, 1);
  //   //       else // if removed was middle
  //   //         members.erase(std::unique(members.begin(), members.end(), [](char c1, char c2){ return c1 == ',' && c2 == ',';}), members.end());
  //   //       if (members.find(target_rid) == std::string::npos) // else target already in memberlist
  //   //         members += "," + target_rid;
  //   //       d_database.exec("UPDATE groups SET members = ? WHERE group_id = ?", {members, recipient_id});
  //   //       Logger::message("  GROUP MEMBERS AFTER : ", members);
  //   //     }
  //   //   }
  //   // }
  // }
      /*

        NOTE The following two routines only work for old-style (v1) group updates.
        For groupV2 updates, a group update might look something like this:


      GroupContextV2 statusmsg(body);
      statusmsg.print();

GROUP V2
Field 1 (optional::bytes): (hex:) 0a 20 f2 f5 8f 60 6e c1 24 [...]
ERROR REQUESTED TYPE TOO SMALL (2)
Field 3 (optional::bytes): (hex:) 12 03 57 4c 53 1a 49 67 [...]
Field 1 (optional::protobuf):
  Field 1 (optional::bytes): (hex:) f2 f5 8f 60 6e c1 24 99 [...]
  Field 2 (optional::uint32): 1
Field 2 (optional::protobuf):
  Field 1 (optional::bytes): (hex:) 6b 1e 76 87 cc [...]
  Field 2 (optional::uint32): 1
  Field 11 (optional::protobuf):
    Field 1 (optional::string): groups/6KM9eoH7qE6OxmycqQW[...]
Field 3 (optional::protobuf):
  Field 2 (optional::string): GROUPTITLE
  Field 3 (optional::string): groups/6KM9eoH7qE6OxmycqQW[...]
  Field 4 (optional::protobuf):
  Field 5 (optional::protobuf):
    Field 1 (optional::enum): 2
    Field 2 (optional::enum): 2
  Field 6 (optional::uint32): 1
  Field 7 (repeated::protobuf) (1/3):
    Field 1 (optional::bytes): (hex:) 93 72 22 73 78 [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) f6 3f 8f 7b a9 a4 [...]
  Field 7 (repeated::protobuf) (2/3):
    Field 1 (optional::bytes): (hex:) 60 f8 08 1b 9f 25 [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) 7e 21 ca f8 cb a8 d[...]
  Field 7 (repeated::protobuf) (3/3):
    Field 1 (optional::bytes): (hex:) 6b 1e 76 87 cc 0d [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) 88 28 52 88 87 2c [...]
Field 4 (optional::protobuf):
  Field 2 (optional::string): WLS
  Field 3 (optional::string): groups/6KM9eoH7qE6OxmycqQW[...]
  Field 4 (optional::protobuf):
  Field 5 (optional::protobuf):
    Field 1 (optional::enum): 2
    Field 2 (optional::enum): 2
  Field 7 (repeated::protobuf) (1/3):
    Field 1 (optional::bytes): (hex:) 93 72 22 73 78 e3 [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) f6 3f 8f 7b a9 a4 [...]
  Field 7 (repeated::protobuf) (2/3):
    Field 1 (optional::bytes): (hex:) 60 f8 08 1b 9f 25 [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) 7e 21 ca f8 cb a8 [...]
  Field 7 (repeated::protobuf) (3/3):
    Field 1 (optional::bytes): (hex:) 6b 1e 76 87 cc 0d [...]
    Field 2 (optional::enum): 2
    Field 3 (optional::bytes): (hex:) 88 28 52 88 87 2c [...]

        Where the repeating fields 3.7.1 & 4.7.1 corresponds to uuid as found in recipient.uuid

      */

  // get groupV1 status message updates:
  SqliteDB::QueryResults results2;

  std::pair<std::string, std::string> smsquery("sms", "SELECT type,body,_id FROM 'sms' "
                                               "WHERE thread_id = " + bepaald::toString(tid) + " AND "
                                               "(type & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + ") IS NOT 0 AND "
                                               "(type & " + bepaald::toString(Types::GROUP_V2_BIT) + ") IS 0");
  std::pair<std::string, std::string> mmsquery(d_mms_table, "SELECT " + d_mms_type + " AS type,body,_id FROM '" + d_mms_table + "' "
                                               "WHERE thread_id = " + bepaald::toString(tid) + " AND "
                                               "(" + d_mms_type + " & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + ") IS NOT 0 AND "
                                               "(" + d_mms_type + " & " + bepaald::toString(Types::GROUP_V2_BIT) + ") IS 0");

  for (auto const &d : {smsquery, mmsquery})
  {
    if (d_database.containsTable(d.first))
    {
      d_database.exec(d.second, &results2);
      if (d_verbose) [[unlikely]]
        results2.prettyPrint(d_truncate);
      for (unsigned int j = 0; j < results2.rows(); ++j)
      {
        std::string body = std::any_cast<std::string>(results2.value(j, "body"));
        long long int type = std::any_cast<long long int>(results2.value(j, "type"));
        long long int msgid = std::any_cast<long long int>(results2.value(j, "_id"));

        GroupContext statusmsg(body);

        if (Types::isGroupUpdate(type))
          Logger::message("Handling group update ", j + 1);

        bool targetpresent = false;
        auto field4 = statusmsg.getField<4>();
        for (unsigned int k = 0; k < field4.size(); ++k)
        {
          Logger::message("memberlist: ", field4[k]);
          if (field4[k] == targetphone)
            targetpresent = true;
        }

        int removed = 0;
        for (unsigned int k = 0; k < phonenumbers.size() - 1; ++k)
        {
          removed = statusmsg.deleteFields(4, &phonenumbers[k]);
          Logger::message("deleted ", removed, " members from group update message");
        }

        if (removed)
        {
          if (!targetpresent) // add target if not present
            statusmsg.addField<4>(targetphone);
          // set body
          d_database.exec("UPDATE " + d.first + " SET body = ? WHERE _id = ?", {statusmsg.getDataString(), msgid});
          Logger::message("Updated ", d_database.changed(), " group updates in '", d.first, "' table");
        }
      }
    }
  }

  return true;
}
