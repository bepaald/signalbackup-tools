/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

bool SignalBackup::setColumnNames()
{
  // started at dbv 174
  d_mms_table = "message";
  if (d_database.containsTable("mms") &&
      !d_database.containsTable("message"))
    d_mms_table = "mms";

  // started at dbv166
  d_thread_recipient_id = "recipient_id";
  // from dbv 108
  if (!d_database.tableContainsColumn("thread", "recipient_id") &&
      d_database.tableContainsColumn("thread", "thread_recipient_id"))
    d_thread_recipient_id = "thread_recipient_id";
  //earliest
  if (!d_database.tableContainsColumn("thread", "recipient_id") &&
      !d_database.tableContainsColumn("thread", "thread_recipient_id") &&
      d_database.tableContainsColumn("thread", "recipient_ids")) // before dbv108
    d_thread_recipient_id = "recipient_ids";

  // started at dbv166
  d_thread_message_count = "meaningful_messages";
  // before 166
  if (!d_database.tableContainsColumn("thread", "meaningful_messages") &&
      d_database.tableContainsColumn("thread", "message_count"))
    d_thread_message_count = "message_count";





  // started at dbv166
  d_sms_date_received = "date_received";
  // before 166
  if (!d_database.tableContainsColumn("sms", "date_received") &&
      d_database.tableContainsColumn("sms", "date"))
    d_sms_date_received = "date";

  // started at dbv166
  d_sms_recipient_id = "recipient_id";
  // before 166
  if (!d_database.tableContainsColumn("sms", "recipient_id") &&
      d_database.tableContainsColumn("sms", "address"))
    d_sms_recipient_id = "address";

  // started at dbv166
  d_sms_recipient_device_id = "recipient_device_id";
  // before 166
  if (!d_database.tableContainsColumn("sms", "recipient_device_id") &&
      d_database.tableContainsColumn("sms", "address_device_id"))
    d_sms_recipient_device_id = "address_device_id";





  // started at dbv166
  d_mms_date_sent = "date_sent";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "date_sent") &&
      d_database.tableContainsColumn(d_mms_table, "date"))
    d_mms_date_sent = "date";

  // started at dbv185
  d_mms_recipient_id = "from_recipient_id";
  // before 185
  if (!d_database.tableContainsColumn(d_mms_table, "from_recipient_id") &&
      d_database.tableContainsColumn(d_mms_table, "recipient_id"))
    d_mms_recipient_id = "recipient_id";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "recipient_id") &&
      d_database.tableContainsColumn(d_mms_table, "address"))
    d_mms_recipient_id = "address";

  // started at dbv185
  d_mms_recipient_device_id = "from_device_id";
  // before 185
  if (!d_database.tableContainsColumn(d_mms_table, "from_device_id") &&
      d_database.tableContainsColumn(d_mms_table, "recipient_device_id"))
    d_mms_recipient_id = "recipient_device_id";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "recipient_device_id") &&
      d_database.tableContainsColumn(d_mms_table, "address_device_id"))
    d_mms_recipient_device_id = "address_device_id";

  // started at dbv166
  d_mms_type = "type";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "type") &&
      d_database.tableContainsColumn(d_mms_table, "msg_box"))
    d_mms_type = "msg_box";

  // started at dbv166
  d_mms_previews = "link_previews";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "link_previews") &&
      d_database.tableContainsColumn(d_mms_table, "previews"))
    d_mms_previews = "previews";

  return true;
}
