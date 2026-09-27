/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "signalbackup.h"

bool SignalBackup::initEmptyDatabase() const
{
  // recipient table
  if (!d_database.exec("CREATE TABLE recipient ("
                       "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "type INTEGER DEFAULT 0,"
                       "e164 TEXT UNIQUE DEFAULT NULL,"
                       "aci TEXT UNIQUE DEFAULT NULL,"
                       "pni TEXT UNIQUE DEFAULT NULL CHECK (pni LIKE 'PNI:%'),"
                       "username TEXT UNIQUE DEFAULT NULL,"
                       "email TEXT UNIQUE DEFAULT NULL,"
                       "group_id TEXT UNIQUE DEFAULT NULL,"
                       "distribution_list_id INTEGER DEFAULT NULL,"
                       "call_link_room_id TEXT DEFAULT NULL,"
                       "registered INTEGER DEFAULT 0,"
                       "unregistered_timestamp INTEGER DEFAULT 0,"
                       "blocked INTEGER DEFAULT 0,"
                       "hidden INTEGER DEFAULT 0,"
                       "profile_key TEXT DEFAULT NULL,"
                       "profile_key_credential TEXT DEFAULT NULL,"
                       "profile_sharing INTEGER DEFAULT 0,"
                       "profile_given_name TEXT DEFAULT NULL,"
                       "profile_family_name TEXT DEFAULT NULL,"
                       "profile_joined_name TEXT DEFAULT NULL,"
                       "profile_avatar TEXT DEFAULT NULL,"
                       "last_profile_fetch INTEGER DEFAULT 0,"
                       "system_given_name TEXT DEFAULT NULL,"
                       "system_family_name TEXT DEFAULT NULL,"
                       "system_joined_name TEXT DEFAULT NULL,"
                       "system_nickname TEXT DEFAULT NULL,"
                       "system_photo_uri TEXT DEFAULT NULL,"
                       "system_phone_label TEXT DEFAULT NULL,"
                       "system_phone_type INTEGER DEFAULT -1,"
                       "system_contact_uri TEXT DEFAULT NULL,"
                       "system_info_pending INTEGER DEFAULT 0,"
                       "notification_channel TEXT DEFAULT NULL,"
                       "message_ringtone TEXT DEFAULT NULL,"
                       "message_vibrate INTEGER DEFAULT 0,"
                       "call_ringtone TEXT DEFAULT NULL,"
                       "call_vibrate INTEGER DEFAULT 0,"
                       "mute_until INTEGER DEFAULT 0,"
                       "message_expiration_time INTEGER DEFAULT 0,"
                       "sealed_sender_mode INTEGER DEFAULT 0,"
                       "storage_service_id TEXT UNIQUE DEFAULT NULL,"
                       "storage_service_proto TEXT DEFAULT NULL,"
                       "mention_setting INTEGER DEFAULT 0,"
                       "capabilities INTEGER DEFAULT 0,"
                       "last_session_reset BLOB DEFAULT NULL,"
                       "wallpaper BLOB DEFAULT NULL,"
                       "wallpaper_uri TEXT DEFAULT NULL,"
                       "about TEXT DEFAULT NULL,"
                       "about_emoji TEXT DEFAULT NULL,"
                       "extras BLOB DEFAULT NULL,"
                       "groups_in_common INTEGER DEFAULT 0,"
                       "avatar_color TEXT DEFAULT NULL,"
                       "chat_colors BLOB DEFAULT NULL,"
                       "custom_chat_colors_id INTEGER DEFAULT 0,"
                       "badges BLOB DEFAULT NULL,"
                       "needs_pni_signature INTEGER DEFAULT 0,"
                       "reporting_token BLOB DEFAULT NULL, phone_number_sharing INTEGER DEFAULT 0, phone_number_discoverable INTEGER DEFAULT 0, pni_signature_verified INTEGER DEFAULT 0, nickname_given_name TEXT DEFAULT NULL, nickname_family_name TEXT DEFAULT NULL, nickname_joined_name TEXT DEFAULT NULL, note TEXT DEFAULT NULL, message_expiration_time_version INTEGER DEFAULT 1 NOT NULL, key_transparency_data BLOB DEFAULT NULL, call_notification_setting INTEGER DEFAULT 0, reply_notification_setting INTEGER DEFAULT 0, blocked_at INTEGER DEFAULT 0,"
                       "unread_reminder INTEGER DEFAULT 0)") ||
      !d_database.exec("CREATE INDEX recipient_type_index ON recipient (type)") ||
      !d_database.exec("CREATE INDEX recipient_aci_profile_key_index ON recipient (aci, profile_key) WHERE aci NOT NULL AND profile_key NOT NULL") ||
      !d_database.exec("CREATE UNIQUE INDEX recipient_username_unique_nocase ON recipient(username COLLATE NOCASE)"))
    return false;

    //groups
    //thread table
    //message table -> reaction, mention, attachment!, receipts?

  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;
  // if (!d_database.exec())
  //   return false;

  return true;
}

#endif
