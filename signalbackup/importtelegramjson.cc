/*
  Copyright (C) 2023-2024  Selwin van Dijk

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
  some known issues:

  - Overlapping text styles are not exported properly by telegram
  - Message delivery info might not be available in json
  - Message types other than 'message' (eg 'service') are currently skipped
  - underline style is not supported by signal
  - stickers turn into normal attachments?
  - 'forwarded from' is not an existing attribute in signal
  - message reaction are not exported by Telegram
  - poll-attachments skipped

 */

#include "signalbackup.ih"

#include "../jsondatabase/jsondatabase.h"

bool SignalBackup::importTelegramJson(std::string const &file, std::vector<long long int> const &chatselection,
                                      std::vector<std::pair<std::string, long long int>> contactmap, std::string const &selfphone)
{
  Logger::message("Import from Telegram json export");

  if (bepaald::isDir(file))
  {
    Logger::error("Did not get regular file as input");
    return false;
  }

  // check and warn about selfid
  d_selfid = selfphone.empty() ? scanSelf() : d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
  if (d_selfid == -1)
  {
    Logger::error("Failed to determine id of 'self'.",
                  (selfphone.empty() ? "Please pass `--setselfid \"[phone]\"' to set it manually" : ""));
    return false;
  }

  // set selfuuid
  d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));

  if (contactmap.size())
  {
    Logger::message("CONTACT MAP: ");
    for (uint i = 0; i < contactmap.size(); ++i)
      Logger::message(contactmap[i].first, " -> ", contactmap[i].second);
  }

  JsonDatabase jsondb(file, d_verbose);
  if (!jsondb.ok())
    return false;

  // get base path of file (we need it to set the resolve relative paths
  // referenced in the JSON data
  std::string datapath;
  std::filesystem::path p(file);
  datapath = p.parent_path().string() + static_cast<char>(std::filesystem::path::preferred_separator);

  // build chatlist:
  std::string chatlist;
  for (auto idx : chatselection)
    chatlist += (chatlist.empty() ? "(" : ", ") + bepaald::toString(idx);
  chatlist += (chatlist.empty() ? "" : ")");

  // get all contacts in json data
  SqliteDB::QueryResults json_contacts;
  if (!jsondb.d_database.exec("SELECT DISTINCT name FROM chats WHERE name IS NOT NULL " + (chatlist.empty() ? "" : ("AND idx IN " + chatlist + " ")) +
                              "UNION "
                              "SELECT DISTINCT from_name AS name FROM messages WHERE from_name IS NOT NULL" + (chatlist.empty() ? "" : (" AND chatidx IN " + chatlist)),
                              &json_contacts))
    return false;

  Logger::message("ALL CONTACTS: ");
  json_contacts.prettyPrint();

  std::vector<long long int> recipientsnotfound;

  for (uint i = 0; i < json_contacts.rows(); ++i)
  {
    std::string contact = json_contacts.valueAsString(i, "name");

    if (std::find_if(contactmap.begin(), contactmap.end(),
                     [contact](auto const &it){ return it.first == contact; }) != contactmap.end()) // we have a matching recipient (possibly user-supplied)
      continue;

    // find it in android db;
    long long int rec_id = getRecipientIdFromName(contact, false);

    if (rec_id == -1)
      recipientsnotfound.push_back(i);
    else
      contactmap.push_back({contact, rec_id});
  }

  if (!recipientsnotfound.empty())
  {
    Logger::error("The following contacts in the JSON input were not found in the Android backup:");
    for (auto const r : recipientsnotfound)
      Logger::error_indent(" - \"", json_contacts.valueAsString(r, "name"), "\"");

    Logger::message("Use `--mapjsoncontacts [NAME1]=[id1],[NAME2]=[id2],...' to map these to an existing recipient id \n"
                    "from the backup. The list of available recipients and their id's can be obtained by running \n"
                    "with `--listrecipients'.");

    return false;
  }

  Logger::message("CONTACT MAP: ");
  for (uint i = 0; i < contactmap.size(); ++i)
    Logger::message(contactmap[i].first, " -> ", contactmap[i].second);

  SqliteDB::QueryResults chats;
  if (!jsondb.d_database.exec("SELECT idx, name, type FROM chats" + (chatlist.empty() ? "" : (" WHERE idx IN " + chatlist)), &chats))
    return false;

  // for each chat, get the messages and insert
  for (uint i = 0; i < chats.rows(); ++i)
  {
    Logger::message("Dealing with conversation ", i + 1, "/", chats.rows());

    if (chats.valueAsString(i, "type") == "private_group" /*|| chats.valueAsString(i, "type") == "some_other_group"*/)
      tgImportMessages(jsondb.d_database, contactmap, datapath, chats.valueAsString(i, "name"), chats.valueAsInt(i, "idx"), true); // deal with group chat
    else if (chats.valueAsString(i, "type") == "personal_chat") // ????
      tgImportMessages(jsondb.d_database, contactmap, datapath, chats.valueAsString(i, "name"), chats.valueAsInt(i, "idx"), false); // deal with 1-on-1 convo
    else
    {
      Logger::warning("Unsupported chat type `", chats.valueAsString(i, "type"), "'. Skipping...");
      continue;
    }
  }

  reorderMmsSmsIds();
  updateThreadsEntries();
  return checkDbIntegrity();

  return false;
}

bool SignalBackup::tgImportMessages(SqliteDB const &db, std::vector<std::pair<std::string, long long int>> const &contactmap,
                                    std::string const &datapath, std::string const &threadname, long long int chat_idx, bool isgroup)
{
  // get recipient id for conversation
  auto it = std::find_if(contactmap.begin(), contactmap.end(),
                         [threadname](auto const &iter){ return iter.first == threadname; });
  if (it == contactmap.end())
  {
    Logger::error("Recipient id not found in contactmap");
    return false;
  }

  // save thread recipient
  long long thread_recipient_id = it->second;

  // get or create thread_id
  long long int thread_id = getThreadIdFromRecipient(thread_recipient_id);
  if (thread_id == -1)
  {
    Logger::warning_start("Failed to find matching thread for conversation, creating. (",
                          threadname, " (id: ", it->second, ")");
    std::any new_thread_id;
    if (!insertRow("thread",
                   {{d_thread_recipient_id, it->second},
                    {"active", 1},
                    {"archived", 0},
                    {"pinned", 0}},
                   "_id", &new_thread_id))
    {
      Logger::message_end();
      Logger::error("Failed to create thread for conversation.");
      return false;
    }
    //std::cout << "Raw any_cast 1" << std::endl;
    thread_id = std::any_cast<long long int>(new_thread_id);
    Logger::message(", thread_id: ", thread_id, ")");
  }

  // loop over messages from requested chat and insert
  SqliteDB::QueryResults message_data;
  if (!db.exec("SELECT type, date, from_name, body, id, reply_to_id, photo, width, height, file, media_type, mime_type, poll FROM messages "
               "WHERE chatidx = ? "
               "ORDER BY date ASC",
               chat_idx, &message_data))
    return false;

  // we save the android message id that was created from telegram message id to be able to
  // properly handle quotes...
  std::map<long long int, long long int> telegram_msg_id_to_adb_msg_id;

  // save timestamp of previous message (to merge messages with multiple attachments)
  std::pair<long long int, long long int> prevtimestamp_to_id;

  for (uint i = 0; i < message_data.rows(); ++i)
  {
    Logger::message("Dealing with message ", i + 1, "/", message_data.rows());

    if (!message_data.isNull(i, "poll"))
    {
      warnOnce("Message is 'poll'. This is not supported in Signal. Skipping...");
      continue;
    }

    if (message_data.valueAsString(i, "type") == "message")
    {
      // gather data
      std::string body = tgBuildBody(message_data.valueAsString(i, "body"));

      std::string from = message_data.valueAsString(i, "from_name");
      it = std::find_if(contactmap.begin(), contactmap.end(),
                        [from](auto const &iter){ return iter.first == from; });
      if (it == contactmap.end())
      {
        Logger::error("Recipient id not found in contactmap");
        return false;
      }
      long long int from_recid = it->second;

      bool incoming = from_recid != d_selfid;
      long long int address = !isgroup ? from_recid : (incoming ? from_recid : thread_recipient_id);

      // check if we need to merge message
      if (message_data.valueAsInt(i, "date") == prevtimestamp_to_id.first &&
          body.empty() &&
          message_data.isNull(i, "reply_to_id") &&
          (!message_data(i, "file").empty() || !message_data(i, "photo").empty()) &&
          from_recid == d_database.getSingleResultAs<long long int>("SELECT " + d_mms_recipient_id + " FROM " + d_mms_table + " WHERE _id = ?", prevtimestamp_to_id.second, -1) &&
          d_database.getSingleResultAs<long long int>("SELECT _id FROM " + d_part_table + " WHERE " + d_part_mid + " = ? LIMIT 1", prevtimestamp_to_id.second, -1) != -1)
      {
        Logger::message("Assuming attachment belongs to previous message");

        // add attachments
        tgSetAttachment(message_data, datapath, i, prevtimestamp_to_id.second);
        continue;
      }

      // make sure date/from/thread is available
      long long int date = message_data.valueAsInt(i, "date") * 1000;
      long long int freedate = getFreeDateForMessage(date, thread_id, incoming ? address : d_selfid);
      if (freedate == -1)
      {
        Logger::error("Getting free date for inserting message into mms");
        continue;
      }

      // insert message
      std::any retval;
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, freedate},
                                   {"date_received", freedate},
                                   {"date_server", freedate},
                                   {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                   {"body", body.empty() ? std::any(nullptr) : std::any(body)},
                                   {"read", 1}, // defaults to 0, but causes tons of unread message notifications
                                   //{"delivery_receipt_count", (incoming ? 0 : 0)}, // set later in setMessagedeliveryreceipts()
                                   //{"read_receipt_count", (incoming ? 0 : 0)},     //     "" ""
                                   {d_mms_recipient_id, incoming ? address : d_selfid},
                                   {"to_recipient_id", incoming ? d_selfid : address},
                                   {"m_type", incoming ? 132 : 128}, // dont know what this is, but these are the values...
                                   //{"quote_id", hasquote ? (bepaald::contains(adjusted_timestamps, mmsquote_id) ? adjusted_timestamps[mmsquote_id] : mmsquote_id) : 0},
                                   //{"quote_author", hasquote ? std::any(mmsquote_author) : std::any(nullptr)},
                                   //{"quote_body", hasquote ? mmsquote_body : nullptr},
                                   //{"quote_missing", hasquote ? mmsquote_missing : 0},
                                   //{"quote_mentions", hasquote ? std::any(mmsquote_mentions) : std::any(nullptr)},
                                   //{"shared_contacts", shared_contacts_json.empty() ? std::any(nullptr) : std::any(shared_contacts_json)},
                                   {"remote_deleted", 0},
                                   {"view_once", 0}}, // if !createrecipient -> this message was already skipped
                     "_id", &retval))
      {
        Logger::error("Failed to insert message");
        continue;
      }
      long long int new_msg_id = std::any_cast<long long int>(retval);

      // add attachments
      if (!tgSetAttachment(message_data, datapath, i, new_msg_id))
      {
        if (body.empty())
        {
          Logger::warning("Failed to set attachment on otherwise empty message. Deleting message...");
          d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id = ?", new_msg_id);
        }
        else
          Logger::warning("Failed to set attachment");
      }

      // save to map for quotes, we do this AFTER adding attachment, because that
      // may delete the new message on failure...
      telegram_msg_id_to_adb_msg_id[message_data.valueAsInt(i, "id")] = new_msg_id;

      // save message timestamp and id
      prevtimestamp_to_id = {message_data.valueAsInt(i, "date"), new_msg_id};

      // deal with quotes
      if (!message_data.isNull(i, "reply_to_id"))
      {
        long long int quotemsg = message_data.valueAsInt(i, "reply_to_id");
        if (bepaald::contains(telegram_msg_id_to_adb_msg_id, quotemsg))
        {
          Logger::message("Found quote: ", telegram_msg_id_to_adb_msg_id[quotemsg]);
          tgSetQuote(telegram_msg_id_to_adb_msg_id[quotemsg], new_msg_id);
        }
        else
        {
          // warn
          // std::cout << "Message was quote, but quoted message not found..." << std::endl;
        }
      }

      // set body ranges
      if (!tgSetBodyRanges(message_data.valueAsString(i, "body"), new_msg_id))
      {
        // warn?
        // error?
        // return false?
        continue;
      }


      if (d_database.getSingleResultAs<std::string>("SELECT body FROM " + d_mms_table + " WHERE _id = ?", new_msg_id, std::string()).empty() && // no message body
          d_database.getSingleResultAs<long long int>("SELECT _id FROM " + d_part_table + " WHERE " + d_part_mid + " = ?", new_msg_id, -1) == -1 && // no attachment
          d_database.getSingleResultAs<long long int>("SELECT quote_id FROM " + d_mms_table + " WHERE _id = ?", new_msg_id, 0) == 0)            // no quote
      {
        Logger::message("Maybe inserted empty message.");
        Logger::message("Data:");
        message_data.getRow(i).prettyPrint();
      }

    }
    // else if...
    else
    {
      warnOnce("Unsupported message type `" + message_data.valueAsString(i, "type") + "'. Skipping...");
      continue;
    }
  }

  return true;
}
