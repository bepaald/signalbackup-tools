/*
  Copyright (C) 2019-2025  Selwin van Dijk

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
  Logger::message(__FUNCTION__);

  //auto t1 = std::chrono::high_resolution_clock::now();
  SqliteDB::QueryResults threads;
  if (!d_database.exec("SELECT _id FROM thread ORDER BY _id ASC", &threads))
    return;

  // note: doing this per thread is not needed, but this gives some sense
  // of progress as the query can be quite slow
  long long int removed_total = 0;
  for (unsigned int i = 0; i < threads.rows(); ++i)
  {
    long long int tid = threads.valueAsInt(i, "_id");
    long long int removed_this_tread = 0;

    //SqliteDB::QueryResults todelete;
    if (d_database.containsTable("sms"))
    {
      if (!d_database.exec("WITH messages_with_attachmentsize AS "
                           "("
                           "  SELECT sms._id, " + d_sms_recipient_id + ", thread_id, date_sent, type, body FROM sms"
                           "    WHERE thread_id = ?"
                           "    GROUP BY sms._id, " + d_sms_recipient_id + ", thread_id, date_sent, type, body"
                           "), "
                           "candidates AS "
                           "("
                           "  SELECT M._id, M." + d_sms_recipient_id + ", M.thread_id, M.date_sent, M.type, M.body FROM"
                           "  ("
                           "    SELECT _id, " + d_sms_recipient_id + ", thread_id, date_sent, type, body FROM messages_with_attachmentsize"
                           "    GROUP BY " + d_sms_recipient_id + ", thread_id, type, COALESCE(body, '') "
                           "    HAVING COUNT(*) > 1"
                           "  ) AS D"
                           "  JOIN messages_with_attachmentsize AS M ON"
                           "    COALESCE(M.body, '') = COALESCE(D.body, '') AND"
                           "    M." + d_sms_recipient_id + " = D." + d_sms_recipient_id + " AND"
                           "    M.type = D.type"
                           "  ORDER BY M._id"
                           "),"
                           "to_delete AS "
                           "("
                           "  SELECT _id FROM candidates C WHERE "
                           "  _id > "
                           "  ("
                           "    SELECT min(_id) FROM candidates WHERE "
                           "      COALESCE(body, '') = COALESCE(C.body, '') AND "
                           "      " + d_sms_recipient_id + " = C." + d_sms_recipient_id + " AND "
                           "      type = C.type AND"
                           "      ABS(date_sent - C.date_sent) <= ?"
                           "  )"
                           ") "
                           //" SELECT _id FROM to_delete", {tid, milliseconds}, &todelete))
                           "DELETE FROM sms WHERE _id IN to_delete", {tid, milliseconds}))
      {
        Logger::error("Failed to delete doubles from table 'sms'");
        return;
      }
      removed_this_tread += d_database.changed();
      removed_total += removed_this_tread;
    }

    //SqliteDB::QueryResults todelete;
    if (!d_database.exec("WITH messages_with_attachmentsize AS "
                         "("
                         "  SELECT " + d_mms_table + "._id, " + d_mms_recipient_id + ", thread_id, " + d_mms_date_sent + ", " + d_mms_type + ", body, IFNULL(COUNT(data_size), 0) AS numattachments, IFNULL(SUM(data_size), 0) AS totalfilesize FROM " + d_mms_table +
                         "    LEFT JOIN " + d_part_table + " ON " + d_part_mid + " IS " + d_mms_table + "._id"
                         "    WHERE thread_id = ?"
                         "    GROUP BY " + d_mms_table + "._id, " + d_mms_recipient_id + ", thread_id, " + d_mms_date_sent + ", " + d_mms_type + ", body"
                         "), "
                         "candidates AS "
                         "("
                         "  SELECT M._id, M." + d_mms_recipient_id + ", M.thread_id, M." + d_mms_date_sent + ", M." + d_mms_type + ", M.body, M.numattachments, M.totalfilesize FROM"
                         "  ("
                         "    SELECT _id, " + d_mms_recipient_id + ", thread_id, " + d_mms_date_sent + ", " + d_mms_type + ", body, numattachments, totalfilesize FROM messages_with_attachmentsize"
                         "    GROUP BY " + d_mms_recipient_id + ", thread_id, " + d_mms_type + ", COALESCE(body, ''), numattachments, totalfilesize"
                         "    HAVING COUNT(*) > 1"
                         "  ) AS D"
                         "  JOIN messages_with_attachmentsize AS M ON"
                         "    COALESCE(M.body, '') = COALESCE(D.body, '') AND"
                         "    M.numattachments = D.numattachments AND"
                         "    M.totalfilesize = D.totalfilesize AND"
                         "    M." + d_mms_recipient_id + " = D." + d_mms_recipient_id + " AND"
                         "    M." + d_mms_type + " = D." + d_mms_type +
                         "  ORDER BY M._id"
                         "),"
                         "to_delete AS "
                         "("
                         "  SELECT _id FROM candidates C WHERE "
                         "  _id < "
                         "  ("
                         "    SELECT max(_id) FROM candidates WHERE "
                         "      COALESCE(body, '') = COALESCE(C.body, '') AND "
                         "      numattachments = C.numattachments AND"
                         "      totalfilesize = C.totalfilesize AND"
                         "      " + d_mms_recipient_id + " = C." + d_mms_recipient_id + " AND "
                         "      " + d_mms_type + " = C." + d_mms_type + " AND"
                         "      ABS(" + d_mms_date_sent + " - C." + d_mms_date_sent + ") <= ?"
                         "  )"
                         ") "
                         //" SELECT _id FROM to_delete", {tid, milliseconds}, &todelete))
                         "DELETE FROM " + d_mms_table + " WHERE _id IN to_delete", {tid, milliseconds}))
    {
      Logger::error("Failed to delete doubles from table '", d_mms_table, "'");
      return;
    }
    removed_this_tread += d_database.changed();
    removed_total += removed_this_tread;

    Logger::message("Deleted ", (removed_this_tread ? Logger::Control::BOLD : Logger::Control::NORMAL), removed_this_tread, Logger::Control::NORMAL, " duplicate entries from thread ", tid, " (", i + 1, "/", threads.rows(), ")");
    //todelete.prettyPrint(false);
  }
  Logger::message("Removed ", (removed_total ? Logger::Control::BOLD : Logger::Control::NORMAL), removed_total, Logger::Control::NORMAL, " doubled messages from database");
  // auto t2 = std::chrono::high_resolution_clock::now();
  // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  // std::cout << " *** TIME: " << ms_int.count() << "ms\n";


  // It is possible one of a doubled pair of message has edits, and the
  // other does not (for example when the doubled messages are imported
  // from Signal Desktop, which currently does not import edits).
  // After this function removes a random one, it is possible there is now
  // a dangling 'latest_revision_id' pointers in the database.
  if (d_database.tableContainsColumn(d_mms_table, "latest_revision_id"))
  {
    // this should be rare, as the choice of which double to remove
    // (id < max instead of id > min), should usually delete the
    // imported desktop message
    d_database.exec("DELETE FROM " + d_mms_table + " WHERE latest_revision_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
    Logger::message("  Removed ", d_database.changed(), " dangling latest_revision_id pointers.");
  }

  // Similarly, in one known case in my own backups, an edit never arrived
  // at the Desktop instance. This causes the *original version* of the
  // message to be doubled.
  // When this function then removes the wrong double, the edited version
  // (which only existed in one of the sources), will have a 'original_message_id'
  // pointing to a non-existing message.
  if (d_database.tableContainsColumn(d_mms_table, "original_message_id"))
  {
    // not sure if I should just delete this message, or if I should clear the
    // 'original_message_id' value.
    // Clearing for now. This means the edited version appears as a separate
    // message (as it does in one of the sources), but at least the latest
    // revision of the message is not lost. Likely the newer revision appears
    // _before_ the old version, which is unfortunate.
    // This should be a rare occurrence however, since it depends on the
    // sources which caused the doubling disagree about whether the message
    // was edited at all. And (at least in case of importfromdesktop()), the
    // choice of which double to remove (id < max instead of id > min), should
    // usually delete the desktop message, which is more likely to be the
    // incorrect one.
    d_database.exec("UPDATE " + d_mms_table + " SET original_message_id = NULL WHERE original_message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
    Logger::message("  Cleared ", d_database.changed(), " dangling original_message_id pointers.");
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
