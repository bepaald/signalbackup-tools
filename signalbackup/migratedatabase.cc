/*
  Copyright (C) 2023  Selwin van Dijk

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

bool SignalBackup::migrateDatabase(int from, int to) const
{
  // interpreted from
  // https://github.com/signalapp/Signal-Android/blob/main/app/src/main/java/org/thoughtcrime/securesms/database/helpers/migration/V168_SingleMessageTableMigration.kt
  //
  // NOTE this function does not perform a full migrate to a (necessarily) working backup file. Its just enough for this programs exportHTML() function.

  Logger::message("Attempting to migrate database from version ", from, " to version ", to, "...");

  if (!d_database.exec("BEGIN TRANSACTION"))
    return false;

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
      SqliteDB::QueryResults results;
      d_database.exec("SELECT _id, reactions FROM "s + msgtable + " WHERE reactions IS NOT NULL", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
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

  // add any missing columns to mms (from dbv 123 ( or hopefully -> 99)
  auto ensureColumns = [&](std::string const &table, std::string const &column, std::string const &columndefinition)
  {
    if (!d_database.tableContainsColumn(table, column))
      if (!d_database.exec("ALTER TABLE " + table + " ADD COLUMN " + column + " " + columndefinition))
        return false;
    return true;
  };

  for (auto const &p : {std::pair<std::string, std::string>{"receipt_timestamp", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"quote_type", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"link_previews", "TEXT DEFAULT NULL"},
                        std::pair<std::string, std::string>{"view_once", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"message_ranges", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"story_type", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"parent_story_id", "INTEGER DEFAULT 0"},
                        std::pair<std::string, std::string>{"export_state", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"exported", "INTEGER DEFAULT 0"}})
  {
    if (!ensureColumns(d_mms_table, p.first, p.second))
    {
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }
  }

  for (auto const &p : {std::pair<std::string, std::string>{"receipt_timestamp", "INTEGER DEFAULT -1"},
                        std::pair<std::string, std::string>{"export_state", "BLOB DEFAULT NULL"},
                        std::pair<std::string, std::string>{"exported", "INTEGER DEFAULT 0"}})
  {
    if (!ensureColumns("sms", p.first, p.second))
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

  for (uint i = min; i < max; ++i)
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
                         + d_mms_delivery_receipts + ", "
                         + d_mms_read_receipts + ", "
                         + d_mms_viewed_receipts + ", "
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
                         "message_ranges, "
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
                         + d_mms_delivery_receipts + ", "
                         + d_mms_read_receipts + ", "
                         + d_mms_viewed_receipts + ", "
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
                         "_id IS ? RETURNING _id", i, &newmmsid))
    {
      Logger::error("copying sms._id: ", i);
      d_database.exec("ROLLBACK TRANSACTION");
      return false;
    }

    if (newmmsid.rows())
    {
      long long int newestmmsid = newmmsid.getValueAs<long long int>(0, "_id");

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
