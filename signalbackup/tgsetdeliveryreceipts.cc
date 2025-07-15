/*
  Copyright (C) 2025  Selwin van Dijk

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

bool SignalBackup::tgSetDeliveryReceipts(std::string const &deliveryreportjson, long long int new_msg_id,
                                         std::vector<std::pair<std::vector<std::string>, long long int>> const &contactmap,
                                         bool isgroup) const
{
  /*
    Accepted delivery statusses "sending"|"failed"|"delivered"|"read"

    In signal android, the status is an ENUM:
    0 = undelivered
    1 = delivered
    2 = read
    3 = viewed
    4 = skipped
   */

  if (deliveryreportjson.empty())
    return true;

  if (isgroup && !d_database.containsTable("group_receipts")) [[unlikely]]
  {
    Logger::warning("Input database does not have group_receipts table (too old?)");
    return false;
  }

  // get recipient id for conversation
  auto find_in_contactmap = [&contactmap](std::string const &identifier) -> long long int
  {
    for (unsigned int i = 0; i < contactmap.size(); ++i)
      for (unsigned int j = 0; j < contactmap[i].first.size(); ++j)
        if (contactmap[i].first[j] == identifier)
          return contactmap[i].second;
    return -1;
  };

  //std::cout << deliveryreportjson << std::endl;

  SqliteDB::QueryResults deliverystatuses_results;
  if (!d_database.exec("SELECT "
                       "json_extract(value, '$.recipient') AS recipient,"
                       "json_extract(value, '$.timestamp') AS timestamp,"
                       "json_extract(value, '$.status') AS status "
                       "FROM json_each(?)", deliveryreportjson, &deliverystatuses_results)) [[unlikely]]
    return false;

  //deliverystatuses_results.prettyPrint(false);

  for (unsigned int i = 0; i < deliverystatuses_results.rows(); ++i)
  {
    long long int recipientid = find_in_contactmap(deliverystatuses_results(i, "recipient"));
    if (recipientid == -1) [[unlikely]]
    {
      Logger::error("Failed to map reaction author '", deliverystatuses_results(i, "recipient"), "' to id in Android backup");
      return false;
    }

    long long int timestamp = deliverystatuses_results.valueAsInt(i, "timestamp", -1);
    if (timestamp == -1) [[unlikely]]
    {
      if (isgroup)
      {
        Logger::error("failed to get timestamp for msg delivery report");
        return false;
      }
      Logger::warning("failed to get timestamp for msg delivery report");
    }
    else if (timestamp < 100000000000) // only 11 digits (max), timestamp is likely in seconds instead of milliseconds
      timestamp *= 1000;

    std::string delivery_status = deliverystatuses_results(i, "status");

    if (delivery_status == "failed") [[unlikely]]
    {
      if (!d_database.exec("UPDATE " + d_mms_table + " SET type = ((type & ~?) | ?) WHERE _id = ?",
                           {Types::BASE_TYPE_MASK, Types::BASE_SENT_FAILED_TYPE, new_msg_id}))
      {
        Logger::error("Failed to set message type to SENT_FAILED");
        return false;
      }
      continue;
    }

    if (delivery_status == "sending") [[unlikely]]
    {
      if (!d_database.exec("UPDATE " + d_mms_table + " SET type = ((type & ~?) | ?) WHERE _id = ?",
                           {Types::BASE_TYPE_MASK, Types::BASE_SENDING_TYPE, new_msg_id}))
      {
        Logger::error("Failed to set message type to SENT_FAILED");
        return false;
      }
      continue;
    }

    if (delivery_status == "read")
    {
      if (isgroup)
      {
        if (!insertRow("group_receipts",
                       {{"mms_id", new_msg_id},
                        {"address", recipientid},
                        {"status", 2},
                        {"timestamp", timestamp}}))
        {
          Logger::error("failed to set message delivery status to read");
          return false;
        }
      }
      if (!d_database.exec("UPDATE " + d_mms_table + " SET " +
                           d_mms_delivery_receipts + " = 1, " +
                           d_mms_read_receipts + " = 1, "
                           "receipt_timestamp = ? "
                           "WHERE _id = ?",
                           {timestamp, new_msg_id}))
      {
        Logger::error("failed to set message delivery status to read");
        return false;
      }
      continue;
    }

    if (delivery_status == "delivered")
    {
      if (isgroup)
      {
        if (!insertRow("group_receipts",
                       {{"mms_id", new_msg_id},
                        {"address", recipientid},
                        {"status", 1},
                        {"timestamp", timestamp}}))
        {
          Logger::error("failed to set message delivery status to read");
          return false;
        }
      }
      if (!d_database.exec("UPDATE " + d_mms_table + " SET " +
                           d_mms_delivery_receipts + " = 1, "
                           "receipt_timestamp = ? "
                           "WHERE _id = ?",
                           {timestamp, new_msg_id}))
      {
        Logger::error("failed to set message delivery status to delivered");
        return false;
      }
      continue;
    }

    Logger::error("Unhandled delivery type: '", delivery_status, "'");
    return false;
  }

  return true;
}
