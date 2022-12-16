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

/*
  NOTE: THESE ARE CUSTOM FUNCTIONS AND WILL BE REMOVED WITHOUT NOTIFICATION IN THE NEAR FUTURE
*/

#include "signalbackup.ih"

#include <sstream>
#include "../sqlcipherdecryptor/sqlcipherdecryptor.h"

bool SignalBackup::hhenkel(std::string const &signaldesktoplocation)
{

  // args


  // open signal desktop database
  SqlCipherDecryptor db(signaldesktoplocation, signaldesktoplocation + "/sql");
  if (!db.ok())
  {
    std::cout << "Error reading signal desktop database" << std::endl;
    return false;
  }
  auto [data, size] = db.data();
  std::pair<unsigned char *, uint64_t> tmp = std::make_pair(data, size);
  SqliteDB desktopdb(&tmp);

  //SqliteDB::QueryResults r;
  //desktopdb.exec("SELECT hex(groupid) FROM conversations", &r);
  //r.prettyPrint();

  // all threads in messages
  SqliteDB::QueryResults list_of_threads;
  d_database.exec("SELECT DISTINCT mms.thread_id FROM mms UNION SELECT DISTINCT sms.thread_id FROM sms", &list_of_threads);
  list_of_threads.prettyPrint();
  std::cout << std::endl;

  // for each thread, find a corresponding conversation in desktop-db
  std::vector<std::tuple<long long int, std::string, std::string>> matches; // thread_id, recipient_id/address, -> desktop.converationId
  for (long long int i = 0; i < static_cast<long long int>(list_of_threads.rows()); ++i)
  {
    SqliteDB::QueryResults message_data;
    d_database.exec("SELECT body," + d_mms_date_sent + "," + d_mms_recipient_id + " FROM mms WHERE thread_id = ?",
                    list_of_threads.value(i, 0), &message_data);
    //message_data.prettyPrint();

    bool matched = false;
    for (uint j = 0; j < message_data.rows(); ++j)
    {
      SqliteDB::QueryResults r2;
      desktopdb.exec("SELECT conversationId FROM messages WHERE sent_at == ? AND body == ?", {message_data.value(j, d_mms_date_sent), message_data.value(j, "body")}, &r2);
      if (r2.rows() == 1)
      {
        matched = true;
        matches.emplace_back(std::make_tuple(list_of_threads.getValueAs<long long int>(i, 0), message_data.valueAsString(j, d_mms_recipient_id), r2.getValueAs<std::string>(0, "conversationId")));
      }

      if (matched)
        break;
    }
    if (!matched)
    {
      d_database.exec("SELECT body,date_sent," + d_sms_recipient_id + " FROM sms WHERE thread_id = ?",
                      list_of_threads.value(i, 0), &message_data);
      //message_data.prettyPrint();
      for (uint j = 0; j < static_cast<long long int>(message_data.rows()); ++j)
      {
        SqliteDB::QueryResults r2;
        desktopdb.exec("SELECT conversationId FROM messages WHERE sent_at == ? AND body == ?", {message_data.value(j, "date_sent"), message_data.value(j, "body")}, &r2);
        if (r2.rows() == 1)
        {
          matched = true;
          matches.emplace_back(std::make_tuple(list_of_threads.getValueAs<long long int>(i, 0), message_data.valueAsString(j, d_sms_recipient_id),
                                               r2.getValueAs<std::string>(0, "conversationId")));
        }

        if (matched)
          break;
      }
    }
    if (!matched)
    {
      std::cout << " - Failed to match thread " << list_of_threads.valueAsString(i, 0) << " to any conversation in Signal Desktop database" << std::endl;
      std::cout << "   Last 10 messages from this thread:" << std::endl;
      // todo

      std::string q =
        "SELECT "
        "sms." + d_sms_date_received + " AS union_date, "
        "sms.date_sent AS union_display_date, "
        "sms.type AS union_type, "
        "sms.body AS union_body "
        "FROM sms WHERE " + list_of_threads.valueAsString(i, 0) + " = sms.thread_id "
        "UNION "
        "SELECT "
        "mms.date_received AS union_date, " // not sure for outgoing
        "mms." + d_mms_date_sent + " AS union_display_date, "
        "mms." + d_mms_type + " AS union_type, "
        "mms.body AS union_body "
        "FROM mms WHERE " + list_of_threads.valueAsString(i, 0) + " = mms.thread_id "
        "ORDER BY union_date DESC, union_display_date ASC LIMIT 10";

      d_database.prettyPrint(q);
      std::cout << std::endl;

    }
  }
    /*
      mms: desktop.messages.sent_at == android.mms.date
      sms: desktop.messages.sent_at == android.sms.date_sent
     */

    // desktop messages:
    // SELECT id,unread,expires_at,sent_at,schemaVersion,conversationId,received_at,source,sourceDevice,hasAttachments,hasFileAttachments,hasVisualMediaAttachments,expireTimer,expirationStartTimestamp,type,body,messageTimer,messageTimerStart,messageTimerExpiresAt,isErased,isViewOnce,sourceUuid FROM messages WHERE body == "Test terug";
    // id|unread|expires_at|sent_at|schemaVersion|conversationId|received_at|source|sourceDevice|hasAttachments|hasFileAttachments|hasVisualMediaAttachments|expireTimer|expirationStartTimestamp|type|body|messageTimer|messageTimerStart|messageTimerExpiresAt|isErased|isViewOnce|sourceUuid
    // 13c39ffe-67e2-43a5-911b-9537e485b75d|||1597327208579|10|82184df5-c89b-4c5c-a0c6-b7c9449b3818|1598179687083|31683616099|1|0|||||incoming|Test terug|||||0|6b7e6b80-c3d9-4701-8f36-bf5e22ebd62c



  d_database.exec("DELETE FROM constraint_spec");
  d_database.exec("DELETE FROM megaphone");
  d_database.exec("DELETE FROM dependency_spec");
  d_database.exec("DELETE FROM push");
  d_database.exec("DELETE FROM drafts");
  d_database.exec("DELETE FROM mms_fts");
  d_database.exec("DELETE FROM recipient");
  d_database.exec("DELETE FROM group_receipts");
  d_database.exec("DELETE FROM mms_fts_config");
  d_database.exec("DELETE FROM sessions");
  d_database.exec("DELETE FROM sticker");
  d_database.exec("DELETE FROM groups");
  d_database.exec("DELETE FROM mms_fts_data");
  d_database.exec("DELETE FROM signed_prekeys");
  d_database.exec("DELETE FROM storage_key");
  d_database.exec("DELETE FROM identities");
  d_database.exec("DELETE FROM mms_fts_docsize");
  d_database.exec("DELETE FROM thread");
  d_database.exec("DELETE FROM job_spec");
  d_database.exec("DELETE FROM mms_fts_idx");
  d_database.exec("DELETE FROM sms_fts");
  d_database.exec("DELETE FROM key_value");
  d_database.exec("DELETE FROM one_time_prekeys");
  d_database.exec("DELETE FROM sms_fts_config");
  d_database.exec("DELETE FROM sms_fts_data");
  d_database.exec("DELETE FROM sms_fts_docsize");
  d_database.exec("DELETE FROM sms_fts_idx");


    /*

      recipient._id      := [s|m]ms.address

      thread._id         := [s|m]ms.thread_id
      thread.recipient_ids/thread.thread_recipient_id := [s|m]ms.address


      recipient.uuid     := conversations.uuid
               .phone    := conversations.e164
               .group_id := decode(conversations.groupId)
               .system_display_name := IF (!group) conversations.name ELSE NULL
               .signal_profile_name := conversations.profileName
               .profile_family_name :=              .profileFamilyName
               .progile_joined_name :=              .profileFullName

      IF GROUP

      group.group_id := recipient.group_id ( == conversations.groupId)
           .title    := conversations.name
           .members  := CONVERT(conversations.members FROM conversation.id -> recipient._id)
           .recipient_ids := [s|m]ms.recipient_ids


     */



  // SKIP GROUPS FOR SECOND PASS
  for (uint t = 0; t < matches.size(); ++t)
  {
    SqliteDB::QueryResults r;
    desktopdb.exec("SELECT id,uuid,e164,groupId,type,name,profileName,profileFamilyName,profileFullName FROM conversations WHERE id == ?", std::get<2>(matches[t]), &r);
    if (r.valueAsString(0, "type") == "group")
      continue;


    std::cout << " - Got match for thread " << std::get<0>(matches[t]) << std::endl;
    r.prettyPrint();
    std::cout << std::endl;

    // add entry in 'thread' database
    //std::cout << "INSERTING : " << std::get<0>(matches[t]) << " " << std::get<1>(matches[t]) << std::endl;
    d_database.exec("INSERT INTO thread (_id, " + d_thread_recipient_id + ") VALUES (?, ?)", {std::get<0>(matches[t]), std::get<1>(matches[t])});

    // add entry in 'recipient' database
    d_database.exec("INSERT INTO recipient (_id, uuid, phone, group_id, system_display_name, signal_profile_name, profile_family_name, profile_joined_name) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                    {std::get<1>(matches[t]), r.value(0, "uuid"), r.value(0, "e164"), r.value(0, "groupId"),
                     r.value(0, "name"), r.value(0, "profileName"), r.value(0, "profileFamilyName"), r.value(0, "profileFullName")});

  }
  // GROUPS
  for (uint t = 0; t < matches.size(); ++t)
  {
    SqliteDB::QueryResults r;
    desktopdb.exec("SELECT id,uuid,e164,groupId,type,name,profileName,profileFamilyName,profileFullName FROM conversations WHERE id == ?", std::get<2>(matches[t]), &r);
    if (r.valueAsString(0, "type") != "group")
      continue;


    std::cout << " - Got match for thread " << std::get<0>(matches[t]) << std::endl;
    r.prettyPrint();
    std::cout << std::endl;


    // NOTE !!!!
    // recipient_id (address) COULD BE WRONG! IT ONLY REPRESENTS THE GROUPS ID ON OUTGOING MESSAGES! (& outgoing message are always in mms table (nothing in sms))
    // on incoming message it is the recip_id of the specific group member sending the message
    SqliteDB::QueryResults group_rec;
    std::string group_recipient_id; // could be string or int, depending on age of database
    if (!d_database.exec("SELECT DISTINCT " + d_mms_recipient_id + " FROM mms WHERE (" + d_mms_type + " & " + bepaald::toString(Types::BASE_TYPE_MASK) +
                         ") BETWEEN " + bepaald::toString(Types::BASE_OUTBOX_TYPE) + " AND " +
                         bepaald::toString(Types::BASE_PENDING_INSECURE_SMS_FALLBACK) +
                         " AND thread_id == ?", std::get<0>(matches[t]), &group_rec))
    {
      std::cout << "ERROR" << std::endl;
      break;
    }
    if (group_rec.rows() == 0)
    {
      std::cout << "WARNING : No outgoing messages found in this group, this is unusual but I'm guessing i can just use any unused recipient_id" << std::endl;

      SqliteDB::QueryResults list_of_addresses;
      d_database.exec("SELECT DISTINCT mms." + d_mms_recipient_id + " AS union_rec_id FROM mms "
                      "UNION SELECT DISTINCT sms." + d_sms_recipient_id + " AS union_rec_id FROM sms", &list_of_addresses);
      // this is a stupid and naive way of looking for a free id, but it's quick to write :P
      long long int free_address = 1;
      bool done = false;
      while (!done)
      {
        bool found = false;
        for (uint i = 0; i < list_of_addresses.rows(); ++i)
        {
          if (list_of_addresses.valueAsString(i, "union_rec_id") == bepaald::toString(free_address))
          {
            ++free_address;
            found = true;
            break;
          }
        }
        if (!found)
          done = true;
      }
      //std::cout << "Got free address : " << free_address << std::endl;
      group_recipient_id = bepaald::toString(free_address);
    }
    else if (group_rec.rows() > 1)
    {
      std::cout << "Unexpectedly got multiple group ids.... this shouldn't happen. skipping" << std::endl;
      continue;
    }
    else
      group_recipient_id = group_rec.valueAsString(0, d_mms_recipient_id);

    // add entry in 'thread' database
    d_database.exec("INSERT INTO thread (_id, " + d_thread_recipient_id + ") VALUES (?, ?)", {std::get<0>(matches[t]), group_recipient_id});

    // add entry in 'recipient' database (Skip 'name' for groups, in desktop it is group name, in app it's NULL
    d_database.exec("INSERT INTO recipient (_id, uuid, phone, group_id, signal_profile_name, profile_family_name, profile_joined_name) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)",
                    {group_recipient_id, r.value(0, "uuid"), r.value(0, "e164"), r.value(0, "groupId"),
                     r.value(0, "profileName"), r.value(0, "profileFamilyName"), r.value(0, "profileFullName")});

    // add group entry
    std::string groupdatastr = r.valueAsString(0, "groupId");
    unsigned char const *groupdata = reinterpret_cast<unsigned char const *>(groupdatastr.c_str());
    unsigned int groupdatasize = groupdatastr.size();
    std::stringstream decoded_group_id;
    decoded_group_id << "__textsecure_group__!";
    unsigned int pos = 0;
    while (pos < groupdatasize)
    {
      if ((static_cast<unsigned int>(groupdata[pos]) & 0xff) <= 0x7f)
        decoded_group_id << std::hex << std::setw(2) << std::setfill('0') << (groupdata[pos] & 0xff);
      else if ((static_cast<unsigned int>(groupdata[pos]) & 0xff) >= 0xc0)
      {
        decoded_group_id << std::hex << std::setw(2) << std::setfill('0') << ((((groupdata[pos] << 6) & 0b11000000) | (groupdata[pos + 1] & 0b00111111)) & 0xff);
        ++pos;
      }
      ++pos;
    }
    std::string members;
    d_database.exec("INSERT INTO groups (group_id, title, members, recipient_id) VALUES(?, ?, ?, ?)", {decoded_group_id.str(), r.value(0, "name"), members, std::get<1>(matches[t])});

  }

  updateThreadsEntries();

  return true;
}

  /*
  SqliteDB::QueryResults t2;
  if (d_database.exec("SELECT * FROM thread WHERE _id == 1", &t2))
    t2.print();
  if (d_database.exec("SELECT * FROM recipient WHERE _id == 2", &t2))
    t2.print();
  if (desktopdb.exec("SELECT id,active_at,type,members,name,profileName,profileFamilyName,profileFullName,e164,uuid,groupId FROM conversations WHERE id == \"4bfad53f-540c-4a54-be37-ce8eb7bc3440\"", &t2))
    t2.print();
  std::cout << "" << std::endl;

  if (d_database.exec("SELECT * FROM thread WHERE _id == 9", &t2))
    t2.print();
  if (d_database.exec("SELECT * FROM recipient WHERE _id == 1", &t2))
    t2.print();
  if (desktopdb.exec("SELECT id,active_at,type,members,name,profileName,profileFamilyName,profileFullName,e164,uuid,groupId FROM conversations WHERE id == \"e6e4d01d-5607-4749-8c95-cde384547a8c\"", &t2))
    t2.print();
  std::cout << "" << std::endl;

  if (d_database.exec("SELECT * FROM thread WHERE _id == 13", &t2))
    t2.print();
  if (d_database.exec("SELECT * FROM recipient WHERE _id == 17", &t2))
    t2.print();
  if (d_database.exec("SELECT * FROM groups WHERE group_id == \"__textsecure_group__!7b8072dc2aa63a7e34dde2d0c5e315a0\"", &t2))
    t2.print();
  if (desktopdb.exec("SELECT id,active_at,type,members,name,profileName,profileFamilyName,profileFullName,e164,uuid,groupId FROM conversations WHERE id == \"90e6212f-01ca-473b-8001-36876fa50146\"", &t2))
    t2.print();
  std::cout << "" << std::endl;

  if (d_database.exec("SELECT * FROM thread WHERE _id == 15", &t2))
    t2.print();
  if (d_database.exec("SELECT * FROM recipient WHERE _id == 6", &t2))
    t2.print();
  if (desktopdb.exec("SELECT id,active_at,type,members,name,profileName,profileFamilyName,profileFullName,e164,uuid,groupId FROM conversations WHERE id == \"b89aaadc-293f-4d67-a17a-8b51f04a4de1\"", &t2))
    t2.print();
  std::cout << "" << std::endl;
  */

/*
void SignalBackup::esokrates()
{
  SqliteDB::QueryResults res;
  d_database.exec("SELECT _id,body,address,date,type "
                  "FROM sms "
                  "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS 0", &res);

  std::cout << "Searching for possible duplicates of " << res.rows() << " unsecured messages" << std::endl;

  std::vector<long long int> ids_to_remove;

  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int msgid = std::any_cast<long long int>(res.value(i, "_id"));

    std::string body;
    bool body_is_null = false;
    if (res.valueHasType<std::string>(i, "body"))
      body = std::any_cast<std::string>(res.value(i, "body"));
    else if (res.isNull(i, "body"))
      body_is_null = true;

    std::string address = std::any_cast<std::string>(res.value(i, "address"));
    long long int date = std::any_cast<long long int>(res.value(i, "date"));
    long long int type = std::any_cast<long long int>(res.value(i, "type"));

    SqliteDB::QueryResults res2;
    if (body_is_null)
      d_database.exec("SELECT _id "
                      "FROM sms "
                      "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body IS NULL "
                      "AND address = ?", {date, address}, &res2);
    else // !body_is_null
      d_database.exec("SELECT _id "
                      "FROM sms "
                      "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body = ? "
                      "AND address = ?", {date, body, address}, &res2);

    if (res2.rows() > 1)
    {
      std::cout << "Unexpectedley got multiple results when searching for duplicates... ignoring" << std::endl;
      continue;
    }
    else if (res2.rows() == 1)
    {
      std::time_t epoch = date / 1000;
      std::cout << " * Found duplicate of message: " << std::endl
                << "   " << msgid << "|" << body << "|" << address << "|"
                << std::put_time(std::localtime(&epoch), "%F %T %z") << "|" << "|" << type << std::endl
                << "   in 'sms' table. Marking for deletion." << std::endl;
      ids_to_remove.push_back(msgid);
      continue;
    }


    if (body_is_null)
      d_database.exec("SELECT _id "
                      "FROM mms "
                      "WHERE (msg_box & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body IS NULL "
                      "AND address = ?", {date, address}, &res2);
    else // !body_is_null
      d_database.exec("SELECT _id "
                      "FROM mms "
                      "WHERE (msg_box & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body = ? "
                      "AND address = ?", {date, body, address}, &res2);
    if (res2.rows() > 1)
    {
      std::cout << "Unexpectedley got multiple results when searching for duplicates... ignoring" << std::endl;
      continue;
    }
    else if (res2.rows() == 1)
    {
      std::time_t epoch = date / 1000;
      std::cout << " * Found duplicate of message: " << std::endl
                << "   " << msgid << "|" << body << "|" << address << "|"
                << std::put_time(std::localtime(&epoch), "%F %T %z") << "|" << "|" << type << std::endl
                << "   in 'mms' table. Marking for deletion." << std::endl;
      ids_to_remove.push_back(msgid);
      continue;
    }
  }

  std::string ids_to_remove_str;
  for (uint i = 0; i < ids_to_remove.size(); ++i)
    ids_to_remove_str += bepaald::toString(ids_to_remove[i]) + ((i < ids_to_remove.size() - 1) ? "," : "");

  std::cout << std::endl << std::endl << "About to remove messages from 'sms' table with _id's = " << std::endl;
  std::cout << ids_to_remove_str << std::endl << std::endl;

  std::cout << "Deleting " << ids_to_remove.size() << " duplicates..." << std::endl;
  d_database.exec("DELETE FROM sms WHERE _id IN (" + ids_to_remove_str + ")");
  std::cout << "Deleted " << d_database.changed() << " entries" << std::endl;
}
*/


bool SignalBackup::sleepyh34d(std::string const &truncatedbackup, std::string const &pwd)
{

  // open truncated
  std::cout << "Opening truncated backup..." << std::endl;
  std::unique_ptr<SignalBackup> tf(new SignalBackup(truncatedbackup, pwd, false, false, false));
  if (!tf->ok())
  {
    std::cout << "Failed to read truncated backup file" << std::endl;
    return false;
  }

  std::cout << "Deleting sms/mms tables from complete backup" << std::endl;
  if (!d_database.exec("DELETE FROM sms") ||
      !d_database.exec("DELETE FROM mms")/* ||
      !d_database.exec("DELETE FROM part")*/)
  {
    std::cout << "Error deleting contents of sms/mms tables" << std::endl;
    return false;
  }
  //d_attachments.clear();

  // delete part entries from truncated which are already in target
  std::cout << "Deleting doubled part entries..." << std::endl;
  SqliteDB::QueryResults r;
  d_database.exec("SELECT _id FROM part", &r);
  std::string q = "DELETE FROM part WHERE _id IN (";
  for (uint i = 0; i < r.rows(); ++i)
    q += bepaald::toString(r.getValueAs<long long int>(i, 0)) + ((i == r.rows() - 1) ? ")" : ",");
  if (!tf->d_database.exec(q))
  {
    std::cout << "Error deleting part entries" << std::endl;
    return false;
  }

  // in truncated: remove part entries (and d_attachments[i]), that are
  // missing or are already present in target (complete) db
  std::cout << "Cleaning up part table/attachments..." << std::endl;
  SqliteDB::QueryResults results;
  tf->d_database.exec("SELECT _id,unique_id FROM part", &results);
  std::vector<std::pair<long long int, long long int>> missingdata;
  for (uint i = 0; i < results.rows(); ++i)
  {
    uint64_t rowid = results.getValueAs<long long int>(i, "_id");
    uint64_t uniqid = results.getValueAs<long long int>(i, "unique_id");

    if (tf->d_attachments.find({rowid, uniqid}) == tf->d_attachments.end()/* ||
        d_attachments.find({rowid, uniqid}) != d_attachments.end()*/)
      missingdata.emplace_back(std::make_pair(rowid, uniqid));
  }

  for (auto const &a : missingdata)
  {
    if (!tf->d_database.exec("DELETE FROM part WHERE _id = ? AND unique_id = ?", {a.first, a.second}))
      std::cout << "Warning failed to remove part entry with missing data" << std::endl;
  }

  std::vector<std::string> tables{"sms", "mms", "part"};
  for (std::string const &table : tables)
  {
    std::cout << "Importing " << table << " entries from truncated file ";
    //SqliteDB::QueryResults results;
    tf->d_database.exec("SELECT * FROM " + table, &results);
    // check if tf (which is newer) contains columns not existing in target
    uint idx = 0;
    while (idx < results.headers().size())
    {
      if (!d_database.tableContainsColumn(table, results.headers()[idx]))
      {
        std::cout << "  NOTE: Dropping column '" << table << "." << results.headers()[idx] << "' from truncated : Column not present in target database" << std::endl;
        if (results.removeColumn(idx))
          continue;
      }
      // else
      ++idx;
    }
    // import
    std::cout << " " << results.rows() << " entries." << std::endl;
    for (uint i = 0; i < results.rows(); ++i)
    {
      SqlStatementFrame newframe = buildSqlStatementFrame(table, results.headers(), results.row(i));
      if (!d_database.exec(newframe.bindStatement(), newframe.parameters()))
        std::cout << "Warning: Failed to import sqlstatement (" << table << ")" << std::endl;
    }
  }

  // check for unreferenced threads
  d_database.exec("SELECT DISTINCT thread_id FROM sms WHERE thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
  if (results.rows() > 0)
  {
    std::cout << "WARNING:" << " Found messages in thread not present in old (complete) database... dropping them!" << std::endl;
    d_database.exec("DELETE FROM sms WHERE thread_id NOT IN (SELECT DISTINCT _id FROM thread)");
  }
  d_database.exec("SELECT DISTINCT thread_id FROM mms WHERE thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
  if (results.rows() > 0)
  {
    std::cout << "WARNING:" << " Found messages in thread not present in old (complete) database... dropping them!" << std::endl;
    d_database.exec("DELETE FROM mms WHERE thread_id NOT IN (SELECT DISTINCT _id FROM thread)");
  }

  // check for unreferenced recipients
  d_database.exec("SELECT DISTINCT " + d_sms_recipient_id + " FROM sms WHERE " + d_sms_recipient_id + " NOT IN (SELECT DISTINCT _id FROM recipient)", &results);
  if (results.rows() > 0)
  {
    std::cout << "WARNING:" << " Found messages referencing recipient not present in old (complete) database... dropping them!" << std::endl;
    d_database.exec("DELETE FROM sms WHERE " + d_sms_recipient_id + " NOT IN (SELECT DISTINCT _id FROM recipient)");
  }
  d_database.exec("SELECT DISTINCT " + d_mms_recipient_id + " FROM mms WHERE " + d_mms_recipient_id + " NOT IN (SELECT DISTINCT _id FROM recipient)", &results);
  if (results.rows() > 0)
  {
    std::cout << "WARNING:" << " Found messages referencing recipient not present in old (complete) database... dropping them!" << std::endl;
    d_database.exec("DELETE FROM mms WHERE " + d_mms_recipient_id + " NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  // CHECK AND WARN FOR MENTIONS
  d_database.exec("SELECT mms." + d_mms_recipient_id + ",DATETIME(ROUND(mms." + d_mms_date_sent + " / 1000), 'unixepoch') AS 'date_sent',mms.thread_id,groups.title FROM mms LEFT JOIN thread ON thread._id == mms.thread_id LEFT JOIN recipient ON recipient._id == thread." + d_thread_recipient_id + " LEFT JOIN groups ON groups.group_id == recipient.group_id WHERE HEX(mms.body) LIKE '%EFBFBC%'", &results);
  if (results.rows() > 0)
  {
    std::cout << "WARNING" << " Mentions found! Probably a good idea to check these messages:" << std::endl;
    for (uint i = 0; i < results.rows(); ++i)
    {
      std::cout << " -  Group: " << results.valueAsString(i, "title") << std::endl;
      std::cout << "    Date : " << results.valueAsString(i, "date_sent") << std::endl;
    }
  }

  // and copy avatars and attachments.
  for (auto &att : tf->d_attachments)
    d_attachments.emplace(std::move(att));

  // update thread snippet and date and count
  updateThreadsEntries();

  d_database.exec("VACUUM");
  d_database.freeMemory();

  return true;
}

/*
  switch sender and recipient in single one-to-one conversation?
 */
bool SignalBackup::hiperfall(uint64_t t_id, std::string const &selfid)
{

  SqliteDB::QueryResults results;
  // get the recipient id's for the participants in this conversation
  long long int self_id = -1;
  if (selfid.empty())
  {
    self_id = scanSelf();
    if (self_id == -1)
    {
      std::cout << "Failed to determine recipient, please add option to specify backup owners own phone number, for example: `--setselfid \"+31612345678\"'" << std::endl;
      return false;
    }
  }
  else
  {
    if (!d_database.exec("SELECT _id FROM recipient WHERE phone = ?", selfid, &results))
    {
      std::cout << "ERROR. sorry" << std::endl;
      return false;
    }
    if (results.rows() != 1)
    {
      std::cout << "Error: Unexpected query results" << std::endl;
      return false;
    }
    self_id = results.getValueAs<long long int>(0, "_id");
  }
  long long int partner_id = -1;
  if (!d_database.exec("SELECT " + d_thread_recipient_id + " FROM thread WHERE _id = ?", t_id, &results))
  {
    std::cout << "ERROR. sorry" << std::endl;
    return false;
  }
  if (results.rows() != 1)
  {
    std::cout << "Error: Unexpected query results" << std::endl;
    return false;
  }
  partner_id = results.getValueAs<long long int>(0, d_thread_recipient_id);

  if (partner_id == self_id)
  {
    std::cout << "ERROR: Got same recipients for sender and receiver: " << self_id << " & " << partner_id << std::endl;
    return false;
  }

  std::cout << "Got recipients: " << self_id << " & " << partner_id << std::endl;

  // now switch them
  if (!d_database.exec("UPDATE recipient SET _id = ? WHERE _id = ?", {-partner_id, self_id}) ||
      !d_database.exec("UPDATE recipient SET _id = ? WHERE _id = ?", {self_id, partner_id}) ||
      !d_database.exec("UPDATE recipient SET _id = ? WHERE _id = ?", {partner_id, -partner_id}))
  {
    std::cout << "Failed to switch recipient id's" << std::endl;
    return false;
  }

  // since msl_ tables (messagesendlog) deal with sent messages, we should clear them
  // (they are received messages now).
  d_database.exec("DELETE FROM msl_payload");
  d_database.exec("DELETE FROM msl_recipient");
  d_database.exec("DELETE FROM msl_message");

  // makes no sense to keep this, user should just make new profiles
  if (d_database.containsTable("notification_profile"))
  {
    d_database.exec("DELETE FROM notification_profile");
    d_database.exec("DELETE FROM notification_profile_schedule");
    d_database.exec("DELETE FROM notification_profile_allowed_members");
  }

  // delete other threads
  d_database.exec("DELETE FROM sms WHERE thread_id IS NOT ?", t_id);
  d_database.exec("DELETE FROM mms WHERE thread_id IS NOT ?", t_id);
  d_database.exec("DELETE FROM thread WHERE _id IS NOT ?", t_id);
  cleanDatabaseByMessages();

  auto setType = [](uint64_t oldtype, uint64_t newtype) { return (oldtype & ~(static_cast<uint64_t> (0x1f))) + newtype; };

  // get min and max id from sms
  d_database.exec("SELECT MIN(_id),MAX(_id) FROM sms", &results);
  if (results.rows() != 1 ||
      !results.valueHasType<long long int>(0, 0) ||
      !results.getValueAs<long long int>(0, 1))
  {
    std::cout << "Error: Unexpected query results" << std::endl;
    return false;
  }

  uint64_t minsmsid = results.getValueAs<long long int>(0, 0);
  uint64_t maxsmsid = results.getValueAs<long long int>(0, 1);
  std::cout << minsmsid << " " << maxsmsid << std::endl;

  std::cout << "Switching sms entries..." << std::endl;
  for (uint i = minsmsid; i <= maxsmsid ; ++i)
  {
    if (!d_database.exec("SELECT * FROM sms WHERE _id = ?", i, &results))
    {
      std::cout << "Error: Query failed" << std::endl;
      return false;
    }
    if (results.rows() == 0)
      continue;
    if (results.rows() > 1)
    {
      std::cout << "Error: Unexpected query results" << std::endl;
      return false;
    }

    uint64_t type = results.getValueAs<long long int>(0, "type");
    using namespace std::string_literals;

    switch (type & 0x1F)
    {

      /*

        For incoming and outgoing calls, mostly only the type changes, but (at least on
        new entries) an incoming call that was not successful (not picked up), the
        notified_timestamp and reactions_last_seen are filled in. For incoming calls that
        do connect, these are empty (as with outgoing).

        When switching, I leave them unchanged (-1 and 0) as older entries, before these fields existed
        have this anyway, so the app should be able to deal with them.

        SELECT * from sms WHERE type BETWEEN 1 AND 3 ORDER BY  + d_sms_date_received  + ASC;
        _id|thread_id|address|address_device_id|person|date|date_sent|date_server|protocol|read|status|type|reply_path_present|delivery_receipt_count|subject|body|mismatched_identities|service_center|subscription_id|expires_in|expire_started|notified|read_receipt_count|unidentified|reactions|reactions_unread|reactions_last_seen|remote_deleted|notified_timestamp|server_guid|receipt_timestamp
        (outgoing, unsuccessful) 30|1|2|1||1633872620815|1633872620813|-1||1|-1|2||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1
        (outgoing, successful)   31|1|2|1||1633872637225|1633872637221|-1||1|-1|2||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1
        (incoming, missed)      32|1|2|1||1633872653837|1633872649947|-1||1|-1|3||0|||||-1|0|0|0|0|0||0|1633872655142|0|1633872654248||-1
        (incoming, accepted)    33|1|2|1||1633872661166|1633872661164|-1||1|-1|1||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1

        SIMILAR GOES FOR VIDEO CALLS
        (outgoing, unsuccessful) 35|1|2|1||1633873910158|1633873910157|-1||1|-1|11||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1
        (outgoing, successful)   36|1|2|1||1633873922647|1633873922644|-1||1|-1|11||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1
        (incoming, missed)      37|1|2|1||1633873937640|1633873934877|-1||1|-1| 8||0|||||-1|0|0|0|0|0||0|1633873939058|0|1633873937835||-1
        (incoming, rejected)    38|1|2|1||1633873946618|1633873946615|-1||1|-1| 8||0|||||-1|0|0|0|0|0||0|1633873947990|0|            0||-1
        (incoming, accepted)    39|1|2|1||1633873954061|1633873954058|-1||1|-1|10||0|||||-1|0|0|0|0|0||0|           -1|0|            0||-1


      */


      case Types::INCOMING_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::OUTGOING_CALL_TYPE);
        d_database.exec("UPDATE sms SET type = ? WHERE _id IS ?", {newtype, i});
        break;
      }
      case Types::OUTGOING_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::INCOMING_CALL_TYPE);
        d_database.exec("UPDATE sms SET type = ? WHERE _id IS ?", {newtype, i});
        break;
      }
      case Types::MISSED_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::OUTGOING_CALL_TYPE);
        if (!d_database.exec("UPDATE sms SET"
                             " type = ?,"
                             " reactions_last_seen = ?,"
                             " notified_timestamp = ?"
                             " WHERE _id = ?",
                             {newtype, -1, 0, i}))
          return false;
        break;
      }
      case Types::JOINED_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'JOINED_TYPE'" << std::endl;
        break;
      }
      case Types::UNSUPPORTED_MESSAGE_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'UNSUPPORTED_MESSAGE_TYPE'" << std::endl;
        break;
      }
      case Types::INVALID_MESSAGE_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'INVALID_MESSAGE_TYPE'" << std::endl;
        break;
      }
      case Types::PROFILE_CHANGE_TYPE:
      {
        // incoming profile change messages are not present for sender
        d_database.exec("DELETE FROM sms WHERE _id = ?", i);
        break;
      }
      case Types::MISSED_VIDEO_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::OUTGOING_VIDEO_CALL_TYPE);
        if (!d_database.exec("UPDATE sms SET"
                             " type = ?,"
                             " reactions_last_seen = ?,"
                             " notified_timestamp = ?"
                             " WHERE _id = ?",
                             {newtype, -1, 0, i}))
          return false;
        break;
      }
      case Types::GV1_MIGRATION_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'GV1_MIGRATION_TYPE'" << std::endl;
        // should not be present in 1-on-1 threads
        break;
      }
      case Types::INCOMING_VIDEO_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::OUTGOING_VIDEO_CALL_TYPE);
        d_database.exec("UPDATE sms SET type = ? WHERE _id IS ?", {newtype, i});
        break;
      }
      case Types::OUTGOING_VIDEO_CALL_TYPE:
      {
        uint64_t newtype = setType(type, Types::INCOMING_VIDEO_CALL_TYPE);
        d_database.exec("UPDATE sms SET type = ? WHERE _id IS ?", {newtype, i});
        break;
      }
      case Types::GROUP_CALL_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'GROUP_CALL_TYPE'" << std::endl;
        break;
      }
      case Types::BASE_INBOX_TYPE:
      {

        /*
          incoming to outgoing message changes:
          date_server -> -1
          protocol    -> NULL
          type        -> |0x1f -> 23
          reply_path_present -> NULL
          delivery_receipt_count -> 1
          service_center -> NULL
          reactions_last_seen -> -1
          notified_timestamp -> -1
          server_guid -> NULL
          receipt_timestamp -> -1(old) ~(date+300)(new)

          NOTE, I use the old value for receipt_timestamp of -1. All messages that were in the db before this field existed are -1
          so the app should be able to deal with this properly
        */

        uint64_t newtype = setType(type, Types::BASE_SENT_TYPE);

        if (d_database.tableContainsColumn("sms", "protocol") &&
            d_database.tableContainsColumn("sms", "service_center") &&
            d_database.tableContainsColumn("sms", "reply_path_present"))        // REMOVED IN DBV166
        {
          if (!d_database.exec("UPDATE sms SET"
                               " date_server = ?,"
                               " protocol = ?,"
                               " type = ?,"
                               " reply_path_present = ?,"
                               " delivery_receipt_count = ?,"
                               " service_center = ?,"
                               " reactions_last_seen = ?,"
                               " notified_timestamp = ?,"
                               " server_guid = ?"
                               //" receipt_timestamp = ?"
                               " WHERE _id = ?",
                               {-1, nullptr, newtype, nullptr, 1, nullptr, -1, -1, nullptr, i}))
            return false;
        }
        else
        {
          if (!d_database.exec("UPDATE sms SET"
                               " date_server = ?,"
                               " type = ?,"
                               " delivery_receipt_count = ?,"
                               " reactions_last_seen = ?,"
                               " notified_timestamp = ?,"
                               " server_guid = ?"
                               //" receipt_timestamp = ?"
                               " WHERE _id = ?",
                               {-1, newtype, 1, -1, -1, nullptr, i}))
            return false;
        }
        break;
      }
      case Types::BASE_OUTBOX_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'BASE_OUTBOX_TYPE'" << std::endl;
        break;
      }
      case Types::BASE_SENDING_TYPE:
      {
        /*
          Not sure what to do with this. Lets say it was eventually successfully sent
          sometime after this backup was made...

          not sure about all the fields, I don't have this type in my db's
        */

        uint64_t newtype = setType(type, Types::BASE_INBOX_TYPE);
        if (d_database.tableContainsColumn("sms", "protocol") &&
            d_database.tableContainsColumn("sms", "service_center") &&
            d_database.tableContainsColumn("sms", "reply_path_present"))        // REMOVED IN DBV166
        {
          if (!d_database.exec("UPDATE sms SET"
                               //" date_server = ?,"
                               " protocol = ?,"
                               " type = ?,"
                               " reply_path_present = ?,"
                               " delivery_receipt_count = ?,"
                               //" reactions_last_seen = ?,"
                               //" notified_timestamp = ?,"
                               //" server_guid = ?,"
                               //" receipt_timestamp = ?,"
                               " service_center = ?"
                               " WHERE _id = ?",
                               {31337, newtype, 1, 0, "GCM"s, i}))
            return false;
        }
        else
        {
          if (!d_database.exec("UPDATE sms SET"
                               //" date_server = ?,"
                               " type = ?,"
                               " delivery_receipt_count = ?,"
                               //" reactions_last_seen = ?,"
                               //" notified_timestamp = ?,"
                               //" server_guid = ?,"
                               //" receipt_timestamp = ?,"
                               " WHERE _id = ?",
                               {newtype, 0, i}))
            return false;
        }
        break;
      }
      case Types::BASE_SENT_TYPE:
      {

        /*
          outgoing to incoming message changes:
          date_server -> -1(old) ~(date-1000)(new)
          protocol -> 31337
          type -> |0x1f -> 20
          reply_path_present -> 1
          delivery_receipt_count -> 0
          service_center -> GCM
          reactions_last_seen -> -1(old) ~(date+40000)(new)
          notified_timestamp -> 0(old) ~(date+150)(new)
          server_guid -> NULL(old) Something(new)
          receipt_timestamp -> -1

          NOTE, I use the old values where available. All messages that were in the db before these fields existed are -1 (or 0)
          so the app should be able to deal with this properly
        */

        uint64_t newtype = setType(type, Types::BASE_INBOX_TYPE);
        if (d_database.tableContainsColumn("sms", "protocol") &&
            d_database.tableContainsColumn("sms", "reply_path_present") &&
            d_database.tableContainsColumn("sms", "service_center")) // removed in dbv 166
        {
          if (!d_database.exec("UPDATE sms SET"
                               //" date_server = ?,"
                               " protocol = ?,"
                               " type = ?,"
                               " reply_path_present = ?,"
                               " delivery_receipt_count = ?,"
                               //" reactions_last_seen = ?,"
                               //" notified_timestamp = ?,"
                               //" server_guid = ?,"
                               //" receipt_timestamp = ?,"
                               " service_center = ?"
                               " WHERE _id = ?",
                               {31337, newtype, 1, 0, "GCM"s, i}))
            return false;
        }
        else
        {
          if (!d_database.exec("UPDATE sms SET"
                               " type = ?,"
                               " delivery_receipt_count = ?,"
                               " WHERE _id = ?",
                               {newtype, 0, i}))
            return false;
        }
        break;
      }
      case Types::BASE_SENT_FAILED_TYPE:
      {
        // failed sent message is not present at receiver?
        d_database.exec("DELETE FROM sms WHERE _id = ?", i);
        break;
      }
      case Types::BASE_PENDING_SECURE_SMS_FALLBACK:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'BASE_PENDING_SECURE_SMS_FALLBACK'" << std::endl;
        break;
      }
      case Types::BASE_PENDING_INSECURE_SMS_FALLBACK:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'BASE_PENDING_INSECURE_SMS_FALLBACK'" << std::endl;
        break;
      }
      case Types::BASE_DRAFT_TYPE:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << " : 'BASE_DRAFT_TYPE'" << std::endl;
        break;
      }
      default:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << std::endl;
        break;
      }
    }
  }

  // get min and max id from mms
  d_database.exec("SELECT MIN(_id),MAX(_id) FROM mms", &results);
  if (results.rows() != 1 ||
      !results.valueHasType<long long int>(0, 0) ||
      !results.getValueAs<long long int>(0, 1))
  {
    std::cout << "Error: Unexpected query results" << std::endl;
    return false;
  }

  uint64_t minmmsid = results.getValueAs<long long int>(0, 0);
  uint64_t maxmmsid = results.getValueAs<long long int>(0, 1);
  std::cout << minmmsid << " " << maxmmsid << std::endl;

  std::cout << "Switching mms entries..." << std::endl;
  for (uint i = minmmsid; i <= maxmmsid ; ++i)
  {
    if (!d_database.exec("SELECT * FROM mms WHERE _id = ?", i, &results))
    {
      std::cout << "Error: Query failed" << std::endl;
      return false;
    }
    if (results.rows() == 0)
      continue;
    if (results.rows() > 1)
    {
      std::cout << "Error: Unexpected query results" << std::endl;
      return false;
    }

    uint64_t type = results.getValueAs<long long int>(0, d_mms_type);
    using namespace std::string_literals;

    switch (type & 0x1F)
    {
      case Types::BASE_INBOX_TYPE:
      {
        uint64_t newtype = setType(type, Types::BASE_SENT_TYPE);
        if (!d_database.exec("UPDATE mms SET"
                             " date_server = ?,"
                             " " + d_mms_type + " = ?,"
                             " m_type = ?,"
                             " st = ?,"
                             " delivery_receipt_count = ?,"
                             " reactions_last_seen = ?,"
                             " notified_timestamp = ?,"
                             " server_guid = ?"
                             //" receipt_timestamp = ?"
                             " WHERE _id = ?",
                             {-1, newtype, 128, nullptr, 1, -1, 0, nullptr, i}))
          return false;
        break;
      }
      case Types::BASE_SENT_TYPE:
      {

        /*
         */

        uint64_t newtype = setType(type, Types::BASE_INBOX_TYPE);
        if (!d_database.exec("UPDATE mms SET"
                             " " + d_mms_type + " = ?,"
                             " m_type = ?,"
                             " st = ?,"
                             " delivery_receipt_count = ?,"
                             " receipt_timestamp = ?"
                             " WHERE _id = ?",
                             {newtype, 132, 1, 0, -1, i}))
          return false;
        break;
      }
      case Types::BASE_SENT_FAILED_TYPE:
      {
        // failed sent message is not present at receiver?
        d_database.exec("DELETE FROM mms WHERE _id = ?", i);
        break;
      }
      default:
      {
        std::cout << "Unhandled type: " << i << " " << (type & 0x1f) << std::endl;
        break;
      }
    }
  }


  return true;

  /*
    Mapping types:
    *20 (incoming)            ->  23 (sent)
    *23 (sent)                ->  20 (incoming)
    22 (sending)              ->  ???
    *1 (incoming audio call)  ->  2 (outgoing audio call)
    *2 (outgoind audio call)  ->  1 (incoming audio call)
    *3 (missed call)          ->  2 (outgoing audio call)?
    *7 (profile change)       ->  REMOVE?
    *8 (missed video call)    ->  11 (outgoing video call)?
    *10 (incoming video call) ->  11 (outgoing video call)
    *11 (outgoing video call) ->  10 (incoming video call)?
    *24 (sent_failed)         ->  ???
    */

}

void SignalBackup::scanMissingAttachments() const
{

  // get 'missing' attachments
  SqliteDB::QueryResults res;
  d_database.exec("SELECT _id,unique_id FROM part", &res);
  std::vector<std::pair<long long int, long long int>> missing;
  for (uint i = 0; i < res.rows(); ++i)
    if (d_attachments.find({res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")}) == d_attachments.end())
      missing.emplace_back(std::make_pair(res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")));

  std::cout << "Got " << missing.size() << " attachments with data not found" << std::endl;

  for (uint i = 0; i < missing.size(); ++i)
  {
    std::cout << "Checking " << (i + 1) << " of " << missing.size() << ": " << std::flush << missing[i].first << "," << missing[i].second << "... ";

    d_database.exec("SELECT _id FROM mms WHERE quote_missing = 1 AND _id IN (SELECT mid FROM part WHERE quote IS 1 AND mid = ?)", missing[i].first, &res);
    if (res.rows())
    {
      std::cout << "OK, EXPECTED (quote missing)" << std::endl;
      continue;
    }

    // quote_missing is not always (often not?) set to 1 even if quote is missing, so manually check
    d_database.exec("SELECT _id FROM mms WHERE remote_deleted IS 1 AND " + d_mms_date_sent + " IN (SELECT quote_id FROM mms WHERE _id IN "
                    "(SELECT mid FROM part WHERE _id = ? AND unique_id = ? AND quote = 1))",
                    {missing[i].first, missing[i].second}, &res);
    if (res.rows())
    {
      std::cout << "OK, EXPECTED (original message missing)" << std::endl;
      continue;
    }

    d_database.exec("SELECT ct FROM part WHERE quote = 1 AND _id = ? AND unique_id = ? AND ct NOT LIKE 'image%' AND ct NOT LIKE 'video%'", {missing[i].first, missing[i].second}, &res);
    if (res.rows())
    {
      std::cout << "OK, EXPECTED (type = " << res.valueAsString(0, 0) << ")" << std::endl;
      continue;
    }

    d_database.exec("SELECT pending_push FROM part WHERE pending_push IS NOT 0 AND _id = ? AND unique_id = ?", {missing[i].first, missing[i].second}, &res);
    if (res.rows())
    {
      std::cout << "OK, EXPECTED (pending_push = " << res.valueAsString(0, 0) << ")" << std::endl;
      continue;
    }

    std::cout << "UNEXPECTED! details:" << std::endl;
    d_database.exec("SELECT quote,ct,pending_push FROM part WHERE _id = ? AND unique_id = ?", {missing[i].first, missing[i].second}, &res);
    res.prettyPrint();
  }

}

void SignalBackup::devCustom() const
{
  SqliteDB::QueryResults res;
  // d_database.exec("SELECT body FROM sms where _id = 120", &res);
  // DecryptedGroupV2Context sts(res.valueAsString(0, "body"));
  // sts.print();

  std::set<std::string> uuids;

  using namespace std::string_literals;
  for (auto const &q : {"SELECT body FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0"s, "SELECT body FROM mms WHERE (" + d_mms_type + " & ?) != 0 AND (" + d_mms_type + " & ?) != 0"s})
  {
    //d_database.exec("SELECT body FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0",
    d_database.exec(q, {Types::GROUP_UPDATE_BIT, Types::GROUP_V2_BIT}, &res);


    for (uint i = 0; i < res.rows(); ++i)
    {
      DecryptedGroupV2Context sts2(res.valueAsString(i, "body"));
      //std::cout << "STATUS MSG " << i << std::endl;

      // NEW DATA
      auto field3 = sts2.getField<3>();
      if (field3.has_value())
      {
        auto field3_7 = field3->getField<7>();
        for (uint j = 0; j < field3_7.size(); ++j)
        {
          auto field3_7_1 = field3_7[j].getField<1>();
          if (field3_7_1.has_value())
            uuids.insert(bepaald::bytesToHexString(*field3_7_1, true));
          // else
          // {
          //   std::cout << "No members found in field 3" << std::endl;
          //   sts2.print();
          // }
        }
        // if (field3_7.size() == 0)
        // {
        //   std::cout << "No members found in field 3" << std::endl;
        //   sts2.print();
        // }
      }
      // else
      // {
      //   std::cout << "No members found in field 3" << std::endl;
      //   sts2.print();
      // }

      // OLD DATA?
      auto field4 = sts2.getField<4>();
      if (field4.has_value())
      {
        auto field4_7 = field4->getField<7>();
        for (uint j = 0; j < field4_7.size(); ++j)
        {
          auto field4_7_1 = field4_7[j].getField<1>();
          if (field4_7_1.has_value())
            uuids.insert(bepaald::bytesToHexString(*field4_7_1, true));
          // else
          // {
          //   std::cout << "No members found in field 4" << std::endl;
          //   sts2.print();
          // }
        }
        // if (field4_7.size() == 0)
        // {
        //   std::cout << "No members found in field 4" << std::endl;
        //   sts2.print();
        // }
      }
      // else
      // {
      //   std::cout << "No members found in field 4" << std::endl;
      //   sts2.print();
      // }
    }
  }

  std::cout << "LIST OF FOUND UUIDS:" << std::endl;
  for (auto &uuid : uuids)
    std::cout << uuid << std::endl;

  if (uuids.size())
  {
    std::string q = "SELECT DISTINCT _id FROM recipient WHERE LOWER(uuid) IN (";
#if __cplusplus > 201703L
    for (int pos = 0; std::string uuid : uuids)
#else
    int pos = 0;
    for (std::string uuid : uuids)
#endif
    {
      if (pos > 0)
        q += ", ";

      uuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      //std::transform(uuid.begin(), uuid.end(), uuid.begin(), [](unsigned char c){ return std::tolower(c); });
      q += "LOWER('" + uuid + "')";
      //std::transform(uuid.begin(), uuid.end(), uuid.begin(), [](unsigned char c){ return std::toupper(c); });
      //q+= "'" + uuid + "'";
      ++pos;
    }
    q += ")";


    std::cout << "'" << q << "'" << std::endl;
    d_database.exec(q, &res);
    res.prettyPrint();
  }

}
