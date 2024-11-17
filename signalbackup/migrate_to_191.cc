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

#include <regex>
#include <openssl/rand.h>

#if __cpp_lib_format >= 201907L
#include <format>
#else
#include <memory>
#include <cstdio>
#endif

bool SignalBackup::migrate_to_191(std::string const &selfphone)
{
  if (d_databaseversion < 98)
  {
    Logger::error("Sorry, db version too old. Not supported (yet?)");
    return false;
  }

  if (selfphone.empty())
  {
    Logger::error("Please provide phone of self with the `--setselfid' option (eg.: `--setself \"+31612345678\"')");
    return false;
  }

  /*
  if (d_databaseversion < )
  {
    Logger::message("To ");

    if (!d_database.exec())
      return false;
  }
  */

  if (d_databaseversion < 98)
  {
    Logger::message("To 98");

    if (!d_database.exec("UPDATE recipient SET storage_service_key = NULL WHERE storage_service_key IS NOT NULL AND (group_type = 1 OR (group_type = 0 AND phone IS NULL AND uuid IS NULL))"))
      return false;
  }

  if (d_databaseversion < 99)
  {
    Logger::message("To 99");

    if (!d_database.exec("ALTER TABLE sms ADD COLUMN server_guid TEXT DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE mms ADD COLUMN server_guid TEXT DEFAULT NULL"))
      return false;
  }

  if (d_databaseversion < 100)
  {
    Logger::message("To 100");

    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN chat_colors BLOB DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE recipient ADD COLUMN custom_chat_colors_id INTEGER DEFAULT 0") ||
        !d_database.exec("CREATE TABLE chat_colors (_id INTEGER PRIMARY KEY AUTOINCREMENT,chat_colors BLOB)"))
      return false;

    // NOTE skipping the rest here, people can just set new chat_colors if they want...
  }

  if (d_databaseversion < 101)
  {
    Logger::message("To 101");

    SqliteDB::QueryResults recipients_without_color;
    if (!d_database.exec("SELECT _id FROM recipient WHERE color IS NULL", &recipients_without_color))
      return false;

    std::vector<std::string> avatar_color_options{"C000", "C010", "C020", "C030", "C040", "C050", "C060", "C070", "C080", "C090", "C100", "C110", "C120", "C130", "C140", "C150", "C160", "C170", "C180", "C190", "C200", "C210", "C220", "C230", "C240", "C250", "C260", "C270", "C280", "C290", "C300", "C310", "C320", "C330", "C340", "C350"};

    for (unsigned int i = 0; i < recipients_without_color.rows(); ++i)
    {
      uint8_t random_idx = 0;
      if (RAND_bytes(&random_idx, 1) != 1)
        Logger::warning("failed to generate random number");
      random_idx = (static_cast<double>(random_idx) / (255 + 1)) * ((avatar_color_options.size() - 1) - 0 + 1) + 0;

      if (!d_database.exec("UPDATE recipient SET color = ? WHERE _id = ?",
                           {avatar_color_options[random_idx], recipients_without_color.value(i, "_id")}))
        return false;
    }
  }

  if (d_databaseversion < 102)
  {
    Logger::message("To 102");

    if (!d_database.exec("CREATE VIRTUAL TABLE emoji_search USING fts5(label, emoji UNINDEXED)"))
      return false;
  }

  // QUESTIONABLE
  if (d_databaseversion < 103)
  {
    Logger::message_start("To 103");
    if (!d_database.containsTable("sender_keys"))
    {
      Logger::message_end(" (really)");

      if (!d_database.exec("CREATE TABLE sender_keys ( _id INTEGER PRIMARY KEY AUTOINCREMENT, recipient_id INTEGER NOT NULL, device INTEGER NOT NULL, distribution_id TEXT NOT NULL, record BLOB NOT NULL, created_at INTEGER NOT NULL, UNIQUE(recipient_id, device, distribution_id) ON CONFLICT REPLACE)"))
        return false;

      if (!d_database.exec("CREATE TABLE sender_key_shared ( _id INTEGER PRIMARY KEY AUTOINCREMENT, distribution_id TEXT NOT NULL, address TEXT NOT NULL, device INTEGER NOT NULL, UNIQUE(distribution_id, address, device) ON CONFLICT REPLACE )"))
        return false;

      if (!d_database.exec("CREATE TABLE pending_retry_receipts ( _id INTEGER PRIMARY KEY AUTOINCREMENT, author TEXT NOT NULL, device INTEGER NOT NULL, sent_timestamp INTEGER NOT NULL, received_timestamp TEXT NOT NULL, thread_id INTEGER NOT NULL, UNIQUE(author, sent_timestamp) ON CONFLICT REPLACE );"))
        return false;

      if (!d_database.exec("ALTER TABLE groups ADD COLUMN distribution_id TEXT DEFAULT NULL") ||
          !d_database.exec("CREATE UNIQUE INDEX IF NOT EXISTS group_distribution_id_index ON groups (distribution_id)"))
        return false;

      union
      {
        struct
        {
          uint32_t time_low;
          uint16_t time_mid;
          uint16_t time_hi_and_version;
          uint8_t  clk_seq_hi_res;
          uint8_t  clk_seq_low;
          uint8_t  node[6];
        } uuidstruct;
        uint8_t rnd[16];
      } uuid;

      SqliteDB::QueryResults group_results;
      if (!d_database.exec("SELECT group_id FROM groups WHERE LENGTH(group_id) = 85", &group_results))
        return false;
      for (unsigned int i = 0; i < group_results.rows(); ++i)
      {
        // generate a new uuid to use as distribution_id
        if (RAND_bytes(uuid.rnd, sizeof(uuid)) != 1)
        {
          Logger::error("Failed to generate 16 random bytes (2)");
          return false;
        }

        // Refer Section 4.2 of RFC-4122
        // https://tools.ietf.org/html/rfc4122#section-4.2
        uuid.uuidstruct.clk_seq_hi_res = (uint8_t) ((uuid.uuidstruct.clk_seq_hi_res & 0x3F) | 0x80);
        uuid.uuidstruct.time_hi_and_version = (uint16_t) ((uuid.uuidstruct.time_hi_and_version & 0x0FFF) | 0x4000);

#if __cpp_lib_format >= 201907L
        std::string distribution_id = std::format("{:0>8x}-{:0>4x}-{:0>4x}-{:0>2x}{:0>2x}-{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}",
                                                  uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                                                  uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                                                  uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                                                  uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]);
#else
        int size = 1 + std::snprintf(nullptr, 0, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                     uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                                     uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                                     uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                                     uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]);
        if (size <= 0)
        {
          Logger::error("Failed to get size of uuid");
          return false;
        }

        std::unique_ptr<char[]> uuid_char(new char[size]);
        if (std::snprintf(uuid_char.get(), size, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                          uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                          uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                          uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                          uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]) < 0)
        {
          Logger::error("failed to format UUID");
          return false;
        }
        std::string distribution_id(uuid_char.get(), uuid_char.get() + size - 1);
#endif
        if (!d_database.exec("UPDATE groups SET distribution_id = ? WHERE group_id = ?", {distribution_id, group_results.value(i, "group_id")}))
          return false;
      }
    }
    else
      Logger::message_end("... (not)");
  }

  if (d_databaseversion < 104)
  {
    Logger::message("To 104");

    if (!d_database.exec("DROP INDEX sms_date_sent_index") ||
        !d_database.exec("CREATE INDEX sms_date_sent_index on sms(date_sent, address, thread_id)") ||
        !d_database.exec("DROP INDEX mms_date_sent_index") ||
        !d_database.exec("CREATE INDEX mms_date_sent_index on mms(date, address, thread_id)"))
      return false;
  }

  if (d_databaseversion < 105)
  {
    Logger::message("To 105");

    if (!d_database.exec("CREATE TABLE message_send_log ( _id INTEGER PRIMARY KEY, date_sent INTEGER NOT NULL, content BLOB NOT NULL, related_message_id INTEGER DEFAULT -1, is_related_message_mms INTEGER DEFAULT 0, content_hint INTEGER NOT NULL, group_id BLOB DEFAULT NULL )"))
      return false;

    if (!d_database.exec("CREATE INDEX message_log_date_sent_index ON message_send_log (date_sent)") ||
        !d_database.exec("CREATE INDEX message_log_related_message_index ON message_send_log (related_message_id, is_related_message_mms)") ||
        !d_database.exec("CREATE TRIGGER msl_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM message_send_log WHERE related_message_id = old._id AND is_related_message_mms = 0; END") ||
        !d_database.exec("CREATE TRIGGER msl_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM message_send_log WHERE related_message_id = old._id AND is_related_message_mms = 1; END"))
      return false;

    if (!d_database.exec("CREATE TABLE message_send_log_recipients ( _id INTEGER PRIMARY KEY, message_send_log_id INTEGER NOT NULL REFERENCES message_send_log (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL, device INTEGER NOT NULL )"))
      return false;

    if (!d_database.exec("CREATE INDEX message_send_log_recipients_recipient_index ON message_send_log_recipients (recipient_id, device)"))
      return false;
  }

  if (d_databaseversion < 106)
  {
    Logger::message("To 106");

    if (!d_database.exec("DROP TABLE message_send_log") ||
        !d_database.exec("DROP INDEX IF EXISTS message_log_date_sent_index") ||
        !d_database.exec("DROP INDEX IF EXISTS message_log_related_message_index") ||
        !d_database.exec("DROP TRIGGER msl_sms_delete") ||
        !d_database.exec("DROP TRIGGER msl_mms_delete") ||
        !d_database.exec("DROP TABLE message_send_log_recipients") ||
        !d_database.exec("DROP INDEX IF EXISTS message_send_log_recipients_recipient_index"))
      return false;

    if (!d_database.exec("CREATE TABLE msl_payload ( _id INTEGER PRIMARY KEY, date_sent INTEGER NOT NULL, content BLOB NOT NULL, content_hint INTEGER NOT NULL )"))
      return false;

    if (!d_database.exec("CREATE INDEX msl_payload_date_sent_index ON msl_payload (date_sent)"))
      return false;

    if (!d_database.exec("CREATE TABLE msl_recipient ( _id INTEGER PRIMARY KEY, payload_id INTEGER NOT NULL REFERENCES msl_payload (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL, device INTEGER NOT NULL)"))
      return false;

    if (!d_database.exec("CREATE INDEX msl_recipient_recipient_index ON msl_recipient (recipient_id, device, payload_id)") ||
        !d_database.exec("CREATE INDEX msl_recipient_payload_index ON msl_recipient (payload_id)"))
      return false;

    if (!d_database.exec("CREATE TABLE msl_message ( _id INTEGER PRIMARY KEY, payload_id INTEGER NOT NULL REFERENCES msl_payload (_id) ON DELETE CASCADE, message_id INTEGER NOT NULL, is_mms INTEGER NOT NULL )"))
      return false;

    if (!d_database.exec("CREATE INDEX msl_message_message_index ON msl_message (message_id, is_mms, payload_id)") ||
        !d_database.exec("CREATE TRIGGER msl_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 0); END") ||
        !d_database.exec("CREATE TRIGGER msl_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 1); END") ||
        !d_database.exec("CREATE TRIGGER msl_attachment_delete AFTER DELETE ON part BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old.mid AND is_mms = 1); END"))
      return false;
  }

  if (d_databaseversion < 107)
  {
    Logger::message("To 107");

    if (!d_database.exec("DELETE FROM sms WHERE thread_id NOT IN (SELECT _id FROM thread)"))
      return false;

    if (!d_database.exec("DELETE FROM mms WHERE thread_id NOT IN (SELECT _id FROM thread)"))
      return false;
  }

  if (d_databaseversion < 108)
  {
    Logger::message("To 108");

    if (!d_database.exec("CREATE TABLE thread_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, thread_recipient_id INTEGER, message_count INTEGER DEFAULT 0, snippet TEXT, snippet_charset INTEGER DEFAULT 0, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type INTEGER DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, unread_count INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0 )"))
      return false;

    if (!d_database.exec("INSERT INTO thread_tmp SELECT _id, date, recipient_ids, message_count, snippet, snippet_cs, snippet_type, snippet_uri, snippet_content_type, snippet_extras, read, type, error, archived, status, expires_in, last_seen, has_sent, delivery_receipt_count, read_receipt_count, unread_count, last_scrolled, pinned FROM thread "))
      return false;


    if (!d_database.exec("DROP TABLE thread") ||
        !d_database.exec("ALTER TABLE thread_tmp RENAME TO thread"))
      return false;

    if (!d_database.exec("CREATE INDEX thread_recipient_id_index ON thread (thread_recipient_id)") ||
        !d_database.exec("CREATE INDEX archived_count_index ON thread (archived, message_count)") ||
        !d_database.exec("CREATE INDEX thread_pinned_index ON thread (pinned)"))
      return false;

    if (!d_database.exec("DELETE FROM remapped_threads"))
      return false;
  }

  if (d_databaseversion < 109)
  {
    Logger::message("To 109");

    if (!d_database.exec("CREATE TABLE mms_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, thread_id INTEGER, date INTEGER, date_received INTEGER, date_server INTEGER DEFAULT -1, msg_box INTEGER, read INTEGER DEFAULT 0, body TEXT, part_count INTEGER, ct_l TEXT, address INTEGER, address_device_id INTEGER, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, delivery_receipt_count INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, subscription_id INTEGER DEFAULT -1, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author TEXT, quote_body TEXT, quote_attachment INTEGER DEFAULT -1, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, shared_contacts TEXT, unidentified INTEGER DEFAULT 0, previews TEXT, reveal_duration INTEGER DEFAULT 0, reactions BLOB DEFAULT NULL, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, viewed_receipt_count INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL );"))
      return false;

    if (!d_database.exec("INSERT INTO mms_tmp SELECT _id, thread_id, date, date_received, date_server, msg_box, read, body, part_count, ct_l, address, address_device_id, exp, m_type, m_size, st, tr_id, delivery_receipt_count, mismatched_identities, network_failures, subscription_id, expires_in, expire_started, notified, read_receipt_count, quote_id, quote_author, quote_body, quote_attachment, quote_missing, quote_mentions, shared_contacts, unidentified, previews, reveal_duration, reactions, reactions_unread, reactions_last_seen, remote_deleted, mentions_self, notified_timestamp, viewed_receipt_count, server_guid FROM mms"))
      return false;


    if (!d_database.exec("DROP TABLE mms") ||
        !d_database.exec("ALTER TABLE mms_tmp RENAME TO mms"))
      return false;

    if (!d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_message_box_index ON mms (msg_box)") ||
        !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (date, address, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
        !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
        !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)"))
      return false;

    if (!d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
        !d_database.exec("CREATE TRIGGER mms_ad AFTER DELETE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END") ||
        !d_database.exec("CREATE TRIGGER mms_au AFTER UPDATE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
        !d_database.exec("CREATE TRIGGER msl_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 1); END"))
      return false;

    if (!d_database.exec("CREATE TABLE sms_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, thread_id INTEGER, address INTEGER, address_device_id INTEGER DEFAULT 1, person INTEGER, date INTEGER, date_sent INTEGER, date_server INTEGER DEFAULT -1, protocol INTEGER, read INTEGER DEFAULT 0, status INTEGER DEFAULT -1, type INTEGER, reply_path_present INTEGER, delivery_receipt_count INTEGER DEFAULT 0, subject TEXT, body TEXT, mismatched_identities TEXT DEFAULT NULL, service_center TEXT, subscription_id INTEGER DEFAULT -1, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, unidentified INTEGER DEFAULT 0, reactions BLOB DEFAULT NULL, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL )"))
      return false;

    if (!d_database.exec("INSERT INTO sms_tmp SELECT _id, thread_id, address, address_device_id, person, date, date_sent, date_server , protocol, read, status , type, reply_path_present, delivery_receipt_count, subject, body, mismatched_identities, service_center, subscription_id , expires_in, expire_started, notified, read_receipt_count, unidentified, reactions BLOB, reactions_unread, reactions_last_seen , remote_deleted, notified_timestamp, server_guid FROM sms"))
      return false;

    if (!d_database.exec("DROP TABLE sms") ||
        !d_database.exec("ALTER TABLE sms_tmp RENAME TO sms"))
      return false;

    if (!d_database.exec("CREATE INDEX sms_read_and_notified_and_thread_id_index ON sms(read, notified, thread_id)") ||
        !d_database.exec("CREATE INDEX sms_type_index ON sms (type)") ||
        !d_database.exec("CREATE INDEX sms_date_sent_index ON sms (date_sent, address, thread_id)") ||
        !d_database.exec("CREATE INDEX sms_date_server_index ON sms (date_server)") ||
        !d_database.exec("CREATE INDEX sms_thread_date_index ON sms (thread_id, date)") ||
        !d_database.exec("CREATE INDEX sms_reactions_unread_index ON sms (reactions_unread)"))
      return false;


    if (!d_database.exec("CREATE TRIGGER sms_ai AFTER INSERT ON sms BEGIN INSERT INTO sms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER sms_ad AFTER DELETE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER sms_au AFTER UPDATE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO sms_fts(rowid, body, thread_id) VALUES(new._id, new.body, new.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER msl_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 0); END"))
      return false;
  }

  if (d_databaseversion < 110)
  {
    Logger::message("To 110");

    if (!d_database.exec("DELETE FROM part WHERE mid != -8675309 AND mid NOT IN (SELECT _id FROM mms)"))
      return false;
  }

  if (d_databaseversion < 111)
  {
    Logger::message("To 111");

    if (!d_database.exec("CREATE TABLE avatar_picker ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "last_used INTEGER DEFAULT 0,"
                         "group_id TEXT DEFAULT NULL,"
                         "avatar BLOB NOT NULL)"))
      return false;

    SqliteDB::QueryResults recipients_without_color;
    if (!d_database.exec("SELECT _id FROM recipient WHERE color IS NULL", &recipients_without_color))
      return false;

    std::vector<std::string> avatar_color_options{"C000", "C010", "C020", "C030", "C040", "C050", "C060", "C070", "C080", "C090", "C100", "C110", "C120", "C130", "C140", "C150", "C160", "C170", "C180", "C190", "C200", "C210", "C220", "C230", "C240", "C250", "C260", "C270", "C280", "C290", "C300", "C310", "C320", "C330", "C340", "C350"};

    for (unsigned int i = 0; i < recipients_without_color.rows(); ++i)
    {
      uint8_t random_idx = 0;
      if (RAND_bytes(&random_idx, 1) != 1)
        Logger::warning("failed to generate random number");
      random_idx = (static_cast<double>(random_idx) / (255 + 1)) * ((avatar_color_options.size() - 1) - 0 + 1) + 0;

      if (!d_database.exec("UPDATE recipient SET color = ? WHERE _id = ?",
                           {avatar_color_options[random_idx], recipients_without_color.value(i, "_id")}))
        return false;
    }
  }

  if (d_databaseversion < 112)
  {
    Logger::message("To 112");

    if (!d_database.exec("DELETE FROM mms WHERE thread_id NOT IN (SELECT _id FROM thread)") ||
        !d_database.exec("DELETE FROM part WHERE mid != -8675309 AND mid NOT IN (SELECT _id FROM mms)"))
      return false;
  }

  if (d_databaseversion < 113)
  {
    Logger::message("To 113");

    if (!d_database.exec("CREATE TABLE sessions_tmp ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "address TEXT NOT NULL,"
                         "device INTEGER NOT NULL,"
                         "record BLOB NOT NULL,"
                         "UNIQUE(address, device))"))
      return false;

    if (!d_database.exec("INSERT INTO sessions_tmp (address, device, record) "
                         "SELECT "
                         "COALESCE(recipient.uuid, recipient.phone) AS new_address, "
                         "sessions.device, "
                         "sessions.record "
                         "FROM sessions INNER JOIN recipient ON sessions.address = recipient._id "
                         "WHERE new_address NOT NULL"))
      return false;

    if (!d_database.exec("DROP TABLE sessions") ||
        !d_database.exec("ALTER TABLE sessions_tmp RENAME TO sessions"))
      return false;
  }

  if (d_databaseversion < 114)
  {
    Logger::message("To 114");

    if (!d_database.exec("CREATE TABLE identities_tmp ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "address TEXT UNIQUE NOT NULL,"
                         "identity_key TEXT,"
                         "first_use INTEGER DEFAULT 0,"
                         "timestamp INTEGER DEFAULT 0,"
                         "verified INTEGER DEFAULT 0,"
                         "nonblocking_approval INTEGER DEFAULT 0)"))
      return false;

    if (!d_database.exec("INSERT INTO identities_tmp (address, identity_key, first_use, timestamp, verified, nonblocking_approval) "
                         "SELECT "
                         "COALESCE(recipient.uuid, recipient.phone) AS new_address,"
                         "identities.key,"
                         "identities.first_use,"
                         "identities.timestamp,"
                         "identities.verified,"
                         "identities.nonblocking_approval "
                         "FROM identities INNER JOIN recipient ON identities.address = recipient._id "
                         "WHERE new_address NOT NULL"))
      return false;

    if (!d_database.exec("DROP TABLE identities") ||
        !d_database.exec("ALTER TABLE identities_tmp RENAME TO identities"))
      return false;
  }

  if (d_databaseversion < 115)
  {
    Logger::message("To 115");

    if (!d_database.exec("CREATE TABLE group_call_ring (_id INTEGER PRIMARY KEY, ring_id INTEGER UNIQUE, date_received INTEGER, ring_state INTEGER)") ||
        !d_database.exec("CREATE INDEX date_received_index on group_call_ring (date_received)"))
      return false;
  }

  if (d_databaseversion < 116)
  {
    Logger::message("To 116");

    if (!d_database.exec("DELETE FROM sessions WHERE address LIKE '+%'"))
      return false;

    // NOT SURE IF THIS IS WHATS INTENDED
    if (!d_database.exec("UPDATE recipient SET storage_service_key = NULL WHERE "
                         "storage_service_key NOT NULL AND group_id IS NULL AND uuid IS NULL"))
      return false;

  }

  if (d_databaseversion < 117)
  {
    Logger::message("To 117");

    if (!d_database.exec("ALTER TABLE sms ADD COLUMN receipt_timestamp INTEGER DEFAULT -1") ||
        !d_database.exec("ALTER TABLE mms ADD COLUMN receipt_timestamp INTEGER DEFAULT -1"))
      return false;
  }

  if (d_databaseversion < 118)
  {
    Logger::message("To 118");

    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN badges BLOB DEFAULT NULL"))
      return false;
  }

  if (d_databaseversion < 119)
  {
    Logger::message("To 119");

    if (!d_database.exec("CREATE TABLE sender_keys_tmp ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "address TEXT NOT NULL,"
                         "device INTEGER NOT NULL,"
                         "distribution_id TEXT NOT NULL,"
                         "record BLOB NOT NULL,"
                         "created_at INTEGER NOT NULL,"
                         "UNIQUE(address, device, distribution_id) ON CONFLICT REPLACE)"))
      return false;

    if (!d_database.exec("INSERT INTO sender_keys_tmp (address, device, distribution_id, record, created_at) "
                         "SELECT "
                         "recipient.uuid AS new_address,"
                         "sender_keys.device,"
                         "sender_keys.distribution_id,"
                         "sender_keys.record,"
                         "sender_keys.created_at "
                         "FROM sender_keys INNER JOIN recipient ON sender_keys.recipient_id = recipient._id "
                         "WHERE new_address NOT NULL"))
      return false;

    if (!d_database.exec("DROP TABLE sender_keys") ||
        !d_database.exec("ALTER TABLE sender_keys_tmp RENAME TO sender_keys"))
      return false;
  }

  if (d_databaseversion < 120)
  {
    Logger::message("To 120");

    if (!d_database.exec("ALTER TABLE sender_key_shared ADD COLUMN timestamp INTEGER DEFAULT 0"))
      return false;
  }

  if (d_databaseversion < 121)
  {
    Logger::message("To 121");

    if (!d_database.exec("CREATE TABLE reaction ("
                         "_id INTEGER PRIMARY KEY,"
                         "message_id INTEGER NOT NULL,"
                         "is_mms INTEGER NOT NULL,"
                         "author_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE,"
                         "emoji TEXT NOT NULL,"
                         "date_sent INTEGER NOT NULL,"
                         "date_received INTEGER NOT NULL,"
                         "UNIQUE(message_id, is_mms, author_id) ON CONFLICT REPLACE)"))
      return false;

    SqliteDB::QueryResults msg_reactions;
    for (auto const &table : {"sms"s, "mms"s})
    {
      if (!d_database.exec("SELECT _id, reactions FROM "s + table + " WHERE reactions NOT NULL", &msg_reactions))
        return false;

      for (unsigned int i = 0; i < msg_reactions.rows(); ++i)
      {
        ReactionList reactions(msg_reactions.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (unsigned int j = 0; j < reactions.numReactions(); ++j)
        {
          if (!insertRow("reaction",
                         {{"message_id", msg_reactions.value(i, "_id")},
                          {"is_mms", (table == "sms" ? 0 : 1)},
                          {"author_id", reactions.getAuthor(j)},
                          {"emoji", reactions.getEmoji(j)},
                          {"date_sent", reactions.getSentTime(j)},
                          {"date_received", reactions.getReceivedTime(j)}}))
            return false;
        }
      }
    }

    if (!d_database.exec("UPDATE reaction SET author_id = IFNULL((SELECT new_id FROM remapped_recipients WHERE author_id = old_id), author_id)") ||
        !d_database.exec("CREATE TRIGGER reactions_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 0; END") ||
        !d_database.exec("CREATE TRIGGER reactions_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 0; END") ||
        !d_database.exec("UPDATE sms SET reactions = NULL WHERE reactions NOT NULL") ||
        !d_database.exec("UPDATE mms SET reactions = NULL WHERE reactions NOT NULL"))
      return false;
  }

  if (d_databaseversion < 122)
  {
    Logger::message("To 122");

    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN pni TEXT DEFAULT NULL") ||
        !d_database.exec("CREATE UNIQUE INDEX IF NOT EXISTS recipient_pni_index ON recipient (pni)"))
      return false;
  }

  if (d_databaseversion < 123)
  {
    Logger::message("To 123");

    if (!d_database.exec("CREATE TABLE notification_profile ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "name TEXT NOT NULL UNIQUE,"
                         "emoji TEXT NOT NULL,"
                         "color TEXT NOT NULL,"
                         "created_at INTEGER NOT NULL,"
                         "allow_all_calls INTEGER NOT NULL DEFAULT 0,"
                         "allow_all_mentions INTEGER NOT NULL DEFAULT 0)"))
      return false;

    if (!d_database.exec("CREATE TABLE notification_profile_schedule ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "notification_profile_id INTEGER NOT NULL REFERENCES notification_profile (_id) ON DELETE CASCADE,"
                         "enabled INTEGER NOT NULL DEFAULT 0,"
                         "start INTEGER NOT NULL,"
                         "end INTEGER NOT NULL,"
                         "days_enabled TEXT NOT NULL)"))
      return false;

    if (!d_database.exec("CREATE TABLE notification_profile_allowed_members ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "notification_profile_id INTEGER NOT NULL REFERENCES notification_profile (_id) ON DELETE CASCADE,"
                         "recipient_id INTEGER NOT NULL,"
                         "UNIQUE(notification_profile_id, recipient_id) ON CONFLICT REPLACE)"))
      return false;

    if (!d_database.exec("CREATE INDEX notification_profile_schedule_profile_index ON notification_profile_schedule (notification_profile_id)") ||
        !d_database.exec("CREATE INDEX notification_profile_allowed_members_profile_index ON notification_profile_allowed_members (notification_profile_id)"))
      return false;
  }

  if (d_databaseversion < 124)
  {
    Logger::message("To 124");

    if (!d_database.exec("UPDATE notification_profile_schedule SET end = 2400 WHERE end = 0"))
      return false;
  }

  if (d_databaseversion < 125)
  {
    Logger::message("To 125");

    if (!d_database.exec("DELETE FROM reaction "
                         "WHERE "
                         "(is_mms = 0 AND message_id NOT IN (SELECT _id FROM sms)) "
                         "OR "
                         "(is_mms = 1 AND message_id NOT IN (SELECT _id FROM mms))"))
      return false;
  }

  if (d_databaseversion < 126)
  {
    Logger::message("To 126");

    if (!d_database.exec("DELETE FROM reaction "
                         "WHERE "
                         "(is_mms = 0 AND message_id IN (SELECT _id from sms WHERE remote_deleted = 1)) "
                         "OR "
                         "(is_mms = 1 AND message_id IN (SELECT _id from mms WHERE remote_deleted = 1))"))
      return false;
  }

  if (d_databaseversion < 127)
  {
    Logger::message("To 127");

    if (!d_database.exec("UPDATE recipient SET pni = NULL WHERE phone IS NULL"))
      return false;
  }

  if (d_databaseversion < 128)
  {
    Logger::message("To 128");

    if (!d_database.exec("ALTER TABLE mms ADD COLUMN ranges BLOB DEFAULT NULL"))
      return false;
  }

  if (d_databaseversion < 129)
  {
    Logger::message("To 129");

    if (!d_database.exec("DROP TRIGGER reactions_mms_delete") ||
        !d_database.exec("CREATE TRIGGER reactions_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 1; END"))
      return false;

    if (!d_database.exec("DELETE FROM reaction "
                         "WHERE "
                         "(is_mms = 0 AND message_id NOT IN (SELECT _id from sms)) "
                         "OR "
                         "(is_mms = 1 AND message_id NOT IN (SELECT _id from mms))"))
      return false;
  }

  // THIS ONE IS QUESTIONABLE
  if (d_databaseversion < 130)
  {
    Logger::message("To 130");

    if (!d_database.exec("CREATE TABLE one_time_prekeys_tmp ("
                         "_id INTEGER PRIMARY KEY,"
                         "account_id TEXT NOT NULL,"
                         "key_id INTEGER,"
                         "public_key TEXT NOT NULL,"
                         "private_key TEXT NOT NULL,"
                         "UNIQUE(account_id, key_id))"))
      return false;

    // localAci should (probably) be set to UUID (aci) of self. Normally it is retrieved from a
    // separate table (not exported to the backup). Not sure if skipping these next three migrations
    // even if localACI _IS_ set is harmful
    // Otherwise, try to use scanself and d_selfuuid;

    // if localAci != null
    //   migrateExistingOneTimePreKeys
    // else
    //   warn("no local aci, not migrating any existing one-time prekeys...")

    if (!d_database.exec("DROP TABLE one_time_prekeys") ||
        !d_database.exec("ALTER TABLE one_time_prekeys_tmp RENAME TO one_time_prekeys"))
      return false;

    if (!d_database.exec("CREATE TABLE signed_prekeys_tmp ("
                         "_id INTEGER PRIMARY KEY,"
                         "account_id TEXT NOT NULL,"
                         "key_id INTEGER,"
                         "public_key TEXT NOT NULL,"
                         "private_key TEXT NOT NULL,"
                         "signature TEXT NOT NULL,"
                         "timestamp INTEGER DEFAULT 0,"
                         "UNIQUE(account_id, key_id))"))
      return false;

    // if localAci != null
    //   migrateExistingSignedPreKeys
    // else
    //   warn("no local aci, not migrating any existing signed prekeys...")

    if (!d_database.exec("DROP TABLE signed_prekeys") ||
        !d_database.exec("ALTER TABLE signed_prekeys_tmp RENAME TO signed_prekeys"))
      return false;

    if (!d_database.exec("CREATE TABLE sessions_tmp ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "account_id TEXT NOT NULL,"
                         "address TEXT NOT NULL,"
                         "device INTEGER NOT NULL,"
                         "record BLOB NOT NULL,"
                         "UNIQUE(account_id, address, device))"))
      return false;

    // if localAci != null
    //   migrateExistingSessions
    // else
    //   warn("no local aci, not migrating any existing sessions...")

    if (!d_database.exec("DROP TABLE sessions") ||
        !d_database.exec("ALTER TABLE sessions_tmp RENAME TO sessions"))
      return false;
  }

  if (d_databaseversion < 131)
  {
    Logger::message("To 131");

    if (!d_database.exec("CREATE TABLE donation_receipt ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "receipt_type TEXT NOT NULL,"
                         "receipt_date INTEGER NOT NULL,"
                         "amount TEXT NOT NULL,"
                         "currency TEXT NOT NULL,"
                         "subscription_level INTEGER NOT NULL)"))
      return false;

    if (!d_database.exec("CREATE INDEX IF NOT EXISTS donation_receipt_type_index ON donation_receipt (receipt_type);") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS donation_receipt_date_index ON donation_receipt (receipt_date);"))
      return false;
  }

  // THIS ONE IS QUESTIONABLE
  if (d_databaseversion < 132)
  {
    Logger::message("To 132");

    if (!d_database.exec("ALTER TABLE mms ADD COLUMN is_story INTEGER DEFAULT 0") ||
        !d_database.exec("ALTER TABLE mms ADD COLUMN parent_story_id INTEGER DEFAULT 0") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_is_story_index ON mms (is_story)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_parent_story_id_index ON mms (parent_story_id)") ||
        !d_database.exec("ALTER TABLE recipient ADD COLUMN distribution_list_id INTEGER DEFAULT NULL"))
      return false;

    if (!d_database.exec("CREATE TABLE distribution_list ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "name TEXT UNIQUE NOT NULL,"
                         "distribution_id TEXT UNIQUE NOT NULL,"
                         "recipient_id INTEGER UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE)"))
      return false;

    if (!d_database.exec("CREATE TABLE distribution_list_member ("
                         "_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "list_id INTEGER NOT NULL REFERENCES distribution_list (_id) ON DELETE CASCADE,"
                         "recipient_id INTEGER NOT NULL,"
                         "UNIQUE(list_id, recipient_id) ON CONFLICT IGNORE)"))
      return false;

    /* THIS IS AN UNKNOWN! */
    // inserts a storage_service_key (random 16 bytes) into the newly created my-story recipient
    unsigned char ssk_buffer[16];
    if (RAND_bytes(ssk_buffer, 16) != 1)
    {
      Logger::error("Failed to generate 16 random bytes");
      return false;
    }

    SqliteDB::QueryResults new_id;
    if (!d_database.exec("INSERT INTO recipient (distribution_list_id, storage_service_key, profile_sharing) VALUES (?, ?, ?) "
                         "RETURNING _id",
                         {1L, Base64::bytesToBase64String(ssk_buffer, 16), 1}, &new_id) ||
        new_id.rows() != 1)
      return false;

    long long int new_id_val = new_id.valueAsInt(0, 0, -1);
    if (new_id_val == -1)
      return false;

    union
    {
      struct
      {
        uint32_t time_low;
        uint16_t time_mid;
        uint16_t time_hi_and_version;
        uint8_t  clk_seq_hi_res;
        uint8_t  clk_seq_low;
        uint8_t  node[6];
      } uuidstruct;
      uint8_t rnd[16];
    } uuid;

    if (RAND_bytes(uuid.rnd, sizeof(uuid)) != 1)
    {
      Logger::error("Failed to generate 16 random bytes (2)");
      return false;
    }

    // Refer Section 4.2 of RFC-4122
    // https://tools.ietf.org/html/rfc4122#section-4.2
    uuid.uuidstruct.clk_seq_hi_res = (uint8_t) ((uuid.uuidstruct.clk_seq_hi_res & 0x3F) | 0x80);
    uuid.uuidstruct.time_hi_and_version = (uint16_t) ((uuid.uuidstruct.time_hi_and_version & 0x0FFF) | 0x4000);

#if __cpp_lib_format >= 201907L
    std::string uuid_str = std::format("{:0>8x}-{:0>4x}-{:0>4x}-{:0>2x}{:0>2x}-{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}{:0>2x}",
                                       uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                                       uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                                       uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                                       uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]);
#else
    int size = 1 + std::snprintf(nullptr, 0, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                 uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                                 uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                                 uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                                 uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]);
    if (size <= 0)
    {
      Logger::error("Failed to get size of uuid");
      return false;
    }

    std::unique_ptr<char[]> uuid_char(new char[size]);
    if (std::snprintf(uuid_char.get(), size, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                      uuid.uuidstruct.time_low, uuid.uuidstruct.time_mid, uuid.uuidstruct.time_hi_and_version,
                      uuid.uuidstruct.clk_seq_hi_res, uuid.uuidstruct.clk_seq_low,
                      uuid.uuidstruct.node[0], uuid.uuidstruct.node[1], uuid.uuidstruct.node[2],
                      uuid.uuidstruct.node[3], uuid.uuidstruct.node[4], uuid.uuidstruct.node[5]) < 0)
    {
      Logger::error("failed to format UUID");
      return false;
    }
    std::string uuid_str(uuid_char.get(), uuid_char.get() + size - 1);
#endif
    if (!d_database.exec("INSERT INTO distribution_list (_id, name, distribution_id, recipient_id) VALUES (?, ?, ?, ?)",
                         {1L, uuid_str, uuid_str, new_id_val}))
      return false;
  }

  if (d_databaseversion < 133)
  {
    Logger::message("To 133");
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN allows_replies INTEGER DEFAULT 1"))
      return false;
  }

  if (d_databaseversion < 134)
  {
    Logger::message("To 134");
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN display_as_story INTEGER DEFAULT 0"))
      return false;
  }

  if (d_databaseversion < 135)
  {
    Logger::message("To 135");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_index ON mms (thread_id, date_received, is_story, parent_story_id)"))
      return false;
  }

  if (d_databaseversion < 136)
  {
    Logger::message("To 136");
    if (!d_database.exec("CREATE TABLE story_sends ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, allows_replies INTEGER NOT NULL )") ||
        !d_database.exec("CREATE INDEX story_sends_recipient_id_sent_timestamp_allows_replies_index ON story_sends (recipient_id, sent_timestamp, allows_replies)"))
      return false;
  }

  if (d_databaseversion < 137)
  {
    Logger::message("To 137");
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN deletion_timestamp INTEGER DEFAULT 0") ||
        !d_database.exec("UPDATE recipient SET group_type = 4 WHERE distribution_list_id IS NOT NULL") ||
        !d_database.exec("UPDATE distribution_list SET name = '00000000-0000-0000-0000-000000000000', distribution_id = '00000000-0000-0000-0000-000000000000' WHERE _id = 1"))
      return false;
  }

  if (d_databaseversion < 138)
  {
    Logger::message("To 138");
    if (!d_database.exec("UPDATE recipient SET storage_service_key = NULL WHERE distribution_list_id IS NOT NULL AND NOT EXISTS(SELECT _id from distribution_list WHERE _id = distribution_list_id)"))
      return false;
  }

  if (d_databaseversion < 139)
  {
    Logger::message("To 139");
    if (!d_database.exec("DELETE FROM storage_key WHERE type <= 4"))
      return false;
  }

  if (d_databaseversion < 140)
  {
    Logger::message("To 140");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS recipient_service_id_profile_key ON recipient (uuid, profile_key) WHERE uuid NOT NULL AND profile_key NOT NULL") ||
        !d_database.exec("CREATE TABLE cds ( _id INTEGER PRIMARY KEY, e164 TEXT NOT NULL UNIQUE ON CONFLICT IGNORE, last_seen_at INTEGER DEFAULT 0)"))
      return false;
  }

  if (d_databaseversion < 141)
  {
    Logger::message("To 141");
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN auth_service_id TEXT DEFAULT NULL"))
      return false;
  }

  if (d_databaseversion < 142)
  {
    Logger::message("To 142");
    if (!d_database.exec("ALTER TABLE mms ADD COLUMN quote_type INTEGER DEFAULT 0"))
      return false;
  }

  if (d_databaseversion < 143)
  {
    Logger::message("To 143");
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN is_unknown INTEGER DEFAULT 0"))
      return false;

    if (!d_database.exec("CREATE TABLE story_sends_tmp ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, allows_replies INTEGER NOT NULL, distribution_id TEXT NOT NULL REFERENCES distribution_list (distribution_id) ON DELETE CASCADE )"))
      return false;

    if (!d_database.exec("INSERT INTO story_sends_tmp (_id, message_id, recipient_id, sent_timestamp, allows_replies, distribution_id) SELECT story_sends._id, story_sends.message_id, story_sends.recipient_id, story_sends.sent_timestamp, story_sends.allows_replies, distribution_list.distribution_id FROM story_sends INNER JOIN mms ON story_sends.message_id = mms._id INNER JOIN distribution_list ON distribution_list.recipient_id = mms.address"))
      return false;

    if (!d_database.exec("DROP TABLE story_sends") ||
        !d_database.exec("DROP INDEX IF EXISTS story_sends_recipient_id_sent_timestamp_allows_replies_index") ||
        !d_database.exec("ALTER TABLE story_sends_tmp RENAME TO story_sends") ||
        !d_database.exec("CREATE INDEX story_sends_recipient_id_sent_timestamp_allows_replies_index ON story_sends (recipient_id, sent_timestamp, allows_replies)"))
      return false;
  }

  if (d_databaseversion < 144)
  {
    Logger::message("To 144");
    if (!d_database.exec("UPDATE mms SET read = 1 WHERE parent_story_id > 0"))
      return false;
  }

  if (d_databaseversion < 145)
  {
    Logger::message("To 145");
    if (!d_database.exec("DELETE FROM mms WHERE parent_story_id > 0 AND parent_story_id NOT IN (SELECT _id FROM mms WHERE remote_deleted = 0)"))
      return false;
  }

  if (d_databaseversion < 146)
  {
    Logger::message("To 146");
    if (!d_database.exec("CREATE TABLE remote_megaphone ( _id INTEGER PRIMARY KEY, uuid TEXT UNIQUE NOT NULL, priority INTEGER NOT NULL, countries TEXT, minimum_version INTEGER NOT NULL, dont_show_before INTEGER NOT NULL, dont_show_after INTEGER NOT NULL, show_for_days INTEGER NOT NULL, conditional_id TEXT, primary_action_id TEXT, secondary_action_id TEXT, image_url TEXT, image_uri TEXT DEFAULT NULL, title TEXT NOT NULL, body TEXT NOT NULL, primary_action_text TEXT, secondary_action_text TEXT, shown_at INTEGER DEFAULT 0, finished_at INTEGER DEFAULT 0)"))
      return false;
  }

  if (d_databaseversion < 147)
  {
    Logger::message("To 147");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_quote_id_quote_author_index ON mms (quote_id, quote_author)"))
      return false;
  }

  if (d_databaseversion < 148)
  {
    Logger::message("To 148");
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN privacy_mode INTEGER DEFAULT 0") ||
        !d_database.exec("UPDATE distribution_list SET privacy_mode = 1 WHERE _id = 1"))
      return false;

    if (!d_database.exec("CREATE TABLE distribution_list_member_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT, list_id INTEGER NOT NULL REFERENCES distribution_list (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id), privacy_mode INTEGER DEFAULT 0)") ||
        !d_database.exec("INSERT INTO distribution_list_member_tmp SELECT _id, list_id, recipient_id, 0 FROM distribution_list_member") ||
        !d_database.exec("DROP TABLE distribution_list_member") ||
        !d_database.exec("ALTER TABLE distribution_list_member_tmp RENAME TO distribution_list_member") ||
        !d_database.exec("UPDATE distribution_list_member SET privacy_mode = 1 WHERE list_id = 1") ||
        !d_database.exec("CREATE UNIQUE INDEX distribution_list_member_list_id_recipient_id_privacy_mode_index ON distribution_list_member (list_id, recipient_id, privacy_mode)"))
      return false;
  }

  if (d_databaseversion < 149)
  {
    Logger::message("To 149");
    if (!d_database.exec("UPDATE recipient SET profile_key_credential = NULL"))
      return false;
  }

  if (d_databaseversion < 150)
  {
    Logger::message("To 150");
    if (!d_database.exec("ALTER TABLE msl_payload ADD COLUMN urgent INTEGER NOT NULL DEFAULT 1"))
      return false;

    // setDBV 150
  }

  // NOTE THIS IS A DUPLICATE OF 153 FOR SOME REASON
  if (d_databaseversion < 151)
  {
    Logger::message("To 151");
    std::string mystory_dist_id = d_database.getSingleResultAs<std::string>("SELECT distribution_id FROM distribution_list WHERE _id = 1", std::string());

    if (mystory_dist_id == "00000000-0000-0000-0000-000000000000") // ok!
      ;
    else
    {
      if (mystory_dist_id.empty()) // need to create...
      {
        // get mystoryrecipient_id
        long long int mystory_rid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE distribution_list_id = 1", -1);
        if (mystory_rid == -1) // create
        {
          Logger::error("Unable to create new recipient");
          return false;
        }
        if (!d_database.exec("INSERT INTO distribution_list (_id, name, distribution_id, recipient_id, privacy_mode) "
                             "VALLUES "
                             "(?, ?, ?, ?, ?)",
                             {1, "00000000-0000-0000-0000-000000000000", "00000000-0000-0000-0000-000000000000", mystory_rid, 2}))
          return false;
      }
      else // wrong dist id
      {
        if (!d_database.exec("UPDATE distribution_list SET distribution_id = ? WHERE _id = 1", "00000000-0000-0000-0000-000000000000"))
          return false;
      }
    }

    // setDBV 151
  }

  if (d_databaseversion < 152)
  {
    Logger::message("To 152");
    if (!d_database.exec("UPDATE recipient SET group_type = 4 WHERE  distribution_list_id IS NOT NULL"))
      return false;

    // setDBV 152
  }


  // NOTE THIS IS A DUPLICATE OF 151 FOR SOME REASON
  if (d_databaseversion < 153)
  {
    Logger::message("To 153");
    std::string mystory_dist_id = d_database.getSingleResultAs<std::string>("SELECT distribution_id FROM distribution_list WHERE _id = 1", std::string());

    if (mystory_dist_id == "00000000-0000-0000-0000-000000000000") // ok!
      ;
    else
    {
      if (mystory_dist_id.empty()) // need to create...
      {
        // get mystoryrecipient_id
        long long int mystory_rid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE distribution_list_id = 1", -1);
        if (mystory_rid == -1) // create
        {
          Logger::error("Unable to create new recipient");
          return false;
        }
        if (!d_database.exec("INSERT INTO distribution_list (_id, name, distribution_id, recipient_id, privacy_mode) "
                             "VALLUES "
                             "(?, ?, ?, ?, ?)",
                             {1, "00000000-0000-0000-0000-000000000000", "00000000-0000-0000-0000-000000000000", mystory_rid, 2}))
          return false;
      }
      else // wrong dist id
      {
        if (!d_database.exec("UPDATE distribution_list SET distribution_id = ? WHERE _id = 1", "00000000-0000-0000-0000-000000000000"))
          return false;
      }
    }

    // setDBV 153
  }

  if (d_databaseversion < 154)
  {
    Logger::message("To 154");
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN needs_pni_signature") ||
        !d_database.exec("CREATE TABLE pending_pni_signature_message (_id INTEGER PRIMARY KEY, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, device_id INTEGER NOT NULL)") ||
        !d_database.exec("CREATE UNIQUE INDEX pending_pni_recipient_sent_device_index ON pending_pni_signature_message (recipient_id, sent_timestamp, device_id)"))
      return false;

    // setDBV154
  }


  if (d_databaseversion < 155)
  {
    Logger::message("To 155");
    if (!d_database.exec("ALTER TABLE mms ADD COLUMN export_state BLOB DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE mms ADD COLUMN exported INTEGER DEFAULT 0") ||
        !d_database.exec("ALTER TABLE sms ADD COLUMN export_state BLOB DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE sms ADD COLUMN exported INTEGER DEFAULT 0"))
      return false;
    // setDBV155
  }


  if (d_databaseversion < 156)
  {
    Logger::message("To 156");
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN unregistered_timestamp INTEGER DEFAULT 0"))
      return false;

    // 2678400000 is suppoedly 31 days in millisecond
    if (!d_database.exec("UPDATE recipient SET unregistered_timestamp = ? WHERE registered = ? AND group_type = ?", {2678400000, 2, 0}))
      return false;

    // setDBV156
  }

  if (d_databaseversion < 157)
  {
    Logger::message("To 157");
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN hidden INTEGER DEFAULT 0"))
      return false;

    // setDBV157
  }

  if (d_databaseversion < 158)
  {
    Logger::message("To 158");
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN last_force_update_timestamp INTEGER DEFAULT 0"))
      return false;

    // setDBV158
  }

  if (d_databaseversion < 159)
  {
    Logger::message("To 159");
    if (!d_database.exec("ALTER TABLE thread ADD COLUMN unread_self_mention_count INTEGER DEFAULT 0"))
      return false;

    // setDBV159
  }

  if (d_databaseversion < 160)
  {
    Logger::message("To 160");

    if (!d_database.exec("CREATE INDEX IF NOT EXISTS sms_exported_index ON sms (exported)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_exported_index ON mms (exported)"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 161)
  {
    Logger::message("To 161");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS story_sends_message_id_distribution_id_index ON story_sends (message_id, distribution_id)"))
      return false;

    // setDBV161
  }

  if (d_databaseversion < 162)
  {
    Logger::message("To 162");
    if (!d_database.tableContainsColumn("thread", "unread_self_mention_count"))
      if (!d_database.exec("ALTER TABLE thread ADD COLUMN unread_self_mention_count INTEGER DEFAULT 0"))
        return false;

    // setDBV162
  }

  if (d_databaseversion < 163)
  {
    Logger::message("To 163");
    if (!d_database.tableContainsColumn("remote_megaphone", "primary_action_data"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN primary_action_data TEXT DEFAULT NULL"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "secondary_action_data"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN secondary_action_data TEXT DEFAULT NULL"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "snoozed_at"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN snoozed_at INTEGER DEFAULT 0"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "seen_count"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN seen_count INTEGER DEFAULT 0"))
        return false;

    // setDBV163
  }

  if (d_databaseversion < 164)
  {
    Logger::message("To 164");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS thread_read ON thread (read)"))
      return false;

    // setDBV164
  }

  if (d_databaseversion < 165)
  {
    Logger::message("To 165");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_id_msg_box_payment_transactions_index ON mms (_id, msg_box) WHERE msg_box & 0x300000000 != 0"))
      return false;

    // setDBV165
  }

  if (d_databaseversion < 166)
  {
    Logger::message("To 166");
    if (d_database.tableContainsColumn("thread", "thread_recipient_id"))
    {
      // removeDuplicateThreadEntries(db)
      SqliteDB::QueryResults res;
      if (!d_database.exec("SELECT thread_recipient_id, COUNT(*) AS thread_count FROM thread GROUP BY thread_recipient_id HAVING thread_count > 1", &res))
        return false;
      for (unsigned int i = 0; i < res.rows(); ++i)
      {
        long long int rid = res.valueAsInt(i, "recipient_id", -1);
        if (rid == -1)
          return false;

        SqliteDB::QueryResults res2;
        if (!d_database.exec("SELECT _id, date FROM thread WHERE thread_recipient_id = ? ORDER BY date DESC", rid, &res2))
          return false;

        long long int mainthread = res2.valueAsInt(0, "_id", -1);
        if (mainthread == -1)
          return false;

        for (unsigned int j = 1; j < res2.rows(); ++j)
        {
          // merge into mainthread
          if (!d_database.exec("UPDATE drafts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE mms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE sms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE pending_retry_receipts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE remapped_threads SET new_id = ? WHERE new_id = ?", {mainthread, res2.value(j, "_id")}))
            return false;

          if (!d_database.exec("DELETE FROM thread WHERE _id = ?", res2.value(j, "_id")))
            return false;
        }
      }

      // updateThreadTableSchema(db)
      if (!d_database.exec("CREATE TABLE thread_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, meaningful_messages INTEGER DEFAULT 0,recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE,read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0,snippet TEXT, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type TEXT DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, unread_count INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, unread_self_mention_count INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO thread_tmp SELECT _id,date,message_count,thread_recipient_id,read,type,error,snippet,snippet_type,snippet_uri,snippet_content_type,snippet_extras,unread_count,archived,status,delivery_receipt_count,read_receipt_count,expires_in,last_seen,has_sent,last_scrolled,pinned,unread_self_mention_count FROM thread"))
        return false;

      if (!d_database.exec("DROP TABLE thread") ||
          !d_database.exec("ALTER TABLE thread_tmp RENAME TO thread") ||
          !d_database.exec("CREATE INDEX thread_recipient_id_index ON thread (recipient_id)") ||
          !d_database.exec("CREATE INDEX archived_count_index ON thread (archived, meaningful_messages)") ||
          !d_database.exec("CREATE INDEX thread_pinned_index ON thread (pinned)") ||
          !d_database.exec("CREATE INDEX thread_read ON thread (read)"))
        return false;

      // fixDanglingSmsMessages(db)
      if (!d_database.exec("DELETE FROM sms WHERE address IS NULL OR address NOT IN (SELECT _id FROM recipient)") ||
          !d_database.exec("DELETE FROM sms WHERE thread_id IS NULL OR thread_id NOT IN (SELECT _id FROM thread)"))
        return false;

      // fixDanglingMmsMessages(db)
      if (!d_database.exec("DELETE FROM mms WHERE address IS NULL OR address NOT IN (SELECT _id FROM recipient)") ||
          !d_database.exec("DELETE FROM mms WHERE thread_id IS NULL OR thread_id NOT IN (SELECT _id FROM thread)"))
        return false;

      // updateSmsTableSchema(db)
      if (!d_database.exec("CREATE TABLE sms_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT,date_sent INTEGER NOT NULL,date_received INTEGER NOT NULL,date_server INTEGER DEFAULT -1,thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE,recipient_id NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE,recipient_device_id INTEGER DEFAULT 1,type INTEGER,body TEXT,read INTEGER DEFAULT 0,status INTEGER DEFAULT -1,delivery_receipt_count INTEGER DEFAULT 0,mismatched_identities TEXT DEFAULT NULL,subscription_id INTEGER DEFAULT -1,expires_in INTEGER DEFAULT 0,expire_started INTEGER DEFAULT 0,notified INTEGER DEFAULT 0,read_receipt_count INTEGER DEFAULT 0,unidentified INTEGER DEFAULT 0,reactions_unread INTEGER DEFAULT 0,reactions_last_seen INTEGER DEFAULT -1,remote_deleted INTEGER DEFAULT 0,notified_timestamp INTEGER DEFAULT 0,server_guid TEXT DEFAULT NULL,receipt_timestamp INTEGER DEFAULT -1,export_state BLOB DEFAULT NULL,exported INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO sms_tmp SELECT _id, date_sent, date, date_server, thread_id, address, address_device_id, type, body, read, status, delivery_receipt_count, mismatched_identities, subscription_id, expires_in, expire_started, notified, read_receipt_count, unidentified, reactions_unread, reactions_last_seen, remote_deleted, notified_timestamp, server_guid, receipt_timestamp, export_state, exported FROM sms"))
        return false;

      if (!d_database.exec("DROP TABLE sms") ||
          !d_database.exec("ALTER TABLE sms_tmp RENAME TO sms") ||
          !d_database.exec("CREATE INDEX sms_read_and_notified_and_thread_id_index ON sms(read, notified, thread_id)") ||
          !d_database.exec("CREATE INDEX sms_type_index ON sms (type)") ||
          !d_database.exec("CREATE INDEX sms_date_sent_index ON sms (date_sent, recipient_id, thread_id)") ||
          !d_database.exec("CREATE INDEX sms_date_server_index ON sms (date_server)") ||
          !d_database.exec("CREATE INDEX sms_thread_date_index ON sms (thread_id, date_received)") ||
          !d_database.exec("CREATE INDEX sms_reactions_unread_index ON sms (reactions_unread)") ||
          !d_database.exec("CREATE INDEX sms_exported_index ON sms (exported)") ||
          !d_database.exec("CREATE TRIGGER sms_ai AFTER INSERT ON sms BEGIN INSERT INTO sms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER sms_ad AFTER DELETE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER sms_au AFTER UPDATE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO sms_fts(rowid, body, thread_id) VALUES(new._id, new.body, new.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER msl_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 0); END"))
        return false;

      // updateMmsTableSchema(db)
      if (!d_database.exec("CREATE TABLE mms_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, date_server INTEGER DEFAULT -1, thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, recipient_device_id INTEGER, type INTEGER NOT NULL, body TEXT, read INTEGER DEFAULT 0, ct_l TEXT, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, subscription_id INTEGER DEFAULT -1, receipt_timestamp INTEGER DEFAULT -1, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, viewed_receipt_count INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author INTEGER DEFAULT 0, quote_body TEXT DEFAULT NULL, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, quote_type INTEGER DEFAULT 0, shared_contacts TEXT DEFAULT NULL, unidentified INTEGER DEFAULT 0, link_previews TEXT DEFAULT NULL, view_once INTEGER DEFAULT 0, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL, message_ranges BLOB DEFAULT NULL, story_type INTEGER DEFAULT 0, parent_story_id INTEGER DEFAULT 0, export_state BLOB DEFAULT NULL, exported INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO mms_tmp SELECT _id, date, date_received, date_server, thread_id, address, address_device_id, msg_box, body, read, ct_l, exp, m_type, m_size, st, tr_id, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, viewed_receipt_count, mismatched_identities, network_failures, expires_in, expire_started, notified, quote_id, quote_author, quote_body, quote_missing, quote_mentions, quote_type, shared_contacts, unidentified, previews, reveal_duration, reactions_unread, reactions_last_seen, remote_deleted, mentions_self, notified_timestamp, server_guid, ranges, is_story, parent_story_id, export_state, exported FROM mms"))
        return false;

      if (!d_database.exec("DROP TABLE mms") ||
          !d_database.exec("ALTER TABLE mms_tmp RENAME TO mms") ||
          !d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
          !d_database.exec("CREATE INDEX mms_type_index ON mms (type)") ||
          !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (date_sent, recipient_id, thread_id)") ||
          !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
          !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
          !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_story_type_index ON mms (story_type)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_parent_story_id_index ON mms (parent_story_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_index ON mms (thread_id, date_received, story_type, parent_story_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_quote_id_quote_author_index ON mms (quote_id, quote_author)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_exported_index ON mms (exported)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_id_type_payment_transactions_index ON mms (_id, type) WHERE type & 0x300000000 != 0") ||
          !d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER mms_ad AFTER DELETE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER mms_au AFTER UPDATE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER msl_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 1); END"))
        return false;
    }
    // setDBV166
  }

  if (d_databaseversion < 167)
  {
    Logger::message("To 167");
    if (!d_database.exec("DELETE FROM reaction  WHERE (is_mms = 0 AND message_id NOT IN (SELECT _id FROM sms)) OR(is_mms = 1 AND message_id NOT IN (SELECT _id FROM mms))") ||
        !d_database.exec("CREATE TRIGGER IF NOT EXISTS reactions_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 0; END") ||
        !d_database.exec("CREATE TRIGGER IF NOT EXISTS reactions_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 1; END"))
      return false;
    // setDBV167
  }


  if (d_databaseversion < 168)
  {
    Logger::message("To 168");

    if (!d_database.exec("DROP TRIGGER msl_sms_delete") ||
        !d_database.exec("DROP TRIGGER reactions_sms_delete") ||
        !d_database.exec("DROP TRIGGER sms_ai") ||
        !d_database.exec("DROP TRIGGER sms_au") ||
        !d_database.exec("DROP TRIGGER sms_ad") ||
        !d_database.exec("DROP TABLE sms_fts") ||
        !d_database.exec("DROP INDEX mms_read_and_notified_and_thread_id_index") ||
        !d_database.exec("DROP INDEX mms_type_index") ||
        !d_database.exec("DROP INDEX mms_date_sent_index") ||
        !d_database.exec("DROP INDEX mms_date_server_index") ||
        !d_database.exec("DROP INDEX mms_thread_date_index") ||
        !d_database.exec("DROP INDEX mms_reactions_unread_index") ||
        !d_database.exec("DROP INDEX mms_story_type_index") ||
        !d_database.exec("DROP INDEX mms_parent_story_id_index") ||
        !d_database.exec("DROP INDEX mms_thread_story_parent_story_index") ||
        !d_database.exec("DROP INDEX mms_quote_id_quote_author_index") ||
        !d_database.exec("DROP INDEX mms_exported_index") ||
        !d_database.exec("DROP INDEX mms_id_type_payment_transactions_index") ||
        !d_database.exec("DROP TRIGGER mms_ai"))
      return false;

    // copySmsToMms(db, nextMmsId)
    SqliteDB::QueryResults minmax;
    if (!d_database.exec("SELECT MIN(_id) AS min, MAX(_id) AS max FROM sms", &minmax))
      return false;
    long long int minsms = minmax.getValueAs<long long int>(0, "min");
    if (!d_database.exec("SELECT MIN(_id) AS min, MAX(_id) AS max FROM mms", &minmax))
      return false;
    long long int maxmms = minmax.getValueAs<long long int>(0, "max");

    long long int id_offset = (maxmms - minsms) + 1;

    if (!d_database.exec("INSERT INTO mms SELECT _id + ?, date_sent, date_received, date_server, thread_id, recipient_id, recipient_device_id, type, body, read, null, 0, 0, 0, status, null, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, 0, mismatched_identities, null, expires_in, expire_started, notified, 0, 0, null, 0, null, 0, null, unidentified, null, 0, reactions_unread, reactions_last_seen, remote_deleted, 0, notified_timestamp, server_guid, null, 0, 0, export_state, exported FROM sms", id_offset))
      return false;

    if (!d_database.exec("DROP TABLE sms"))
      return false;

    if (!d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_type_index ON mms (type)") ||
        !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (date_sent, recipient_id, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
        !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
        !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)") ||
        !d_database.exec("CREATE INDEX mms_story_type_index ON mms (story_type)") ||
        !d_database.exec("CREATE INDEX mms_parent_story_id_index ON mms (parent_story_id)") ||
        !d_database.exec("CREATE INDEX mms_thread_story_parent_story_index ON mms (thread_id, date_received, story_type, parent_story_id)") ||
        !d_database.exec("CREATE INDEX mms_quote_id_quote_author_index ON mms (quote_id, quote_author)") ||
        !d_database.exec("CREATE INDEX mms_exported_index ON mms (exported)") ||
        !d_database.exec("CREATE INDEX mms_id_type_payment_transactions_index ON mms (_id, type) WHERE type & 0x300000000 != 0") ||
        !d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts (rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;"))
      return false;

    if (!d_database.exec("UPDATE reaction SET message_id = message_id + ? WHERE is_mms = 0", id_offset) ||
        !d_database.exec("UPDATE msl_message SET message_id = message_id + ? WHERE is_mms = 0", id_offset))
      return false;

    // setDBV168
  }

  if (d_databaseversion < 169)
  {
    Logger::message("To 169");
    if (!d_database.exec("CREATE TABLE emoji_search_tmp ( _id INTEGER PRIMARY KEY, label TEXT NOT NULL, emoji TEXT NOT NULL, rank INTEGER DEFAULT 2147483647)" /*int.MAX_VALUE*/) ||
        !d_database.exec("INSERT INTO emoji_search_tmp (label, emoji) SELECT label, emoji from emoji_search") ||
        !d_database.exec("DROP TABLE emoji_search") ||
        !d_database.exec("ALTER TABLE emoji_search_tmp RENAME TO emoji_search") ||
        !d_database.exec("CREATE INDEX emoji_search_rank_covering ON emoji_search (rank, label, emoji)"))
      return false;
    // setDBV169
  }

  if (d_databaseversion < 170)
  {
    Logger::message("To 170");
    if (!d_database.exec("CREATE TABLE call ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL UNIQUE, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, peer INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL)") ||
        !d_database.exec("CREATE INDEX call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX call_message_id_index ON call (message_id)"))
      return false;
    // setDBV170
  }

  if (d_databaseversion < 171)
  {
    Logger::message("To 171");

    // removeDuplicateThreadEntries // almost (no sms) duplicates 166
    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT recipient_id, COUNT(*) AS thread_count FROM thread GROUP BY recipient_id HAVING thread_count > 1", &res))
      return false;

    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      long long int rid = res.valueAsInt(i, "recipient_id", -1);
      if (rid == -1)
        return false;


      SqliteDB::QueryResults res2;
      if (!d_database.exec("SELECT _id, date FROM thread WHERE recipient_id = ? ORDER BY date DESC", rid, &res2))
        return false;

      long long int mainthread = res2.valueAsInt(0, "recipient_id", -1);
      if (mainthread == -1)
        return false;

      for (unsigned int j = 1; j < res2.rows(); ++j)
      {
        // merge into mainthread
        if (!d_database.exec("UPDATE drafts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE mms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE pending_retry_receipts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE remapped_threads SET new_id = ? WHERE new_id = ?", {mainthread, res2.value(j, "_id")}))
          return false;

        if (!d_database.exec("DELETE FROM thread WHERE _id = ?", res2.value(j, "_id")))
          return false;
      }
    }

    // updateThreadTableSchema(db)
    if (!d_database.exec("CREATE TABLE thread_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, meaningful_messages INTEGER DEFAULT 0, recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE, read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0, snippet TEXT, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type TEXT DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, unread_count INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, unread_self_mention_count INTEGER DEFAULT 0)"))
      return false;

    if (!d_database.exec("INSERT INTO thread_tmp SELECT _id, date, meaningful_messages, recipient_id, read, type, error, snippet, snippet_type, snippet_uri, snippet_content_type, snippet_extras, unread_count, archived, status, delivery_receipt_count, read_receipt_count, expires_in, last_seen, has_sent, last_scrolled, pinned, unread_self_mention_count FROM thread"))
      return false;

    if (!d_database.exec("DROP TABLE thread") ||
        !d_database.exec("ALTER TABLE thread_tmp RENAME TO thread") ||
        !d_database.exec("CREATE INDEX thread_recipient_id_index ON thread (recipient_id)") ||
        !d_database.exec("CREATE INDEX archived_count_index ON thread (archived, meaningful_messages)") ||
        !d_database.exec("CREATE INDEX thread_pinned_index ON thread (pinned)") ||
        !d_database.exec("CREATE INDEX thread_read ON thread (read)"))
      return false;

    // setDBV171
  }

  if (d_databaseversion < 172)
  {
    Logger::message("To 172");
    if (!d_database.exec("CREATE TABLE group_membership (_id INTEGER PRIMARY KEY,group_id TEXT NOT NULL,recipient_id INTEGER NOT NULL,UNIQUE(group_id, recipient_id))"))
      return false;

    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT members, group_id FROM groups", &res))
      return false;

    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      std::string group_id(res(i, "group_id"));
      if (group_id.empty())
        return false;

      std::string membersstring(res.valueAsString(i, "members"));
      std::regex comma(",");
      std::sregex_token_iterator iter(membersstring.begin(), membersstring.end(), comma, -1);
      std::vector<long long int> members_list;
      std::transform(iter, std::sregex_token_iterator(), std::back_inserter(members_list),
                     [](std::string const &m) -> long long int { return bepaald::toNumber<long long int>(m); });

      for (unsigned int j = 0; j < members_list.size(); ++j)
        if (!d_database.exec("INSERT INTO group_membership (group_id, recipient_id) VALUES (?, ?)", {group_id, members_list[j]}))
          return false;
    }

    if (!d_database.exec("ALTER TABLE groups DROP COLUMN members"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 173)
  {
    Logger::message("To 173");

    if (!d_database.exec("ALTER TABLE mms ADD COLUMN scheduled_date INTEGER DEFAULT -1") ||
        !d_database.exec("DROP INDEX mms_thread_story_parent_story_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_scheduled_date_index ON mms (thread_id, date_received,story_type,parent_story_id,scheduled_date);"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 174)
  {
    Logger::message("To 174");
    if (!d_database.exec("CREATE TABLE reaction_tmp ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, author_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, emoji TEXT NOT NULL, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, UNIQUE(message_id, author_id) ON CONFLICT REPLACE )"))
      return false;
    if (!d_database.exec("INSERT INTO reaction_tmp SELECT _id, message_id, author_id, emoji, date_sent, date_received FROM reaction WHERE message_id IN (SELECT _id FROM mms)"))
      return false;

    if (!d_database.exec("DROP TABLE reaction") ||
        !d_database.exec("DROP TRIGGER IF EXISTS reactions_mms_delete") ||
        !d_database.exec("ALTER TABLE reaction_tmp RENAME TO reaction") ||
        !d_database.exec("ALTER TABLE mms RENAME TO message"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 175)
  {
    Logger::message("To 175");

    if (!d_database.exec("DROP TABLE mms_fts") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_ai") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_ad") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_au") ||
        !d_database.exec("CREATE VIRTUAL TABLE message_fts USING fts5(body, thread_id UNINDEXED, content=message, content_rowid=_id)") ||
        !d_database.exec("CREATE TRIGGER message_ai AFTER INSERT ON message BEGIN INSERT INTO message_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER message_ad AFTER DELETE ON message BEGIN INSERT INTO message_fts(message_fts, rowid, body, thread_id) VALUES ('delete', old._id, old.body, old.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER message_au AFTER UPDATE ON message BEGIN INSERT INTO message_fts(message_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO message_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 176)
  {
    Logger::message("To 176");
    if (!d_database.exec("DROP INDEX IF EXISTS mms_quote_id_quote_author_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_index ON message (quote_id, quote_author, scheduled_date);"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 177)
  {
    Logger::message("To 177");
    if (!d_database.exec("DROP TRIGGER IF EXISTS msl_mms_delete") ||
        !d_database.exec("DROP TRIGGER IF EXISTS msl_attachment_delete") ||
        !d_database.exec("DROP INDEX IF EXISTS msl_message_message_index"))
      return false;

    if (!d_database.exec("DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id NOT IN (SELECT _id FROM message))") ||
        !d_database.exec("CREATE TABLE msl_message_tmp (_id INTEGER PRIMARY KEY,payload_id INTEGER NOT NULL REFERENCES msl_payload (_id) ON DELETE CASCADE,message_id INTEGER NOT NULL)") ||
        !d_database.exec("INSERT INTO msl_message_tmp SELECT _id, payload_id, message_id FROM msl_message") ||
        !d_database.exec("DROP TABLE msl_message") ||
        !d_database.exec("ALTER TABLE msl_message_tmp RENAME TO msl_message") ||
        !d_database.exec("CREATE INDEX msl_message_message_index ON msl_message (message_id, payload_id)") ||
        !d_database.exec("CREATE TRIGGER msl_message_delete AFTER DELETE ON message BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id); END") ||
        !d_database.exec("CREATE TRIGGER msl_attachment_delete AFTER DELETE ON part BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old.mid); END"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 178)
  {
    Logger::message("To 178");
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN reporting_token BLOB DEFAULT NULL"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 179)
  {
    Logger::message("To 179");
    if (!d_database.exec("DELETE FROM msl_message WHERE payload_id NOT IN (SELECT _id FROM msl_payload)") ||
        !d_database.exec("DELETE FROM msl_recipient WHERE payload_id NOT IN (SELECT _id FROM msl_payload)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 180)
  {
    Logger::message("To 180");
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN system_nickname TEXT DEFAULT NULL"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 181)
  {
    Logger::message("To 181");
    if (!d_database.exec("DELETE FROM thread WHERE recipient_id NOT IN (SELECT _id FROM recipient)"))
      return false;
    if (d_database.changed())
    {
      if (!d_database.exec("DELETE FROM message WHERE thread_id NOT IN (SELECT _id FROM thread)"))
        return false;
      if (d_database.changed())
      {
        if (!d_database.exec("DELETE FROM story_send  WHERE message_id NOT IN (SELECT _id FROM message)") ||
            !d_database.exec("DELETE FROM reaction WHERE message_id NOT IN (SELECT _id FROM message)") ||
            !d_database.exec("DELETE FROM call WHERE message_id NOT IN (SELECT _id FROM message)"))
          return false;
      }
    }
    // setDBV
  }


  if (d_databaseversion < 182)
  {
    Logger::message("To 182");
    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL UNIQUE, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0 )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, type, direction, event, (SELECT date_sent FROM message WHERE message._id = call.message_id) as timestamp, NULL as ringer, 0 as deletion_timestamp FROM call"))
      return false;

    if (!d_database.exec("DROP TABLE group_call_ring") ||
        !d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 183)
  {
    Logger::message("To 183");

    if (!d_database.exec("CREATE TABLE call_link (_id INTEGER PRIMARY KEY)"))
      return false;

    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link INTEGER DEFAULT NULL REFERENCES call_link (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)) )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;

    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 184)
  {
    Logger::message("To 184");
    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link INTEGER DEFAULT NULL REFERENCES call_link (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (call_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)) )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;


    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call") ||
        !d_database.exec("CREATE INDEX call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX call_message_id_index ON call (message_id)"))
      return false;

    // setDBV184
  }

  if (d_databaseversion < 185)
  {
    Logger::message("To 185");
    d_selfid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
    if (d_selfid == -1)
    {
      Logger::error("Failed to get _id of self. This will probably end badly...");
      return false;
    }
    d_selfuuid = d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string());

    //getdeps
    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT type, name, sql FROM sqlite_schema WHERE tbl_name = 'message' AND type != 'table'", &res))
      return false;
    for (unsigned int i = 0; i < res.rows(); ++i)
      if (!d_database.exec("DROP " + res(i, "type") + " IF EXISTS " + res(i, "name")))
        return false;

    // redo message table
    if (!d_database.exec("CREATE TABLE message_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, date_server INTEGER DEFAULT -1, thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE, from_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, from_device_id INTEGER, to_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, body TEXT, read INTEGER DEFAULT 0, ct_l TEXT, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, subscription_id INTEGER DEFAULT -1, receipt_timestamp INTEGER DEFAULT -1, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, viewed_receipt_count INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author INTEGER DEFAULT 0, quote_body TEXT DEFAULT NULL, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, quote_type INTEGER DEFAULT 0, shared_contacts TEXT DEFAULT NULL, unidentified INTEGER DEFAULT 0, link_previews TEXT DEFAULT NULL, view_once INTEGER DEFAULT 0, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL, message_ranges BLOB DEFAULT NULL, story_type INTEGER DEFAULT 0, parent_story_id INTEGER DEFAULT 0, export_state BLOB DEFAULT NULL, exported INTEGER DEFAULT 0, scheduled_date INTEGER DEFAULT -1, latest_revision_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, original_message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, revision_number INTEGER DEFAULT 0 )"))
      return false;

    if (!d_database.exec("INSERT INTO message_tmp SELECT _id, date_sent, date_received, date_server, thread_id, recipient_id, recipient_device_id, recipient_id, type, body, read, ct_l, exp, m_type, m_size, st, tr_id, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, viewed_receipt_count, mismatched_identities, network_failures, expires_in, expire_started, notified, quote_id, quote_author, quote_body, quote_missing, quote_mentions, quote_type, shared_contacts, unidentified, link_previews, view_once, reactions_unread, reactions_last_seen, remote_deleted, mentions_self, notified_timestamp, server_guid, message_ranges, story_type, parent_story_id, export_state, exported, scheduled_date, NULL AS latest_revision_id, NULL AS original_message_id, 0 as revision_number FROM message"))
      return false;

    if (!d_database.exec("UPDATE message_tmp SET to_recipient_id = from_recipient_id,from_recipient_id = ?,from_device_id = 1 WHERE (type & 0x1f IN (21, 23, 22, 24, 25, 26, 2, 11))", d_selfid))
      return false;

    if (!d_database.exec("DROP TABLE message") ||
        !d_database.exec("ALTER TABLE message_tmp RENAME TO message"))
      return false;

    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      if (res(i, "name") == "mms_thread_story_parent_story_scheduled_date_index")
      {
        if (!d_database.exec("CREATE INDEX message_thread_story_parent_story_scheduled_date_latest_revision_id_index ON message (thread_id, date_received, story_type, parent_story_id, scheduled_date, latest_revision_id)"))
          return false;
      }
      else if (res(i, "name") == "mms_quote_id_quote_author_scheduled_date_index")
      {
        if (!d_database.exec("CREATE INDEX message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
          return false;
      }
      else if (res(i, "name") == "mms_date_sent_index")
      {
        if (!d_database.exec("CREATE INDEX message_date_sent_from_to_thread_index ON message (date_sent, from_recipient_id, to_recipient_id, thread_id)"))
          return false;
      }
      else
      {
        std::string statement(res(i, "sql"));
        if (!statement.empty())
        {
          statement = std::regex_replace(statement, std::regex("CREATE INDEX mms_"), "CREATE INDEX message_");

          // dont actually see the altered statements being executed...
          if (!d_database.exec(statement))
            return false;
        }
      }
    }

    if (!checkDbIntegrity())
      return false;

    // setDBV
  }

  if (d_databaseversion < 186)
  {
    Logger::message("To 186");
    if (d_database.tableContainsColumn("message", "from_recipient_id"))
    {
      if (!d_database.exec("CREATE INDEX IF NOT EXISTS message_original_message_id_index ON message (original_message_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_latest_revision_id_index ON message (latest_revision_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_from_recipient_id_index ON message (from_recipient_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_to_recipient_id_index ON message (to_recipient_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS reaction_author_id_index ON reaction (author_id)") ||
          !d_database.exec("DROP INDEX IF EXISTS message_quote_id_quote_author_scheduled_date_index") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
        return false;

      if (!d_database.exec("ANALYZE message"))
        return false;
    }
    // setDBV
  }

  if (d_databaseversion < 187)
  {
    Logger::message("To 187");
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS call_call_link_index ON call (call_link)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_peer_index ON call (peer)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS distribution_list_member_recipient_id ON distribution_list_member (recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS msl_message_payload_index ON msl_message (payload_id)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 188)
  {
    Logger::message("To 188");
    if (!d_database.tableContainsColumn("message", "from_recipient_id"))
    {
      Logger::error("This should not happen... ");
      return false;
    }

    // These are the indexes that should have been created in V186 -- conditionally done here in case it didn't run properly
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS message_original_message_id_index ON message (original_message_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_latest_revision_id_index ON message (latest_revision_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_from_recipient_id_index ON message (from_recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_to_recipient_id_index ON message (to_recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS reaction_author_id_index ON reaction (author_id)") ||
        !d_database.exec("DROP INDEX IF EXISTS message_quote_id_quote_author_scheduled_date_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 189)
  {
    Logger::message("To 189");
    if (!d_database.exec("CREATE TABLE call_link_tmp ( _id INTEGER PRIMARY KEY, root_key BLOB NOT NULL, room_id TEXT NOT NULL UNIQUE, admin_key BLOB, name TEXT NOT NULL, restrictions INTEGER NOT NULL, revoked INTEGER NOT NULL, expiration INTEGER NOT NULL, avatar_color TEXT NOT NULL )"))
      return false;

    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link TEXT DEFAULT NULL REFERENCES call_link (room_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (call_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)))"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;


    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call") ||
        !d_database.exec("DROP TABLE call_link") ||
        !d_database.exec("ALTER TABLE call_link_tmp RENAME TO call_link") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_message_id_index ON call (message_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_call_link_index ON call (call_link)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_peer_index ON call (peer)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 190)
  {
    Logger::message("To 190");
    // (yes this one's empty, it was buggy and replaced by 191
    // setDBV
  }

  if (d_databaseversion < 191)
  {
    Logger::message("To 191");

    if (!d_database.exec("DROP INDEX IF EXISTS message_unique_sent_from_thread"))
      return false;

    // change date for bad decrypt, expiration_timer_update and chat_session_refreshed duplicates
    if (!d_database.exec("WITH needs_update AS "
                         "("
                         "  SELECT _id FROM message M WHERE "
                         "  ("
                         "    type & 0x40000 != 0 OR "
                         "    type & 0x10000000 != 0 OR "
                         "    type = 13"
                         "  )"
                         "  AND "
                         "  ("
                         "    SELECT COUNT(*) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id"
                         "  ) > 1"
                         ") "
                         "UPDATE message SET date_sent = date_sent - 1 WHERE _id IN needs_update"))
      return false;

    // delete other duplicates if body is identical and type = groupupdate
    if (!d_database.exec("WITH needs_delete AS "
                         "("
                         "  SELECT _id FROM message M WHERE "
                         "  _id > "
                         "  ("
                         "    SELECT min(_id) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id AND "
                         "      ( "
                         "        COALESCE(body, '') = COALESCE(M.body, '') OR "
                         "        type & 0x10000 != 0"
                         "      )"
                         "  )"
                         ") "
                         "DELETE FROM message WHERE _id IN needs_delete"))
      return false;


    // #warning !!! REMOVE THIS !!!
    // SqliteDB::QueryResults res2;
    // if (d_database.exec("SELECT type, date_received, date_sent, from_recipient_id, to_recipient_id, thread_id, body FROM message WHERE _id IN (6851,6852,6853,6867,6868,6869,6803,6804,6805)", &res2))
    // {
    //   for (unsigned int i = 0; i < 5; ++i)
    //     for (unsigned int j = 0; j < res2.rows(); ++j)
    //       if (d_database.exec("INSERT INTO message (type, date_received, date_sent, from_recipient_id, to_recipient_id, thread_id, body) VALUES (?, ?, ?, ?, ?, ?, ?)",
    //                           {res2.value(j, "type"), res2.value(j, "date_received"), res2.value(0, "date_sent"), res2.value(j, "from_recipient_id"), res2.value(j, "to_recipient_id"), res2.value(j, "thread_id"), res2.valueAsString(j, "body") + " " + bepaald::toString(i + 1)}))
    //         ;//std::cout << "added some dupes");
    // }


    //findRemainingDuplicates(db)
    SqliteDB::QueryResults res;
    if (!d_database.exec("WITH dupes AS "
                         "("
                         "  SELECT _id, date_sent, from_recipient_id, thread_id, type FROM message M WHERE "
                         "  ("
                         "    SELECT COUNT(*) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id"
                         "  ) > 1"
                         ") "
                         "SELECT _id, date_sent, from_recipient_id, thread_id, type, body FROM message WHERE "
                         "  _id IN (SELECT _id FROM dupes) ORDER BY date_sent ASC, _id ASC", &res))
      return false;

    if (!res.empty())
    {
      //std::cout << "DUPES REMAINING: " << res.rows());
      //res.prettyPrint(d_truncate);

      struct DupeKey
      {
        long long int date_sent;
        long long int thread_id;
        long long int from_recipient_id;
        bool operator<(DupeKey const &other) const
        {
          return date_sent < other.date_sent ||
            (date_sent == other.date_sent &&
             thread_id < other.thread_id) ||
            (date_sent == other.date_sent &&
             thread_id == other.thread_id &&
             from_recipient_id < other.from_recipient_id);
        }
      };
      std::map<DupeKey, std::vector<long long int>> dupemap;
      for (unsigned int i = 0; i < res.rows(); ++i)
        dupemap[{res.valueAsInt(i, "date_sent"), res.valueAsInt(i, "thread_id"), res.valueAsInt(i, "from_recipient_id")}].push_back(res.valueAsInt(i, "_id"));

      auto dateTaken = [&](long long int date, long long int frid, long long int tid)
      {
        return d_database.getSingleResultAs<long long int>("SELECT EXISTS (SELECT 1 FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                                                           "date_sent = ? AND from_recipient_id = ? AND thread_id = ?)", {date, frid, tid}, 1) == 1;
      };

      for (auto const &p : dupemap)
      {
        // 'p' is one dupe-group, fix it...
        std::vector<long long int> dupe_ids(p.second);

        // make sure the dupes are sorted highest to lowest _id
        std::sort(dupe_ids.begin(), dupe_ids.end(), std::greater{});

        // pun intended
        long long int candiDATE = p.first.date_sent - 1;

        // drop the first (highest _id), it can stay
        dupe_ids.erase(dupe_ids.begin());

        for (auto const &i : dupe_ids)
        {
          while (dateTaken(candiDATE, p.first.from_recipient_id, p.first.thread_id))
            --candiDATE;
          if (!d_database.exec("UPDATE message SET date_sent = ? WHERE _id = ?", {candiDATE, i}))
            return false;
          --candiDATE;
        }
      }

      //d_database.prettyPrint(d_truncate, "SELECT _id, date_sent, from_recipient_id, thread_id, type, body FROM message WHERE _id = 6803 OR _id BETWEEN 73817 AND 73861 ORDER BY from_recipient_id ASC, thread_id ASC, date_sent ASC");

    }

    // setDBV
  }

  // adjust DatabaseVersionFrame
  DeepCopyingUniquePtr<DatabaseVersionFrame> d_new_dbvframe;
  if (!setFrameFromStrings(&d_new_dbvframe, std::vector<std::string>{"VERSION:uint32:191"}))
  {
    Logger::error("Failed to create new databaseversionframe");
    return false;
  }
  d_databaseversionframe.reset(d_new_dbvframe.release());
  d_databaseversion = 191;
  setColumnNames();

  return checkDbIntegrity();
}
