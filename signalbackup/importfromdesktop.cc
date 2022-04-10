/*
    Copyright (C) 2022  Selwin van Dijk

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

#include "../sqlcipherdecryptor/sqlcipherdecryptor.h"

bool SignalBackup::importFromDesktop(std::string const &dir, bool ignorewal)
{
  if (dir.empty())
  {
    // try to set dir automatically
  }

  // check if a wal (Write-Ahead Logging) file is present in path, and warn user to (cleanly) shut Signal Desktop down
  if (!ignorewal &&
      bepaald::fileOrDirExists(dir + "/db.sqlite-wal"))
  {
    // warn
    return false;
  }

  SqlCipherDecryptor sqlcipherdecryptor(dir);
  if (!sqlcipherdecryptor.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }

  auto [data, size] = sqlcipherdecryptor.data(); // unsigned char *, uint64_t

  // disable WAL (Write-Ahead Logging) on database, reading from memory otherwise will not work
  // see https://www.sqlite.org/fileformat.html
  if (data[0x12] == 2)
    data[0x12] = 1;
  if (data[0x13] == 2)
    data[0x13] = 1;

  std::pair<unsigned char *, uint64_t> desktopdata = {data, size};
  SqliteDB ddb(&desktopdata);
  if (!ddb.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }

  // actual functionality comes here :)
  // ...

  // get all conversations (conversationpartners) from ddb
  SqliteDB::QueryResults results;
  if (!ddb.exec("SELECT id,uuid,groupId FROM conversations WHERE json_extract(json, '$.messageCount') > 0", &results))
    return false;

  results.prettyPrint();

  // for each conversation
  for (uint i = 0; i < results.rows(); ++i)
  {
    // get the actual id
    std::string person_or_group_id = results.valueAsString(i, "uuid");
    if (person_or_group_id.empty())
    {
      auto [groupid_data, groupid_data_length] = Base64::base64StringToBytes(results.valueAsString(i, "groupId"));
      if (groupid_data && groupid_data_length != 0)
      {
        //std::cout << bepaald::bytesToHexString(groupid_data, groupid_data_length, true) << std::endl;
        person_or_group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, groupid_data_length, true);
        bepaald::destroyPtr(&groupid_data, &groupid_data_length);
      }
    }

    if (person_or_group_id.empty())
    {
      //std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << "Useful error message" << std::endl;
      continue;
    }

    // get matching thread id from android database
    SqliteDB::QueryResults results2;
    if (!d_database.exec("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE (uuid = ? OR group_id = ?))",
                         {person_or_group_id, person_or_group_id}, &results2) ||
        results2.rows() != 1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " : Failed to find matching thread for conversation, skipping. (id: " << person_or_group_id << ")" << std::endl;
      continue;
    }

    std::cout << "Match for " << person_or_group_id << std::endl;
    results2.prettyPrint();

    long long int ttid = results2.getValueAs<long long int>(0, "_id"); // ttid : target thread id
    std::cout << ttid << std::endl;

    // now lets get all messages for this conversation
    SqliteDB::QueryResults results3;
    /*
      EXAMPLE

      DESKTOP DB:
                     rowid = 56
                        id = 845bff95-[...]-4b53efcba27b
                      json = {"timestamp":1643874290360,"attachments":[{"contentType":"application/pdf","fileName":"qrcode.pdf","path":"21/21561db325667446c84702bc2af2cb779aaaeb32c6b3d190d41f86d12e8bf5f0","size":38749,"pending":false,"url":"/home/svandijk/.config/Signal/drafts.noindex/4b/4bb11cd1be7c718ae8ed57dc28f34d57a1032d4ab0595128527466e876ddde9d"}],"type":"outgoing","body":"qrcode","conversationId":"d6b93b26-[...]-b949d4de0aba","preview":[],"sent_at":1643874290360,"received_at":1623335267006,"received_at_ms":1643874290360,"recipients":["93722273-[...]-c8261969714c"],"bodyRanges":[],"sendHQImages":false,"sendStateByConversationId":{"d6b93b26-[...]-b949d4de0aba":{"status":"Delivered","updatedAt":1643874294845},"87e8067b-[...]-011b5c5ee23a":{"status":"Sent","updatedAt":1643874291830}},"schemaVersion":10,"hasAttachments":1,"hasFileAttachments":1,"contact":[],"destination":"93722273-[...]-c8261969714c","id":"845bff95-[...]-4b53efcba27b","readStatus":0,"expirationStartTimestamp":1643874291546,"unidentifiedDeliveries":["93722273-[...]-c8261969714c"],"errors":[],"synced":true}
                readStatus = 0
                expires_at =
                   sent_at = 1643874290360
             schemaVersion = 10
            conversationId = d6b93b26-[...]-b949d4de0aba
               received_at = 1623335267006
                    source =
    deprecatedSourceDevice =
            hasAttachments = 1
        hasFileAttachments = 1
 hasVisualMediaAttachments = 0
               expireTimer =
  expirationStartTimestamp = 1643874291546
                      type = outgoing
                      body = qrcode
              messageTimer =
         messageTimerStart =
     messageTimerExpiresAt =
                  isErased = 0
                isViewOnce = 0
                sourceUuid =
                serverGuid =
                 expiresAt =
              sourceDevice =
                   storyId =
                   isStory = 0
       isChangeCreatedByUs = 0
      shouldAffectActivity = 1
       shouldAffectPreview = 1
    isUserInitiatedMessage = 1
     isTimerChangeFromSync = 0
         isGroupLeaveEvent = 0
isGroupLeaveEventFromOther = 0


     ANDROID DB:
                        _id = 631
             thread_id = 1
                  date = 1643874290360
         date_received = 1643874294496
           date_server = -1
               msg_box = 10485783
                  read = 1
                  body = qrcode
            part_count = 1
                  ct_l =
               address = 2
     address_device_id =
                   exp =
                m_type = 128
                m_size =
                    st =
                 tr_id =
delivery_receipt_count = 1
 mismatched_identities =
      network_failures =
       subscription_id = -1
            expires_in = 0
        expire_started = 0
              notified = 0
    read_receipt_count = 0
              quote_id = 0
          quote_author =
            quote_body =
      quote_attachment = -1
         quote_missing = 0
        quote_mentions =
       shared_contacts =
          unidentified = 1
              previews =
       reveal_duration = 0
             reactions =
      reactions_unread = 0
   reactions_last_seen = -1
        remote_deleted = 0
         mentions_self = 0
    notified_timestamp = 0
  viewed_receipt_count = 0
           server_guid =
     receipt_timestamp = 1643874295302
                ranges =
              is_story = 0
       parent_story_id = 0
     */

    if (!ddb.exec("SELECT body FROM messages WHERE converation_id = ?",
                  results.getValueAs<long long int>(i, "id"), &results3))
      return false;



  }

  return false;
}
