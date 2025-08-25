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

#include "../adbbackupdatabase/adbbackupdatabase.h"
#include "../adbbackupattachmentreader/adbbackupattachmentreader.h"

bool SignalBackup::importFromAdbBackup(std::unique_ptr<AdbBackupDatabase> const &adbdb,
                                       std::vector<std::string> const &daterangelist, bool isdummy [[maybe_unused]])
{
  if (d_database.tableContainsColumn(d_part_table, "unique_id")) [[unlikely]]
  {
    Logger::error("Unsupported database version (", d_databaseversion, "). Please use a newer databsae as input");
    return false;
  }

  adbdb->d_db.setCacheSize(15);
  d_database.setCacheSize(15);

  // get all conversations
  SqliteDB::QueryResults thread_results;
  if (!adbdb->d_db.exec("SELECT _id, recipient_ids FROM thread", &thread_results))
    return false;

  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (unsigned int i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});

  std::string datewhereclause;
  for (unsigned int i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      Logger::error("Invalid range: '", dateranges[i].first, "' - '", dateranges[i].second, "' (", startrange, " - ", endrange, ")");
      return false;
    }
    Logger::message("  Using range: ", dateranges[i].first, " - ", dateranges[i].second, " (", startrange, " - ", endrange, ")");

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "date_received BETWEEN "s + bepaald::toString(startrange) + " AND " + bepaald::toString(endrange);
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }

  for (unsigned int it = 0; it < thread_results.rows(); ++it)
  {
    Logger::message("Dealing with thread ", it + 1, "/", thread_results.rows());

    std::string thread_recipient_e164 =
      adbdb->version() < 276 ?
      adbdb->d_db.getSingleResultAs<std::string>("SELECT address FROM canonical_addresses WHERE ca.canonical_addresses._id = ?",
                                                 thread_results.value(it, "recipient_ids"),
                                                 std::string()) :
      thread_results(it, "recipient_ids");


    if (thread_recipient_e164.empty())
    {
      Logger::error("Failed to get conversation partner for thread. Skipping...");
      continue;
    }

    if (STRING_STARTS_WITH(thread_recipient_e164, "__textsecure_group__!"))
    {
      Logger::warning("Thread appears to be a group conversation. This is not yet implemented (no example data available). Contact the developer");
      continue;
    }

    if (d_verbose) [[unlikely]]
      Logger::message("Got address for thread: ", thread_recipient_e164);

    // get recipient_id from target database, create if necessary
    long long int thread_recipient_id = getRecipientIdFromPhone(thread_recipient_e164, false /*withthread*/);
    if (thread_recipient_id == -1)
    {
      if (d_verbose) [[unlikely]]
        Logger::message("No recipient found for e164... creating.");

      std::any new_rid;
      if (!insertRow("recipient",
                     {{d_recipient_e164, thread_recipient_e164}},
                     "_id", &new_rid))
        return false;

      thread_recipient_id = std::any_cast<long long int>(new_rid);

      // make sure there is a valid identity key if this is not a dummy
      // THIS IS UNTESTED
      if (!isdummy)
      {
        std::string identity_key = adbdb->d_db.getSingleResultAs<std::string>("SELECT key FROM identities WHERE recipient = ?",
                                                                              thread_results.value(it, "recipient_ids"),
                                                                              std::string());
        if (identity_key.empty()) // no key found in source data, insert fake one:
                                  // keys always start with 0x05 for some reason...
          identity_key = "BUZBS0VLRVkgRkFLRUtFWSBGQUtFS0VZIEZBS0VLRVkh";
          // $ echo "BUZBS0VLRVkgRkFLRUtFWSBGQUtFS0VZIEZBS0VLRVkh" | base64 -d | xxd -g 1 -c 33
          // 00000000: 05 46 41 4b 45 4b 45 59 20 46 41 4b 45 4b 45 59 20 46 41 4b 45 4b 45 59 20 46 41 4b 45 4b 45 59 21  .FAKEKEY FAKEKEY FAKEKEY FAKEKEY!

        // Normally, 'address' points to UUID. We don't have that, so we use
        // e164 instead. I have 3 entries in my identites table with e164
        // in the address column, so it should be valid?
        if (!insertRow("identities",
                       {{"address", thread_recipient_e164},
                        {"identity_key", identity_key}/*,
                        {"first_use", res("firstUse")},              // default 0
                        {"timestamp", res.value(0, "timestamp")},    // default 0
                        {"verified", res.value(0, "verified")},      // default 0
                        {"nonblocking_approval", res("nonblockingApproval")}*/})) // default 0
        {
          Logger::error("Failed to insert identity key for contact: ", thread_recipient_e164);
          continue;
        }
      }
    }

    // match thread id in target backup, create if needed
    long long int thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE recipient_id = ?", thread_recipient_id, -1);
    if (thread_id == -1)
    {
      std::any new_thread_id;
      if (!insertRow("thread",
                     {{d_thread_recipient_id, thread_recipient_id},
                      {"active", 1}},
                     "_id", &new_thread_id))
      {
        Logger::error("Failed to create thread for conversation... skipping");
        continue;
      }
      thread_id = std::any_cast<long long int>(new_thread_id);
    }

    // get messages
    SqliteDB::QueryResults message_results;
    if (!adbdb->d_db.exec("SELECT _id, thread_id, body, date AS date_received, date_sent, read, type, delivery_receipt_count, expires_in, 0 AS is_mms FROM sms WHERE thread_id = ?1 " + datewhereclause +
                          " UNION ALL "
                          "SELECT _id, thread_id, body, date AS date_received, date AS date_sent, read, msg_box AS type, delivery_receipt_count, expires_in, 1 AS is_mms FROM mms WHERE thread_id = ?1 " + datewhereclause,
                          thread_results.value(it, 0),
                          &message_results))
      return false;

    if (message_results.rows() == 0)
    {
      Logger::message("No messages in thread", datewhereclause.empty() ? "" : " (in selected time period)");
      continue;
    }

    //message_results.printLineMode();

    for (unsigned int im = 0; im < message_results.rows(); ++im)
    {
      Logger::message_overwrite("Importing message ", im + 1, "/", message_results.rows());

      // the types in this database seem to be prepended with 11111111'11111111'11111111'11111111 (making it appear signed negative as well), lets remove that.
      long long int type = message_results.valueAsInt(im, "type") & Types::TOTAL_MASK;

      AdbBackupDatabase::EncryptionType encryptiontype = type & AdbBackupDatabase::EncryptionType::ENCRYPTION_ASYMMETRIC_BIT ?
        AdbBackupDatabase::EncryptionType::ENCRYPTION_ASYMMETRIC_BIT :
        AdbBackupDatabase::EncryptionType::ENCRYPTION_SYMMETRIC_BIT;

      // these message still have
      //  long ENCRYPTION_SYMMETRIC_BIT         = 0x80000000; Deprecated
      //  long ENCRYPTION_ASYMMETRIC_BIT        = 0x40000000; Deprecated
      // bits set. Remove them.
      // type &= ~0x40000000;
      // type &= ~0x80000000;
      // ~(0x40000000 | 0x80000000) == 0x3FFFFFFF
      //type &= 0x3FFFFFFF;
      type &= 0x3FFFFFFF;

      bool incoming = Types::isInboxType(type);

      if (encryptiontype == AdbBackupDatabase::EncryptionType::ENCRYPTION_ASYMMETRIC_BIT) [[unlikely]]
      {
        Logger::warning("Message encrypted with asymmetric encryption not (yet) supported. Skipping...)");
        continue;
      }

      std::string body(message_results(im, "body"));
      if (!body.empty())
      {
        auto opt_body = adbdb->decryptMessageBody(body);
        if (!opt_body.has_value())
        {
          Logger::error("Failed to decrypt message body");
          continue;
        }
        body = std::move(*opt_body);
      }

      std::any new_mms_id;
      if (!tryInsertRowElseAdjustDate(d_mms_table,
                                      {{"thread_id", thread_id},
                                       {d_mms_date_sent, message_results.value(im, "date_sent")},
                                       {"date_received", message_results.value(im, "date_received")},
                                       {d_mms_recipient_id, incoming ? thread_recipient_id : d_selfid},
                                       {"to_recipient_id", incoming ? d_selfid : thread_recipient_id},
                                       {d_mms_type, type},
                                       {!body.empty() ? "body" : "", body},
                                       {"read", 1},
                                       {"m_type", incoming ? 132 : 128},
                                       {"expires_in", message_results.value(im, "expires_in")},
                                       {d_mms_delivery_receipts, message_results.value(im, "delivery_receipt_count")}},
                                      1 /*date_sent idx*/, message_results.valueAsInt(im, "date_sent", 0), thread_id, incoming ? thread_recipient_id : d_selfid, nullptr,
                                      "_id",
                                      &new_mms_id))
      {
        Logger::warning("Failed to insert message");
        continue;
      }

      // add attachment if present...
      if (message_results.valueAsInt(im, "is_mms", 0) == 1)
      {
        bool attachmentadded = false;
        SqliteDB::QueryResults part_results;
        if (adbdb->d_db.exec("SELECT "
                             "ct, " // content_type
                             //"name, " //
                             "cd, " // remote_key
                             "cl, " // remote_location
                             "digest, "  // remote_digest
                             "pending_push, " // transfer_state
                             "_data, " // data_file
                             "data_size,"
                             "file_name "
                             "FROM part WHERE mid = ?", message_results.value(im, "_id"), &part_results)) [[likely]]
        {
          for (unsigned int ip = 0; ip < part_results.rows(); ++ip)
          {
            // get the transfer state to detect failed attachments;
            long long int pending_push = part_results.valueAsInt(ip, "pending_push", 3/* = TRANSFER_PROGRESS_FAILED*/);

            // _data is '/data/user/0/org.thoughtcrime.securesms/app_parts/partXXXXXXX.mms'
            // our files live in '[backupdir]/r/app_parts/partXXXXXXXX.mms'
            std::string data = part_results(ip, "_data");
            if (!STRING_STARTS_WITH(data, "/data/user/0/org.thoughtcrime.securesms/"))
            {
              if (data.empty() && pending_push != 0)
                Logger::warning("Skipping attachment with failed transfer state");
              else
                Logger::warning("Attachment _data entry has unexpected value: '", data, "', skipping");
              continue;
            }
            std::string attachment_filepath(adbdb->backupRoot() + "/r/" + data.substr(STRLEN("/data/user/0/org.thoughtcrime.securesms/")));

            AdbBackupAttachmentReader adb_attachment(attachment_filepath,
                                                     adbdb->macSecret(), adbdb->macSecretLength(),
                                                     adbdb->encryptionSecret(), adbdb->encryptionSecretLength());
#if __cpp_lib_out_ptr >= 202106L
            std::unique_ptr<unsigned char[]> att_data;
            if (adb_attachment.getAttachmentData(std::out_ptr(att_data), d_verbose) != AdbBackupAttachmentReader::ReturnCode::OK)
#else
            unsigned char *att_data = nullptr; // !! NOTE RAW POINTER
            if (adb_attachment.getAttachmentData(&att_data, d_verbose) != AdbBackupAttachmentReader::ReturnCode::OK)
#endif
            {
              Logger::error("Failed to get attachment data for file '", attachment_filepath, "'");
              continue;
            }

#if __cpp_lib_out_ptr >= 202106L
            AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(attachment_filepath, att_data.get(), adb_attachment.size()); // get metadata from heap
#else
            AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(attachment_filepath, att_data, adb_attachment.size());       // get metadata from heap
            if (att_data)
              delete[] att_data;
#endif

            long long int datasize = part_results.valueAsInt(ip, "data_size", 0);

            std::string file_name(part_results(ip, "file_name"));
            if (!file_name.empty())
            {
              auto opt_file_name = adbdb->decryptMessageBody(file_name);
              if (!opt_file_name.has_value()) [[unlikely]]
                Logger::error("Failed to decrypt attachment file name");
              else
                file_name = std::move(*opt_file_name);
            }

            //insert into part
            std::any new_part_id_any;
            if (!insertRow(d_part_table,
                           {{d_part_mid, new_mms_id},
                            {d_part_ct, part_results.value(ip, "ct")},
                            {d_part_cd, part_results.value(ip, "cd")},
                            {d_part_cl, part_results.value(ip, "cl")},
                            {"remote_digest", part_results.value(ip, "digest")},
                            {d_part_pending, pending_push},
                            {"data_file", part_results.value(ip, "_data")},
                            {"data_size", datasize},
                            {"file_name", file_name},
                            {amd.width > -1 ? "width" : "", amd.width},
                            {amd.height > -1 ? "height" : "", amd.height},
                            {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), amd.hash},
                            {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), amd.hash},
                            {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), amd.hash}},
                           "_id", &new_part_id_any))
            {
              Logger::error("Inserting part-data");
              continue;
            }
            long long int new_part_id = std::any_cast<long long int>(new_part_id_any);

            DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
            if (setFrameFromStrings(&new_attachment_frame,
                                    std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                             "LENGTH:uint32:" + bepaald::toString(datasize)}))
            {
              new_attachment_frame->setReader(new AdbBackupAttachmentReader(attachment_filepath,
                                                                            adbdb->macSecret(), adbdb->macSecretLength(),
                                                                            adbdb->encryptionSecret(), adbdb->encryptionSecretLength()));
              d_attachments.emplace(std::make_pair(new_part_id, -1), new_attachment_frame.release());
              attachmentadded = true;
            }
          }
        }
        if (!attachmentadded && body.empty() && !Types::isExpirationTimerUpdate(type)) [[unlikely]] // remove new message if body is empty and no attachments were successfully added
        {
          Logger::warning("Removing empty message (no message body, no attachments)");
          d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id = ?", new_mms_id);
          d_database.exec("DELETE FROM " + d_part_table + " WHERE " + d_part_mid + " = ?", new_mms_id);
        }
      }
    }
  }

  updateThreadsEntries();

  if (!isdummy/* || true*/)
    reorderMmsSmsIds();

  return true;
}
