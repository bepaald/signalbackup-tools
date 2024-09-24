/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

void SignalBackup::dtSetMessageDeliveryReceipts(SqliteDB const &ddb, long long int rowid, std::map<std::string, long long int> *savedmap,
                                                std::string const &databasedir, bool createcontacts, long long int msg_id, bool is_mms,
                                                bool isgroup, bool *warn)
{
  //  public static final int STATUS_UNKNOWN     = -1;
  //  public static final int STATUS_UNDELIVERED = 0;
  //  public static final int STATUS_DELIVERED   = 1;
  //  public static final int STATUS_READ        = 2;
  //  public static final int STATUS_VIEWED      = 3;
  //  public static final int STATUS_SKIPPED     = 4;
  long long int constexpr STATUS_DELIVERED = 1;
  long long int constexpr STATUS_READ = 2;

  SqliteDB::QueryResults status_results;
  if (!ddb.exec("SELECT "
                "delivery_details.key AS conv_id,"
                "conversations." + d_dt_c_uuid + " AS uuid,"
                "conversations.e164 AS e164,"
                "json_extract(delivery_details.value, '$.status') AS status,"
                "COALESCE(json_extract(delivery_details.value, '$.updatedAt'), delivery_details.sent_at) AS updated_timestamp"
                " FROM "
                "(SELECT sent_at,key,value FROM messages,json_each(messages.json, '$.sendStateByConversationId') WHERE rowid IS ?) AS delivery_details"
                " LEFT JOIN conversations ON conversations.id IS conv_id", rowid, &status_results))
  {
    Logger::error("Getting message delivery status");
    return;
  }

  // results:
  //              key = 37fb0475-13e7-43e6-965a-4b11d9370488
  //           status = Sent
  //updated_timestamp = 1668710610793
  //
  //              key = d1c1693d-91e1-486f-8634-29c22afb881b
  //           status = Delivered
  //updated_timestamp = 1668710612716

  //status_results.prettyPrint();

  long long int deliveryreceiptcount = 0;
  long long int readreceiptcount = 0;
  long long int updatedtimestamp = -1;
  for (unsigned int i = 0; i < status_results.rows(); ++i)
  {
    if (status_results.valueAsString(i, "status") == "Delivered")
    {
      ++deliveryreceiptcount;
      if (updatedtimestamp == -1)
        updatedtimestamp = status_results.valueAsInt(i, "updated_timestamp", -1);

      if (isgroup && !status_results.isNull(i, "updated_timestamp")) // add per-group-member details to cdelivery_receipts table
      {
        long long int member_id = getRecipientIdFromUuidMapped(status_results.valueAsString(i, "uuid"), savedmap, createcontacts);
        if (member_id == -1) // try phone
          member_id = getRecipientIdFromPhoneMapped(status_results.valueAsString(i, "e164"), savedmap, createcontacts);
        if (member_id == -1)
        {
          if (createcontacts)
          {
            if ((member_id = dtCreateRecipient(ddb, status_results.valueAsString(i, "uuid"), std::string(), std::string(), databasedir, savedmap, warn)) == -1)
            {
              Logger::error("Failed to create delivery_receipt member. Skipping");
              continue;
            }
          }
          else
          {
            Logger::error("Failed to get id of delivery_receipt member. Skipping");
            continue;
          }
        }
        if (!insertRow("group_receipts", {{"mms_id", msg_id},
                                          {"address", member_id},
                                          {"status", STATUS_DELIVERED},
                                          {"timestamp", status_results.getValueAs<long long int>(i, "updated_timestamp")}}))
          Logger::error("Inserting group_receipt");
      }
    }
    else if (status_results.valueAsString(i, "status") == "Read")
    {
      ++readreceiptcount;
      if (updatedtimestamp == -1)
        updatedtimestamp = status_results.valueAsInt(i, "updated_timestamp", -1);

      if (isgroup && !status_results.isNull(i, "updated_timestamp")) // add per-group-member details to cdelivery_receipts table
      {
        long long int member_id = getRecipientIdFromUuidMapped(status_results.valueAsString(i, "uuid"), savedmap, createcontacts);
        if (member_id == -1) // try phone
          member_id = getRecipientIdFromPhoneMapped(status_results.valueAsString(i, "e164"), savedmap, createcontacts);
        if (member_id == -1)
        {
          if (createcontacts)
          {
            if ((member_id = dtCreateRecipient(ddb, status_results.valueAsString(i, "uuid"), std::string(), std::string(), databasedir, savedmap, warn)) == -1)
            {
              Logger::error("Failed to create delivery_receipt member. Skipping");
              continue;
            }
          }
          else
          {
            Logger::error("Failed to get id of delivery_receipt member. Skipping");
            continue;
          }
        }
        if (!insertRow("group_receipts", {{"mms_id", msg_id},
                                          {"address", member_id},
                                          {"status", STATUS_READ},
                                          {"timestamp", status_results.getValueAs<long long int>(i, "updated_timestamp")}}))
          Logger::error("Inserting group_receipt");
      }
    }
  }

  // update the message in its table (mms/sms)
  if (deliveryreceiptcount < readreceiptcount)
    deliveryreceiptcount = readreceiptcount; // lets just say read messages are also delivered...
  if (deliveryreceiptcount)
    if (!d_database.exec("UPDATE " + (is_mms ? d_mms_table : "sms"s) + " SET " + d_mms_delivery_receipts + " = ? WHERE _id = ?", {deliveryreceiptcount, msg_id}))
      Logger::error("Updating ", (is_mms ? d_mms_table : "sms"), " ", d_mms_delivery_receipts, ".");
  if (readreceiptcount)
    if (!d_database.exec("UPDATE " + (is_mms ? d_mms_table : "sms"s) + " SET " + d_mms_read_receipts + " = ? WHERE _id = ?", {readreceiptcount, msg_id}))
      Logger::error("Updating ", (is_mms ? d_mms_table : "sms"), " ", d_mms_read_receipts, ".");

  // update receipt timestamp (if available)
  if (d_database.tableContainsColumn((is_mms ? d_mms_table : "sms"s), "receipt_timestamp"))
    if (!d_database.exec("UPDATE " + (is_mms ? d_mms_table : "sms"s) + " SET receipt_timestamp = ? WHERE _id = ?", {updatedtimestamp, msg_id}))
      Logger::error("Updating ", (is_mms ? d_mms_table : "sms"), " receipt_timestamp.");



  //insert into group_receipts
  //          sqlite> SELECT * from group_receipts  where _id = 8;
  //         _id = 8
  //      mms_id = 27
  //     address = 4
  //      status = 1
  //   timestamp = 1669579600157
  //unidentified = 1
}
