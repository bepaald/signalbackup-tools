/*
  Copyright (C) 2022  Selwin van Dijk

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

void SignalBackup::setMessageDeliveryReceipts(SqliteDB const &ddb, long long int rowid, std::map<std::string, long long int> *savedmap,
                                              long long int msg_id, bool is_mms, bool isgroup) const
{

  using namespace std::string_literals;

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
                "conversations.uuid AS uuid,"
                "json_extract(delivery_details.value, '$.status') AS status,"
                "json_extract(delivery_details.value, '$.updatedAt') AS updated_timestamp"
                " FROM "
                "(SELECT key,value FROM messages,json_each(messages.json, '$.sendStateByConversationId') WHERE rowid IS ?) delivery_details"
                " LEFT JOIN conversations ON conversations.id IS conv_id", rowid, &status_results))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting message delivery status" << std::endl;
  }
  else
  {
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
    for (uint i = 0; i < status_results.rows(); ++i)
    {
      if (status_results.valueAsString(i, "status") == "Delivered")
      {
        ++deliveryreceiptcount;
        if (isgroup) // add per-group-member details to cdelivery_receipts table
        {
          long long int member_uuid = getRecipientIdFromUuid(status_results.valueAsString(i, "uuid"), savedmap);
          if (member_uuid == -1)
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to get uuid of delivery_receipt member. Skipping" << std::endl;
            continue;
          }
          if (!insertRow("group_receipts", {{"mms_id", msg_id},
                                            {"address", member_uuid},
                                            {"status", STATUS_DELIVERED},
                                            {"timestamp", status_results.getValueAs<long long int>(i, "updated_timestamp")}}))
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting group_receipt" << std::endl;
        }
      }
      else if (status_results.valueAsString(i, "status") == "Read")
      {
        ++readreceiptcount;
        if (isgroup) // add per-group-member details to cdelivery_receipts table
        {
          long long int member_uuid = getRecipientIdFromUuid(status_results.valueAsString(i, "uuid"), savedmap);
          if (member_uuid == -1)
          {
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to get uuid of delivery_receipt member. Skipping" << std::endl;
            continue;
          }
          if (!insertRow("group_receipts", {{"mms_id", msg_id},
                                            {"address", member_uuid},
                                            {"status", STATUS_READ},
                                            {"timestamp", status_results.getValueAs<long long int>(i, "updated_timestamp")}}))
            std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting group_receipt" << std::endl;
        }
      }
    }
    // update the message in its table (mms/sms)
    if (deliveryreceiptcount)
      if (!d_database.exec("UPDATE " + (is_mms ? "mms"s : "sms"s) + " SET delivery_receipt_count = ? WHERE _id = ?", {deliveryreceiptcount, msg_id}))
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Updating " << (is_mms ? "mms" : "sms") << " delivery_receipt_count." << std::endl;
    if (readreceiptcount)
      if (!d_database.exec("UPDATE " + (is_mms ? "mms"s : "sms"s) + " SET read_receipt_count = ? WHERE _id = ?", {readreceiptcount, msg_id}))
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Updating " << (is_mms ? "mms" : "sms") << " read_receipt_count." << std::endl;


    //insert into group_receipts
    //          sqlite> SELECT * from group_receipts  where _id = 8;
    //         _id = 8
    //      mms_id = 27
    //     address = 4
    //      status = 1
    //   timestamp = 1669579600157
    //unidentified = 1
  }
}
