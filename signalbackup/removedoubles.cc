/*
    Copyright (C) 2019  Selwin van Dijk

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
  d_database.exec("DELETE FROM sms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1.date = t2.date AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1.address = t2.address AND t1.read = t2.read AND t1.type = t2.type AND (t1.protocol = t2.protocol OR (t1.protocol IS NULL AND t2.protocol IS NULL)) AND t1.date_sent = t2.date_sent AND t1._id <> t2._id) AS doubles ORDER BY date ASC, date_sent ASC, body ASC, thread_id ASC, address ASC, read ASC, type ASC, protocol ASC,_id ASC) t WHERE RNum%2 = 0)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;

  std::cout << "  Removing duplicate entries from sms table..." << std::endl;
  d_database.exec("DELETE FROM mms WHERE _id IN (SELECT _id FROM (SELECT ROW_NUMBER() OVER () RNum,* FROM (SELECT DISTINCT t1.* FROM mms AS t1 INNER JOIN mms AS t2 ON t1.date = t2.date AND (t1.body = t2.body OR (t1.body IS NULL AND t2.body IS NULL)) AND t1.thread_id = t2.thread_id AND t1.address = t2.address AND t1.read = t2.read AND t1.msg_box = t2.msg_box AND t1.date_received = t2.date_received AND t1._id <> t2._id) AS doubles ORDER BY date ASC, date_received ASC, body ASC, thread_id ASC, address ASC, read ASC, msg_box ASC, _id ASC) t WHERE RNum%2 = 0)");
  std::cout << "  Removed " << d_database.changed() << " entries." << std::endl;

  cleanDatabaseByMessages();
}
