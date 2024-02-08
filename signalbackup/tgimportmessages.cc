/*
  Copyright (C) 2024  Selwin van Dijk

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

bool SignalBackup::tgImportMessages(SqliteDB const &db, std::vector<std::pair<std::vector<std::string>, long long int>> const &contactmap,
                                    std::string const &datapath, std::string const &threadid, long long int chat_idx, bool prependforwarded,
                                    bool isgroup)
{
  // get recipient id for conversation
  auto find_in_contactmap = [&contactmap](std::string const &identifier) -> long long int
  {
    for (uint i = 0; i < contactmap.size(); ++i)
      for (uint j = 0; j < contactmap[i].first.size(); ++j)
        if (contactmap[i].first[j] == identifier)
          return contactmap[i].second;
    return -1;
  };

  long long int thread_recipient_id = -1;
  if ((thread_recipient_id = find_in_contactmap(threadid)) == -1)
  {
    Logger::error("Recipient id not found in contactmap");
    return false;
  }

  // get or create thread_id
  long long int thread_id = getThreadIdFromRecipient(thread_recipient_id);
  if (thread_id == -1)
  {
    Logger::warning_start("Failed to find matching thread for conversation, creating. (",
                          threadid, " (id: ", thread_recipient_id, ")");
    std::any new_thread_id;
    if (!insertRow("thread",
                   {{d_thread_recipient_id, thread_recipient_id},
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
  if (!db.exec("SELECT type, date, from_id, forwarded_from, body, id, reply_to_id, photo, width, height, file, media_type, mime_type, poll FROM messages "
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

      // prepend notice to forwarded message
      std::string bodyjson = message_data.valueAsString(i, "body");
      if (prependforwarded && !message_data.isNull(i, "forwarded_from"))
      {
        // Logger::message("Body json before: ", bodyjson);
        std::string fname = message_data(i, "forwarded_from");
        std::string tmp = db.getSingleResultAs<std::string>("SELECT json_array(json_object('type', 'italic', 'text', 'Forwarded from " + fname + ":'), "
                                                            "json_object('type', 'plain', 'text', '\n'), "
                                                            "json_extract('" + bodyjson +"', '$[0]'))", std::string());
        if (!tmp.empty())
          bodyjson = std::move(tmp);
        // Logger::message("Body json after: ", bodyjson);
      }

      // gather data
      std::string body = tgBuildBody(bodyjson);

      std::string fromid = message_data.valueAsString(i, "from_id");
      long long int from_recid = -1;
      if ((from_recid = find_in_contactmap(fromid)) == -1)
      {
        Logger::error("Recipient id not found in contactmap");
        return false;
      }

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
        Logger::message("Attachment-only message with same timestamp as previous: assuming attachment belongs to previous message");

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
                                   //{d_mms_delivery_receipts, (incoming ? 0 : 1)}, //
                                   //{d_mms_read_receipts, (incoming ? 0 : 1)}, //
                                   //{d_mms_viewed_receipts, (incoming ? 0 : 1)}, //
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
          if (d_verbose) [[unlikely]]
            Logger::message("Found quote: ", telegram_msg_id_to_adb_msg_id[quotemsg]);
          tgSetQuote(telegram_msg_id_to_adb_msg_id[quotemsg], new_msg_id);
        }
        else
          Logger::message("Message was wuote, but quoted message not found...");
      }

      // set body ranges
      if (!tgSetBodyRanges(bodyjson, new_msg_id))
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
