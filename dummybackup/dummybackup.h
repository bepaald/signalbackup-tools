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

#ifndef DUMMYBACKUP_H_
#define DUMMYBACKUP_H_

#include "../signalbackup/signalbackup.h"
#include "../desktopdatabase/desktopdatabase.h"

class DummyBackup : public SignalBackup
{
 public:
  inline DummyBackup(std::unique_ptr<DesktopDatabase> const &ddb,
                     /*std::string const &configdir, std::string const &databasedir,
                     std::string const &hexkey, long long int cipherversion,
                     bool ignorewal, */bool verbose, bool truncate, bool showprogress);
};

inline DummyBackup::DummyBackup(std::unique_ptr<DesktopDatabase> const &ddb,
                                /*std::string const &configdir, std::string const &databasedir,
                                std::string const &hexkey, long long int cipherversion,
                                bool ignorewal, */bool verbose, bool truncate, bool showprogress)
  :
  SignalBackup(verbose, truncate, showprogress)
{
  // set up required tables (database version 223

  // MESSAGE
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS message (_id INTEGER PRIMARY KEY AUTOINCREMENT, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, date_server INTEGER DEFAULT -1, thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE, from_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, from_device_id INTEGER, to_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, body TEXT, read INTEGER DEFAULT 0, ct_l TEXT, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, subscription_id INTEGER DEFAULT -1, receipt_timestamp INTEGER DEFAULT -1, has_delivery_receipt INTEGER DEFAULT 0, has_read_receipt INTEGER DEFAULT 0, viewed INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author INTEGER DEFAULT 0, quote_body TEXT DEFAULT NULL, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, quote_type INTEGER DEFAULT 0, shared_contacts TEXT DEFAULT NULL, unidentified INTEGER DEFAULT 0, link_previews TEXT DEFAULT NULL, view_once INTEGER DEFAULT 0, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL, message_ranges BLOB DEFAULT NULL, story_type INTEGER DEFAULT 0, parent_story_id INTEGER DEFAULT 0, export_state BLOB DEFAULT NULL, exported INTEGER DEFAULT 0, scheduled_date INTEGER DEFAULT -1, latest_revision_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, original_message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, revision_number INTEGER DEFAULT 0, message_extras BLOB DEFAULT NULL)"))
    return;

  // THREAD
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS thread ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, meaningful_messages INTEGER DEFAULT 0, recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE, read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0, snippet TEXT, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type TEXT DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, unread_count INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, has_delivery_receipt INTEGER DEFAULT 0, has_read_receipt INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, unread_self_mention_count INTEGER DEFAULT 0, active INTEGER DEFAULT 0, snippet_message_extras BLOB DEFAULT NULL)"))
    return;

  // RECIPIENT
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS recipient ( _id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER DEFAULT 0, e164 TEXT UNIQUE DEFAULT NULL, aci TEXT UNIQUE DEFAULT NULL, pni TEXT UNIQUE DEFAULT NULL CHECK (pni LIKE 'PNI:%'), username TEXT UNIQUE DEFAULT NULL, email TEXT UNIQUE DEFAULT NULL, group_id TEXT UNIQUE DEFAULT NULL, distribution_list_id INTEGER DEFAULT NULL, call_link_room_id TEXT DEFAULT NULL, registered INTEGER DEFAULT 0, unregistered_timestamp INTEGER DEFAULT 0, blocked INTEGER DEFAULT 0, hidden INTEGER DEFAULT 0, profile_key TEXT DEFAULT NULL, profile_key_credential TEXT DEFAULT NULL, profile_sharing INTEGER DEFAULT 0, profile_given_name TEXT DEFAULT NULL, profile_family_name TEXT DEFAULT NULL, profile_joined_name TEXT DEFAULT NULL, profile_avatar TEXT DEFAULT NULL, last_profile_fetch INTEGER DEFAULT 0, system_given_name TEXT DEFAULT NULL, system_family_name TEXT DEFAULT NULL, system_joined_name TEXT DEFAULT NULL, system_nickname TEXT DEFAULT NULL, system_photo_uri TEXT DEFAULT NULL, system_phone_label TEXT DEFAULT NULL, system_phone_type INTEGER DEFAULT -1, system_contact_uri TEXT DEFAULT NULL, system_info_pending INTEGER DEFAULT 0, notification_channel TEXT DEFAULT NULL, message_ringtone TEXT DEFAULT NULL, message_vibrate INTEGER DEFAULT 0, call_ringtone TEXT DEFAULT NULL, call_vibrate INTEGER DEFAULT 0, mute_until INTEGER DEFAULT 0, message_expiration_time INTEGER DEFAULT 0, sealed_sender_mode INTEGER DEFAULT 0, storage_service_id TEXT UNIQUE DEFAULT NULL, storage_service_proto TEXT DEFAULT NULL, mention_setting INTEGER DEFAULT 0, capabilities INTEGER DEFAULT 0, last_session_reset BLOB DEFAULT NULL, wallpaper BLOB DEFAULT NULL, wallpaper_uri TEXT DEFAULT NULL, about TEXT DEFAULT NULL, about_emoji TEXT DEFAULT NULL, extras BLOB DEFAULT NULL, groups_in_common INTEGER DEFAULT 0, avatar_color TEXT DEFAULT NULL, chat_colors BLOB DEFAULT NULL, custom_chat_colors_id INTEGER DEFAULT 0, badges BLOB DEFAULT NULL, needs_pni_signature INTEGER DEFAULT 0, reporting_token BLOB DEFAULT NULL , phone_number_sharing INTEGER DEFAULT 0, phone_number_discoverable INTEGER DEFAULT 0, pni_signature_verified INTEGER DEFAULT 0, nickname_given_name TEXT DEFAULT NULL, nickname_family_name TEXT DEFAULT NULL, nickname_joined_name TEXT DEFAULT NULL, note TEXT DEFAULT NULL)"))
    return;

  // ATTACHMENT
  if (!d_database.exec("CREATE TABLE attachment ( _id INTEGER PRIMARY KEY AUTOINCREMENT, message_id INTEGER, content_type TEXT, remote_key TEXT, remote_location TEXT, remote_digest BLOB, remote_incremental_digest BLOB, remote_incremental_digest_chunk_size INTEGER DEFAULT 0, cdn_number INTEGER DEFAULT 0, transfer_state INTEGER, transfer_file TEXT DEFAULT NULL, data_file TEXT, data_size INTEGER, data_random BLOB, file_name TEXT, fast_preflight_id TEXT, voice_note INTEGER DEFAULT 0, borderless INTEGER DEFAULT 0, video_gif INTEGER DEFAULT 0, quote INTEGER DEFAULT 0, width INTEGER DEFAULT 0, height INTEGER DEFAULT 0, caption TEXT DEFAULT NULL, sticker_pack_id TEXT DEFAULT NULL, sticker_pack_key DEFAULT NULL, sticker_id INTEGER DEFAULT -1, sticker_emoji STRING DEFAULT NULL, blur_hash TEXT DEFAULT NULL, transform_properties TEXT DEFAULT NULL, display_order INTEGER DEFAULT 0, upload_timestamp INTEGER DEFAULT 0 , data_hash_start TEXT DEFAULT NULL, data_hash_end TEXT DEFAULT NULL)"))
    return;

  // REACTION
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS reaction ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES message (_id) ON DELETE CASCADE, author_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, emoji TEXT NOT NULL, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, UNIQUE(message_id, author_id) ON CONFLICT REPLACE)"))
    return;

  // STICKER
  if (!d_database.exec("CREATE TABLE sticker (_id INTEGER PRIMARY KEY AUTOINCREMENT, pack_id TEXT NOT NULL, pack_key TEXT NOT NULL, pack_title TEXT NOT NULL, pack_author TEXT NOT NULL, sticker_id INTEGER, cover INTEGER, emoji TEXT NOT NULL, last_used INTEGER, installed INTEGER,file_path TEXT NOT NULL, file_length INTEGER, file_random BLOB, pack_order INTEGER DEFAULT 0, content_type TEXT DEFAULT NULL, UNIQUE(pack_id, sticker_id, cover) ON CONFLICT IGNORE);"))
    return;

  // MENTION
  if (!d_database.exec("CREATE TABLE mention (_id INTEGER PRIMARY KEY AUTOINCREMENT, thread_id INTEGER, message_id INTEGER, recipient_id INTEGER, range_start INTEGER, range_length INTEGER)"))
    return;

  // GROUPS
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS groups ( _id INTEGER PRIMARY KEY, group_id TEXT NOT NULL UNIQUE, recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE, title TEXT DEFAULT NULL, avatar_id INTEGER DEFAULT 0, avatar_key BLOB DEFAULT NULL, avatar_content_type TEXT DEFAULT NULL, avatar_digest BLOB DEFAULT NULL, timestamp INTEGER DEFAULT 0, active INTEGER DEFAULT 1, mms INTEGER DEFAULT 0, master_key BLOB DEFAULT NULL, revision BLOB DEFAULT NULL, decrypted_group BLOB DEFAULT NULL, expected_v2_id TEXT UNIQUE DEFAULT NULL, unmigrated_v1_members TEXT DEFAULT NULL, distribution_id TEXT UNIQUE DEFAULT NULL, show_as_story_state INTEGER DEFAULT 0, last_force_update_timestamp INTEGER DEFAULT 0)"))
    return;

  // GROUP MEMBERSHIP
  if (!d_database.exec("CREATE TABLE IF NOT EXISTS group_membership ( _id INTEGER PRIMARY KEY, group_id TEXT NOT NULL REFERENCES groups (group_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, UNIQUE(group_id, recipient_id))"))
    return;

  // GROUP RECEIPTS
  if (!d_database.exec("CREATE TABLE group_receipts (_id INTEGER PRIMARY KEY, mms_id INTEGER, address TEXT, status INTEGER, timestamp INTEGER, unidentified INTEGER DEFAULT 0)"))
    return;

  // set database version
  d_databaseversion = 223;
  setColumnNames();

  // open desktopdb, scan for self id, add to recipient and set d_selfphone/id
  //DesktopDatabase ddb(configdir, databasedir, hexkey, verbose, ignorewal, cipherversion, truncate);
  if (!ddb->ok())
    Logger::error("DesktopDatabase was not ok");
  dtSetColumnNames(&ddb->d_database);

  // on messages sent from Desktop, sourceServiceId/sourceUuid is empty
  std::string uuid = ddb->d_database.getSingleResultAs<std::string>("SELECT DISTINCT NULLIF(" + d_dt_m_sourceuuid + ", '') FROM messages "
                                                                   "WHERE type = 'outgoing' AND " + d_dt_m_sourceuuid + " IS NOT NULL", std::string());
  if (uuid.empty())  // on messages sent from Desktop, sourceServiceId/sourceUuid is empty
  {
    // a bit more complicated:
    uuid = ddb->d_database.getSingleResultAs<std::string>("SELECT DISTINCT NULLIF(key, '') FROM messages, json_each(messages.json, '$.sendStateByConversationId') WHERE messages.type = 'outgoing' AND key IS NOT messages.conversationId AND messages.conversationId NOT IN (SELECT id FROM conversations WHERE type = 'group')", std::string());
    if (uuid.empty())
    {
      Logger::error("Failed to determin uuid of self");
      return;
    }
  }

  SqliteDB::QueryResults selfdata;
  if (!ddb->d_database.exec("SELECT profileName, profileFamilyName, profileFullName, e164, json_extract(json, '$.color') AS color "
                           "FROM conversations WHERE " + d_dt_c_uuid + " = ?",
                           uuid, &selfdata) ||
      selfdata.rows() != 1)
    // warning
    return;

  std::any new_rid;
  if (!insertRow("recipient",
                 {{d_recipient_profile_given_name, selfdata.value(0, "profileName")},
                  {"profile_family_name", selfdata.value(0, "profileFamilyName")},
                  {"profile_joined_name", selfdata.value(0, "profileFullName")},
                  {d_recipient_e164, selfdata.value(0, "e164")},
                  {d_recipient_aci, uuid},
                  {d_recipient_avatar_color, selfdata.value(0, "color")}}, "_id", &new_rid))
    return;

  d_selfid = std::any_cast<long long int>(new_rid);
  d_selfuuid = uuid;

  d_ok = true;
}

#endif
