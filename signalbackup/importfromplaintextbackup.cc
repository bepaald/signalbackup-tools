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

#include "../signalplaintextbackupdatabase/signalplaintextbackupdatabase.h"
#include "../signalplaintextbackupattachmentreader/signalplaintextbackupattachmentreader.h"
#include "../msgtypes/msgtypes.h"

bool SignalBackup::importFromPlaintextBackup(std::unique_ptr<SignalPlaintextBackupDatabase> const &ptdb, bool skipmessagereorder,
                                             std::vector<std::pair<std::string, long long int>> const &initial_contactmap,
                                             std::vector<std::string> const &daterangelist, std::vector<std::string> const &chats,
                                             bool createmissingcontacts, bool markdelivered, bool markread, bool autodates,
                                             std::string const &selfphone, bool isdummy)
{
  if (d_selfid == -1)
  {
    if (isdummy && !selfphone.empty()) // lets just create a recipient
    {
      d_selfid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
      if (d_selfid == -1)
      {
        // it is possible the contactname is set for this contact in ptdb through --mapxmlcontactnames
        std::string contact_name = ptdb->d_database.getSingleResultAs<std::string>("SELECT MAX(contact_name) FROM smses WHERE address = ? "
                                                                                   "AND contact_name IS NOT NULL AND contact_name IS NOT ''",
                                                                                   selfphone, std::string());

        std::any new_rid;
        if (!insertRow("recipient",
                       {{d_recipient_e164, selfphone},
                        {(contact_name.empty() ? "" : "profile_given_name"), contact_name},
                        {(contact_name.empty() ? "" : "profile_joined_name"), contact_name}},
                       "_id", &new_rid))
          return false;

        d_selfid = std::any_cast<long long int>(new_rid);
      }
    }
    else
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
  }

  if (!ptdb->ok())
  {
    Logger::error("Failed to open Signal Plaintext Backup database");
    return false;
  }

  // mms messages seem to usually omit the self-address, it is implied to be the sourceaddress when the message is outgoing,
  // or (one of the) targetaddresses when the message is incoming.
  std::string selfe164(selfphone);
  if (selfe164.empty())
    selfe164 = d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_e164 + " FROM recipient WHERE _id = ?", d_selfid, std::string());
  if (selfe164.empty())
    Logger::warning("Failed to get self phone");
  else
  {

    //ptdb->d_database.prettyPrint(false, "SELECT DISTINCT sourceaddress FROM smses WHERE ismms = 1 AND type = 2");
    // set source address to self for outgoing
    ptdb->d_database.exec("UPDATE smses SET sourceaddress = ? WHERE "
                          "NULLIF(sourceaddress, '') IS NULL AND ismms = 1 AND type = 2", selfe164);
    //ptdb->d_database.prettyPrint(false, "SELECT DISTINCT sourceaddress FROM smses WHERE ismms = 1 AND type = 2");

    //ptdb->d_database.prettyPrint(false, "SELECT DISTINCT targetaddresses FROM smses WHERE ismms = 1 AND type = 1");
    // set target address:
    ptdb->d_database.exec("UPDATE smses SET targetaddresses = "
                          "CASE "
                          "  WHEN targetaddresses IS NULL THEN json_array(?1)"
                          "  ELSE json_insert(targetaddresses, '$[#]', ?1) "
                          "END "
                          "WHERE ismms = 1 AND type = 1", selfe164);
    //ptdb->d_database.prettyPrint(false, "SELECT DISTINCT targetaddresses FROM smses WHERE ismms = 1 AND type = 1");

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
  // fill contactmap with known recipients:
  {
    SqliteDB::QueryResults recipient_results;
    if (d_database.exec("SELECT _id, " + d_recipient_e164 + " FROM recipient", &recipient_results))
      for (unsigned int i = 0; i < recipient_results.rows(); ++i)
        contactmap.emplace(recipient_results(i, "e164"), recipient_results.valueAsInt(i, "_id"));
  }

  /*
    threadmap
  */
  std::map<long long int, long long int> threadmap;

  // READ always seems to be 1....
  //ptdb->d_database.prettyPrint(true, "SELECT DISTINCT type, read FROM smses");
  // in signal android backup, all messages are read as well, only old versions of edited are not...

  //ptdb->d_database.saveToFile("plainttext.sqlite");

  SqliteDB::QueryResults pt_messages;
  if (!ptdb->d_database.exec("SELECT "
                             "rowid, "
                             "date, type, read, body, contact_name, address, numattachments, COALESCE(sourceaddress, address) AS sourceaddress, ismms, skip "
                             "FROM smses" + datewhereclause + chatselectionclause +
                             (datewhereclause.empty() && chatselectionclause.empty() ? " WHERE skip = 0" : " AND skip = 0") +
                             " ORDER BY date", &pt_messages))
    return false;

  //pt_messages.prettyPrint(d_truncate);

  bool warned_createcontacts = (isdummy ? true : false);

  // saves {rowid, {new_mms_id, unique_id}} for messages with attachments
  std::map<long long int, std::pair<long long int, long long int>> attachment_messages;

  //auto t1 = std::chrono::high_resolution_clock::now();
  for (unsigned int i = 0; i < pt_messages.rows(); ++i)
  {
    if (i % (pt_messages.rows() > 100 ? 100 : 1) == 0)
      Logger::message_overwrite("Importing messages into backup... ", i, "/", pt_messages.rows());//,
    //" (", pt_messages.valueAsInt(i, "ismms"), ",", pt_messages.valueAsInt(i, "type"), ")");

    std::string body = pt_messages(i, "body");
    if (body.empty() && pt_messages.valueAsInt(i, "numattachments") <= 0)
    {
      Logger::warning("Not inserting message with empty body and no attachments. (date: ", pt_messages.valueAsInt(i, "date", -1), ", rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
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
    {
      trid = it->second;
      if (d_verbose) [[unlikely]]
        Logger::message("Got trid from contactmap: ", makePrintable(pt_messages_address));
    }
    else
    {
      if (isdummy) /// only try by address (=phone). Names may be falsely doubled in the XML file
      {            ///
        if (isgroup)
          trid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE group_id = ?", "__signal_group__fake__" + pt_messages_address, -1);
        else
          trid = getRecipientIdFromPhone(pt_messages_address, false);

        if (trid != -1)
        {
          contactmap[pt_messages_address] = trid;
          if (d_verbose) [[unlikely]]
            Logger::message("Got trid from addres: ", makePrintable(pt_messages_address));
        }
      }
      else
      {
        trid = getRecipientIdFromName(pt_messages_contact_name, false);
        if (trid != -1)
        {
          contactmap[pt_messages_address] = trid;
          if (d_verbose) [[unlikely]]
            Logger::message("Got trid from name: ", makePrintable(pt_messages_contact_name));
        }
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
          {
            contactmap[pt_messages_address] = trid;
            if (d_verbose) [[unlikely]]
              Logger::message("Got trid from addres: ", makePrintable(pt_messages_address));
          }
        }
      }
    }

    if (trid == -1)
    {
      if (createmissingcontacts || isdummy)
      {
        if ((trid = ptCreateRecipient(ptdb, &contactmap, &warned_createcontacts, pt_messages_contact_name,
                                      pt_messages_address, isgroup)) == -1)
        {
          Logger::warning("Failed to create thread-recipient for address ", makePrintable(pt_messages_address), " (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
          continue;
        }
        if (d_verbose) [[unlikely]]
          Logger::message("Created thread-recipient id for address ", makePrintable(pt_messages_address));
      }
      else
        Logger::error("Thread recipient not found in database.");
    }

    long long int tid = -1;
    // get matching thread
    if (auto it = threadmap.find(trid); it != threadmap.end())
      tid = it->second;
    else
    {
      tid = getThreadIdFromRecipient(trid);
      if (tid == -1)
      {
        // create thread
        Logger::message_start("Failed to find matching thread for conversation, creating. (e164: ", makePrintable(pt_messages_address), " -> ", trid);
        std::any new_thread_id;
        if (!insertRow("thread",
                       {{d_thread_recipient_id, trid},
                        {"active", 1}},
                       "_id", &new_thread_id))
        {
          Logger::message_end();
          Logger::error("Failed to create thread for conversation. Skipping message. (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
          continue;
        }
        tid = std::any_cast<long long int>(new_thread_id);
        Logger::message_end(" -> thread_id: ", tid, ")");
      }
      threadmap.emplace(trid, tid);
    }

    long long int rid = -1;
    if (auto it = contactmap.find(pt_messages_sourceaddress); it != contactmap.end()) [[likely]]
      rid = it->second;
    if (rid == -1)
    {
      Logger::error("Failed to find source_recipient_id in contactmap (", makePrintable(pt_messages_sourceaddress), "). Should be present at this point. Skipping message (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");

      Logger::error_indent("Extra info:");
      Logger::error_indent("Thread recipient: ", trid);
      std::string groupid = d_database.getSingleResultAs<std::string>("SELECT group_id FROM recipient WHERE _id = ?", trid, std::string());
      if (!groupid.empty())
      {
        Logger::error_indent("Thread is group");

        std::vector<long long int> members;
        if (!getGroupMembersModern(&members, groupid))
          Logger::error("Failed to get group members");

        Logger::error_indent("Known group members (", members.size(), "): ");

        for (auto m : members)
        {
          std::string phone = d_database.getSingleResultAs<std::string>("SELECT e164 FROM recipient WHERE _id = ?", m, std::string());
          Logger::error_indent(" - ", m, " : ", phone.empty() ? "(empty)" : makePrintable(phone));
        }
      }
      else
        Logger::error_indent("Thread is 1-on-1");

      Logger::error_indent("First message for this address was (probably):");
      ptdb->d_database.prettyPrint(false,
                                   "SELECT rowid, date, type, read, contact_name, address, numattachments, "
                                   "sourceaddress, targetaddresses, ismms, skip FROM smses WHERE address = ? "
                                   "ORDER BY date LIMIT 1", pt_messages_address);
      Logger::error_indent("Current message for this address was:");
      ptdb->d_database.prettyPrint(false,
                                   "SELECT rowid, date, type, read, contact_name, address, numattachments, "
                                   "sourceaddress, targetaddresses, ismms, skip FROM smses WHERE rowid = ? "
                                   "ORDER BY date LIMIT 1", pt_messages.value(i, "rowid"));

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
        Logger::warning("Unsupported message type (", pt_messages.valueAsInt(i, "type", -1), "). Skipping... (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
        continue;
    }

    if (!unescapeXmlString(&body)) [[unlikely]]
      Logger::warning("Failed to escape message body: '", body, "'");

    long long int freedate = pt_messages.valueAsInt(i, "date", -1);
    if (!isdummy)
    {
      // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
      // we try to get the first free date_sent
      long long int originaldate = freedate;
      if (originaldate == -1)
      {
        Logger::error("Failed to get message date. Skipping... (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
        continue;
      }

      //std::cout << "Get free date for message: '" << body << "'" << std::endl;
      freedate = getFreeDateForMessage(originaldate, tid, incoming ? trid : d_selfid);
      if (freedate == -1)
      {
        if (d_verbose) [[unlikely]] Logger::message_end();
        Logger::error("Getting free date for inserting message into mms. Skipping... (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
        continue;
      }
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
                   (pt_messages.valueAsInt(i, "numattachments", -1) > 0 ? "_id" : ""), &newid)) [[unlikely]]
    {
      Logger::warning("Failed to insert message (rowid: ", pt_messages.valueAsInt(i, "rowid"), ")");
      continue;
    }

    if (pt_messages.valueAsInt(i, "numattachments", 0) > 0)
      attachment_messages.emplace_hint(attachment_messages.end(),
                                       pt_messages.valueAsInt(i, "rowid", 0),
                                       std::pair<long long int, long long int>{std::any_cast<long long int>(newid), freedate});
  }

  Logger::message_overwrite("Importing messages into backup... ", pt_messages.rows(), "/", pt_messages.rows(), " done!", Logger::Control::ENDOVERWRITE);


  SqliteDB::QueryResults attachment_res;
  if (!ptdb->d_database.exec("SELECT data, filename, pos, size, ct, cl, mid FROM attachments "
                             "WHERE mid IN (SELECT rowid FROM smses" + datewhereclause + chatselectionclause +
                             (datewhereclause.empty() && chatselectionclause.empty() ? " WHERE skip = 0" : " AND skip = 0") + ")",
                             &attachment_res))
    return false;

  for (unsigned int j = 0; j < attachment_res.rows(); ++j)
  {
    if (j % (attachment_res.rows() > 100 ? 100 : 1) == 0)
      Logger::message_overwrite("Importing attachments into backup... ", j, "/", attachment_res.rows());

    MEMINFO("START ATTACHMENT LOOP");

    std::string data = attachment_res(j, "data");
    std::string file = attachment_res(j, "filename");
    long long int pos = attachment_res.valueAsInt(j, "pos", -1);
    long long int size = attachment_res.valueAsInt(j, "size", -1);
    std::string ct = attachment_res(j, "ct");
    std::string cl = attachment_res(j, "cl");

    auto amit = attachment_messages.find(attachment_res.valueAsInt(j, "mid", -1));
    if (amit == attachment_messages.end()) [[unlikely]]
    {
      Logger::warning("Found attachment that belongs to no message (mid: ", attachment_res.valueAsInt(j, "mid", -1), ", size: ", size, ")");
      continue;
    }
    long long int new_message_id = amit->second.first;
    long long int unique_id = amit->second.second;

    uint64_t att_data_size;
    int width = 0;
    int height = 0;
    std::string hash;
    // get attachment metadata
    SignalPlainTextBackupAttachmentReader ptar(data, file, pos, size);

    // if it's not just for HTML export we need data hash, and resolution
    if (!isdummy)
    {
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
      AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(std::string(), att_data, ptar.dataSize());       // get metadata from heap
      if (att_data)
        delete[] att_data;
#endif
      att_data_size = amd.filesize;
      width = amd.width == -1 ? 0 : amd.width;
      height = amd.height== -1 ? 0 : amd.height;
      hash = amd.hash;
    }
    else
      att_data_size = ptar.dataSize();

    // not supported on signal android anyway
    if (att_data_size == 0) [[unlikely]]
    {
      Logger::warning("Not inserting 0 byte attachment");
      continue;
    }

    // add entry to attachment table;
    std::any new_aid;
    if (!insertRow(d_part_table,
                   {{d_part_mid, new_message_id},
                    {d_part_ct, ct},
                    {!cl.empty() ? "file_name" : "", cl},
                    {d_part_pending, 0},
                    {"data_size", att_data_size},
                    {"voice_note", 0},
                    {"width", width},
                    {"height", height},
                    {"quote", 0},
                    {((d_database.tableContainsColumn(d_part_table, "data_hash") && !isdummy) ? "data_hash" : ""), hash},
                    {((d_database.tableContainsColumn(d_part_table, "data_hash_start") && !isdummy) ? "data_hash_start" : ""), hash},
                    {((d_database.tableContainsColumn(d_part_table, "data_hash_end") && !isdummy) ? "data_hash_end" : ""), hash}},
                   "_id", &new_aid))
      continue;
    long long int new_part_id = std::any_cast<long long int>(new_aid);

    DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                                            (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                                                             "ATTACHMENTID:uint64:" + bepaald::toString(unique_id) : ""),
                                                                            "LENGTH:uint32:" + bepaald::toString(att_data_size)}))
    {
      new_attachment_frame->setReader(new SignalPlainTextBackupAttachmentReader(data, file, pos, size));
      d_attachments.emplace(std::make_pair(new_part_id,
                                           d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                           unique_id : -1), new_attachment_frame.release());
    }
    else
    {
      Logger::error("Failed to create AttachmentFrame for data");
      Logger::error_indent("       rowid       : ", new_part_id);
      Logger::error_indent("       attachmentid: ", d_database.tableContainsColumn(d_part_table, "unique_id") ? unique_id : -1);
      Logger::error_indent("       length      : ", att_data_size);

      // try to remove the inserted part entry:
      d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
      continue;
    }
    MEMINFO("END ATTACHMENT LOOP");
  }

  //auto t2 = std::chrono::high_resolution_clock::now();
  //auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  //std::cout << " *** TIME: " << ms_int.count() << "ms\n";

  Logger::message_overwrite("Importing attachments into backup... ", attachment_res.rows(), "/", attachment_res.rows(), " done!", Logger::Control::ENDOVERWRITE);

  // count entities still present...
  //ptdb->d_database.exec("SELECT rowid,body,LENGTH(body) - LENGTH(REPLACE(body, '&', '')) AS entities FROM smses ORDER BY entities ASC");

  // save to disk
  //ptdb->d_database.saveToFile("xmldb.sqlite");

  if (!skipmessagereorder) [[likely]]
    if (!isdummy)
      reorderMmsSmsIds();
  updateThreadsEntries();
  return checkDbIntegrity();
}
