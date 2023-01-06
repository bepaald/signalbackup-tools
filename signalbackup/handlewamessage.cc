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

bool SignalBackup::handleWAMessage(long long int thread_id, long long int time, std::string const &chatname, std::string const &author, std::string const &message,
                                   std::string const &selfid, bool isgroup, std::map<std::string, std::string> const &name_to_recipientid)
{
  //std::cout << "Dealing with message:" << std::endl;
  //std::cout << "Time: '" << time << "'" << std::endl;
  //std::cout << "Author: '" << author << "'" << std::endl;
  //std::cout << "Message: '" << message << "'" << std::endl;

  bool mark_as_read = false;
  bool outgoing = author == selfid || (!isgroup && author != chatname);

  std::string address;
  // author's address not in map yet
  std::string addresstofind = (isgroup && outgoing) ? chatname : author;
  if (name_to_recipientid.find(addresstofind) == name_to_recipientid.end())
  {
    std::cout << "Error finding recipient_id for " << addresstofind << std::endl;
    return false;
  }
  address = name_to_recipientid.at(addresstofind);

  // maybe make (secure_message_bit|pushmessage) an option?
  long long int type = (outgoing ? Types::BASE_SENT_TYPE : Types::BASE_INBOX_TYPE) | Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT;

  // delivery_receipt_count
  // 0 for incoming
  // if outgoing -> 1 for 1-on-1 chats
  //             -> group size for group chats -> also append to 'group_receipts' table (_id|mms_id|address|status|timestamp|unidentified)
  long long int delivery_receipt_count = outgoing ? 1 : 0;
  if (outgoing && isgroup)
  {
    delivery_receipt_count = name_to_recipientid.size() - 1; // delivered to all members, -1 because map also contains chat name
    if (name_to_recipientid.find(selfid) != name_to_recipientid.end()) // likely, map also contains self
      --delivery_receipt_count;
  }
  long long int read_receipt_count = (mark_as_read && outgoing ? 1 : 0);
  if (mark_as_read && outgoing && isgroup)
  {
    read_receipt_count = name_to_recipientid.size() - 1; // delivered to all members, -1 because map also contains chat name
    if (name_to_recipientid.find(selfid) != name_to_recipientid.end()) // likely, map also contains self
      --read_receipt_count;
  }


  // TODO:
  // scan message for mentions, edit body, update mentions-table

  // read_receipt_count?     -> 0 default, 1 on option && outgoing
  std::string statement = "INSERT INTO ";
  if (isgroup && author == selfid)
    statement += "mms (thread_id, " + d_mms_recipient_id + ", " + d_mms_date_sent + ", date_received, date_server, read, " + d_mms_type;
  else
    statement += "sms (thread_id, " + d_sms_recipient_id + ", " + d_sms_date_received + ", date_sent, date_server, read, type";
  statement += ", body, delivery_receipt_count, read_receipt_count) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

  if (!d_database.exec(statement, {thread_id, address, time, time, outgoing ? -1ll : time, 1ll, type, message, delivery_receipt_count, read_receipt_count}))
    return false;

  // update delivery_receipt table
  if (outgoing && isgroup)
  {
    long long int last_insert_id = d_database.lastInsertRowid();
    for (auto const &a : name_to_recipientid)
    {
      if (a.first == selfid || a.first == chatname)
        continue;
      if (!d_database.exec("INSERT INTO group_receipts (mms_id, address, timestamp, status) VALUES (?, ?, ?, ?)", {last_insert_id, a.second, time, (mark_as_read ? 2ll : 1ll)}))
        return false;
    }
  }

  return true;
}
