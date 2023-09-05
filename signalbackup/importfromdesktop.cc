/*
  Copyright (C) 2022-2023  Selwin van Dijk

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
#include "../msgtypes/msgtypes.h"

/*
  TODO
  DONE? - limit timeframe
  DONE? - AUTO timeframe
  DONE? - fix address for call messages
  DONE? - implement call messages (group video done?)
  - implement group-v2- stuff

  Known missing things:
  DONE ? - messages for conversation that is not in thread table (-> create new thread for recipient)
  - when recipient is not present in backup?
   - message types other than 'incoming' and 'outgoing'
     - 'group-v2-change' (group member add/remove/change group name/picture
     - other status messages, like disappearing msgs timer change, profile key change etc
   - inserting into group-v1-type groups
  DONE? - all received/read receipts
  DONE? - voice_note flag in part table?
   - any group-v1 stuff
   - stories?
   - payments
   - badges
   - more...
 */

/*
///////////////////////////////  SMS COLUMNS:
//_id // this is an AUTO value
"thread_id,"
"address/recipient_id,"
//"address_device_id/recipient_device_id,"
//"person,"// =
"date,"// = 1663067790169
"date_sent,"// = 1663067792779
"date_server,"// = 1663067790149
//"protocol,"// always 31337? maybe not necessary?
//"read,"
//"status,"
"type," // = incoming/outgoing + secure
//"reply_path_preseny,"// = 1
//"delivery_receipt_count,"// = 0
//"subject,"// =
"body,"//
//"mismatched_identities,"// =
//"service_center,"// = GCM
//"subscription_id,"// = -1
//"expires_in,"// = 0
//"expire_started,"// = 0
//"notified,"// = 0
//"read_receipt_count,"// = 0
//"unidentified,"// = 1
//"reactions,"// DOES NOT EXIST IN NEWER DATABASES
//"reactions_unread,"// = 0
//"reactions_last_seen,"// = 1663078811832
"remote_deleted,"// = 0
//"notified_timestamp,"// = 1663072365960
"server_guid"// = 0bb19070-e1a2-4c52-b637-00e905583bc1
//"receipt_timestamp"// = -1 // is -1 default?


///////////////////////////////  MMS COLUMNS:
//"_id,"// AUTO VALUE
"thread_id,"
"date,"// =  = 1474184079794
"date_received,"// =  = 1474184079855
"date_server,"// =  = -1
"msg_box,"// =  = 10485783
//"read,"// =  = 1
"body,"//
//"part_count,"// don't know what this is... not number of attachments // REMOVED IN DBV166
//"ct_l,"// =  =
"address,/recipient_id"// =  = 53
//"address_device_id/recipient_device_id,"// =  =
//"exp,"// =  =
"m_type,"// =  = 128
//"m_size,"// =  =
//"st,"// =  =
//"tr_id,"// =  =
//"delivery_receipt_count,"// =  = 2
//"mismatched_identities,"// =  =
//"network_failures,"// =  =
//"subscription_id,"// =  = -1
//"expires_in,"// =  = 0
//"expire_started = 0
//"notified,"// =  = 0
//"read_receipt_count,"// =  = 0
"quote_id,"//  corresponds to 'messages.date' of quoted message
"quote_author,"// =  =
"quote_body,"// =  =
"quote_attachment,"// =  = -1
"quote_missing,"// =  = 0
"quote_mentions,"// =  =
//"shared_contacts,"// =  =
//"unidentified,"// =  = 0
//"previews,"// =  =
//"reveal_duration,"// =  = 0
//"reactions,"// =  =
//"reactions_unread,"// =  = 0
//"reactions_last_seen,"// =  = -1
"remote_deleted,"// =  = 0
//"mentions_self,"// =  = 0
//"notified_timestamp,"// =  = 0
//"viewed_receipt_count,"// =  = 0
//"server_guid,"// =
//"receipt_timestamp,"// =  = -1
//"ranges,"// =  =
//"is_story,"// =  = 0
//"parent_story_id,"// =  = 0
"quote_type"// =  = 0

/////////////////////////////// PART COLUMNS:
//"_id," // = AUTO VALUE
"mid," // = 5500
//"seq," // = 0
"ct," // = image/jpeg
//"name," // =
//"chset," // =
//"cd," // = A1sAd5JPAdm5SxZ9q2Bn/2X7BQw/vJfmaWk1zet2cFgb9D+2xpSRyMjuOcUZP7Lic3AEp38BIxKg/LCLMr2v5w==
//"fn," // =
//"cid," // =
//"cl," // = DlImHS8vRhF5VM5ueDVh
//"ctt_s," // =
//"ctt_t," // =
//"encrypted," // =
"pending_push," // MUST BE ZERO (i think)
//"_data," // = FILLED IN ON RESTORE?  /data/user/0/org.thoughtcrime.securesms/app_parts/part7685241378172293912.mms
"data_size," // = 421
"file_name," // =
//"thumbnail," // =
//"aspect_ratio," // =
"unique_id," // = 1630950584787
//"digest," // = (binary)
//"fast_preflight_id," // =
"voice_note," // = 0
//"data_random," // = FILLED IN ON RESTORE? (binary)
//"thumbnail_random," // =
"width," // = 16
"height," // = 16
"quote," // = 0
//"caption," // =
//"sticker_pack_id," // =
//"sticker_pack_key," // =
//"sticker_id," // = -1
"data_hash," // = Msx++MxFQPNuuCPnsO5Q9H2twoNFPMOKpH521FDVn+U=
//"blur_hash," // = LN7nwD_M_M_M_M_M_M_M_M_M_M_M
//"transform_properties," // = {"skipTransform":true,"videoTrim":false,"videoTrimStartTimeUs":0,"videoTrimEndTimeUs":0,"sentMediaQuality":0,"videoEdited":false}
//"transfer_file," // =
//"display_order," // = 0
//"upload_timestamp," // = 1630950581728
"cdn_number" // = 2
//"borderless," // = 0
//"sticker_emoji," // =
//"video_gif" // = 0


///////////////////////////////  REACTION COLUMNS
//           _id = 80
//    message_id = 66869
//        is_mms = 0
//     author_id = 7
//         emoji = 👍
//     date_sent = 1662929051259
// date_received = 1662929052309

///////////////////////////////   MENTION COLUMN
// mention entry in android db
//          _id = 10
//    thread_id = 43
//   message_id = 5910
// recipient_id = 71
//  range_start = 40
// range_length = 1

*/

bool SignalBackup::importFromDesktop(std::string configdir, std::string databasedir,
                                     long long int sqlcipherversion,
                                     std::vector<std::string> const &daterangelist,
                                     bool createmissingcontacts,
                                     bool autodates, bool ignorewal, std::string const &selfphone)
{

  d_selfid = selfphone.empty() ? scanSelf() : d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
  if (d_selfid == -1)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to determine id of 'self'.";
    if (selfphone.empty())
      std::cout << "Please pass `--setselfid \"[phone]\"' to set it manually";
    std::cout << std::endl;
    return false;
  }
  d_selfuuid = d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string());

  if (configdir.empty() || databasedir.empty())
  {
    // try to set dir automatically
    auto [cd, dd] = getDesktopDir();
    configdir = cd;
    databasedir = dd;
  }

  // check if a wal (Write-Ahead Logging) file is present in path, and warn user to (cleanly) shut Signal Desktop down
  if (!ignorewal &&
      bepaald::fileOrDirExists(databasedir + "/db.sqlite-wal"))
  {
    // warn
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Found Sqlite-WAL file (write-ahead logging)."
              << "Make sure Signal Desktop is cleanly shut down." << std::endl;
    // << " or pass --ignorewall"?
    return false;
  }

  SqlCipherDecryptor sqlcipherdecryptor(configdir, databasedir, sqlcipherversion);
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

  dtSetColumnNames(&ddb);

  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (uint i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});

  // set daterange automatically
  if (dateranges.empty() && autodates)
  {
    SqliteDB::QueryResults res;
    if ((d_database.containsTable("sms") && !d_database.exec("SELECT MIN(mindate) FROM (SELECT MIN(sms." + d_sms_date_received + ", " + d_mms_table + ".date_received) AS mindate FROM sms "
                                                             "LEFT JOIN " + d_mms_table + " WHERE sms." + d_sms_date_received + " IS NOT NULL AND " + d_mms_table + ".date_received IS NOT NULL)", &res)) ||
        (!d_database.containsTable("sms") && !d_database.exec("SELECT MIN(" + d_mms_table + ".date_received) AS mindate FROM " + d_mms_table + " WHERE " + d_mms_table + ".date_received IS NOT NULL", &res)))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << "Failed to automatically determine data-range" << std::endl;
      return false;
    }
    dateranges.push_back({"0", res.valueAsString(0, 0)});
  }

  std::string datewhereclause;
  for (uint i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      std::cout << "Error: Skipping range: '" << dateranges[i].first << " - " << dateranges[i].second << "'. Failed to parse or invalid range." << std::endl;
      continue;
    }
    std::cout << "  Using range: " << dateranges[i].first << " - " << dateranges[i].second
              << " (" << startrange << " - " << endrange << ")" << std::endl;

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "sent_at BETWEEN "s + bepaald::toString(startrange) + " AND " + bepaald::toString(endrange);
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }

  bool warned_createcontacts = false;

  // find out which database is newer
  long long int maxdate_desktop_db = ddb.getSingleResultAs<long long int>("SELECT MAX(MAX(json_extract(json, '$.received_at_ms')),MAX(received_at)) FROM messages", 0);
  long long int maxdate_android_db = d_database.getSingleResultAs<long long int>("SELECT MAX(date_received) FROM " + d_mms_table, 0);
  if (d_database.containsTable("sms"))
    maxdate_android_db = d_database.getSingleResultAs<long long int>("SELECT MAX((SELECT MAX(date_received) FROM " + d_mms_table + "),(SELECT MAX(" + d_sms_date_received + ") FROM sms))", 0);
  bool desktop_is_newer = maxdate_desktop_db > maxdate_android_db;

  // get all conversations (conversationpartners) from ddb
  SqliteDB::QueryResults results_all_conversations;
  if (!ddb.exec("SELECT "
                "rowid,"
                "id,"
                "e164,"
                "type,"
                "LOWER(" + d_dt_c_uuid + ") AS 'uuid',"
                "groupId,"
                "IFNULL(json_extract(json,'$.isArchived'), false) AS 'is_archived',"
                "IFNULL(json_extract(json,'$.isPinned'), false) AS 'is_pinned',"
                "IFNULL(json_extract(json,'$.groupId'),'') AS 'json_groupId',"
                "IFNULL(json_extract(json,'$.derivedGroupV2Id'),'') AS 'derivedGroupV2Id',"
                "IFNULL(json_extract(json,'$.groupVersion'), 1) AS groupVersion"
                " FROM conversations WHERE json_extract(json, '$.messageCount') > 0", &results_all_conversations))
    return false;

  //std::cout << "Conversations in desktop:" << std::endl;
  //results.prettyPrint();

  // this map will map desktop-recipient-uuid's to android recipient._id's
  std::map<std::string, long long int> recipientmap;

  // for each conversation
  for (uint i = 0; i < results_all_conversations.rows(); ++i)
  {
    // skip convo's with no messages...
    SqliteDB::QueryResults messagecount;
    if (ddb.exec("SELECT COUNT(*) AS count FROM messages WHERE conversationId = ?", results_all_conversations(i, "id"), &messagecount))
      if (messagecount.rows() == 1 &&
          messagecount.getValueAs<long long int>(0, "count") == 0)
      {
        std::cout << "Skipping conversation, conversation has no messages (" << i + 1 << "/" << results_all_conversations.rows() << ")" << std::endl;
        continue;
      }

    std::cout << "Trying to match conversation (" << i + 1 << "/" << results_all_conversations.rows() << ") (type: " << results_all_conversations.valueAsString(i, "type") << ")" << std::endl;

    //long long int conversation_rowid = results_all_conversations.getValueAs<long long int>(i, "rowid");

    // get the actual id
    bool isgroupconversation = false;
    std::string person_or_group_id;
    if (results_all_conversations.valueAsString(i, "type") == "group")
    {
      if (results_all_conversations.getValueAs<long long int>(i, "groupVersion") > 1)
      {
        std::pair<unsigned char *, size_t> groupid_data = Base64::base64StringToBytes(results_all_conversations.valueAsString(i, "json_groupId"));
        if (!groupid_data.first || groupid_data.second == 0) // data was not valid base64 string, lets try the other one
          groupid_data = Base64::base64StringToBytes(results_all_conversations.valueAsString(i, "groupId"));

        if (groupid_data.first && groupid_data.second != 0)
        {
          //std::cout << bepaald::bytesToHexString(groupid_data, groupid_data_length, true) << std::endl;
          person_or_group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, true);
          isgroupconversation = true;
          bepaald::destroyPtr(&groupid_data.first, &groupid_data.second);
        }
        else
        {
          std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Conversation is 'group'-type, but groupId "
                  << "unexpectedly was not base64 data. Maybe this is a groupV1 group? Here is the data: " << std::endl;
          ddb.printLineMode("SELECT * FROM conversations WHERE id = ?", results_all_conversations.value(i, "id"));
          continue;
        }
      }
      else // group v1 maybe?
      {
        // see if it has a 'derivedgroupv2id', and if that can be matched...
        bool found_new_group = false;
        std::pair<unsigned char *, size_t> groupid_data = Base64::base64StringToBytes(results_all_conversations.valueAsString(i, "derivedGroupV2Id"));
        if (groupid_data.first || groupid_data.second > 0)
        {
          if (d_verbose) [[unlikely]] std::cout << "Trying to match group-v1 by 'derivedGroupV2Id'" << std::endl;
          person_or_group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, true);
          if (getRecipientIdFromUuid(person_or_group_id, &recipientmap) != -1)
            found_new_group = true;
          bepaald::destroyPtr(&groupid_data.first, &groupid_data.second);
        }

        if (found_new_group)
          isgroupconversation = true;
        else
        {
          /*
          std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Group V1 type not yet supported" << std::endl;
          SqliteDB::QueryResults groupid_res;
          ddb.exec("SELECT HEX(groupId) FROM conversations WHERE id = ?", results_all_conversations.value(i, "id"), &groupid_res);
          if (groupid_res.rows())
            std::cout << "       Possible group id: " << groupid_res.valueAsString(0, 0) << std::endl;
          */
          // lets just for fun try to find an old-style group with this id:
          if (results_all_conversations.valueHasType<std::string>(i, "groupId"))
          {
            std::string groupv1id_str = results_all_conversations.valueAsString(i, "groupId");
            person_or_group_id = "__textsecure_group__!" + utf8BytesToHexString(groupv1id_str);
            isgroupconversation = true;
            //std::cout << "Possible GroupV1 id from STRING: " << gid << std::endl;
            //d_database.prettyPrint("SELECT _id,group_id FROM groups WHERE LOWER(group_id) == LOWER(?)", gid);
          }
          else
            continue;
          // person_or_group_id = "__textsecure_group__!" + bepaald::bytesToHexString(reinterpret_cast<unsigned char const *>(giddata.data()), giddata.size());
          // isgroupconversation = true;
        }
      }
    }
    else // type != 'group' ( == 'private'?)
      person_or_group_id = results_all_conversations.valueAsString(i, "uuid"); // single person id, if group, this is empty

    // get/create matching thread id from android database
    long long int recipientid_for_thread = -1;
    std::string phone;
    if (!person_or_group_id.empty())
      recipientid_for_thread = getRecipientIdFromUuid(person_or_group_id, &recipientmap, createmissingcontacts);
    else
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to determine uuid. Trying with phone number..." << std::endl;
      phone = results_all_conversations.valueAsString(i, "e164");
      if (!phone.empty())
        recipientid_for_thread = getRecipientIdFromPhone(phone, &recipientmap, createmissingcontacts);
    }

    if (recipientid_for_thread == -1)
    {
      if (createmissingcontacts)
      {
        recipientid_for_thread = dtCreateRecipient(ddb, person_or_group_id, results_all_conversations.valueAsString(i, "e164"),
                                                   results_all_conversations.valueAsString(i, "groupId"), databasedir, &recipientmap,
                                                   &warned_createcontacts);
        if (recipientid_for_thread == -1)
        {
          std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                    << ": Failed to create missing recipient. Skipping." << std::endl;
          continue;
        }
      }
    }

    if (recipientid_for_thread == -1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                << ": Chat partner was not found in recipient-table. Skipping. (id: "
                << (person_or_group_id.empty() ? results_all_conversations.valueAsString(i, "e164") : person_or_group_id) << ")" << std::endl;
      continue;
    }

    SqliteDB::QueryResults results2;
    long long int ttid = -1;
    if (!d_database.exec("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", recipientid_for_thread, &results2))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Executing query." << std::endl;
      continue;
    }
    if (results2.rows() == 1) // we have found our matching thread
      ttid = results2.getValueAs<long long int>(0, "_id"); // ttid : target thread id
    else if (results2.rows() == 0) // the query was succesful, but yielded no results -> create thread
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to find matching thread for conversation, creating. ("
                << (person_or_group_id.empty() ? "from e164" : ("id: " + person_or_group_id)) << ")" << std::endl;
      std::any new_thread_id;
      if (!insertRow("thread",
                     {{d_thread_recipient_id, recipientid_for_thread},
                      {"archived", results_all_conversations.getValueAs<long long int>(i, "is_archived")},
                      {"pinned", results_all_conversations.getValueAs<long long int>(i, "is_pinned")}},
                     "_id", &new_thread_id))
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create thread for desktop conversation. ("
                  << (person_or_group_id.empty() ? "from e164" : ("id: " + person_or_group_id)) << "), skipping." << std::endl;
        continue;
      }
      //std::cout << "Raw any_cast 1" << std::endl;
      ttid = std::any_cast<long long int>(new_thread_id);
    }
    if (ttid < 0)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": No thread for this conversation was found or created. Skipping." << std::endl;
      continue;
    }

    // in newer databases, the date_sent may need to be adjusted on insertion because of a UNIQUE constraint.
    // however this date is used as an id for the source of a quoted message. This map saves any adjusted
    // timestamps <to, from>
    std::map<long long int, long long int> adjusted_timestamps;

    //std::cout << "Match for " << person_or_group_id << std::endl;
    //std::cout << " - ID of thread in Android database that matches the conversation in desktopdb: " << ttid << std::endl;

    // we have the Android thread id (ttid) and the desktop data (results_all_conversations.value(i, "xxx")), update
    // Androids pinned and archived status if desktop is newer:
    if (desktop_is_newer)
      if (!d_database.exec("UPDATE thread SET archived = ?, pinned = ? WHERE _id = ?", {results_all_conversations.getValueAs<long long int>(i, "is_archived"),
                                                                                        results_all_conversations.getValueAs<long long int>(i, "is_pinned"),
                                                                                        ttid}))
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to update thread properties (id: " << ttid << ")" << std::endl;

    // now lets get all messages for this conversation
    SqliteDB::QueryResults results_all_messages_from_conversation;
    if (!ddb.exec("SELECT "
                  "rowid,"
                  "json_extract(json, '$.quote') AS quote,"
                  "IFNULL(json_array_length(json, '$.attachments'), 0) AS numattachments,"
                  "IFNULL(json_array_length(json, '$.reactions'), 0) AS numreactions,"
                  "IFNULL(json_array_length(json, '$.bodyRanges'), 0) AS nummentions,"
                  "IFNULL(json_array_length(json, '$.editHistory'), 0) AS editrevisions,"
                  "json_extract(json, '$.callHistoryDetails.creatorUuid') AS group_call_init,"
                  "IFNULL(json_extract(json, '$.flags'), 0) AS flags," // see 'if (type.empty())' below for FLAGS enum
                  "body,"
                  "type,"
                  "COALESCE(sent_at, json_extract(json, '$.sent_at'), json_extract(json, '$.received_at_ms'), received_at, json_extract(json, '$.received_at')) AS sent_at,"
                  "hasAttachments,"      // any attachment
                  "hasFileAttachments,"  // non-media files? (any attachment that does not get a preview?)
                  "hasVisualMediaAttachments," // ???
                  "IFNULL(isErased, 0) AS isErased,"
                  "IFNULL(isViewOnce, 0) AS isViewOnce,"
                  "serverGuid,"
                  "LOWER(" + d_dt_m_sourceuuid + ") AS 'sourceUuid',"
                  "json_extract(json, '$.source') AS sourcephone,"
                  "seenStatus,"
                  "IFNULL(json_array_length(json, '$.preview'), 0) AS haspreview,"
                  "IFNULL(json_array_length(json, '$.bodyRanges'), 0) AS hasranges,"
                  "IFNULL(json_extract(json, '$.callId'), '') AS callId,"
                  "json_extract(json, '$.sticker') IS NOT NULL AS issticker,"
                  "isStory"
                  " FROM messages WHERE conversationId = ?" + datewhereclause,
                  results_all_conversations.value(i, "id"), &results_all_messages_from_conversation))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to retrieve message from this conversation." << std::endl;
      continue;
    }
    //results_all_messages_from_conversation.prettyPrint();

    std::cout << " - Importing " << results_all_messages_from_conversation.rows() << " messages into thread._id " << ttid << std::endl;
    for (uint j = 0; j < results_all_messages_from_conversation.rows(); ++j)
    {
      std::string type = results_all_messages_from_conversation.valueAsString(j, "type");
      if (d_verbose) [[unlikely]] std::cout << "Message " << j + 1 << "/" << results_all_messages_from_conversation.rows() << ":" << (!type.empty() ? " '" + type + "'" : "") << std::endl;

      long long int rowid = results_all_messages_from_conversation.getValueAs<long long int>(j, "rowid");
      //bool hasattachments = (results_all_messages_from_conversation.getValueAs<long long int>(j, "hasAttachments") == 1);
      bool outgoing = type == "outgoing";
      bool incoming = (type == "incoming" || type == "profile-change" || type == "keychange" || type == "verified-change" || type == "change-number-notification");
      long long int numattachments = results_all_messages_from_conversation.getValueAs<long long int>(j, "numattachments");
      long long int numreactions = results_all_messages_from_conversation.getValueAs<long long int>(j, "numreactions");
      long long int nummentions = results_all_messages_from_conversation.getValueAs<long long int>(j, "nummentions");
      bool hasquote = !results_all_messages_from_conversation.isNull(j, "quote");
      long long int flags = results_all_messages_from_conversation.getValueAs<long long int>(j, "flags");
      long long int haspreview = results_all_messages_from_conversation.getValueAs<long long int>(j, "haspreview");
      long long int hasranges = results_all_messages_from_conversation.getValueAs<long long int>(j, "hasranges");
      bool issticker = results_all_messages_from_conversation.getValueAs<long long int>(j, "issticker");

      // get address (needed in both mms and sms databases)
      // for 1-on-1 messages, address is conversation partner (with uuid 'person_or_group_id')
      // for group messages, incoming: address is person originating the message (sourceUuid)
      //                     outgoing: address is id of group (with group_id 'person_or_group_id')
      // for group calls
      long long int address = -1;
      if (!results_all_messages_from_conversation.isNull(j, "group_call_init"))
      {
        // group calls always have address set to the one initiating the call
        address = getRecipientIdFromUuid(results_all_messages_from_conversation.valueAsString(j, "group_call_init"),
                                         &recipientmap, createmissingcontacts);
      }
      else if (isgroupconversation && incoming && type != "group-v1-migration")
        //if (isgroupconversation && (incoming || (type == "call-history" & something)))
      {
        // incoming group messages have 'address' set to the group member who sent the message/

        // profile change has source and sourceUuid NULL. There is 'changedId' which is the
        // conversationId of the source.
        SqliteDB::QueryResults statusmsguuid;
        if (type == "profile-change")
        {
          if (!ddb.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                        "id IS (SELECT json_extract(json, '$.changedId') FROM messages WHERE rowid IS ?) OR "
                        "e164 IS (SELECT json_extract(json, '$.changedId') FROM messages WHERE rowid IS ?)", // maybe id can be a phone number?
                        {rowid, rowid}, &statusmsguuid))
          {
            std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " failed to get uuid for incoming group profile-change." << std::endl;
            // print some extra info
            //ddb.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
          }
        }
        else if (type == "keychange")
        {
          if (!ddb.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                        + d_dt_c_uuid + " IS (SELECT json_extract(json, '$.key_changed') FROM messages WHERE rowid IS ?) OR "
                        "e164 IS (SELECT json_extract(json, '$.key_changed') FROM messages WHERE rowid IS ?)",     // 'key_changed' can be a phone number (confirmed)
                        {rowid, rowid}, &statusmsguuid))
          {
            std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " failed to get uuid for incoming group keychange." << std::endl;
            // print some extra info
            //ddb.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
          }
        }
        else if (type == "verified-change")
        {
          if (!ddb.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                        "id IS (SELECT json_extract(json, '$.verifiedChanged') FROM messages WHERE rowid IS ?) OR "
                        "e164 IS (SELECT json_extract(json, '$.verifiedChanged') FROM messages WHERE rowid IS ?)",// maybe id can be a phone number?
                        {rowid, rowid}, &statusmsguuid))
          {
            std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " failed to get uuid for incoming group verified-change." << std::endl;
            // print some extra info
            //ddb.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
          }
        }

        // NOTE this might fail on messages sent from a desktop app, those may
        // have sourceuuid == NULL (only verified on outgoing though)
        std::string source_uuid = (type == "profile-change" || type == "keychange" || type == "verified-change") ?
          statusmsguuid.valueAsString(0, "uuid") :
          results_all_messages_from_conversation.valueAsString(j, "sourceUuid");
        std::string source_phone = (type == "profile-change" || type == "keychange" || type == "verified-change") ?
          statusmsguuid.valueAsString(0, "e164") :
          results_all_messages_from_conversation.valueAsString(j, "sourcephone");

        if (source_uuid.empty() || (address = getRecipientIdFromUuid(source_uuid, &recipientmap, createmissingcontacts)) == -1) // try with phone number
          address = getRecipientIdFromPhone(source_phone, &recipientmap, createmissingcontacts);
        if (address == -1)
        {
          if (createmissingcontacts)
          {
            if ((address = dtCreateRecipient(ddb, source_uuid, source_phone,
                                             std::string(), databasedir,
                                             &recipientmap, &warned_createcontacts)) == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create contact for incoming group message. Skipping" << std::endl;
              continue;
            }
          }
          else
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to set address of incoming group message. Skipping" << std::endl;
            //std::cout << "Some more info: " << std::endl;
            //ddb.printLineMode("SELECT * from messages WHERE rowid = ?", results_all_messages_from_conversation.value(j, "rowid"));
            continue;
          }
        }
      }
      else
        address = recipientid_for_thread; // message is 1-on-1 or outgoing_group

      if (address == -1)
      {
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " failed to get recipient id for message partner. Skipping message." << std::endl;
        continue;
      }

      // PROCESS THE MESSAGE
      if (type == "call-history")
      {
        if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        handleDTCallTypeMessage(ddb, results_all_messages_from_conversation(j, "callId"), rowid, ttid, address, createmissingcontacts);
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "group-v2-change")
      {
        //if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        handleDTGroupChangeMessage(ddb, rowid, ttid, address, results_all_messages_from_conversation.valueAsInt(j, "sent_at"), &adjusted_timestamps, &recipientmap, false);

        warnOnce("Unsupported message type 'group-v2-change'. Skipping..."
                 " (this warning will be shown only once)");
        //if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "group-v1-migration")
      {
        // std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Unsupported message type '"
        //           << results_all_messages_from_conversation.valueAsString(j, "type") << "'. ";
        // // ddb.printLineMode("SELECT json_extract(json, '$.groupMigration.areWeInvited') AS areWeInvited,"
        // //                   "json_extract(json, '$.groupMigration.invitedMembers') AS invitedMembers,"
        // //                   "json_extract(json, '$.groupMigration.droppedMemberIds') AS droppedmemberIds"
        // //                   " FROM messages WHERE rowid = ?", rowid);
        // std::cout << "Skipping message." << std::endl;
        // continue;

        if (!handleDTGroupV1Migration(ddb, rowid, ttid,
                                      results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"),
                                      recipientid_for_thread, &recipientmap, createmissingcontacts, databasedir,
                                      &warned_createcontacts))
          return false;

      }
      else if (type == "timer-notification" || flags == 2) // type can be also be 'incoming' or empty
      {

        //if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        //if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        if (isgroupconversation) // in groups these are groupv2updates (not handled (yet))
        {
          if (createmissingcontacts)
            handleDTGroupChangeMessage(ddb, rowid, ttid, address, results_all_messages_from_conversation.valueAsInt(j, "sent_at"), &adjusted_timestamps, &recipientmap, true);
          else
          {
            warnOnce("Unsupported message type 'timer-notification (in group)'. Skipping... "
                     "(this warning will be shown only once)");
            //handleDTGroupChangeMessage(ddb, rowid, ttid, address, true);
          }
          continue;
        }

        if (!handleDTExpirationChangeMessage(ddb, rowid, ttid,
                                             results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"),
                                             address))
          return false;

        continue;
      }
      else if (flags == 1) // END_SESSION
      {
        long long int endsessiontype = Types::SECURE_MESSAGE_BIT |
          Types::END_SESSION_BIT |
          Types::PUSH_MESSAGE_BIT |
          (outgoing ? Types::BASE_SENDING_TYPE : Types::BASE_INBOX_TYPE);

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms",
                         {{"thread_id", ttid},
                          {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                          {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                          {"type", endsessiontype},
                          {d_sms_recipient_id, address},
                          {"read", 1}}))
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting session reset into sms" << std::endl;
        }
        else
        {
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
          {
            if (!insertRow(d_mms_table,
                           {{"thread_id", ttid},
                            {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                            {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                            {d_mms_type, endsessiontype},
                            {d_mms_recipient_id, address},
                            {"read", 1}}))
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting session reset into mms" << std::endl;
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, Types::isOutgoing(endsessiontype) ? d_selfid : address);
            if (freedate == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting session reset into mms" << std::endl;
              continue;
            }
            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table,
                           {{"thread_id", ttid},
                            {d_mms_date_sent, freedate},
                            {"date_received", freedate},
                            {d_mms_type, endsessiontype},
                            {d_mms_recipient_id, Types::isOutgoing(endsessiontype) ? d_selfid : address},
                            {"to_recipient_id", Types::isOutgoing(endsessiontype) ? address : address},
                            {"read", 1}}))
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting session reset into mms" << std::endl;
          }
        }
        continue;
      }
      else if (type == "change-number-notification")
      {
        if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", Types::CHANGE_NUMBER_TYPE},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting number-change into sms" << std::endl;
            return false;
          }
        }
        else
        {
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
          {
            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, Types::CHANGE_NUMBER_TYPE},
                                         {d_mms_recipient_id, address},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting number-change into mms" << std::endl;
              ddb.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, address);
            if (freedate == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting number-change into mms" << std::endl;
              continue;
            }

            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, freedate},
                                         {"date_received", freedate},
                                         {d_mms_type, Types::CHANGE_NUMBER_TYPE},
                                         {d_mms_recipient_id, address},
                                         {"to_recipient_id", d_selfid},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting number-change into mms" << std::endl;
              ddb.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "keychange")
      {
        if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", Types::BASE_INBOX_TYPE | Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT | Types::PUSH_MESSAGE_BIT},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting keychange into sms" << std::endl;
            return false;
          }
        }
        else
        {
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
          {
            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, Types::BASE_INBOX_TYPE | Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT | Types::PUSH_MESSAGE_BIT},
                                         {d_mms_recipient_id, address},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting keychange into mms" << std::endl;
              ddb.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, address);
            if (freedate == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting keychange into mms" << std::endl;
              continue;
            }
            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, freedate},
                                         {"date_received", freedate},
                                         {d_mms_type, Types::BASE_INBOX_TYPE | Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT | Types::PUSH_MESSAGE_BIT},
                                         {d_mms_recipient_id, address},
                                         {"to_recipient_id", d_selfid},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting keychange into mms" << std::endl;
              ddb.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "verified-change")
      {
        if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        SqliteDB::QueryResults identityverification_results;
        if (!ddb.exec("SELECT "
                      "json_extract(json, '$.local') AS 'local', "
                      "json_extract(json, '$.verified') AS 'verified' "
                      "FROM messages WHERE rowid = ?", rowid, &identityverification_results))
        {
           std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": failed to query verified-change data. Skipping message" << std::endl;
           continue;
        }

        // if local == false, it would be an incoming message on Android and
        // marked as 'You marked your safety number with CONTACT verified from another device'
        // instead of just 'You marked your safety number with CONTACT verified'
        [[maybe_unused]] bool local = identityverification_results.getValueAs<long long int>(0, "local") == 0 ? false : true;
        bool verified = identityverification_results.getValueAs<long long int>(0, "verified") == 0 ? false : true;

        // not sure if I should do anythng with local... the desktop may have been 'another device', but
        // who's to say what this android backup we're importing into is...
        long long int verifytype =
          Types::PUSH_MESSAGE_BIT | Types::SECURE_MESSAGE_BIT |
          Types::BASE_SENDING_TYPE | // if (local == false) BASE_INBOX_TYPE
          (verified ? Types::KEY_EXCHANGE_IDENTITY_VERIFIED_BIT : Types::KEY_EXCHANGE_IDENTITY_DEFAULT_BIT);

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", verifytype},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting verified-change into mms" << std::endl;
            return false;
          }
        }
        else
        {
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
          {
            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, verifytype},
                                         {d_mms_recipient_id, address},
                                         {"m_type", 128}, // probably also if (local == false) 132
                                         {"read", 1}}))              // hardcoded to 1 in Signal Android
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting verified-change into mms" << std::endl;
              return false;
            }
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, Types::isOutgoing(verifytype) ? d_selfid : address);
            if (freedate == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                        << ": Getting free date for inserting verified-change message into mms" << std::endl;
              continue;
            }
            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, verifytype},
                                         {d_mms_recipient_id, Types::isOutgoing(verifytype) ? d_selfid : address},
                                         {"to_recipient_id", Types::isOutgoing(verifytype) ? address : d_selfid},
                                         {"m_type", 128}, // probably also if (local == false) 132
                                         {"read", 1}}))              // hardcoded to 1 in Signal Android
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting verified-change into mms" << std::endl;
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "profile-change")
      {
        if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        SqliteDB::QueryResults profilechange_data;
        if (!ddb.exec("SELECT "
                      "json_extract(json, '$.profileChange.type') AS type, "
                      "IFNULL(json_extract(json, '$.profileChange.oldName'), '') AS old_name, "
                      "IFNULL(json_extract(json, '$.profileChange.newName'), '') AS new_name "
                      "FROM messages WHERE rowid = ?", rowid, &profilechange_data))
        {
           std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": failed to query profile change data. Skipping message" << std::endl;
           continue;
        }
        //profilechange_data.prettyPrint();
        if (profilechange_data.valueAsString(0, "type") != "name")
        {
          std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Unsupported message type 'profile-change' (change type: "
                    << profilechange_data.valueAsString(0, "type") << ". Skipping message" << std::endl;
          continue;
        }
        /*
          // from app/src/main/proto/Database.proto
          message ProfileChangeDetails {
            message StringChange {
              string previous = 1;
              string new      = 2;
            }
            StringChange profileNameChange = 1;
          }
        */
        std::string previousname = profilechange_data.valueAsString(0, "old_name");
        std::string newname = profilechange_data.valueAsString(0, "new_name");

        // subobject namechange:
        ProtoBufParser<protobuffer::optional::STRING,
                       protobuffer::optional::STRING> profilenamechange;
        profilenamechange.addField<1>(previousname);
        profilenamechange.addField<2>(newname);

        // full profilechange object:
        ProtoBufParser<ProtoBufParser<protobuffer::optional::STRING, // previous
                                      protobuffer::optional::STRING>> profchangefull;
        profchangefull.addField<1>(profilenamechange);

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", Types::PROFILE_CHANGE_TYPE},
                                 {"body", profchangefull.getDataString()},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting profile-change into sms" << std::endl;
            return false;
          }
        }
        else
        {
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
          {
            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, Types::PROFILE_CHANGE_TYPE},
                                         {"body", profchangefull.getDataString()},
                                         {d_mms_recipient_id, address},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting profile-change into mms" << std::endl;
              return false;
            }
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, Types::isOutgoing(Types::PROFILE_CHANGE_TYPE) ? d_selfid : address);
            if (freedate == -1)
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting profile-change into mms" << std::endl;
              continue;
            }
            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                         {"date_received", freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                         {d_mms_type, Types::PROFILE_CHANGE_TYPE},
                                         {"body", profchangefull.getDataString()},
                                         {d_mms_recipient_id, Types::isOutgoing(Types::PROFILE_CHANGE_TYPE) ? d_selfid : address},
                                         {"to_recipient_id", Types::isOutgoing(Types::PROFILE_CHANGE_TYPE) ? address : d_selfid},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting profile-change into mms" << std::endl;
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type.empty())
      {
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                  << ": Unsupported message type (empty type, flags = " << flags << "). Skipping..." << std::endl;
        /*
          Most (the only) empty message types I've seen have json$.flags = 2, but that is handled above

          enum Flags {
            END_SESSION             = 1;
            EXPIRATION_TIMER_UPDATE = 2;
            PROFILE_KEY_UPDATE      = 4;
          }
        */
        continue;
      }
      else if (!outgoing && !incoming)
      {
        warnOnce("Unsupported message type '" + type + "'. Skipping message. "
                 "(this warning will be shown only once)");
        continue;
      }


      // skip viewonce messages
      if (results_all_messages_from_conversation.valueHasType<long long int>(j, "isViewOnce") != 0 &&
          results_all_messages_from_conversation.getValueAs<long long int>(j, "isViewOnce") != 0 &&
          !createmissingcontacts)
        continue;

      // get emoji reactions
      if (d_verbose) [[unlikely]] std::cout << "Handling reactions..." << std::flush;
      std::vector<std::vector<std::string>> reactions;
      getDTReactions(ddb, rowid, numreactions, &reactions);
      if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;

      // insert the collected data in the correct tables
      if (!d_database.containsTable("sms") || // starting at dbv168, the sms table is removed altogether
          (numattachments > 0 || nummentions > 0 || hasquote || (isgroupconversation && outgoing))) // this goes in mms table on older database versions
      {
        // get quote stuff
        // if message has quote attachments, find the original message (the quote json does not contain all info)
        long long int mmsquote_id = 0;
        std::string mmsquote_author_uuid;
        long long int mmsquote_author = -1;
        std::any mmsquote_body;
        //long long int mmsquote_attachment = -1; // always -1???
        long long int mmsquote_missing = 0;
        std::pair<std::shared_ptr<unsigned char []>, size_t> mmsquote_mentions{nullptr, 0};
        long long int mmsquote_type = 0; // 0 == NORMAL, 1 == GIFT_BADGE (src/main/java/org/thoughtcrime/securesms/mms/QuoteModel.java)
        if (hasquote)
        {
          if (d_verbose) [[unlikely]] std::cout << "Gathering quote data..." << std::flush;

          //std::cout << "  Message has quote" << std::endl;
          SqliteDB::QueryResults quote_results;
          if (!ddb.exec("SELECT "
                        "json_extract(messages.json, '$.quote.id') AS quote_id,"
                        "json_extract(messages.json, '$.quote.author') AS quote_author_phone,"     // in old databases, authorUuid does not exist, but this holds the phone number
                        "conversations." + d_dt_c_uuid + " AS quote_author_uuid_from_phone,"       // this is filled from a left join on the possible phone number above
                        "LOWER(json_extract(messages.json, '$.quote.authorAci')) AS quote_author_aci," // in newer databases, this replaces the 'authorUuid'
                        "LOWER(json_extract(messages.json, '$.quote.authorUuid')) AS quote_author_uuid,"
                        "json_extract(messages.json, '$.quote.text') AS quote_text,"
                        "IFNULL(json_array_length(messages.json, '$.quote.attachments'), 0) AS num_quote_attachments,"
                        "IFNULL(json_array_length(messages.json, '$.quote.bodyRanges'), 0) AS num_quote_bodyranges,"
                        "IFNULL(json_extract(messages.json, '$.quote.type'), 0) AS quote_type,"
                        "IFNULL(json_extract(messages.json, '$.quote.referencedMessageNotFound'), 0) AS quote_referencedmessagenotfound,"
                        "IFNULL(json_extract(messages.json, '$.quote.isGiftBadge'), 0) AS quote_isgiftbadge,"  // if null because it probably does not exist in older databases
                        "IFNULL(json_extract(messages.json, '$.quote.isViewOnce'), 0) AS quote_isviewonce"
                        " FROM messages "
                        "LEFT JOIN conversations ON json_extract(messages.json, '$.quote.author') = conversations.e164 "
                        "WHERE messages.rowid = ?", rowid, &quote_results))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Quote error msg" << std::endl;
          }

          // try to set quote author from uuid or phone
          mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_aci");
          if (mmsquote_author_uuid.empty()) // possibly older database, try authorUuid
            mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_uuid");
          if (mmsquote_author_uuid.empty()) // possibly old database, try conversations.uuid
            mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_uuid_from_phone");
          if (mmsquote_author_uuid.empty()) // failed to get uuid from desktopdatabase, try matching on phone number
            mmsquote_author = getRecipientIdFromPhone(quote_results.valueAsString(0, "quote_author_phone"), &recipientmap, createmissingcontacts);
          else
            mmsquote_author = getRecipientIdFromUuid(mmsquote_author_uuid, &recipientmap, createmissingcontacts);
          if (mmsquote_author == -1)
          {
            std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to find quote author. skipping" << std::endl;

            // DEBUG
            std::cout << "Additional info:" << std::endl;
            ddb.print("SELECT json_extract(json, '$.quote') FROM messages WHERE rowid = ?", rowid);
            hasquote = false;
          }

          mmsquote_body = quote_results.valueAsString(0, "quote_text"); // check if this can be null (if quote exists, dont think so)
          mmsquote_missing = (quote_results.getValueAs<long long int>(0, "quote_referencedmessagenotfound") == false ? 0 : 1);
          mmsquote_type = (quote_results.getValueAs<long long int>(0, "quote_isgiftbadge") == false ? 0 : 1);
          if (quote_results.valueHasType<long long int>(0, "quote_id"))
            mmsquote_id = quote_results.getValueAs<long long int>(0, "quote_id"); // this is the messages.json.$timestamp or messages.sent_at. In the android
                                                                                  // db, it should be mms.date, but this should be set by this import anyway
                                                                                  // *EDIT* since there is a unique constraint on mms.date, the quoted message's
                                                                                  // date may have been adjusted!!! This needs work
          else // type is string
            mmsquote_id = bepaald::toNumber<long long int>(quote_results.valueAsString(0, "quote_id"));

          if (quote_results.getValueAs<long long int>(0, "num_quote_bodyranges") > 0)
          {
            // HEX(quote_mentions) = 0A2A080A10011A2439333732323237332D373865332D343133362D383634302D633832363139363937313463
            // PROTOBUF
            // Field #1: 0A String Length = 42, Hex = 2A, UTF8 = " $93722273-7 ..." (total 42 chars)
            // As sub-object :
            // Field #1: 08 Varint Value = 10, Hex = 0A
            // Field #2: 10 Varint Value = 1, Hex = 01
            // Field #3: 1A String Length = 36, Hex = 24, UTF8 = "93722273-78e3-41 ..." (total 36 chars)
            //
            // (actual mention)
            //         _id = 3
            //    thread_id = 39
            //   message_id = 4584
            // recipient_id = 71  (= 93722273-78e3-4136-8640-c8261969714c)
            //  range_start = 10
            // range_length = 1
            //
            // protospec (app/src/main/proto/Database.proto):
            // message BodyRangeList {
            //     message BodyRange {
            //         enum Style {
            //             BOLD   = 0;
            //             ITALIC = 1;
            //         }
            //
            //         message Button {
            //             string label  = 1;
            //             string action = 2;
            //         }
            //
            //         int32 start  = 1;
            //         int32 length = 2;
            //
            //         oneof associatedValue {
            //             string mentionUuid = 3;
            //             Style  style       = 4;
            //             string link        = 5;
            //             Button button      = 6;
            //         }
            //     }
            //     repeated BodyRange ranges = 1;
            // }

            ProtoBufParser<std::vector<ProtoBufParser<protobuffer::optional::INT32, // int32 start
                                                      protobuffer::optional::INT32, // int32 length
                                                      protobuffer::optional::STRING // in place of the oneof?
                                                      >>> bodyrangelist;
            for (uint qbr = 0; qbr < quote_results.getValueAs<long long int>(0, "num_quote_bodyranges"); ++qbr)
            {
              SqliteDB::QueryResults qbrres;
              if (!ddb.exec("SELECT "
                            "json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].start') AS qbr_start,"
                            "json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].length') AS qbr_length,"
                            "LOWER(COALESCE(json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].mentionAci'), json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].mentionUuid'))) AS qbr_uuid"
                            " FROM messages WHERE rowid = ?", rowid, &qbrres))
              {
                std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Retrieving quote bodyranges" << std::endl;
                continue;
              }
              //qbrres.prettyPrint();

              ProtoBufParser<protobuffer::optional::INT32, // int32 start
                             protobuffer::optional::INT32, // int32 length
                             protobuffer::optional::STRING // in place of the oneof?
                             > bodyrange;
              bodyrange.addField<1>(qbrres.getValueAs<long long int>(0, "qbr_start"));
              bodyrange.addField<2>(qbrres.getValueAs<long long int>(0, "qbr_length"));
              bodyrange.addField<3>(qbrres.valueAsString(0, "qbr_uuid"));
              bodyrangelist.addField<1>(bodyrange);
            }
#if __cpp_lib_shared_ptr_arrays >= 201707L
            mmsquote_mentions.first = std::make_shared<unsigned char []>(bodyrangelist.size());
#else
            mmsquote_mentions.first = std::shared_ptr<unsigned char []>(new unsigned char[bodyrangelist.size()],
                                                                        [](unsigned char *p) { delete[] p; } );
#endif
            mmsquote_mentions.second = bodyrangelist.size();
            std::memcpy(mmsquote_mentions.first.get(), bodyrangelist.data(), bodyrangelist.size());
          }
          //"mms.quote_attachment,"// = -1 Always -1??

          //quote_results.prettyPrint();
          if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        }

        if (d_verbose) [[unlikely]] std::cout << "Inserting message..." << std::flush;
        std::any retval;
        if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
        {
          if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                       {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_server", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                       {"body", results_all_messages_from_conversation.value(j, "body")},
                                       {"read", 1}, // defaults to 0, but causes tons of unread message notifications
                                       //{"delivery_receipt_count", (incoming ? 0 : 0)}, // set later in setMessagedeliveryreceipts()
                                       //{"read_receipt_count", (incoming ? 0 : 0)},     //     "" ""
                                       {d_mms_recipient_id, address},
                                       {"m_type", incoming ? 132 : 128}, // dont know what this is, but these are the values...
                                       {"quote_id", hasquote ? (bepaald::contains(adjusted_timestamps, mmsquote_id) ? adjusted_timestamps[mmsquote_id] : mmsquote_id) : 0},
                                       {"quote_author", hasquote ? std::any(mmsquote_author) : std::any(nullptr)},
                                       {"quote_body", hasquote ? mmsquote_body : nullptr},
                                       //{"quote_attachment", hasquote ? mmsquote_attachment : -1}, // removed since dbv166 so probably not important, was always -1 before
                                       {"quote_missing", hasquote ? mmsquote_missing : 0},
                                       {"quote_mentions", hasquote ? std::any(mmsquote_mentions) : std::any(nullptr)},
                                       {"remote_deleted", results_all_messages_from_conversation.value(j, "isErased")},
                                       {"view_once", results_all_messages_from_conversation.value(j, "isViewOnce")}, // if !createrecipient -> this message was already skipped
                                       {"quote_type", hasquote ? mmsquote_type : 0}}, "_id", &retval))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting into mms" << std::endl;
            return false;
          }
        }
        else
        {
          // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
          // we try to get the first free date_sent
          long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
          long long int freedate = getFreeDateForMessage(originaldate, ttid, incoming ? address : d_selfid);
          if (freedate == -1)
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting message into mms" << std::endl;
            continue;
          }
          if (originaldate != freedate)
            adjusted_timestamps[originaldate] = freedate;
          if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                       {d_mms_date_sent, freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_received", freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_server", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                       {"body", results_all_messages_from_conversation.value(j, "body")},
                                       {"read", 1}, // defaults to 0, but causes tons of unread message notifications
                                       //{"delivery_receipt_count", (incoming ? 0 : 0)}, // set later in setMessagedeliveryreceipts()
                                       //{"read_receipt_count", (incoming ? 0 : 0)},     //     "" ""
                                       {d_mms_recipient_id, incoming ? address : d_selfid},
                                       {"to_recipient_id", incoming ? d_selfid : address},
                                       {"m_type", incoming ? 132 : 128}, // dont know what this is, but these are the values...
                                       {"quote_id", hasquote ? (bepaald::contains(adjusted_timestamps, mmsquote_id) ? adjusted_timestamps[mmsquote_id] : mmsquote_id) : 0},
                                       {"quote_author", hasquote ? std::any(mmsquote_author) : std::any(nullptr)},
                                       {"quote_body", hasquote ? mmsquote_body : nullptr},
                                       //{"quote_attachment", hasquote ? mmsquote_attachment : -1}, // removed since dbv166 so probably not important, was always -1 before
                                       {"quote_missing", hasquote ? mmsquote_missing : 0},
                                       {"quote_mentions", hasquote ? std::any(mmsquote_mentions) : std::any(nullptr)},
                                       {"remote_deleted", results_all_messages_from_conversation.value(j, "isErased")},
                                       {"view_once", results_all_messages_from_conversation.value(j, "isViewOnce")}, // if !createrecipient -> this message was already skipped
                                       {"quote_type", hasquote ? mmsquote_type : 0}}, "_id", &retval))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting into mms" << std::endl;
            return false;
          }
        }

        //std::cout << "Raw any_cast 2" << std::endl;
        long long int new_mms_id = std::any_cast<long long int>(retval);
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;

        //std::cout << "  Inserted mms message, new id: " << new_mms_id << std::endl;

        // add ranges if present
        // note 'bodyRanges' on desktop is also used for mentions,
        // in that case a range is {start, end, mentionUuid}, instead of {start, end, style}.
        // (so style == NULL) these must be skipped here.
        if (hasranges)
        {
          //ddb.prettyPrint("SELECT json_extract(json, '$.bodyRanges') FROM messages WHERE rowid IS ?", rowid);
          BodyRanges bodyrangelist;
          SqliteDB::QueryResults ranges_results;
          for (uint r = 0; r < hasranges; ++r)
          {
            if (ddb.exec("SELECT "
                         "json_extract(json, '$.bodyRanges[" + bepaald::toString(r) + "].start') AS range_start,"
                         "json_extract(json, '$.bodyRanges[" + bepaald::toString(r) + "].length') AS range_length,"
                         "json_extract(json, '$.bodyRanges[" + bepaald::toString(r) + "].style') AS range_style"
                         " FROM messages WHERE rowid IS ?", rowid, &ranges_results))
            {
              if (ranges_results.isNull(0, "range_style"))
                continue;

              //ranges_results.prettyPrint();

              BodyRange bodyrange;
              if (ranges_results.getValueAs<long long int>(0, "range_start") != 0)
                bodyrange.addField<1>(ranges_results.getValueAs<long long int>(0, "range_start"));
              bodyrange.addField<2>(ranges_results.getValueAs<long long int>(0, "range_length"));
              bodyrange.addField<4>(ranges_results.getValueAs<long long int>(0, "range_style") - 1); // NOTE desktop style enum starts at 1 (android 0)
              bodyrangelist.addField<1>(bodyrange);
            }
          }
          if (bodyrangelist.size())
          {
            std::pair<unsigned char *, size_t> bodyrangesdata(bodyrangelist.data(), bodyrangelist.size());
            d_database.exec("UPDATE " + d_mms_table + " SET message_ranges = ? WHERE rowid = ?", {bodyrangesdata, new_mms_id});
          }
        }

        // insert message attachments
        if (d_verbose) [[unlikely]] std::cout << "Inserting attachments..." << std::flush;
        insertAttachments(new_mms_id, results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"), numattachments, haspreview,
                          rowid, ddb, "WHERE rowid = " + bepaald::toString(rowid), databasedir, false, issticker);
        if (hasquote && !mmsquote_missing)
        {
          // insert quotes attachments
          insertAttachments(new_mms_id, results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"), -1, 0, rowid, ddb,
                            //"WHERE (sent_at = " + bepaald::toString(mmsquote_id) + " AND sourceUuid = '" + mmsquote_author_uuid + "')", databasedir, true); // sourceUuid IS NULL if sent from desktop
                            "WHERE sent_at = " + bepaald::toString(mmsquote_id), databasedir, true, false /*issticker, not in quotes right now, need to test that*/);
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;

        if (outgoing)
          dtSetMessageDeliveryReceipts(ddb, rowid, &recipientmap, databasedir, createmissingcontacts,
                                       new_mms_id, true/*mms*/, isgroupconversation, &warned_createcontacts);

        // insert into reactions
        if (d_verbose) [[unlikely]] std::cout << "Inserting reactions..." << std::flush;
        insertReactions(new_mms_id, reactions, true, &recipientmap);
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;

        // insert into mentions
        if (d_verbose) [[unlikely]] std::cout << "Inserting mentions..." << std::flush;
        for (uint k = 0; k < nummentions; ++k)
        {
          SqliteDB::QueryResults results_mentions;
          if (!ddb.exec("SELECT "
                        "json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].start') AS start,"
                        "json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].length') AS length,"
                        "LOWER(COALESCE(json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].mentionAci'), json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].mentionUuid'))) AS mention_uuid"
                        " FROM messages WHERE rowid = ?", rowid, &results_mentions))
          {
            std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " Failed to retrieve mentions. Skipping." << std::endl;
            continue;
          }
          //std::cout << "  Mention " << k + 1 << "/" << nummentions << std::endl;

          // NOTE Desktop uses the same bodyRanges field for styling {start,length,style} and mentions {start,length,mentionUuid}.
          // if this is a style, mentionUuid will not exist, and we should skip it.
          if (results_mentions.isNull(0, "mention_uuid"))
            continue;

          long long int rec_id = getRecipientIdFromUuid(results_mentions.valueAsString(0, "mention_uuid"), &recipientmap, createmissingcontacts);
          if (rec_id == -1)
          {
            if (createmissingcontacts)
            {
              if ((rec_id = dtCreateRecipient(ddb, results_mentions("mention_uuid"), std::string(), std::string(), databasedir, &recipientmap, &warned_createcontacts)) == -1)
              {
                std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " Failed to create recipient for mention. Skipping." << std::endl;
                continue;
              }
            }
            else
            {
              std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " Failed to find recipient for mention. Skipping." << std::endl;
              continue;
            }
          }

          if (!insertRow("mention",
                         {{"thread_id", ttid},
                          {"message_id", new_mms_id},
                          {"recipient_id", rec_id},
                          {"range_start", results_mentions.getValueAs<long long int>(0, "start")},
                          {"range_length", results_mentions.getValueAs<long long int>(0, "length")}}))
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting into mention" << std::endl;
          }
          //else
          //  std::cout << "  Inserted mention" << std::endl;
        }
        if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;

      }
      else // database contains sms-table and message has no attachment/quote/mention and is not group
      {
        // insert into sms
        std::any retval;
        if (!insertRow("sms",
                       {{"thread_id", ttid},
                        {d_sms_recipient_id, address},
                        {d_sms_date_received, results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at")},
                        {"date_sent", results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at")},
                        {"date_server", results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at")},
                        {"type", Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                        {"body", results_all_messages_from_conversation.value(j, "body")},
                        {"read", 1},
                        //{"delivery_receipt_count", (incoming ? 0 : 0)}, // set later in setMessagedeliveryreceipts()
                        //{"read_receipt_count", (incoming ? 0 : 0)},     //     "" ""
                        {"remote_deleted", results_all_messages_from_conversation.value(j, "isErased")},
                        {"server_guid", results_all_messages_from_conversation.value(j, "serverGuid")}},
                       "_id", &retval))
        {
          std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting into sms" << std::endl;
          continue;
        }
        //std::cout << "Raw any_cast 3" << std::endl;
        long long int new_sms_id = std::any_cast<long long int>(retval);
        //std::cout << "  Inserted sms message, new id: " << new_sms_id << std::endl;

        // set delivery/read counts
        if (outgoing)
          dtSetMessageDeliveryReceipts(ddb, rowid, &recipientmap, databasedir, createmissingcontacts,
                                       new_sms_id, false/*mms*/, isgroupconversation, &warned_createcontacts);

        // insert into reactions
        insertReactions(new_sms_id, reactions, false, &recipientmap);
      }
    }
    //updateThreadsEntries(ttid);
  }

  for (auto const &r : recipientmap)
  {
    //std::cout << "Recpients in map: " << r.first << " : " << r.second << std::endl;
    long long int profile_date_desktop = ddb.getSingleResultAs<long long int>("SELECT profileLastFetchedAt FROM conversations WHERE " + d_dt_c_uuid + " = ? OR groupId = ? OR e164 = ?", {r.first, r.first, r.first}, 0);
    long long int profile_date_android = d_database.getSingleResultAs<long long int>("SELECT last_profile_fetch FROM recipient WHERE _id = ?", r.second, 0);

    if (profile_date_desktop > profile_date_android)
    {
      //std::cout << "Need to update profile!" << std::endl;
      // update profile from desktop.
      if (!dtUpdateProfile(ddb, r.first, r.second, databasedir))
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to update profile data." << std::endl;
    }
  }

  reorderMmsSmsIds();
  updateThreadsEntries();
  return checkDbIntegrity();
}

  /*
      EXAMPLE

      DESKTOP DB:
                     rowid = 56
                        id = 845bff95-[...]-4b53efcba27b
                      json = {"timestamp":1643874290360,
                              "attachments":[{"contentType":"application/pdf","fileName":"qrcode.pdf","path":"21/21561db325667446c84702bc2af2cb779aaaeb32c6b3d190d41f86d12e8bf5f0","size":38749,"pending":false,"url":"/home/svandijk/.config/Signal/drafts.noindex/4b/4bb11cd1be7c718ae8ed57dc28f34d57a1032d4ab0595128527466e876ddde9d"}],
                              "type":"outgoing",
                              "body":"qrcode",
                              "conversationId":"d6b93b26-[...]-b949d4de0aba",
                              "preview":[],
                              "sent_at":1643874290360,
                              "received_at":1623335267006,
                              "received_at_ms":1643874290360,
                              "recipients":["93722273-[...]-c8261969714c"],
                              "bodyRanges":[],
                              "sendHQImages":false,
                              "sendStateByConversationId":{"d6b93b26-[...]-b949d4de0aba":{"status":"Delivered","updatedAt":1643874294845},
                                                           "87e8067b-[...]-011b5c5ee23a":{"status":"Sent","updatedAt":1643874291830}},
                              "schemaVersion":10,
                              "hasAttachments":1,
                              "hasFileAttachments":1,
                              "contact":[],
                              "destination":"93722273-[...]-c8261969714c",
                              "id":"845bff95-[...]-4b53efcba27b",
                              "readStatus":0,
                              "expirationStartTimestamp":1643874291546,
                              "unidentifiedDeliveries":["93722273-[...]-c8261969714c"],
                              "errors":[],
                              "synced":true}
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
