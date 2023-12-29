/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

void SignalBackup::removeDoubles()
{
  Logger::message(__FUNCTION__ );

  if (d_database.containsTable("sms"))
  {
    Logger::message("  Removing duplicate entries from sms table...");

    // if (d_database.tableContainsColumn("sms", "protocol")) // removed in dbv166
    //   d_database.exec("DELETE FROM sms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1." + d_sms_date_received + " = t2." + d_sms_date_received + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_sms_recipient_id +" = t2." + d_sms_recipient_id +" AND t1.read = t2.read AND t1.type = t2.type AND (t1.protocol = t2.protocol OR (t1.protocol IS NULL AND t2.protocol IS NULL)) AND t1.date_sent = t2.date_sent AND t1._id <> t2._id) AS doubles ORDER BY " + d_sms_date_received + " ASC, date_sent ASC, body ASC, thread_id ASC, " + d_sms_recipient_id + " ASC, read ASC, type ASC, protocol ASC,_id ASC) t WHERE RNum%2 = 0)");
    // else
    //   d_database.exec("DELETE FROM sms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1." + d_sms_date_received + " = t2." + d_sms_date_received + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_sms_recipient_id +" = t2." + d_sms_recipient_id +" AND t1.read = t2.read AND t1.type = t2.type AND t1.date_sent = t2.date_sent AND t1._id <> t2._id) AS doubles ORDER BY " + d_sms_date_received + " ASC, date_sent ASC, body ASC, thread_id ASC, " + d_sms_recipient_id + " ASC, read ASC, type ASC, _id ASC) t WHERE RNum%2 = 0)");

    d_database.exec("DELETE FROM sms WHERE _id IN "
                    "(SELECT _id FROM sms GROUP BY " + d_sms_date_received + ", body, thread_id, CAST(" + d_sms_recipient_id + " AS STRING), read, type, " +
                    (d_database.tableContainsColumn("sms", "protocol") ? "protocol, " : "") + "date_sent HAVING COUNT(*) IS 2)");

    Logger::message("  Removed ", d_database.changed(), " entries.");
  }

  Logger::message("  Removing duplicate entries from mms table...");
  //d_database.exec("DELETE FROM mms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM mms AS t1 INNER JOIN mms AS t2 ON t1." + d_mms_date_sent + " = t2." + d_mms_date_sent + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_mms_recipient_id + " = t2." +d_mms_recipient_id + " AND t1.read = t2.read AND t1." + d_mms_type + " = t2." + d_mms_type + " AND t1.date_received = t2.date_received AND t1._id <> t2._id) AS doubles ORDER BY " + d_mms_date_sent +" ASC, date_received ASC, body ASC, thread_id ASC, " + d_mms_recipient_id + " ASC, read ASC, " + d_mms_type + " ASC, _id ASC) t WHERE RNum%2 = 0)");

  if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
  {
    d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id IN "
                    "(SELECT _id FROM " + d_mms_table + " GROUP BY body, " + d_mms_date_sent + ", thread_id, CAST(" + d_mms_recipient_id + " AS STRING), read, " + d_mms_type + ", date_received HAVING COUNT(*) IS 2)");
    Logger::message("  Removed ", d_database.changed(), " entries.");
  }
  else
  {
    d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id IN "
                    "(SELECT _id FROM " + d_mms_table + " GROUP BY body, " + d_mms_date_sent + ", thread_id, "
                    "CAST(from_recipient_id AS STRING), CAST(to_recipient_id AS STRING), read, " + d_mms_type + ", date_received HAVING COUNT(*) IS 2)");
    Logger::message("  Removed ", d_database.changed(), " entries.");
  }


  //cleanDatabaseByMessages();
  // cleandatabasebymessages is a complicated (risky?) function because it messes with recipients.
  // theoretically, removing doubled messages should not affect the recipient table
  // here we duplicate the relevant parts of cleanDatabaseByMessages()

  // remove doubled parts
  Logger::message("  Deleting attachment entries from 'part' not belonging to remaining mms entries");
  d_database.exec("DELETE FROM part WHERE mid NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
  Logger::message("  Removed ", d_database.changed(), " entries.");

  // remove unused attachments
  Logger::message("  Deleting unused attachments...");
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id,unique_id FROM part", &results);
  for (auto it = d_attachments.begin(); it != d_attachments.end();)
  {
    bool found = false;
    for (uint i = 0; i < results.rows(); ++i)
    {
      long long int rowid = -1;
      if (results.valueHasType<long long int>(i, "_id"))
        rowid = results.getValueAs<long long int>(i, "_id");
      long long int uniqueid = -1;
      if (results.valueHasType<long long int>(i, "unique_id"))
        uniqueid = results.getValueAs<long long int>(i, "unique_id");

      if (rowid != -1 && uniqueid != -1 &&
          it->first.first == static_cast<uint64_t>(rowid) && it->first.second == static_cast<uint64_t>(uniqueid))
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
  Logger::message("  Removed ", d_database.changed(), " entries.");

  // remove unused mentions
  if (d_database.containsTable("mention"))
  {
    Logger::message("  Deleting entries from 'mention' not belonging to remaining mms entries");
    d_database.exec("DELETE FROM mention WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
  }

  // remove unused call details
  if (d_database.containsTable("call"))
  {
    Logger::message("  Deleting entries from 'call' not belonging to remaining message entries");
    d_database.exec("DELETE FROM call WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");
  }

  // remove unreferencing reactions
  if (d_database.containsTable("reaction"))
  {
    if (d_database.containsTable("sms"))
    {
      Logger::message("  Deleting entries from 'reaction' not belonging to remaining sms entries");
      d_database.exec("DELETE FROM reaction WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT DISTINCT _id FROM sms)");
    }
    Logger::message("  Deleting entries from 'reaction' not belonging to remaining mms entries");
    d_database.exec("DELETE FROM reaction WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")" +
                    (d_database.tableContainsColumn("reaction", "is_mms") ? " AND is_mms IS 1" : ""));
  }

  // remove msl_message
  if (d_database.containsTable("msl_message"))
  {
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
}
