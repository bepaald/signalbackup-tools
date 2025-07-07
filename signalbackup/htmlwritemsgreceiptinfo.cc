/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

void SignalBackup::HTMLwriteMsgReceiptInfo(std::ofstream &htmloutput, std::map<long long int, RecipientInfo> *recipient_info,
                                           long long int message_id, bool isgroup, long long int read_count, long long int delivered_count,
                                           long long int timestamp, int indent) const
{
  if (!isgroup)
  {
    if (read_count > 0)
      htmloutput << std::string(indent, ' ') << "                <div class=\"msg-receipt-info\">\n"
                 << std::string(indent, ' ') << "                  Read" << (timestamp != -1 ?
                                                                             " - " + bepaald::toDateString(timestamp / 1000, "%b %d, %Y %H:%M:%S") : "") << '\n'
                 << std::string(indent, ' ') << "                </div>\n";
    else if (delivered_count > 0)
      htmloutput << std::string(indent, ' ') << "                <div class=\"msg-receipt-info\">\n"
                 << std::string(indent, ' ') << "                  Delivered" << (timestamp != -1 ?
                                                                                  " - " + bepaald::toDateString(timestamp / 1000, "%b %d, %Y %H:%M:%S") : "") << '\n'
                 << std::string(indent, ' ') << "                </div>\n";
  }
  else // isgroup
  {
    SqliteDB::QueryResults group_receipts;
    if (d_database.exec("SELECT address, status, timestamp FROM group_receipts WHERE mms_id = ? ORDER BY status DESC",
                        message_id, &group_receipts) &&
        group_receipts.rows() > 0)
    {
      htmloutput << std::string(indent, ' ') << "                <div class=\"msg-receipt-info\">\n"
                 << std::string(indent, ' ') << "                  <span class=\"columnview\">\n";
      long long int prevstatus = -10;
      for (unsigned int i = 0; i < group_receipts.rows(); ++i)
      {
        long long int status = group_receipts.valueAsInt(i, "status", -10);
        if (status != prevstatus)
        {
          switch (status)
          {
            case 4: // enum SKIPPED
              htmloutput << std::string(indent, ' ') << "                    "
                "<span class=\"columnview-header\">Skipped</span>\n";
              break;
            case 3: // enum VIEWED
              htmloutput << std::string(indent, ' ') << "                    "
                "<span class=\"columnview-header\">Viewed by</span>\n";
                break;
            case 2: // enum READ
              htmloutput << std::string(indent, ' ') << "                    "
                "<span class=\"columnview-header\">Read by</span>\n";
                break;
            case 1: // enum DELIVERED
              htmloutput << std::string(indent, ' ') << "                    "
                "<span class=\"columnview-header\">Delivered to</span>\n";
                break;
            case 0: // enum UNDELIVERED
              htmloutput << std::string(indent, ' ') << "                    "
                "<span class=\"columnview-header\">Sent to</span>\n"; // I think...
              break;
            default: // -1 // enum UNKNOWN
              break;
          }
          prevstatus = status;
        }
        if (status >= 0)
          htmloutput << std::string(indent, ' ') << "                    <span class=\"column-left-align\">"
                     << HTMLescapeString(getRecipientInfoFromMap(recipient_info, group_receipts.valueAsInt(i, "address", -1)).display_name) << "</span>"
                     << "<span class=\"column-right-align\">"
                     << bepaald::toDateString(group_receipts.valueAsInt(i, "timestamp", -1) / 1000, "%b %d, %Y %H:%M:%S") << "</span>\n";
      }
      htmloutput << std::string(indent, ' ') << "                  </span>\n"
                 << std::string(indent, ' ') << "                </div>\n";
    }
  }
}
