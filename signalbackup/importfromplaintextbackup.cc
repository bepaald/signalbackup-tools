/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

//#include <chrono>

#include "../signalplaintextbackupdatabase/signalplaintextbackupdatabase.h"
#include "../signalplaintextbackupattachmentreader/signalplaintextbackupattachmentreader.h"
#include "../msgtypes/msgtypes.h"

bool SignalBackup::importFromPlaintextBackup(std::unique_ptr<SignalPlaintextBackupDatabase> const &ptdb, bool skipmessagereorder,
                                             std::vector<std::pair<std::string, long long int>> const &initial_contactmap,
                                             std::vector<std::string> const &daterangelist, std::vector<std::string> const &chats,
                                             bool createmissingcontacts, bool markdelivered, bool markread, bool autodates,
                                             std::string const &selfphone)
{
  if (d_selfid == -1)
  {
    d_selfid = selfphone.empty() ? scanSelf() : d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
    if (d_selfid == -1)
    {
      Logger::error_start("Failed to determine id of 'self'.");
      if (selfphone.empty())
        Logger::message_start(" Please pass `--setselfid \"[phone]\"' to set it manually");
      Logger::message_end();
      return false;
    }
    if (d_selfuuid.empty())
      d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));
  }

  if (!ptdb->ok())
  {
    Logger::error("Failed to open Signal Plaintext Backup database");
    return false;
  }

  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (unsigned int i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});

  // set daterange automatically
  if (dateranges.empty() && autodates)
  {
    SqliteDB::QueryResults res;
    if ((d_database.containsTable("sms") &&
         !d_database.exec("SELECT MIN(mindate) FROM (SELECT MIN(sms." + d_sms_date_received + ", " + d_mms_table + ".date_received) AS mindate FROM sms "
                          "LEFT JOIN " + d_mms_table + " WHERE sms." + d_sms_date_received + " IS NOT NULL AND " + d_mms_table + ".date_received IS NOT NULL)", &res))
        ||
        (!d_database.containsTable("sms") &&
         !d_database.exec("SELECT MIN(" + d_mms_table + ".date_received) AS mindate, MAX(" + d_mms_table + ".date_received) AS maxdate FROM " + d_mms_table + " WHERE " + d_mms_table + ".date_received IS NOT NULL", &res)))
    {
      Logger::error("Failed to automatically determine data-range");
      return false;
    }
    dateranges.push_back({"0", res.valueAsString(0, "mindate")});
    dateranges.push_back({res.valueAsString(0, "maxdate"), bepaald::toString(std::numeric_limits<long long int>::max())});
  }

  std::string datewhereclause;
  for (unsigned int i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      Logger::error("Skipping range: '", dateranges[i].first, " - ", dateranges[i].second, "'. Failed to parse or invalid range.");
      continue;
    }
    Logger::message("  Using range: ", dateranges[i].first, " - ", dateranges[i].second, " (", startrange, " - ", endrange, ")");

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    datewhereclause += (datewhereclause.empty() ? " WHERE (" : " OR ") + "date BETWEEN "s + bepaald::toString(startrange) + " AND " + bepaald::toString(endrange);
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }

  std::string chatselectionclause;
  if (!chats.empty())
  {
    chatselectionclause += (datewhereclause.empty() ? " WHERE address IN (" : " AND address IN (");
    for (unsigned int i = 0; i < chats.size(); ++i)
      chatselectionclause += "'" + chats[i] + (i < chats.size() - 1 ? "', " : "')");
  }

  /*
    contactmap:
    SELECT address,max(contact_name) FROM smses GROUP BY address ORDER BY address;
   */
  std::map<std::string, long long int> contactmap(initial_contactmap.begin(), initial_contactmap.end());

  // READ always seems to be 1....
  //ptdb->d_database.prettyPrint(true, "SELECT DISTINCT type, read FROM smses");
  // in signal android backup, all messages are read as well, only old versions of edited are not...

  //ptdb->d_database.saveToFile("plainttext.sqlite");

  SqliteDB::QueryResults pt_messages;
  if (!ptdb->d_database.exec("SELECT "
                             "rowid, "
                             "date, type, read, body, contact_name, address, numattachments, COALESCE(sourceaddress, address) AS sourceaddress, numaddresses, ismms, skip "
                             "FROM smses" + datewhereclause + chatselectionclause + " ORDER BY date", &pt_messages))
    return false;

  //pt_messages.prettyPrint(d_truncate);

  bool warned_createcontacts = false;

  //auto t1 = std::chrono::high_resolution_clock::now();
  for (unsigned int i = 0; i < pt_messages.rows(); ++i)
  {
    if (i % 100 == 0)
      Logger::message_overwrite("Importing messages into backup... ", i, "/", pt_messages.rows());

    if (pt_messages.valueAsInt(i, "skip") == 1) [[unlikely]]
      continue;

    std::string body = pt_messages(i, "body");
    if (body.empty() && pt_messages.valueAsInt(i, "numattachments") <= 0)
    {
      Logger::warning("Not inserting message with empty body and no attachments.");
      continue;
    }

    std::string pt_messages_address = pt_messages(i, "address");
    std::string pt_messages_sourceaddress = pt_messages(i, "sourceaddress");
    std::string pt_messages_contact_name = pt_messages(i, "contact_name");

    bool isgroup = false;
    if (std::string::size_type pos = pt_messages_address.find('~');
        pos != std::string::npos &&
        pos != 0 &&
        pos != pt_messages_address.size() - 1)
      isgroup = true;

    // match phone number to thread recipient_id
    long long int trid = -1;
    if (auto it = contactmap.find(pt_messages_address); it != contactmap.end())
      trid = it->second;
    else
    {
      trid = getRecipientIdFromName(pt_messages_contact_name, false);
      if (trid != -1)
        contactmap[pt_messages_address] = trid;
      else // try by phone number...
      {
        // this can go wrong these days. When an old contact is no longer on signal, it
        // is possible the database has 2 entries for this contact, one with nothing but
        // phone number (from the system address book, possibly not a valid signal contact),
        // and one with names/aci/pni etc (which, while no longer registered, is valid).
        //
        // since the xml only has e164 to match, it will match the former (which is possibly
        // not a valid signal contact and likely causes problems when restoring)
        trid = getRecipientIdFromPhone(pt_messages_address, false);
        if (trid != -1)
          contactmap[pt_messages_address] = trid;
      }
    }

    if (trid == -1 && createmissingcontacts)
      if ((trid = ptCreateRecipient(ptdb, &contactmap, &warned_createcontacts, pt_messages_contact_name,
                                   pt_messages_address, isgroup)) == -1)
        continue;

    // get matching thread
    long long int tid = getThreadIdFromRecipient(trid);
    if (tid == -1)
    {
      // create thread
      Logger::message_start("Failed to find matching thread for conversation, creating. (e164: ", pt_messages_address, " -> ", trid);
      std::any new_thread_id;
      if (!insertRow("thread",
                     {{d_thread_recipient_id, trid},
                      {"active", 1}},
                     "_id", &new_thread_id))
      {
        Logger::message_end();
        Logger::error("Failed to create thread for conversation. Skipping message.");
        continue;
      }
      tid = std::any_cast<long long int>(new_thread_id);
      Logger::message_end(" -> thread_id: ", tid, ")");
    }

    long long int rid = -1;
    if (auto it = contactmap.find(pt_messages_sourceaddress); it != contactmap.end()) [[likely]]
      rid = it->second;
    if (rid == -1)
    {
      Logger::error("Failed to find source_recipient_id in contactmap. Should be present at this point. Skipping message");
      continue;
    }


    //std::cout << pt_messages(i, "address") << "/" << pt_messages(i, "contact_name") << " : " << trid << "/" << getNameFromRecipientId(trid) << std::endl;

    /* XML type : 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox, 5 = Failed, 6 = Queued */
    bool incoming;
    switch (pt_messages.valueAsInt(i, "type", -1))
    {
      case 1:
        incoming = true;
        break;
      case 2:
      case 4: // ?
        incoming = false;
        break;
      case 3:
      case 5:
      case 6:
      default:
        Logger::warning("Unsupported message type (", pt_messages.valueAsInt(i, "type", -1), "). Skipping...");
        continue;
    }

    if (!unescapeXmlString(&body)) [[unlikely]]
      Logger::warning("Failed to escape message body: '", body, "'");

    // insert?

    // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
    // we try to get the first free date_sent
    long long int originaldate = pt_messages.valueAsInt(i, "date", -1);
    if (originaldate == -1)
    {
      Logger::error("Failed to get message date. Skipping...");
      continue;
    }

    //std::cout << "Get free date for message: '" << body << "'" << std::endl;
    long long int freedate = getFreeDateForMessage(originaldate, tid, incoming ? trid : d_selfid);
    if (freedate == -1)
    {
      if (d_verbose) [[unlikely]] Logger::message_end();
      Logger::error("Getting free date for inserting message into mms. Skipping...");
      continue;
    }

    std::any newid;
    if (!insertRow(d_mms_table,
                   {{"thread_id", tid},
                    {d_mms_date_sent, freedate},
                    {"date_received", freedate},
                    {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                    {"body", body},
                    {"read", 1}, // defaults to 0, but causes tons of unread message notifications
                    {d_mms_delivery_receipts, (incoming ? 0 : (markdelivered ? 1 : 0))},
                    {d_mms_read_receipts, (incoming ? 0 : (markread ? 1 : 0))},
                    {d_mms_recipient_id, incoming ? (isgroup ? rid : trid) : d_selfid}, // FROM_RECIPIENT_ID
                    {"to_recipient_id", incoming ? d_selfid : trid},
                    {"m_type", incoming ? 132 : 128}}, // dont know what this is, but these are the values...
                   "_id", &newid))
      Logger::warning("Failed to insert message");

    long long int new_msg_id = std::any_cast<long long int>(newid);
    if (pt_messages.valueAsInt(i, "numattachments", -1) > 0)
    {
      SqliteDB::QueryResults attachment_res;
      if (!ptdb->d_database.exec("SELECT data, filename, pos, size, ct, cl FROM attachments WHERE mid = ?", pt_messages.value(i, "rowid"), &attachment_res))
        continue;

      //attachment_res.prettyPrint(false);

      for (unsigned int j = 0; j < attachment_res.rows(); ++j)
      {
        std::string data = attachment_res(j, "data");
        std::string file = attachment_res(j, "filename");
        long long int pos = attachment_res.valueAsInt(j, "pos", -1);
        long long int size = attachment_res.valueAsInt(j, "size", -1);
        std::string ct = attachment_res(j, "ct");
        std::string cl = attachment_res(j, "cl");

        // get attachment metadata
        SignalPlainTextBackupAttachmentReader ptar(data, file, pos, size);
#if __cpp_lib_out_ptr >= 202106L
        std::unique_ptr<unsigned char[]> att_data;
        if (ptar.getAttachmentData(std::out_ptr(att_data), d_verbose) != 0)
#else
        unsigned char *att_data = nullptr; // !! NOTE RAW POINTER
        if (ptar.getAttachmentData(&att_data, d_verbose) != 0)
#endif
      {
        Logger::error("Failed to get attachment data");
        continue;
      }

#if __cpp_lib_out_ptr >= 202106L
      AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(std::string(), att_data.get(), ptar.dataSize()); // get metadata from heap
#else
      AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(std::string(), att_data,  ptar.dataSize());       // get metadata from heap
      if (att_data)
        delete[] att_data;
#endif

        // add entry to attachment table;
        std::any new_aid;
        if (!insertRow(d_part_table,
                       {{d_part_mid, new_msg_id},
                        {d_part_ct, ct},
                        {!cl.empty() ? "file_name" : "", cl},
                        {d_part_pending, 0},
                        {"data_size", amd.filesize},
                        {"voice_note", 0},
                        {"width", amd.width == -1 ? 0 : amd.width},
                        {"height", amd.height == -1 ? 0 : amd.height},
                        {"quote", 0},
                        {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), amd.hash},
                        {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), amd.hash},
                        {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), amd.hash}},
                       "_id", &new_aid))
          continue;
        long long int new_part_id = std::any_cast<long long int>(new_aid);

        DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
        if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                                                (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                                                                 "ATTACHMENTID:uint64:" + bepaald::toString(freedate) : ""),
                                                                                "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
        {
          new_attachment_frame->setReader(new SignalPlainTextBackupAttachmentReader(data, file, pos, size));
          d_attachments.emplace(std::make_pair(new_part_id,
                                               d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                               freedate : -1), new_attachment_frame.release());
        }
        else
        {
          Logger::error("Failed to create AttachmentFrame for data");
          Logger::error_indent("       rowid       : ", new_part_id);
          Logger::error_indent("       attachmentid: ", d_database.tableContainsColumn(d_part_table, "unique_id") ? freedate : -1);
          Logger::error_indent("       length      : ", amd.filesize);

          // try to remove the inserted part entry:
          d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
          continue;
        }
      }
    }

    // mark thread as active??
  }
  //auto t2 = std::chrono::high_resolution_clock::now();
  //auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  //std::cout << " *** TIME: " << ms_int.count() << "ms\n";

  Logger::message_overwrite("Importing messages into backup... ", pt_messages.rows(), "/", pt_messages.rows(), " done!", Logger::Control::ENDOVERWRITE);

  // count entities still present...
  //ptdb->d_database.exec("SELECT rowid,body,LENGTH(body) - LENGTH(REPLACE(body, '&', '')) AS entities FROM smses ORDER BY entities ASC");

  // sage to disk
  //ptdb->d_database.saveToFile("xmldb.sqlite");

  if (!skipmessagereorder) [[likely]]
    reorderMmsSmsIds();
  updateThreadsEntries();
  return checkDbIntegrity();
}
