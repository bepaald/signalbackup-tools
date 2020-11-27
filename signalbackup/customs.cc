/*
    Copyright (C) 2019-2020  Selwin van Dijk

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
  SqlCipherDecryptor db(signaldesktoplocation);
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
    d_database.exec("SELECT body,date,address FROM mms WHERE thread_id = ?",
                    list_of_threads.value(i, 0), &message_data);
    //message_data.prettyPrint();

    bool matched = false;
    for (uint j = 0; j < message_data.rows(); ++j)
    {
      SqliteDB::QueryResults r2;
      desktopdb.exec("SELECT conversationId FROM messages WHERE sent_at == ? AND body == ?", {message_data.value(j, "date"), message_data.value(j, "body")}, &r2);
      if (r2.rows() == 1)
      {
        matched = true;
        matches.emplace_back(std::make_tuple(list_of_threads.getValueAs<long long int>(i, 0), message_data.valueAsString(j, "address"), r2.getValueAs<std::string>(0, "conversationId")));
      }

      if (matched)
        break;
    }
    if (!matched)
    {
      d_database.exec("SELECT body,date_sent,address FROM sms WHERE thread_id = ?",
                      list_of_threads.value(i, 0), &message_data);
      //message_data.prettyPrint();
      for (uint j = 0; j < static_cast<long long int>(message_data.rows()); ++j)
      {
        SqliteDB::QueryResults r2;
        desktopdb.exec("SELECT conversationId FROM messages WHERE sent_at == ? AND body == ?", {message_data.value(j, "date_sent"), message_data.value(j, "body")}, &r2);
        if (r2.rows() == 1)
        {
          matched = true;
          matches.emplace_back(std::make_tuple(list_of_threads.getValueAs<long long int>(i, 0), message_data.valueAsString(j, "address"), r2.getValueAs<std::string>(0, "conversationId")));
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
        "sms.date AS union_date, "
        "sms.date_sent AS union_display_date, "
        "sms.type AS union_type, "
        "sms.body AS union_body "
        "FROM sms WHERE " + list_of_threads.valueAsString(i, 0) + " = sms.thread_id "
        "UNION "
        "SELECT "
        "mms.date_received AS union_date, " // not sure for outgoing
        "mms.date AS union_display_date, "
        "mms.msg_box AS union_type, "
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
      thread.recipient_ids := [s|m]ms.address


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
           .members  := CONVERT(convertations.members FROM conversation.id -> recipient._id)
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
    d_database.exec("INSERT INTO thread (_id, recipient_ids) VALUES (?, ?)", {std::get<0>(matches[t]), std::get<1>(matches[t])});

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
    if (!d_database.exec("SELECT DISTINCT address FROM mms WHERE (msg_box & " + bepaald::toString(Types::BASE_TYPE_MASK) +
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
      d_database.exec("SELECT DISTINCT mms.address FROM mms UNION SELECT DISTINCT sms.address FROM sms", &list_of_addresses);
      // this is a stupid and naive way of looking for a free id, but it's quick to write :P
      long long int free_address = 1;
      bool done = false;
      while (!done)
      {
        bool found = false;
        for (uint i = 0; i < list_of_addresses.rows(); ++i)
        {
          if (list_of_addresses.valueAsString(i, "address") == bepaald::toString(free_address))
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
      group_recipient_id = group_rec.valueAsString(0, "address");

    // add entry in 'thread' database
    d_database.exec("INSERT INTO thread (_id, recipient_ids) VALUES (?, ?)", {std::get<0>(matches[t]), group_recipient_id});

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
    else if (res.valueHasType<std::nullptr_t>(i, "body"))
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
