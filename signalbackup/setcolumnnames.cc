/*
  Copyright (C) 2021-2022  Selwin van Dijk

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
  // started at dbv166
  d_thread_recipient_id = "recipient_id";
  // from dbv 108
  if (!d_database.tableContainsColumn("thread", "recipient_id") &&
      d_database.tableContainsColumn("thread", "thread_recipient_id"))
    d_thread_recipient_id = "thread_recipient_ids";
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
    d_thread_recipient_id = "message_count";





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
    d_sms_date_received = "address";

  // started at dbv166
  d_sms_recipient_device_id = "recipient_device_id";
  // before 166
  if (!d_database.tableContainsColumn("sms", "recipient_device_id") &&
      d_database.tableContainsColumn("sms", "address_device_id"))
    d_sms_date_received = "address_device_id";





  // started at dbv166
  d_mms_date_sent = "date_sent";
  // before 166
  if (!d_database.tableContainsColumn("mms", "date_sent") &&
      d_database.tableContainsColumn("mms", "date"))
    d_mms_date_sent = "date";

  // started at dbv166
  d_mms_recipient_id = "recipient_id";
  // before 166
  if (!d_database.tableContainsColumn("mms", "recipient_id") &&
      d_database.tableContainsColumn("mms", "address"))
    d_mms_recipient_id = "address";

  // started at dbv166
  d_mms_recipient_device_id = "recipient_device_id";
  // before 166
  if (!d_database.tableContainsColumn("mms", "recipient_device_id") &&
      d_database.tableContainsColumn("mms", "address_device_id"))
    d_mms_recipient_device_id = "address_device_id";

  // started at dbv166
  d_mms_type = "type";
  // before 166
  if (!d_database.tableContainsColumn("mms", "type") &&
      d_database.tableContainsColumn("mms", "msg_box"))
    d_mms_recipient_device_id = "msg_box";

  return true;
}
