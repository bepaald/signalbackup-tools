/*
  Copyright (C) 2021-2024  Selwin van Dijk

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
  // started at dbv 215
  d_part_table = "attachment";
  if (d_database.containsTable("part") &&
      !d_database.containsTable("attachment"))
    d_part_table = "part";


  // started at dbv 174
  d_mms_table = "message";
  if (d_database.containsTable("mms") &&
      !d_database.containsTable("message"))
    d_mms_table = "mms";


  d_recipient_aci = "aci";
  if (d_database.tableContainsColumn("recipient", "uuid")) // before dbv200
    d_recipient_aci = "uuid";

  d_recipient_e164 = "e164";
  if (d_database.tableContainsColumn("recipient", "phone")) // before dbv201
    d_recipient_e164 = "phone";

  d_recipient_avatar_color = "avatar_color";
  if (d_database.tableContainsColumn("recipient", "color")) // before dbv201
    d_recipient_avatar_color = "color";

  d_recipient_system_joined_name = "system_joined_name";
  if (d_database.tableContainsColumn("recipient", "system_display_name")) // before dbv201
    d_recipient_system_joined_name = "system_display_name";

  d_recipient_profile_given_name = "profile_given_name";
  if (d_database.tableContainsColumn("recipient", "signal_profile_name")) // before dbv201
    d_recipient_profile_given_name = "signal_profile_name";

  d_recipient_storage_service = "storage_service_id";
  if (!d_database.tableContainsColumn("recipient", "storage_service_id") &&
      d_database.tableContainsColumn("recipient", "storage_service_key"))
    d_recipient_storage_service = "storage_service_key";

  d_recipient_type = "type";
  if (!d_database.tableContainsColumn("recipient", "type") &&    // before dbv201
      d_database.tableContainsColumn("recipient", "group_type"))
    d_recipient_type = "group_type";

  d_recipient_profile_avatar = "profile_avatar";
  if (!d_database.tableContainsColumn("recipient", "profile_avatar") &&    // before dbv201
      d_database.tableContainsColumn("recipient", "signal_profile_avatar"))
    d_recipient_profile_avatar = "signal_profile_avatar";



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

  // from dbv211
  d_thread_delivery_receipts = "has_delivery_receipt";
  // before 211
  if (!d_database.tableContainsColumn("thread", "has_delivery_receipt") &&
      d_database.tableContainsColumn("thread", "delivery_receipt_count"))
    d_thread_delivery_receipts = "delivery_receipt_count";

  // from dbv211
  d_thread_read_receipts = "has_read_receipt";
  // before 211
  if (!d_database.tableContainsColumn("thread", "has_read_receipt") &&
      d_database.tableContainsColumn("thread", "read_receipt_count"))
    d_thread_read_receipts = "read_receipt_count";



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





  // from dbv211
  d_mms_delivery_receipts = "has_delivery_receipt";
  // before 211
  if (!d_database.tableContainsColumn(d_mms_table, "has_delivery_receipt") &&
      d_database.tableContainsColumn(d_mms_table, "delivery_receipt_count"))
    d_mms_delivery_receipts = "delivery_receipt_count";

  // from dbv211
  d_mms_read_receipts = "has_read_receipt";
  // before 211
  if (!d_database.tableContainsColumn(d_mms_table, "has_read_receipt") &&
      d_database.tableContainsColumn(d_mms_table, "read_receipt_count"))
    d_mms_read_receipts = "read_receipt_count";

  // from dbv211
  d_mms_viewed_receipts = "viewed";
  // before 211
  if (!d_database.tableContainsColumn(d_mms_table, "viewed") &&
      d_database.tableContainsColumn(d_mms_table, "viewed_receipt_count"))
    d_mms_viewed_receipts = "viewed_receipt_count";

  // started at dbv166
  d_mms_date_sent = "date_sent";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "date_sent") &&
      d_database.tableContainsColumn(d_mms_table, "date"))
    d_mms_date_sent = "date";

  // started at dbv166
  d_mms_ranges = "message_ranges";
  // before 166
  if (!d_database.tableContainsColumn(d_mms_table, "message_ranges") &&
      d_database.tableContainsColumn(d_mms_table, "ranges"))
    d_mms_ranges = "ranges";

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
    d_mms_recipient_device_id = "recipient_device_id";
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



  d_groups_v1_members = "unmigrated_v1_members";
  if (!d_database.tableContainsColumn("groups", "unmigrated_v1_members") &&
      d_database.tableContainsColumn("groups", "former_v1_members"))
    d_groups_v1_members = "former_v1_members";



  d_part_mid = "message_id";  // dbv 215
  if (!d_database.tableContainsColumn(d_part_table, "message_id") &&
      d_database.tableContainsColumn(d_part_table, "mid"))
    d_part_mid = "mid";

  d_part_ct = "content_type"; // dbv 215
  if (!d_database.tableContainsColumn(d_part_table, "content_type") &&
      d_database.tableContainsColumn(d_part_table, "ct"))
    d_part_ct = "ct";

  d_part_pending = "transfer_state"; // dbv 215
  if (!d_database.tableContainsColumn(d_part_table, "transfer_state") &&
      d_database.tableContainsColumn(d_part_table, "pending_push"))
    d_part_pending = "pending_push";

  d_part_cd = "remote_key"; // dbv 215
  if (!d_database.tableContainsColumn(d_part_table, "remote_key") &&
      d_database.tableContainsColumn(d_part_table, "cd"))
    d_part_cd = "cd";

  d_part_cl = "remote_location"; // dbv 215
  if (!d_database.tableContainsColumn(d_part_table, "remote_location") &&
      d_database.tableContainsColumn(d_part_table, "cl"))
    d_part_cl = "cl";

  return true;
}
