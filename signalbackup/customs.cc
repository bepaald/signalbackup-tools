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
  for (long long int i = 0; i < static_cast<long long int>(list_of_threads.rows()); ++i)
  {
    SqliteDB::QueryResults message_data;
    d_database.exec("SELECT body,date FROM mms WHERE thread_id = ?",
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
        std::cout << " - Got match for thread " << list_of_threads.valueAsString(i, 0) << std::endl;
        desktopdb.prettyPrint("SELECT name,profileName,profileFamilyName,profileFullName FROM conversations WHERE id == ?", r2.value(0, 0));
        std::cout << std::endl;
      }

      if (matched)
        break;
    }
    if (!matched)
    {
      d_database.exec("SELECT body,date_sent FROM sms WHERE thread_id = ?",
                      list_of_threads.value(i, 0), &message_data);
      //message_data.prettyPrint();
      for (uint j = 0; j < static_cast<long long int>(message_data.rows()); ++j)
      {
        SqliteDB::QueryResults r2;
        desktopdb.exec("SELECT conversationId FROM messages WHERE sent_at == ? AND body == ?", {message_data.value(j, "date_sent"), message_data.value(j, "body")}, &r2);
        if (r2.rows() == 1)
        {
          matched = true;
          std::cout << " - Got match for thread " << list_of_threads.valueAsString(i, 0) << ":" << std::endl;
          desktopdb.prettyPrint("SELECT name,profileName,profileFamilyName,profileFullName FROM conversations WHERE id == ?", r2.value(0, 0));
          std::cout << std::endl;
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

    /*
      mms: desktop.messages.sent_at == android.mms.date
      sms: desktop.messages.sent_at == android.sms.date_sent
     */

    // desktop messages:
    // SELECT id,unread,expires_at,sent_at,schemaVersion,conversationId,received_at,source,sourceDevice,hasAttachments,hasFileAttachments,hasVisualMediaAttachments,expireTimer,expirationStartTimestamp,type,body,messageTimer,messageTimerStart,messageTimerExpiresAt,isErased,isViewOnce,sourceUuid FROM messages WHERE body == "Test terug";
    //id|unread|expires_at|sent_at|schemaVersion|conversationId|received_at|source|sourceDevice|hasAttachments|hasFileAttachments|hasVisualMediaAttachments|expireTimer|expirationStartTimestamp|type|body|messageTimer|messageTimerStart|messageTimerExpiresAt|isErased|isViewOnce|sourceUuid
    //13c39ffe-67e2-43a5-911b-9537e485b75d|||1597327208579|10|82184df5-c89b-4c5c-a0c6-b7c9449b3818|1598179687083|31683616099|1|0|||||incoming|Test terug|||||0|6b7e6b80-c3d9-4701-8f36-bf5e22ebd62c


  }



  return true;
}


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
