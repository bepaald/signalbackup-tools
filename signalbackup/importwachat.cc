/*
    Copyright (C) 2021  Selwin van Dijk

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

//#include <iomanip>

/*
  Limitations and requirements

  - no media (yet?)
  - no mentions YET
  - filename must exactly and uniquely identify an existing conversation (group or contact name) in the Signal backup
  - messages have format '<timestamp><author>: <message>' where timestamp is passed as 'fmt', author exactly
    and uniquely identifies an existing contact in the Signal backup and contains no colons (':')
  -

*/



bool SignalBackup::handleWAMessage(long long int thread_id, long long int time, std::string const &chatname, std::string const &author, std::string const &message,
                                   std::string const &selfid, bool isgroup, std::map<std::string, std::string> *name_to_recipientid)
{
  // std::cout << "Dealing with message:" << std::endl;
  // std::cout << "Time: '" << time << "'" << std::endl;
  // std::cout << "Author: '" << author << "'" << std::endl;
  // std::cout << "Message: '" << message << "'" << std::endl;

  bool outgoing = author == selfid || (!isgroup && author != chatname);

  std::string address;
  // author's address not in map yet
  std::string addresstofind = (isgroup && outgoing) ? chatname : author;
  if (name_to_recipientid->find(addresstofind) == name_to_recipientid->end())
  {
    SqliteDB::QueryResults results;
    if (!d_database.exec("SELECT _id, COALESCE(system_display_name, profile_joined_name, phone) AS 'identifier' "
                         "FROM recipient WHERE identifier == ?", addresstofind, &results))
      return false;
    if (results.rows() != 1)
      return false;

    (*name_to_recipientid)[addresstofind] = results.valueAsString(0, "_id");
  }
  address = name_to_recipientid->at(addresstofind);

  // maybe make (secure_message_bit|pushmessage) an option?
  long long int type = (outgoing ? Types::BASE_SENT_TYPE : Types::BASE_INBOX_TYPE) | Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT;

  // delivery_receipt_count
  // 0 for incoming
  // if outgoing -> 1 for 1-on-1 chats
  //             -> group size for group chats -> also append to 'group_receipts' table (_id|mms_id|address|status|timestamp|unidentified)
  long long int delivery_receipt_count = outgoing ? 1 : 0;
  if (outgoing && isgroup)
    ;// deal with delivery_receipt_count

  // TODO:
  // scan message for mentions, edit body, update mentions-table

  // read_receipt_count?     -> 0 default, 1 on option && outgoing
  std::string statement = "INSERT INTO ";
  if (isgroup && author == selfid)
    statement += "mms (thread_id, address, date, date_received, date_server, read, msg_box";
  else
    statement += "sms (thread_id, address, date, date_sent, date_server, read, type";
  statement += ", body, delivery_receipt_count) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

  return d_database.exec(statement, {thread_id, address, time, time, outgoing ? -1ll : time, 1ll, type, message, delivery_receipt_count});
}

bool SignalBackup::importWAChat(std::string const &file, std::string const &fmt, std::string const &self)
{
  std::ifstream chatfile(file);
  if (!chatfile.is_open())
  {
    std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
              << " opening file '" << file << "' for reading." << std::endl;
    return false;
  }


  // get global address (recipient_id of chat partner for 1-on-1, group_id for groupchats
  std::string chatname = file.substr(0, file.length() - STRLEN(".txt"));
  std::string globaladdress;
  bool isgroup = false;
  SqliteDB::QueryResults results;
  std::map<std::string, std::string> name_to_recipientid;

  std::cout << "Looking for conversation: '" << file.substr(0, file.length() - STRLEN(".txt")) << "'" << std::endl;

  if (!d_database.exec("SELECT recipient._id, recipient.group_id, COALESCE(groups.title, recipient.system_display_name, recipient.profile_joined_name, recipient.phone) AS 'identifier' "
                       "FROM recipient "
                       "LEFT JOIN groups ON recipient.group_id == groups.group_id "
                       "WHERE identifier == ?", chatname, &results))
    return false;

  //results.prettyPrint();

  if (results.rows() == 1)
  {
    globaladdress = results.valueAsString(0, "_id");
    name_to_recipientid[chatname] = globaladdress;
    if (results.valueHasType<std::string>(0, "group_id"))
    {
      isgroup = true;
      if (self.empty())
      {
        std::cout << "Error: dealing with group chat, but self-id not supplied. No way of determining which messages are outgoing and which are incoming." << std::endl;
        return false;
      }
    }
  }
  else
  {
    std::cout << "Failed to find conversation partner/group for chat: '" << file << "' : query returned " << results.rows() << " results." << std::endl;
    return false;
  }

  // get thread id from recipient
  long long int tid = -1;
  if (!d_database.exec("SELECT _id FROM thread WHERE recipient_ids == ?", globaladdress, &results))
    return false;

  if (results.rows() != 1)
  {
    std::cout << "Failed to find thread for chat. Query returned " << results.rows() << " results." << std::endl;
    return false;
  }

  tid = results.getValueAs<long long int>(0, "_id");

  std::cout << "Importing messages into thread: " << tid << std::endl;

  std::string line;
  long long int time = 0;
  long long int previous_time = 0;
  long long int previous_time_adj = 0;
  std::string author;
  std::string message;

  while (std::getline(chatfile, line))
  {
    std::tm tmb = {};
    std::istringstream ss(line);
    ss >> std::get_time(&tmb, fmt.c_str());
    if (ss.fail())
    {
      std::cout << "Invalid timefmt (" << fmt << ") => continue previous message" << std::endl;

      ss.clear();
      ss.seekg(0);

      std::string msg_cnt;
      if (!std::getline(ss, msg_cnt))
      {
        std::cout << "Some error reading the line" << std::endl;
        continue;
      }
      message += "\n" + msg_cnt;
    }
    else
    {
      // found start of new message, deal with previous
      if (time != 0) // first run
        if (!handleWAMessage(tid, time, chatname, author, message, self, isgroup, &name_to_recipientid))
          return false;

      // get a unique sequentially later time
      time = std::mktime(&tmb) * 1000;
      if (time == previous_time)
        time = previous_time_adj + 1;
      else
        previous_time = time;
      previous_time_adj = time;
      //std::cout << "Time: " << std::put_time(&tmb, "%c") << " (" << time << ")" << std::endl;

      std::getline(ss, author, ':');
      if (ss.eof())
      {
        std::cout << "No colon => some type of status message?" << std::endl;
        continue;
      }
      //std::cout << "Author: " << author << std::endl;

      std::getline(ss, message);
      message = message.substr(1); // remove the single whitespace after colon??

      //std::cout << "Message: " << message << std::endl;
    }
  }

  // deal with last message
  if (time != 0)
    if (!handleWAMessage(tid, time, chatname, author, message, self, isgroup, &name_to_recipientid))
      return false;

  return true;
}
