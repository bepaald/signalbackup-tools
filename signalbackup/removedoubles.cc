/*
  Copyright (C) 2019-2022  Selwin van Dijk

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
  std::cout << __FUNCTION__  << std::endl;

  std::cout << "  Removing duplicate entries from sms table..." << std::endl;

  if (d_database.tableContainsColumn("sms", "protocol")) // removed in dbv166
    d_database.exec("DELETE FROM sms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1." + d_sms_date_received + " = t2." + d_sms_date_received + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_sms_recipient_id +" = t2." + d_sms_recipient_id +" AND t1.read = t2.read AND t1.type = t2.type AND (t1.protocol = t2.protocol OR (t1.protocol IS NULL AND t2.protocol IS NULL)) AND t1.date_sent = t2.date_sent AND t1._id <> t2._id) AS doubles ORDER BY " + d_sms_date_received + " ASC, date_sent ASC, body ASC, thread_id ASC, " + d_sms_recipient_id + " ASC, read ASC, type ASC, protocol ASC,_id ASC) t WHERE RNum%2 = 0)");
  else
    d_database.exec("DELETE FROM sms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1." + d_sms_date_received + " = t2." + d_sms_date_received + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_sms_recipient_id +" = t2." + d_sms_recipient_id +" AND t1.read = t2.read AND t1.type = t2.type AND t1.date_sent = t2.date_sent AND t1._id <> t2._id) AS doubles ORDER BY " + d_sms_date_received + " ASC, date_sent ASC, body ASC, thread_id ASC, " + d_sms_recipient_id + " ASC, read ASC, type ASC, _id ASC) t WHERE RNum%2 = 0)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;

  std::cout << "  Removing duplicate entries from mms table..." << std::endl;
  d_database.exec("DELETE FROM mms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM mms AS t1 INNER JOIN mms AS t2 ON t1." + d_mms_date_sent + " = t2." + d_mms_date_sent + " AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1." + d_mms_recipient_id + " = t2." +d_mms_recipient_id + " AND t1.read = t2.read AND t1." + d_mms_type + " = t2." + d_mms_type + " AND t1.date_received = t2.date_received AND t1._id <> t2._id) AS doubles ORDER BY " + d_mms_date_sent +" ASC, date_received ASC, body ASC, thread_id ASC, " + d_mms_recipient_id + " ASC, read ASC, " + d_mms_type + " ASC, _id ASC) t WHERE RNum%2 = 0)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;


  //cleanDatabaseByMessages();
  // cleandatabasebymessages is a complicated (risky?) function because it messes with recipients.
  // theoretically, removing doubled messages should not affect the recipient table
  // here we duplicate the relevant parts of cleanDatabaseByMessages()

  // remove doubled parts
  std::cout << "  Deleting attachment entries from 'part' not belonging to remaining mms entries" << std::endl;
  d_database.exec("DELETE FROM part WHERE mid NOT IN (SELECT DISTINCT _id FROM mms)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;

  // remove unused attachments
  std::cout << "  Deleting unused attachments..." << std::endl;
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
  std::cout << "  Deleting group receipts entries from deleted messages..." << std::endl;
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM mms)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;

  // remove unused mentions
  if (d_database.containsTable("mention"))
  {
    std::cout << "  Deleting entries from 'mention' not belonging to remaining mms entries" << std::endl;
    d_database.exec("DELETE FROM mention WHERE message_id NOT IN (SELECT DISTINCT _id FROM mms)");
  }
}
