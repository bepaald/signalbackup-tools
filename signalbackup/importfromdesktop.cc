/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

#include "../desktopdatabase/desktopdatabase.h"
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
 |-> "data_hash_start,"
 \-> "data_hash_end,"
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

bool SignalBackup::importFromDesktop(std::unique_ptr<DesktopDatabase> const &dtdb, bool skipmessagereorder,
                                     std::vector<std::string> const &daterangelist, bool createmissingcontacts,
                                     bool createmissingcontacts_valid, bool autodates, bool importstickers,
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
    {
      d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));
      if (d_selfuuid.empty())
        Logger::warning("Failed to set self-uuid");
    }
  }

  // DesktopDatabase dtdb(configdir_hint, databasedir_hint, hexkey, d_verbose, ignorewal, sqlcipherversion, d_truncate);
  if (!dtdb->ok())
  {
    Logger::error("Failed to open Signal Desktop sqlite database");
    return false;
  }

  dtSetColumnNames(&dtdb->d_database);
  //std::string configdir = dtdb->getConfigDir();
  std::string const &databasedir = dtdb->getDatabaseDir();

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

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "JSONLONG(sent_at) BETWEEN "s + bepaald::toString(startrange) + " AND " + bepaald::toString(endrange);
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }

  bool warned_createcontacts = createmissingcontacts_valid; // no warning if explicitly requesting this...

  // find out which database is newer
  long long int maxdate_desktop_db = dtdb->d_database.getSingleResultAs<long long int>("SELECT MAX(MAX(json_extract(json, '$.received_at_ms')),MAX(received_at)) FROM messages", 0);
  long long int maxdate_android_db = d_database.getSingleResultAs<long long int>("SELECT MAX(date_received) FROM " + d_mms_table, 0);
  if (d_database.containsTable("sms"))
    maxdate_android_db = d_database.getSingleResultAs<long long int>("SELECT MAX((SELECT MAX(date_received) FROM " + d_mms_table + "),(SELECT MAX(" + d_sms_date_received + ") FROM sms))", 0);
  bool desktop_is_newer = maxdate_desktop_db > maxdate_android_db;

  // get all conversations (conversationpartners) from ddb
  SqliteDB::QueryResults results_all_conversations;
  if (!dtdb->d_database.exec("SELECT "
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
  //results_all_conversations.prettyPrint(true);

  // this map will map desktop-recipient-uuid's to android recipient._id's
  std::map<std::string, long long int> recipientmap;

  // for each conversation
  for (unsigned int i = 0; i < results_all_conversations.rows(); ++i)
  {
    // skip convo's with no messages...
    SqliteDB::QueryResults messagecount;
    if (dtdb->d_database.exec("SELECT COUNT(*) AS count FROM messages WHERE conversationId = ?" + datewhereclause, results_all_conversations(i, "id"), &messagecount))
      if (messagecount.rows() == 1 && messagecount.getValueAs<long long int>(0, "count") == 0)
      {
        Logger::message("Skipping conversation, conversation has no messages ",
                        (datewhereclause.empty() ? "" : "in requested time period "),
                        "(", i + 1, "/", results_all_conversations.rows(), ")");
        continue;
      }

    Logger::message("Trying to match conversation "
                    "(", Logger::Control::BOLD, i + 1, "/", results_all_conversations.rows(), Logger::Control::NORMAL, ")"
                    " (type: ", results_all_conversations.valueAsString(i, "type"), ")");

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
          Logger::error("Conversation is 'group'-type, but groupId unexpectedly was not base64 data. Maybe this is a groupV1 group? Here is the data: ");
          dtdb->d_database.printLineMode("SELECT * FROM conversations WHERE id = ?", results_all_conversations.value(i, "id"));
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
          if (d_verbose) [[unlikely]]
            Logger::message("Trying to match group-v1 by 'derivedGroupV2Id'");
          person_or_group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, true);
          if (getRecipientIdFromUuidMapped(person_or_group_id, &recipientmap) != -1)
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
          dtdb->d_database.exec("SELECT HEX(groupId) FROM conversations WHERE id = ?", results_all_conversations.value(i, "id"), &groupid_res);
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
      recipientid_for_thread = getRecipientIdFromUuidMapped(person_or_group_id, &recipientmap, createmissingcontacts);
    else
    {
      Logger::warning("Failed to determine uuid. Trying with phone number...");
      phone = results_all_conversations.valueAsString(i, "e164");
      if (!phone.empty())
        recipientid_for_thread = getRecipientIdFromPhoneMapped(phone, &recipientmap, createmissingcontacts);
    }

    if (recipientid_for_thread == -1)
    {
      if (createmissingcontacts)
      {
        recipientid_for_thread = dtCreateRecipient(dtdb->d_database, person_or_group_id, results_all_conversations.valueAsString(i, "e164"),
                                                   results_all_conversations.valueAsString(i, "groupId"), databasedir, &recipientmap,
                                                   createmissingcontacts_valid,  &warned_createcontacts);
        if (recipientid_for_thread == -1)
        {
          Logger::warning("Failed to create missing recipient. Skipping.");
          continue;
        }
      }
    }

    if (recipientid_for_thread == -1)
    {
      Logger::warning("Chat partner was not found in recipient-table. Skipping. (id: ",
                      (person_or_group_id.empty() ? results_all_conversations.valueAsString(i, "e164") : person_or_group_id), ")");
      continue;
    }

    SqliteDB::QueryResults results2;
    long long int ttid = -1;
    if (!d_database.exec("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", recipientid_for_thread, &results2))
      continue;
    if (results2.rows() == 1) // we have found our matching thread
      ttid = results2.getValueAs<long long int>(0, "_id"); // ttid : target thread id
    else if (results2.rows() == 0) // the query was succesful, but yielded no results -> create thread
    {
      Logger::warning_start("Failed to find matching thread for conversation, creating. (",
                            (person_or_group_id.empty() ? "from e164" : ("id: " + person_or_group_id)));
      std::any new_thread_id;
      if (!insertRow("thread",
                     {{d_thread_recipient_id, recipientid_for_thread},
                      {"active", 1},
                      {"archived", results_all_conversations.getValueAs<long long int>(i, "is_archived")},
                      {"pinned", results_all_conversations.getValueAs<long long int>(i, "is_pinned")}},
                     "_id", &new_thread_id))
      {
        Logger::message_end();
        Logger::error("Failed to create thread for desktop conversation. (",
                      (person_or_group_id.empty() ? "from e164" : ("id: " + person_or_group_id)), "), skipping.");
        continue;
      }
      //std::cout << "Raw any_cast 1" << std::endl;
      ttid = std::any_cast<long long int>(new_thread_id);
      Logger::message(", thread_id: ", ttid, ")");
    }
    if (ttid < 0)
    {
      Logger::error("No thread for this conversation was found or created. Skipping.");
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
        Logger::warning("Failed to update thread properties (id: ", ttid, ")");

    // now lets get all messages for this conversation
    SqliteDB::QueryResults results_all_messages_from_conversation;
    if (!dtdb->d_database.exec("SELECT "
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
                              "JSONLONG(COALESCE(sent_at, json_extract(json, '$.sent_at'), json_extract(json, '$.received_at_ms'), received_at, json_extract(json, '$.received_at'))) AS sent_at,"
                              "hasAttachments,"      // any attachment
                              "hasFileAttachments,"  // non-media files? (any attachment that does not get a preview?)
                              "hasVisualMediaAttachments," // ???
                              "IFNULL(isErased, 0) AS isErased,"
                              "IFNULL(isViewOnce, 0) AS isViewOnce,"
                              "serverGuid,"
                              "LOWER(" + d_dt_m_sourceuuid + ") AS 'sourceUuid',"
                              "json_extract(json, '$.source') AS sourcephone,"
                              "JSONLONG(expireTimer) AS expireTimer,"
                              "seenStatus,"
                              "IFNULL(json_array_length(json, '$.preview'), 0) AS haspreview,"
                              "IFNULL(json_array_length(json, '$.bodyRanges'), 0) AS hasranges,"
                              "IFNULL(json_array_length(json, '$.contact'), 0) AS hassharedcontact,"
                              "IFNULL(json_extract(json, '$.callId'), '') AS callId,"
                              "json_extract(json, '$.sticker') IS NOT NULL AS issticker,"
                              "isStory"
                              " FROM messages WHERE conversationId = ?" + datewhereclause,
                              results_all_conversations.value(i, "id"), &results_all_messages_from_conversation))
    {
      Logger::error("Failed to retrieve message from this conversation.");
      continue;
    }
    //results_all_messages_from_conversation.printLineMode();

    Logger::message(" - Importing ", results_all_messages_from_conversation.rows(), " messages into thread._id ", ttid);
    for (unsigned int j = 0; j < results_all_messages_from_conversation.rows(); ++j)
    {
      std::string type = results_all_messages_from_conversation.valueAsString(j, "type");
      if (d_verbose) [[unlikely]]
        Logger::message("Message ", j + 1, "/", results_all_messages_from_conversation.rows(), ":",
                        (!type.empty() ? " '" + type + "'" : ""),
                        " (rowid: ", results_all_messages_from_conversation.getValueAs<long long int>(j, "rowid"), ")");

      long long int rowid = results_all_messages_from_conversation.getValueAs<long long int>(j, "rowid");
      //bool hasattachments = (results_all_messages_from_conversation.getValueAs<long long int>(j, "hasAttachments") == 1);
      bool outgoing = (type == "outgoing" || type == "message-request-response-event");
      bool incoming = (type == "incoming" || type == "profile-change" || type == "keychange" || type == "verified-change" || type == "change-number-notification");
      long long int numattachments = results_all_messages_from_conversation.getValueAs<long long int>(j, "numattachments");
      long long int numreactions = results_all_messages_from_conversation.getValueAs<long long int>(j, "numreactions");
      long long int nummentions = results_all_messages_from_conversation.getValueAs<long long int>(j, "nummentions");
      bool hasquote = !results_all_messages_from_conversation.isNull(j, "quote");
      long long int flags = results_all_messages_from_conversation.getValueAs<long long int>(j, "flags");
      long long int haspreview = results_all_messages_from_conversation.getValueAs<long long int>(j, "haspreview");
      long long int hasranges = results_all_messages_from_conversation.getValueAs<long long int>(j, "hasranges");
      long long int hassharedcontact = results_all_messages_from_conversation.getValueAs<long long int>(j, "hassharedcontact");
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
        address = getRecipientIdFromUuidMapped(results_all_messages_from_conversation.valueAsString(j, "group_call_init"),
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
          if (!dtdb->d_database.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                                    "id IS (SELECT json_extract(json, '$.changedId') FROM messages WHERE rowid IS ?1) OR "
                                    "e164 IS (SELECT json_extract(json, '$.changedId') FROM messages WHERE rowid IS ?1)", // maybe id can be a phone number?
                                    rowid, &statusmsguuid))
          {
            Logger::warning("Failed to get uuid for incoming group profile-change.");
            // print some extra info
            //dtdb->d_database.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
          }
        }
        else if (type == "keychange")
        {
          if (!dtdb->d_database.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                                    + d_dt_c_uuid + " IS (SELECT json_extract(json, '$.key_changed') FROM messages WHERE rowid IS ?1) OR "
                                    "e164 IS (SELECT json_extract(json, '$.key_changed') FROM messages WHERE rowid IS ?1)",     // 'key_changed' can be a phone number (confirmed)
                                    rowid, &statusmsguuid))
          {
            Logger::warning("Failed to get uuid for incoming group keychange.");
            // print some extra info
            //dtdb->d_database.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
          }
        }
        else if (type == "verified-change")
        {
          if (!dtdb->d_database.exec("SELECT " + d_dt_c_uuid + " AS uuid, e164 FROM conversations WHERE "
                                    "id IS (SELECT json_extract(json, '$.verifiedChanged') FROM messages WHERE rowid IS ?1) OR "
                                    "e164 IS (SELECT json_extract(json, '$.verifiedChanged') FROM messages WHERE rowid IS ?1)",// maybe id can be a phone number?
                                    rowid, &statusmsguuid))
          {
            Logger::warning("Failed to get uuid for incoming group verified-change.");
            // print some extra info
            //dtdb->d_database.printLineMode("SELECT * FROM messaages WHERE rowid IS ?)", rowid);
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

        if (source_uuid.empty() || (address = getRecipientIdFromUuidMapped(source_uuid, &recipientmap, createmissingcontacts)) == -1) // try with phone number
          address = getRecipientIdFromPhoneMapped(source_phone, &recipientmap, createmissingcontacts);
        if (address == -1)
        {
          if (createmissingcontacts)
          {
            if ((address = dtCreateRecipient(dtdb->d_database, source_uuid, source_phone,
                                             std::string(), databasedir, &recipientmap,
                                             createmissingcontacts_valid, &warned_createcontacts)) == -1)
            {
              Logger::error("Failed to create contact for incoming group message. Skipping");
              continue;
            }
          }
          else
          {
            Logger::error("Failed to set address of incoming group message. Skipping");
            //std::cout << "Some more info: " << std::endl;
            //dtdb->d_database.printLineMode("SELECT * from messages WHERE rowid = ?", results_all_messages_from_conversation.value(j, "rowid"));
            continue;
          }
        }
      }
      else
        address = recipientid_for_thread; // message is 1-on-1 or outgoing_group

      if (address == -1)
      {
        Logger::warning("Failed to get recipient id for message partner. Skipping message.");
        continue;
      }

      // PROCESS THE MESSAGE
      if (type == "call-history")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");
        handleDTCallTypeMessage(dtdb->d_database, results_all_messages_from_conversation(j, "callId"), rowid, ttid, address, createmissingcontacts);
        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type == "group-v2-change")
      {
        //if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        handleDTGroupChangeMessage(dtdb->d_database, rowid, ttid, address, results_all_messages_from_conversation.valueAsInt(j, "sent_at"), &adjusted_timestamps, &recipientmap, false);

        warnOnce("Unsupported message type 'group-v2-change'. Skipping..."
                 " (this warning will be shown only once)");
        //if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        continue;
      }
      else if (type == "group-v1-migration")
      {
        // std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Unsupported message type '"
        //           << results_all_messages_from_conversation.valueAsString(j, "type") << "'. ";
        // // dtdb->d_database.printLineMode("SELECT json_extract(json, '$.groupMigration.areWeInvited') AS areWeInvited,"
        // //                   "json_extract(json, '$.groupMigration.invitedMembers') AS invitedMembers,"
        // //                   "json_extract(json, '$.groupMigration.droppedMemberIds') AS droppedmemberIds"
        // //                   " FROM messages WHERE rowid = ?", rowid);
        // std::cout << "Skipping message." << std::endl;
        // continue;

        if (!handleDTGroupV1Migration(dtdb->d_database, rowid, ttid,
                                      results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"),
                                      recipientid_for_thread, &recipientmap, createmissingcontacts, databasedir,
                                      createmissingcontacts_valid, &warned_createcontacts))
          return false;

      }
      else if (type == "timer-notification" || flags == 2) // type can be also be 'incoming' or empty
      {

        //if (d_verbose) [[unlikely]] std::cout << "Dealing with " << type << " message... " << std::flush;
        //if (d_verbose) [[unlikely]] std::cout << "done" << std::endl;
        if (isgroupconversation) // in groups these are groupv2updates (not handled (yet))
        {
          if (createmissingcontacts)
            handleDTGroupChangeMessage(dtdb->d_database, rowid, ttid, address, results_all_messages_from_conversation.valueAsInt(j, "sent_at"), &adjusted_timestamps, &recipientmap, true);
          else
          {
            warnOnce("Unsupported message type 'timer-notification (in group)'. Skipping... "
                     "(this warning will be shown only once)");
            //handleDTGroupChangeMessage(dtdb->d_database, rowid, ttid, address, true);
          }
          continue;
        }

        if (!handleDTExpirationChangeMessage(dtdb->d_database, rowid, ttid,
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
            Logger::error("Inserting session reset into sms");
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
              Logger::error("Inserting session reset into mms");
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, Types::isOutgoing(endsessiontype) ? d_selfid : address);
            if (freedate == -1)
            {
              Logger::error("Getting free date for inserting session reset into mms");
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
              Logger::error("Inserting session reset into mms");
          }
        }
        continue;
      }
      else if (type == "change-number-notification")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", Types::CHANGE_NUMBER_TYPE},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting number-change into sms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting number-change into mms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Getting free date for inserting number-change into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting number-change into mms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type == "keychange")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");
        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", Types::BASE_INBOX_TYPE | Types::KEY_EXCHANGE_IDENTITY_UPDATE_BIT | Types::PUSH_MESSAGE_BIT},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting keychange into sms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting keychange into mms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Getting free date for inserting keychange into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting keychange into mms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type == "verified-change")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");
        SqliteDB::QueryResults identityverification_results;
        if (!dtdb->d_database.exec("SELECT "
                                  "json_extract(json, '$.local') AS 'local', "
                                  "json_extract(json, '$.verified') AS 'verified' "
                                  "FROM messages WHERE rowid = ?", rowid, &identityverification_results))
        {
          if (d_verbose) [[unlikely]] Logger::message_end();
          Logger::error("Failed to query verified-change data. Skipping message");
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
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting verified-change into sms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting verified-change into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Getting free date for inserting verified-change message into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting verified-change into mms");
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type == "profile-change")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");
        SqliteDB::QueryResults profilechange_data;
        if (!dtdb->d_database.exec("SELECT "
                                  "json_extract(json, '$.profileChange.type') AS type, "
                                  "IFNULL(json_extract(json, '$.profileChange.oldName'), '') AS old_name, "
                                  "IFNULL(json_extract(json, '$.profileChange.newName'), '') AS new_name "
                                  "FROM messages WHERE rowid = ?", rowid, &profilechange_data))
        {
          if (d_verbose) [[unlikely]] Logger::message_end();
          Logger::error("Failed to query profile change data. Skipping message");
          continue;
        }
        //profilechange_data.prettyPrint();
        if (profilechange_data.valueAsString(0, "type") != "name")
        {
          if (d_verbose) [[unlikely]] Logger::message_end();
          Logger::warning("Unsupported message type 'profile-change' (change type: ",
                          profilechange_data.valueAsString(0, "type"), ". Skipping message");
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
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting profile-change into sms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting profile-change into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Getting free date for inserting profile-change into mms");
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
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting profile-change into mms");
              return false;
            }
          }
        }
        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type == "message-request-response-event")
      {
        if (d_verbose) [[unlikely]]
          Logger::message_start("Dealing with ", type, " message... ");

        /*
          message MessageRequestResponse {
          enum Type {
          UNKNOWN          = 0;
          ACCEPT           = 1;
          DELETE           = 2;
          BLOCK            = 3;
          BLOCK_AND_DELETE = 4;
          SPAM             = 5;
          BLOCK_AND_SPAM   = 6;
        }*/
        std::string request_response(dtdb->d_database.getSingleResultAs<std::string>("SELECT json_extract(json, '$.messageRequestResponseEvent') FROM messages WHERE rowid = ?", rowid, std::string()));
        uint64_t response_type = 0;
        if (request_response == "ACCEPT")
          response_type = Types::SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED;
        //else if (request_response == "DELETE")
        //  ;
        //else if (request_response == "BLOCK")
        //  ;
        //else if (request_response == "BLOCK_AND_DELETE")
        //  ;
        //else if (request_response == "SPAM")
        //  response_type = Types::SPECIAL_TYPE_REPORTED_SPAM;
        //else if (request_response == "BLOCK_AND_SPAM")
        //  response_type = Types::SPECIAL_TYPE_REPORTED_SPAM;
        //else if (request_response == "")
        //  ;
        else
        {
          // unupported (yet?)
          // im not sure any of the other response options are actually saved in the android message table.
          // possibly a 'SPAM' response gets inserted with the SPECIAL_TYPE_REPORTED_SPAM type, but this is not
          // confirmed.
          if (d_verbose) [[unlikely]] Logger::message_end();
          Logger::warning("Unsupported message type 'message-request-response-event' WITH "
                          "messageRequestResponseEvent='", request_response, ". Skipping message");
          continue;
        }

        long long message_request_accepted_type = response_type | Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (outgoing ? Types::BASE_SENDING_TYPE : Types::BASE_INBOX_TYPE);

        if (d_database.containsTable("sms"))
        {
          if (!insertRow("sms", {{"thread_id", ttid},
                                 {"date_sent", results_all_messages_from_conversation.value(j, "sent_at")},
                                 {d_sms_date_received, results_all_messages_from_conversation.value(j, "sent_at")},
                                 {"type", message_request_accepted_type},
                                 {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                 {d_sms_recipient_id, address}}))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting ", type, " into sms");
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
                                         {d_mms_type, message_request_accepted_type},
                                         {d_mms_recipient_id, address},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting ", type, " into sms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
          else
          {
            // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
            // we try to get the first free date_sent
            long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
            long long int freedate = getFreeDateForMessage(originaldate, ttid, Types::isOutgoing(message_request_accepted_type) ? d_selfid : address);
            if (freedate == -1)
            {
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Getting free date for inserting ", type, " into mms");
              continue;
            }
            if (originaldate != freedate)
              adjusted_timestamps[originaldate] = freedate;

            if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                         {d_mms_date_sent, freedate},
                                         {"date_received", freedate},
                                         {d_mms_type, message_request_accepted_type},
                                         {d_mms_recipient_id, Types::isOutgoing(message_request_accepted_type) ? d_selfid : address},
                                         {"to_recipient_id", Types::isOutgoing(message_request_accepted_type) ? address : d_selfid},
                                         {d_mms_recipient_device_id, 1}, // not sure what this is but at least for profile-change
                                         {"read", 1}}))                  // it is hardcoded to 1 in Signal Android (as is 'read')
            {
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::error("Inserting ", type, " into mms");
              dtdb->d_database.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
              return false;
            }
          }
        }

        if (d_verbose) [[unlikely]]
          Logger::message_end("done");
        continue;
      }
      else if (type.empty())
      {
        Logger::warning("Unsupported message type (empty type, flags = ", flags, "). Skipping...");
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
        if (!d_verbose) [[likely]]
          warnOnce("Unhandled message type '" + type + "'. Skipping message. "
                   "(this warning will be shown only once)");
        else [[unlikely]]
        {
          // get some extra info and show it (threadname, timestamp?)
          long long int originaldate = results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at");
          std::string date = "(unknown)";
          std::string threadtitle = getNameFromRecipientId(address);
          if (threadtitle.empty())
            threadtitle = "(unknown)";
          if (originaldate != -1)
            date = bepaald::toDateString(originaldate / 1000, "%Y-%m-%d %H:%M:%S");

          Logger::warning("Unhandled message type '" + type + "'. Skipping message. Threadtitle: \"", threadtitle, "\", Date: ", date);
          Logger::warning_indent("Raw message data:");
          results_all_messages_from_conversation.printLineMode(j);
        }

        continue;
      }

      // skip viewonce messages
      if (results_all_messages_from_conversation.valueHasType<long long int>(j, "isViewOnce") != 0 &&
          results_all_messages_from_conversation.getValueAs<long long int>(j, "isViewOnce") != 0 &&
          !createmissingcontacts)
        continue;

      std::string shared_contacts_json;
      if (hassharedcontact)
      {
        shared_contacts_json = dtSetSharedContactsJsonString(dtdb->d_database, rowid);
        //std::cout << shared_contacts_json << std::endl;
        //warnOnce("Message is 'contact share'. This is not yet supported, skipping...");
        //continue;
      }

      // get emoji reactions
      if (d_verbose) [[unlikely]] Logger::message_start("Handling reactions...");
      std::vector<std::vector<std::string>> reactions;
      getDTReactions(dtdb->d_database, rowid, numreactions, &reactions);
      if (d_verbose) [[unlikely]] Logger::message_end("done");

      // LONG_TEXT messages (> 2000 bytes) are sent with an attachment in android (not on desktop)
      std::string msgbody = results_all_messages_from_conversation(j, "body");
      std::string msgbody_full;
      if (utf8Chars(msgbody) > 2000)
      {
        msgbody_full = msgbody;
        resizeToNUtf8Chars(msgbody, 2000);
      }

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
          if (d_verbose) [[unlikely]] Logger::message_start("Gathering quote data...");

          //std::cout << "  Message has quote" << std::endl;
          SqliteDB::QueryResults quote_results;
          if (!dtdb->d_database.exec("SELECT "
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
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Quote error msg");
          }

          // try to set quote author from uuid or phone
          mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_aci");
          if (mmsquote_author_uuid.empty()) // possibly older database, try authorUuid
            mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_uuid");
          if (mmsquote_author_uuid.empty()) // possibly old database, try conversations.uuid
            mmsquote_author_uuid = quote_results.valueAsString(0, "quote_author_uuid_from_phone");
          if (mmsquote_author_uuid.empty()) // failed to get uuid from desktopdatabase, try matching on phone number
            mmsquote_author = getRecipientIdFromPhoneMapped(quote_results.valueAsString(0, "quote_author_phone"), &recipientmap, createmissingcontacts);
          else
            mmsquote_author = getRecipientIdFromUuidMapped(mmsquote_author_uuid, &recipientmap, createmissingcontacts);
          if (mmsquote_author == -1)
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::warning("Failed to find quote author. skipping");

            // DEBUG
            Logger::message("Additional info:");
            dtdb->d_database.print("SELECT json_extract(json, '$.quote') FROM messages WHERE rowid = ?", rowid);
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
            // recipient_id = 71  (= 93722273-78e3-41 ...)
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

            BodyRanges bodyrangelist;
            for (unsigned int qbr = 0; qbr < quote_results.getValueAs<long long int>(0, "num_quote_bodyranges"); ++qbr)
            {
              SqliteDB::QueryResults qbrres;
              if (!dtdb->d_database.exec("SELECT "
                                        "json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].start') AS qbr_start,"
                                        "json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].length') AS qbr_length,"
                                        "json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].style') AS qbr_style,"
                                        "LOWER(COALESCE(json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].mentionAci'), json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "].mentionUuid'))) AS qbr_uuid "
                                        "FROM messages WHERE rowid = ?", rowid, &qbrres))
              {
                if (d_verbose) [[unlikely]] Logger::message_end();
                Logger::error("Retrieving quote bodyranges");
                continue;
              }
              //qbrres.prettyPrint();

              if (qbrres.isNull(0, "qbr_style"))
              {
                if (qbrres.isNull(0, "qbr_uuid")) [[unlikely]] // if style = null, this must be a mention
                {
                  if (d_verbose) [[unlikely]] Logger::message_end();
                  Logger::warning("Quote-bodyrange contains no recipient and no style. Skipping.");
                  dtdb->d_database.prettyPrint(d_truncate,
                                              "SELECT json_extract(json, '$.quote.bodyRanges[" + bepaald::toString(qbr) + "] FROM messages WHERE rowid = ?", rowid);
                  continue;
                }

                long long int rec_id = getRecipientIdFromUuidMapped(qbrres.valueAsString(0, "qbr_uuid"), &recipientmap, createmissingcontacts);
                if (rec_id == -1)
                {
                  if (createmissingcontacts)
                  {
                    if (dtCreateRecipient(dtdb->d_database, qbrres.valueAsString(0, "qbr_uuid"), std::string(), std::string(),
                                          databasedir, &recipientmap, createmissingcontacts_valid, &warned_createcontacts) == -1)
                    {
                      if (d_verbose) [[unlikely]] Logger::message_end();
                      Logger::warning("Failed to create recipient for quote-mention. Skipping.");
                      continue;
                    }
                  }
                  else
                  {
                    if (d_verbose) [[unlikely]] Logger::message_end();
                    Logger::warning("Failed to find recipient for quote-mention. Skipping.");
                    continue;
                  }
                }
              }

              BodyRange bodyrange;
              bodyrange.addField<1>(qbrres.getValueAs<long long int>(0, "qbr_start"));
              bodyrange.addField<2>(qbrres.getValueAs<long long int>(0, "qbr_length"));

              if (qbrres.isNull(0, "qbr_style"))
                bodyrange.addField<3>(qbrres.valueAsString(0, "qbr_uuid"));
              else
                bodyrange.addField<4>(qbrres.getValueAs<long long int>(0, "qbr_style") - 1); // NOTE desktop style enum starts at 1 (android at 0)

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
          if (d_verbose) [[unlikely]] Logger::message_end("done");
        }

        if (d_verbose) [[unlikely]] Logger::message_start("Inserting message...");
        std::any retval;
        if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
        {
          if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                       {d_mms_date_sent, results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_received", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_server", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                       {"body", msgbody},
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
                                       {"shared_contacts", shared_contacts_json.empty() ? std::any(nullptr) : std::any(shared_contacts_json)},
                                       {"remote_deleted", results_all_messages_from_conversation.value(j, "isErased")},
                                       {((!results_all_messages_from_conversation.isNull(j, "expireTimer") &&
                                          results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) != 0) ? "expires_in" : ""), results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) * 1000},
                                       {"view_once", results_all_messages_from_conversation.value(j, "isViewOnce")}, // if !createrecipient -> this message was already skipped
                                       {"quote_type", hasquote ? mmsquote_type : 0}}, "_id", &retval))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting into mms");
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
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Getting free date for inserting message into mms");
            continue;
          }
          if (originaldate != freedate)
            adjusted_timestamps[originaldate] = freedate;
          if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                       {d_mms_date_sent, freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_received", freedate},//results_all_messages_from_conversation.value(j, "sent_at")},
                                       {"date_server", results_all_messages_from_conversation.value(j, "sent_at")},
                                       {d_mms_type, Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                       {"body", msgbody},
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
                                       {"shared_contacts", shared_contacts_json.empty() ? std::any(nullptr) : std::any(shared_contacts_json)},
                                       {"remote_deleted", results_all_messages_from_conversation.value(j, "isErased")},
                                       {((!results_all_messages_from_conversation.isNull(j, "expireTimer") &&
                                          results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) != 0) ? "expires_in" : ""), results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) * 1000},
                                       {"view_once", results_all_messages_from_conversation.value(j, "isViewOnce")}, // if !createrecipient -> this message was already skipped
                                       {"quote_type", hasquote ? mmsquote_type : 0}}, "_id", &retval))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting into mms");
            return false;
          }
        }

        //std::cout << "Raw any_cast 2" << std::endl;
        long long int new_mms_id = std::any_cast<long long int>(retval);
        if (d_verbose) [[unlikely]] Logger::message_end("done");

        // add ranges if present
        // note 'bodyRanges' on desktop is also used for mentions,
        // in that case a range is {start, end, mentionUuid}, instead of {start, end, style}.
        // (so style == NULL) these must be skipped here.
        if (hasranges)
        {
          //dtdb->d_database.prettyPrint("SELECT json_extract(json, '$.bodyRanges') FROM messages WHERE rowid IS ?", rowid);
          BodyRanges bodyrangelist;
          SqliteDB::QueryResults ranges_results;
          for (unsigned int r = 0; r < hasranges; ++r)
          {
            if (dtdb->d_database.exec("SELECT "
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
          if (bodyrangelist.size() && d_database.tableContainsColumn(d_mms_table, d_mms_ranges))
          {
            std::pair<unsigned char *, size_t> bodyrangesdata(bodyrangelist.data(), bodyrangelist.size());
            d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_ranges + " = ? WHERE rowid = ?", {bodyrangesdata, new_mms_id});
          }
        }

        // insert message attachments
        if (d_verbose) [[unlikely]] Logger::message_start("Inserting attachments...");
        dtInsertAttachments(new_mms_id, results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"), numattachments, haspreview,
                          rowid, dtdb->d_database, "WHERE rowid = " + bepaald::toString(rowid), databasedir, false, issticker);
        if (hasquote && !mmsquote_missing)
        {
          // insert quotes attachments
          dtInsertAttachments(new_mms_id, results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"), -1, 0, rowid, dtdb->d_database,
                            //"WHERE (sent_at = " + bepaald::toString(mmsquote_id) + " AND sourceUuid = '" + mmsquote_author_uuid + "')", databasedir, true); // sourceUuid IS NULL if sent from desktop
                            "WHERE JSONLONG(sent_at) = " + bepaald::toString(mmsquote_id), databasedir, true, false /*issticker, not in quotes right now, need to test that*/);
        }
        if (d_verbose) [[unlikely]] Logger::message_end("done");

        // insert LONG_TEXT attachment
        if (!msgbody_full.empty())
          dtImportLongText(msgbody_full, new_mms_id,
                           results_all_messages_from_conversation.getValueAs<long long int>(j, "sent_at"));

        if (outgoing)
          dtSetMessageDeliveryReceipts(dtdb->d_database, rowid, &recipientmap, databasedir, createmissingcontacts,
                                       new_mms_id, true/*mms*/, isgroupconversation, createmissingcontacts_valid,
                                       &warned_createcontacts);

        // insert into reactions
        if (d_verbose) [[unlikely]] Logger::message_start("Inserting reactions...");
        insertReactions(new_mms_id, reactions, true, &recipientmap);
        if (d_verbose) [[unlikely]] Logger::message_end("done");

        // insert into mentions
        if (d_verbose) [[unlikely]] Logger::message_start("Inserting mentions...");
        for (unsigned int k = 0; k < nummentions; ++k)
        {
          SqliteDB::QueryResults results_mentions;
          if (!dtdb->d_database.exec("SELECT "
                                    "json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].start') AS start,"
                                    "json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].length') AS length,"
                                    "LOWER(COALESCE(json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].mentionAci'), json_extract(json, '$.bodyRanges[" + bepaald::toString(k) + "].mentionUuid'))) AS mention_uuid"
                                    " FROM messages WHERE rowid = ?", rowid, &results_mentions))
          {
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::warning("Failed to retrieve mentions. Skipping.");
            continue;
          }
          //std::cout << "  Mention " << k + 1 << "/" << nummentions << std::endl;

          // NOTE Desktop uses the same bodyRanges field for styling {start,length,style} and mentions {start,length,mentionUuid}.
          // if this is a style, mentionUuid will not exist, and we should skip it.
          if (results_mentions.isNull(0, "mention_uuid"))
            continue;

          long long int rec_id = getRecipientIdFromUuidMapped(results_mentions.valueAsString(0, "mention_uuid"), &recipientmap, createmissingcontacts);
          if (rec_id == -1)
          {
            if (createmissingcontacts)
            {
              if ((rec_id = dtCreateRecipient(dtdb->d_database, results_mentions("mention_uuid"), std::string(), std::string(),
                                              databasedir, &recipientmap, createmissingcontacts_valid, &warned_createcontacts)) == -1)
              {
                if (d_verbose) [[unlikely]] Logger::message_end();
                Logger::warning("Failed to create recipient for mention. Skipping.");
                continue;
              }
            }
            else
            {
              if (d_verbose) [[unlikely]] Logger::message_end();
              Logger::warning("Failed to find recipient for mention. Skipping.");
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
            if (d_verbose) [[unlikely]] Logger::message_end();
            Logger::error("Inserting into mention");
          }
          //else
          //  std::cout << "  Inserted mention" << std::endl;
        }
        if (d_verbose) [[unlikely]] Logger::message_end("done");

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
                        {((!results_all_messages_from_conversation.isNull(j, "expireTimer") &&
                           results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) != 0) ? "expires_in" : ""), results_all_messages_from_conversation.valueAsInt(j, "expireTimer", 0) * 1000},
                        {"server_guid", results_all_messages_from_conversation.value(j, "serverGuid")}},
                       "_id", &retval))
        {
          Logger::error("Inserting into sms");
          continue;
        }
        //std::cout << "Raw any_cast 3" << std::endl;
        long long int new_sms_id = std::any_cast<long long int>(retval);
        //std::cout << "  Inserted sms message, new id: " << new_sms_id << std::endl;

        // set delivery/read counts
        if (outgoing)
          dtSetMessageDeliveryReceipts(dtdb->d_database, rowid, &recipientmap, databasedir, createmissingcontacts,
                                       new_sms_id, false/*mms*/, isgroupconversation, createmissingcontacts_valid,
                                       &warned_createcontacts);

        // insert into reactions
        insertReactions(new_sms_id, reactions, false, &recipientmap);
      }
    }
    //updateThreadsEntries(ttid);
  }

  for (auto const &r : recipientmap)
  {
    //std::cout << "Recpients in map: " << r.first << " : " << r.second << std::endl;
    long long int profile_date_desktop = dtdb->d_database.getSingleResultAs<long long int>("SELECT profileLastFetchedAt FROM conversations WHERE " + d_dt_c_uuid + " = ?1 OR groupId = ?1 OR e164 = ?1", r.first, 0);
    long long int profile_date_android = d_database.getSingleResultAs<long long int>("SELECT last_profile_fetch FROM recipient WHERE _id = ?", r.second, 0);
    //std::cout << "Profile update? : " << r.first << " " << profile_date_desktop << " " << profile_date_android << std::endl;
    if (profile_date_desktop > profile_date_android)
    {
      //std::cout << "Need to update profile!" << std::endl;
      // update profile from desktop.
      if (d_verbose) [[unlikely]]
        Logger::message("Attempting to update profile");

      if (!dtUpdateProfile(dtdb->d_database, r.first, r.second, databasedir))
        Logger::warning("Failed to update profile data.");
    }
  }

  if (importstickers)
  {
    Logger::message("Importing installed stickerpacks");
    if (!dtImportStickerPacks(dtdb->d_database, databasedir))
    {
      Logger::error("Failed to import stickers");
      return false;
    }
  }

  if (!skipmessagereorder) [[likely]]
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
