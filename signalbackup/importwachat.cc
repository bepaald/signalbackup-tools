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

#include <iomanip>

/*
  Limitations and requirements

  - no media (yet?)
  - no mentions YET
  - filename must exactly and uniquely identify an existing conversation (group or contact name) in the Signal backup
  - messages have format '<timestamp><author>: <message>' where timestamp is passed as 'fmt', author exactly
    and uniquely identifies an existing contact in the Signal backup and contains no colons (':')
  -

*/

/*
bool handleWAMessage(long long int time, std::string const &chatname, std::string const &author, std::string const &message)
{
  // find thread_id for conversation 'chatname'


}
*/

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
  std::string globaladdress;
  bool isgroup = false;
  SqliteDB::QueryResults results;

  std::cout << "Looking for conversation: '" << file.substr(0, file.length() - STRLEN(".txt")) << "'" << std::endl;

  if (!d_database.exec("SELECT recipient._id, recipient.group_id, COALESCE(groups.title, recipient.system_display_name, recipient.profile_joined_name, recipient.phone) AS 'identifier' "
                       "FROM recipient "
                       "LEFT JOIN groups ON recipient.group_id == groups.group_id "
                       "WHERE identifier == ?", file.substr(0, file.length() - STRLEN(".txt")), &results))
    return false;

  //results.prettyPrint();

  if (results.rows() == 1)
  {
    globaladdress = results.valueAsString(0, "_id");
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
      {
        // deal with message
        std::cout << "Dealing with message:" << std::endl;
        std::cout << "Time: '" << time << "'" << std::endl;
        std::cout << "Author: '" << author << "'" << std::endl;
        std::cout << "Message: '" << message << "'" << std::endl;
      }

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
  {
    std::cout << "Dealing with message:" << std::endl;
    std::cout << "Time: '" << time << "'" << std::endl;
    std::cout << "Author: '" << author << "'" << std::endl;
    std::cout << "Message: '" << message << "'" << std::endl;
  }

  return false;
}
