/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

#include <regex>

#include "../reactionlist/reactionlist.h"

bool SignalBackup::migrateDatabase(int from, int to) const
{
  // NOTE: This function does not perform a full migrate to a (necessarily) working
  // backup file. Its only just enough for this programs exportHTML() function.

  // interpreted from
  // https://github.com/signalapp/Signal-Android/blob/main/app/src/main/java/org/thoughtcrime/securesms/database/helpers/migration/V168_SingleMessageTableMigration.kt

  Logger::message("Attempting to migrate database from version ", from, " to version ", to, "...");

  if (!d_database.exec("BEGIN TRANSACTION"))
    return false;


  // this is a tough one, from 23 -> ~27
  if (d_database.containsTable("recipient_preferences") &&
      !d_database.containsTable("recipient"))
  {
    // -> 24, adapted from RecipientidMigrationHelper.java
    // insert missing recipients mentioned in other tables
    auto insertMissingRecipients = [&](std::string const &table, std::string const &column)
    {
      if (!d_database.exec("INSERT INTO recipient_preferences(recipient_ids) SELECT DISTINCT " + column + " FROM " + table + " WHERE " +
                           column + " != '' AND " +
                           column + " != 'insert-address-column' AND " +
                           column + " NOT NULL AND " +
                           column + " NOT IN (SELECT recipient_ids FROM recipient_preferences)"))
        return false;
      return true;
    };

    for (auto const &p : {std::pair<std::string, std::string>{"identities", "address"},
                          std::pair<std::string, std::string>{"sessions", "address"},
                          std::pair<std::string, std::string>{"thread", "recipient_ids"},
                          std::pair<std::string, std::string>{"sms", "address"},
                          std::pair<std::string, std::string>{"mms", "address"},
                          std::pair<std::string, std::string>{"mms", "quote_author"},
                          std::pair<std::string, std::string>{"group_receipts", "address"},
                          std::pair<std::string, std::string>{"groups", "group_id"}})
    {
      if (!insertMissingRecipients(p.first, p.second))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }
    }

    // update invalid/missing addresses
    auto updateMissingAddress = [&](std::string const &table, std::string const &column)
    {
      if (!d_database.exec("UPDATE " + table + " SET " + column + " = -1 " + "WHERE " +
                           column + " = '' OR " +
                           column + " IS NULL OR " +
                           column + " = 'insert-address-token'"))
        return false;
      return true;
    };

    for (auto const &p : {std::pair<std::string, std::string>{"sms", "address"},
                          std::pair<std::string, std::string>{"mms", "address"},
                          std::pair<std::string, std::string>{"mms", "quote_author"}})
    {
      if (!updateMissingAddress(p.first, p.second))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }
    }

    // add column to groups
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN recipient_id INTEGER DEFAULT 0"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    // update address -> recipient_id
    auto addressTorecipientId = [&](std::string const &table, std::string const &column)
    {
      if (!d_database.exec("UPDATE " + table + " SET " + column + " = "
                           "(SELECT _id FROM recipient_preferences WHERE recipient_preferences.recipient_ids = " + table + "." + column + ")"))
        return false;
      return true;
    };

    for (auto const &p : {std::pair<std::string, std::string>{"identities", "address"},
                          std::pair<std::string, std::string>{"sessions", "address"},
                          std::pair<std::string, std::string>{"thread", "recipient_ids"},
                          std::pair<std::string, std::string>{"sms", "address"},
                          std::pair<std::string, std::string>{"mms", "address"},
                          std::pair<std::string, std::string>{"mms", "quote_author"},
                          std::pair<std::string, std::string>{"group_receipts", "address"}})
    {
      if (!addressTorecipientId(p.first, p.second))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }
    }

    // same for new groups.recipient_id column
    if (!d_database.exec("UPDATE groups SET recipient_id = (SELECT _id FROM recipient_preferences WHERE recipient_preferences.recipient_ids = groups.group_id)"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    // find missing recipients in group members
    std::set<std::string> missinggroupmembers;
    SqliteDB::QueryResults groupmembers;
    if (!d_database.exec("SELECT members FROM groups", &groupmembers))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    for (unsigned int i = 0; i < groupmembers.rows(); ++i)
    {
      std::vector<std::string> individual_groupmembers;

      std::string membersstring(groupmembers(i, "members"));
      std::regex comma(",");
      std::sregex_token_iterator iter(membersstring.begin(), membersstring.end(), comma, -1);
      std::transform(iter, std::sregex_token_iterator(), std::back_inserter(individual_groupmembers),
                     [](std::string const &m) STATICLAMBDA -> std::string { return m; });
      for (auto const &m : individual_groupmembers)
        if (!m.empty() &&
            d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient_preferences WHERE recipient_ids = ?", m, -1) == -1)
          missinggroupmembers.insert(m);
    }

    // insert missing group members:
    for (auto const &mm : missinggroupmembers)
    {
      //std::cout << "Missing member: " << mm << std::endl;
      if (!d_database.exec("INSERT INTO recipient_preferences(recipient_ids) VALUES (?)", mm))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }
    }

    // now migrate group members address -> _id
    if (!d_database.exec("SELECT _id, members FROM groups", &groupmembers))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
    for (unsigned int i = 0; i < groupmembers.rows(); ++i)
    {
      long long int gid = groupmembers.getValueAs<long long int>(i, "_id");
      std::vector<std::string> individual_groupmembers;
      std::string membersstring(groupmembers(i, "members"));
      std::regex comma(",");
      std::sregex_token_iterator iter(membersstring.begin(), membersstring.end(), comma, -1);
      std::transform(iter, std::sregex_token_iterator(), std::back_inserter(individual_groupmembers),
                     [](std::string const &m) STATICLAMBDA -> std::string { return m; });
      std::string members_id_str;

      for (auto const &m : individual_groupmembers)
      {
        long long int mid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient_preferences WHERE recipient_ids = ?", m, -1);
        if (mid == -1)
        {
          d_database.exec("ROLLBACK TRANSACTION");
          return false;
        }
        members_id_str += (members_id_str.empty() ? "" : ",") + bepaald::toString(mid);
      }
      //std::cout << membersstring << " -> " << members_id_str << std::endl;
      if (!d_database.exec("UPDATE groups SET members = ? WHERE _id = ?", {members_id_str, gid}))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }
    }

    // create recipient table
    // NOTE setColumnNames() WAS ALREADY CALLED AT THIS POINT, SINCE IT CHECKS FOR COLUMN NAMES IN THE 'recipient' TABLE WHICH
    // DID NOT EXIST AT THIS POINT, SOME COLUMNS ARE EXPECTED TO HAVE DIFFERENT (MORE MODERN) NAMES
    if (!d_database.exec("CREATE TABLE recipient (_id INTEGER PRIMARY KEY AUTOINCREMENT, " + d_recipient_aci + " TEXT UNIQUE DEFAULT NULL, " + d_recipient_e164 + " TEXT UNIQUE DEFAULT NULL, email TEXT UNIQUE DEFAULT NULL, group_id TEXT UNIQUE DEFAULT NULL, blocked INTEGER DEFAULT 0, message_ringtone TEXT DEFAULT NULL, message_vibrate INTEGER DEFAULT 0, call_ringtone TEXT DEFAULT NULL, call_vibrate INTEGER DEFAULT 0, notification_channel TEXT DEFAULT NULL, mute_until INTEGER DEFAULT 0, " + d_recipient_avatar_color + " TEXT DEFAULT NULL, seen_invite_reminder INTEGER DEFAULT 0, default_subscription_id INTEGER DEFAULT -1, message_expiration_time INTEGER DEFAULT 0, registered INTEGER DEFAULT 0, " + d_recipient_system_joined_name + " TEXT DEFAULT NULL, system_photo_uri TEXT DEFAULT NULL, system_phone_label TEXT DEFAULT NULL, system_contact_uri TEXT DEFAULT NULL, profile_key TEXT DEFAULT NULL, " + d_recipient_profile_given_name + " TEXT DEFAULT NULL, " + d_recipient_profile_avatar + " TEXT DEFAULT NULL, profile_sharing INTEGER DEFAULT 0, unidentified_access_mode INTEGER DEFAULT 0, force_sms_selection INTEGER DEFAULT 0)"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    // fill new table
    SqliteDB::QueryResults recipient_preferences_contents;
    if (!d_database.exec("SELECT * FROM recipient_preferences", &recipient_preferences_contents))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    for (unsigned int i = 0; i < recipient_preferences_contents.rows(); ++i)
    {
      std::string address = recipient_preferences_contents(i, "recipient_ids");
      bool isgroup = STRING_STARTS_WITH(address, "__textsecure_group__!");
      bool isemail = (address.find('@') != std::string::npos) && (address.find('.') != std::string::npos); // THIS IS CERTAINLY NOT CORRECT
      bool isphone = !isgroup && !isemail;

      insertRow("recipient",
                {{"_id", recipient_preferences_contents.value(i, "_id")},
                 {isphone ? d_recipient_e164 : "", address},
                 {isemail ? "email" : "", address},
                 {isgroup ? "group_id" : "", address},
                 {"blocked", recipient_preferences_contents.value(i, "block")},
                 {"message_ringtone", recipient_preferences_contents.value(i, "notification")},
                 {"message_vibrate", recipient_preferences_contents.value(i, "vibrate")},
                 {"call_ringtone", recipient_preferences_contents.value(i, "call_ringtone")},
                 {"call_vibrate", recipient_preferences_contents.value(i, "call_vibrate")},
                 {"notification_channel", recipient_preferences_contents.value(i, "notification_channel")},
                 {"mute_until", recipient_preferences_contents.value(i, "mute_until")},
                 {d_recipient_avatar_color, recipient_preferences_contents.value(i, "color")},
                 {"seen_invite_reminder", recipient_preferences_contents.value(i, "seen_invite_reminder")},
                 {"default_subscription_id", recipient_preferences_contents.value(i, "default_subscription_id")},
                 {"message_expiration_time", recipient_preferences_contents.value(i, "expire_messages")},
                 {"registered", recipient_preferences_contents.value(i, "registered")},
                 {d_recipient_system_joined_name, recipient_preferences_contents.value(i, "system_display_name")},
                 {"system_photo_uri", recipient_preferences_contents.value(i, "system_phone_label")},
                 {"system_contact_uri", recipient_preferences_contents.value(i, "system_contact_uri")},
                 {"profile_key", recipient_preferences_contents.value(i, "profile_key")},
                 {d_recipient_profile_given_name, recipient_preferences_contents.value(i, "signal_profile_name")},
                 {d_recipient_profile_avatar, recipient_preferences_contents.value(i, "signal_profile_avatar")},
                 {"profile_sharing", recipient_preferences_contents.value(i, "profile_sharing_approval")},
                 {"unidentified_access_mode", recipient_preferences_contents.value(i, "unidentified_access_mode")},
                 {"force_sms_selection", recipient_preferences_contents.value(i, "force_sms_selection")}});
    }

    // drop old
    d_database.exec("DROP TABLE recipient_preferences");

    // -> 25
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN system_phone_type INTEGER DEFAULT -1"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    // then it makes sure own phone number is in recipient table and
    // sets phone/registered/profile_sharing/signal_profile_name columns
    // but we can't do that because we cant know own phone number...
    // let's assume it is present already (I think it usually is)

    // -> 26
    // this migration attempts to find non-group recipients that are not used anywhere to delete them (unless
    // their 'email' column is not null for some reason). we dont care and will just leave them.

    // -> 27
    // appears to set address to -1 if address is 0 in mms table...
    if (!d_database.exec("UPDATE mms SET address = -1 WHERE address = 0"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  // create reaction table if not present
  if (!d_database.containsTable("reaction"))
  {
    if (!d_database.exec("CREATE TABLE reaction (_id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL, is_mms INTEGER NOT NULL, author_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, emoji TEXT NOT NULL, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, UNIQUE(message_id, is_mms, author_id) ON CONFLICT REPLACE)"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    // fill it
    for (auto const &msgtable : {"sms"s, d_mms_table})
    {

      // skip if not present
      if (!d_database.tableContainsColumn(msgtable, "reactions"))
        continue;

      SqliteDB::QueryResults results;
      d_database.exec("SELECT _id, reactions FROM "s + msgtable + " WHERE reactions IS NOT NULL", &results);
      for (unsigned int i = 0; i < results.rows(); ++i)
      {
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (unsigned int j = 0; j < reactions.numReactions(); ++j)
        {
          if (!insertRow("reaction",
                         {{"message_id", results.getValueAs<long long int>(i, "_id")},
                          {"is_mms", (msgtable == "sms" ? 0 : 1)},
                          {"author_id", reactions.getAuthor(j)},
                          {"emoji", reactions.getEmoji(j)},
                          {"date_sent", reactions.getSentTime(j)},
                          {"date_received", reactions.getReceivedTime(j)}}))
          {
            d_database.exec("ROLLBACK TRANSACTION");
            return false;
          }
        }
      }
      d_database.exec("ALTER TABLE " + msgtable + " DROP COLUMN reactions");
    }
  }

  // create mention table if not present
  if (!d_database.containsTable("mention"))
  {
    if (!d_database.exec("CREATE TABLE mention (_id INTEGER PRIMARY KEY AUTOINCREMENT, thread_id INTEGER, message_id INTEGER, recipient_id INTEGER, range_start INTEGER, range_length INTEGER)"))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  // add any missing columns to mms (from dbv 123 ( or hopefully -> 99)
  auto ensureColumns = [&](std::string const &table, std::string const &column, std::string const &columndefinition)
  {
    if (!d_database.tableContainsColumn(table, column))
      if (!d_database.exec("ALTER TABLE " + table + " ADD COLUMN " + column + " " + columndefinition))
        return false;
    return true;
  };

  for (auto const &p : {std::pair<std::string, std::string>{"receipt_timestamp", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"date_server", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"reactions_unread", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"remote_deleted", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"mentions_self", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"reactions_last_seen", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"quote_mentions", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"quote_type", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"link_previews", "TEXT DEFAULT NULL"},
                        std::pair<std::string, std::string>{"view_once", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{d_mms_ranges, "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"story_type", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"parent_story_id", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"export_state", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"server_guid", "TEXT DEFAULT NULL"},
                        std::pair<std::string, std::string>{"exported", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"viewed_receipt_count", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"notified_timestamp", "INTEGER DEFAULT 0"}})
  {
    if (!ensureColumns(d_mms_table, p.first, p.second))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  for (auto const &p : {std::pair<std::string, std::string>{"receipt_timestamp", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"date_server", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"reactions_unread", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"reactions_last_seen", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"remote_deleted", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"export_state", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"server_guid", "TEXT DEFAULT NULL"},
                        std::pair<std::string, std::string>{"exported", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"notified_timestamp", "INTEGER DEFAULT 0"}})
  {
    if (!ensureColumns("sms", p.first, p.second))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  for (auto const &p : {std::pair<std::string, std::string>{"wallpaper", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"chat_colors", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"username", "TEXT DEFAULT NULL"},
                        std::pair<std::string, std::string>{"hidden", "INTEGER DEFAULT 0"}})
  {
    if (!ensureColumns("recipient", p.first, p.second))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  if (!d_database.exec("DROP TRIGGER IF EXISTS msl_sms_delete") ||
      !d_database.exec("DROP TRIGGER IF EXISTS reactions_sms_delete") ||
      !d_database.exec("DROP TRIGGER IF EXISTS sms_ai") ||
      !d_database.exec("DROP TRIGGER IF EXISTS sms_au") ||
      !d_database.exec("DROP TRIGGER IF EXISTS sms_ad") ||
      !d_database.exec("DROP TABLE IF EXISTS sms_fts") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_read_and_notified_and_thread_id_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_type_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_date_sent_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_date_server_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_thread_date_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_reactions_unread_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_story_type_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_parent_story_id_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_thread_story_parent_story_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_quote_id_quote_author_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_exported_index") ||
      !d_database.exec("DROP INDEX IF EXISTS mms_id_type_payment_transactions_index") ||
      !d_database.exec("DROP TRIGGER IF EXISTS mms_ai"))
  {
    d_database.exec("ROLLBACK TRANSACTION");
    return false;
  }

  SqliteDB::QueryResults minmax;
  if (!d_database.exec("SELECT MIN(_id) AS min, MAX(_id) AS max FROM sms", &minmax))
  {
    d_database.exec("ROLLBACK TRANSACTION");
    return false;
  }

  long long int min = minmax.getValueAs<long long int>(0, "min");
  long long int max = minmax.getValueAs<long long int>(0, "max");

  for (unsigned int i = min; i <= max; ++i)
  {
    SqliteDB::QueryResults newmmsid;
    if (!d_database.exec("INSERT INTO mms "
                         "(" + d_mms_date_sent + ", "
                         "date_received, "
                         "date_server, "
                         "thread_id, "
                         + d_mms_recipient_id + ", "
                         + d_mms_recipient_device_id + ", "
                         + d_mms_type + ", "
                         "body, "
                         "read, "
                         "ct_l, "
                         "exp, "
                         "m_type, "
                         "m_size, "
                         "st, "
                         "tr_id, "
                         "subscription_id, "
                         "receipt_timestamp, "
                         "delivery_receipt_count, " // renamed (has_delivery_receipt), but after v170
                         "read_receipt_count, "     //   "
                         "viewed_receipt_count, "   //   "
                         "mismatched_identities, "
                         "network_failures, "
                         "expires_in, "
                         "expire_started, "
                         "notified, "
                         "quote_id, "
                         "quote_author, "
                         "quote_body, "
                         "quote_missing, "
                         "quote_mentions, "
                         "quote_type, "
                         "shared_contacts, "
                         "unidentified, "
                         "link_previews, "
                         "view_once, "
                         "reactions_unread, "
                         "reactions_last_seen, "
                         "remote_deleted, "
                         "mentions_self, "
                         "notified_timestamp, "
                         "server_guid, "
                         + d_mms_ranges + ", "
                         "story_type, "
                         "parent_story_id, "
                         "export_state, "
                         "exported) "

                         "SELECT "

                         "date_sent, "
                         + d_sms_date_received + ", "
                         "date_server, "
                         "thread_id, "
                         + d_sms_recipient_id + ", "
                         + d_sms_recipient_device_id + ", "
                         "type, "
                         "body, "
                         "read, "
                         "null, "
                         "0, "
                         "0, "
                         "0, "
                         "status, "
                         "null, "
                         "subscription_id, "
                         "receipt_timestamp, "
                         "delivery_receipt_count, "
                         "read_receipt_count, "
                         "0, " // view_receipt (not present in sms table)
                         "mismatched_identities, "
                         "null, "
                         "expires_in, "
                         "expire_started, "
                         "notified, "
                         "0, "
                         "0, "
                         "null, "
                         "0, "
                         "null, "
                         "0, "
                         "null, "
                         "unidentified, "
                         "null, "
                         "0, "
                         "reactions_unread, "
                         "reactions_last_seen, "
                         "remote_deleted, "
                         "0, "
                         "notified_timestamp, "
                         "server_guid, "
                         "null, "
                         "0, "
                         "0, "
                         "export_state, "
                         "exported "
                         "FROM "
                         "sms "
                         "WHERE "
#if SQLITE_VERSION_NUMBER < 3035000 // RETURNING was not available prior to 3.35.0
                         "_id IS ?", i))
#else
                         "_id IS ? RETURNING _id", i, &newmmsid))
#endif
    {
      Logger::error("copying sms._id: ", i);
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

#if SQLITE_VERSION_NUMBER < 3035000 // RETURNING was not available prior to 3.35.0
    if (true)
    {
      long long int newestmmsid = d_database.lastId();
#else
    if (newmmsid.rows())
    {
      long long int newestmmsid = newmmsid.getValueAs<long long int>(0, "_id");
#endif
      // update reactions
      if (!d_database.exec("UPDATE reaction SET message_id = ?, is_mms = 1 WHERE message_id IS ? AND is_mms = 0", {newestmmsid, i}))
      {
        d_database.exec("ROLLBACK TRANSACTION");
        return false;
      }

      // update msl_tables (probably not necessary)
      if (d_database.containsTable("msl_message"))
      {
        if (!d_database.exec("UPDATE msl_message SET message_id = ?, is_mms = 1 WHERE message_id IS ? AND is_mms = 0", {newestmmsid, i}))
        {
          d_database.exec("ROLLBACK TRANSACTION");
          return false;
        }
      }
    }
  }

  if (!d_database.exec("DROP TABLE sms"))
  {
    d_database.exec("ROLLBACK TRANSACTION");
    return false;
  }

  if  (!d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
       !d_database.exec("CREATE INDEX mms_type_index ON mms (" + d_mms_type + ")") ||
       !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (" + d_mms_date_sent + ", " + d_mms_recipient_id + ", thread_id)") ||
       !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
       !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
       !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)") ||
       !d_database.exec("CREATE INDEX mms_story_type_index ON mms (story_type)") ||
       !d_database.exec("CREATE INDEX mms_parent_story_id_index ON mms (parent_story_id)") ||
       !d_database.exec("CREATE INDEX mms_thread_story_parent_story_index ON mms (thread_id, date_received, story_type, parent_story_id)") ||
       !d_database.exec("CREATE INDEX mms_quote_id_quote_author_index ON mms (quote_id, quote_author)") ||
       !d_database.exec("CREATE INDEX mms_exported_index ON mms (exported)") ||
       !d_database.exec("CREATE INDEX mms_id_type_payment_transactions_index ON mms (_id, " + d_mms_type + ") WHERE " + d_mms_type + " & " + bepaald::toString(Types::SPECIAL_TYPE_PAYMENTS_NOTIFICATION) + " != 0") ||
       !d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts (rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;"))
  {
    d_database.exec("ROLLBACK TRANSACTION");
    return false;
  }

  if (d_database.exec("COMMIT"))
    return true;

  return false;
}
