/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SignalBackup::removeDoubles(long long int milliseconds)
{
  Logger::message(__FUNCTION__ );

  SqliteDB::QueryResults threads;
  if (!d_database.exec("SELECT _id FROM thread ORDER BY _id ASC", &threads))
    return;

  // note: doing this per thread is not needed, but this gives some sense of progress as
  // the query is slow as ....
  for (unsigned int i = 0; i < threads.rows(); ++i)
  {
    long long int tid = threads.valueAsInt(i, "_id");

    /*
    // show duplicates:
    d_database.printLineMode("SELECT _id FROM " + d_mms_table + " M WHERE "
                             "  _id > "
                             "  (SELECT min(_id) FROM " + d_mms_table + " WHERE "
                             "      " + d_mms_recipient_id + " = M." + d_mms_recipient_id + " AND "
                             "      thread_id = ? AND "
                             "      COALESCE(body, '') = COALESCE(M.body, '') AND "
                             "      " + d_mms_type + " = M." + d_mms_type + " AND "
                             "      ABS(date_sent - M.date_sent) <= ?) AND thread_id = ?", {tid, milliseconds, tid});
    */

    // delete duplicates
    if (!d_database.exec("WITH to_delete AS "
                         "("
                         "  SELECT _id FROM " + d_mms_table + " M WHERE "
                         "  _id > "
                         "  ("
                         "    SELECT min(_id) FROM " + d_mms_table + " WHERE "
                         "      " + d_mms_recipient_id + " = M." + d_mms_recipient_id + " AND "
                         "      thread_id = ? AND "
                         "      COALESCE(body, '') = COALESCE(M.body, '') AND "
                         "      " + d_mms_type + " = M." + d_mms_type + " AND "
                         "      ABS(date_sent - M.date_sent) <= ?"
                         "  ) AND "
                         "  thread_id = ?"
                         ") "
                         "DELETE FROM " + d_mms_table + " WHERE _id IN to_delete", {tid, milliseconds, tid}))
      return;
    Logger::message("Deleted ", d_database.changed(), " duplicate entries from thread ", tid, " (", i + 1, "/", threads.rows(), ")");
  }

  //cleanDatabaseByMessages();
  // cleandatabasebymessages is a complicated (risky?) function because it messes with recipients.
  // theoretically, removing doubled messages should not affect the recipient table
  // here we duplicate the relevant parts of cleanDatabaseByMessages()

  // remove dangling parts
  Logger::message("  Deleting attachment entries from '", d_part_table, "' not belonging to remaining mms entries");
  d_database.exec("DELETE FROM " + d_part_table + " WHERE " + d_part_mid + " NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
  Logger::message("  Removed ", d_database.changed(), " ", d_part_table, "-entries.");

  // remove unused attachments
  Logger::message("  Deleting unused attachments...");
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id," +
                  (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) +
                  " FROM " + d_part_table, &results);
  int constexpr INVALID_ID = -10;
  for (auto it = d_attachments.begin(); it != d_attachments.end();)
  {
    bool found = false;
    for (unsigned int i = 0; i < results.rows(); ++i)
    {
      long long int rowid = INVALID_ID;
      if (results.valueHasType<long long int>(i, "_id"))
        rowid = results.getValueAs<long long int>(i, "_id");
      long long int uniqueid = INVALID_ID;
      if (results.valueHasType<long long int>(i, "unique_id"))
        uniqueid = results.getValueAs<long long int>(i, "unique_id");

      if (rowid != INVALID_ID && uniqueid != INVALID_ID &&
          it->first.first == static_cast<uint64_t>(rowid) && it->first.second == static_cast<int64_t>(uniqueid))
      {
        found = true;
        break;
      }
    }
    if (!found)
      it = d_attachments.erase(it);
    else
      ++it;
  }

  // remove unused group_receipts
  Logger::message("  Deleting group receipts entries from deleted messages...");
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
  Logger::message("  Removed ", d_database.changed(), " group_receipts-entries.");

  // remove unused mentions
  if (d_database.containsTable("mention"))
  {
    Logger::message_start("  Deleting entries from 'mention' not belonging to remaining mms entries");
    d_database.exec("DELETE FROM mention WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
    Logger::message_end(" (", d_database.changed(), ")");
  }

  // remove unused call details
  if (d_database.containsTable("call"))
  {
    Logger::message_start("  Deleting entries from 'call' not belonging to remaining message entries");
    d_database.exec("DELETE FROM call WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
    Logger::message_end(" (", d_database.changed(), ")");
  }

  // remove unreferencing reactions
  if (d_database.containsTable("reaction"))
  {
    if (d_database.containsTable("sms"))
    {
      Logger::message_start("  Deleting entries from 'reaction' not belonging to remaining sms entries");
      d_database.exec("DELETE FROM reaction WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT DISTINCT _id FROM sms)");
      Logger::message_end(" (", d_database.changed(), ")");
    }
    Logger::message_start("  Deleting entries from 'reaction' not belonging to remaining mms entries");
    d_database.exec("DELETE FROM reaction WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")" +
                    (d_database.tableContainsColumn("reaction", "is_mms") ? " AND is_mms IS 1" : ""));
    Logger::message_end(" (", d_database.changed(), ")");
  }

  // remove msl_message
  if (d_database.containsTable("msl_message"))
  {
    // note this function is generally called because messages (and/or attachments) have been deleted
    // the msl_payload table has triggers that delete its entries:
    // (delete from msl_payload where _id in (select payload_id from message where message_id = (message.deleted_id/part.deletedmid)))
    // apparently these triggers even when editing within this program, even though the 'ON DELETE CASCADE' stuff does not and
    // foreign key constraints are not enforced... This causes a sort of circular thing here but I think we can just clean up the
    // msl_message table according to still-existing msl_payloads first
    d_database.exec("DELETE FROM msl_message WHERE payload_id NOT IN (SELECT DISTINCT _id FROM msl_payload)");

    Logger::message("  Deleting entries from 'msl_message' not belonging to remaining messages");
    if (d_database.containsTable("sms"))
    {
      d_database.exec("DELETE FROM msl_message WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT DISTINCT _id FROM sms)");
      d_database.exec("DELETE FROM msl_message WHERE is_mms IS 1 AND message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
    }
    else
      d_database.exec("DELETE FROM msl_message WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");

    // now delete msl_payloads from non-existing msl_messages ?
    d_database.exec("DELETE FROM msl_payload WHERE _id NOT IN (SELECT DISTINCT payload_id FROM msl_message)");

    // now delete msl_recipients from non existing payloads?
    d_database.exec("DELETE FROM msl_recipient WHERE payload_id NOT IN (SELECT DISTINCT _id FROM msl_payload)");
  }

  if (d_database.containsTable("story_sends"))
  {
    Logger::message_start("  Deleting entries from 'story_sends' not belonging to remaining message entries");
    d_database.exec("DELETE FROM story_sends WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
    Logger::message_end(" (", d_database.changed(), ")");
  }

}
