/*
    Copyright (C) 2019  Selwin van Dijk

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

/*
  NOTE: THIS IS A CUSTOM FUNCTION IT WILL BE REMOVED WITHOUT NOTIFICATION IN THE NEAR FUTURE
*/

#include "signalbackup.ih"

void SignalBackup::esokrates()
{
  SqliteDB::QueryResults res;
  d_database.exec("SELECT _id,body,address,date,type "
                  "FROM sms "
                  "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS 0", &res);

  std::cout << "Searching for possible duplicates of " << res.rows() << " unsecured messages" << std::endl;

  std::vector<long long int> ids_to_remove;

  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int msgid = std::any_cast<long long int>(res.value(i, "_id"));

    std::string body;
    bool body_is_null = false;
    if (res.valueHasType<std::string>(i, "body"))
      body = std::any_cast<std::string>(res.value(i, "body"));
    else if (res.valueHasType<std::nullptr_t>(i, "body"))
      body_is_null = true;

    std::string address = std::any_cast<std::string>(res.value(i, "address"));
    long long int date = std::any_cast<long long int>(res.value(i, "date"));
    long long int type = std::any_cast<long long int>(res.value(i, "type"));

    SqliteDB::QueryResults res2;
    if (body_is_null)
      d_database.exec("SELECT _id "
                      "FROM sms "
                      "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body IS NULL "
                      "AND address = ?", {date, address}, &res2);
    else // !body_is_null
      d_database.exec("SELECT _id "
                      "FROM sms "
                      "WHERE (type & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body = ? "
                      "AND address = ?", {date, body, address}, &res2);

    if (res2.rows() > 1)
    {
      std::cout << "Unexpectedley got multiple results when searching for duplicates... ignoring" << std::endl;
      continue;
    }
    else if (res2.rows() == 1)
    {
      std::time_t epoch = date / 1000;
      std::cout << " * Found duplicate of message: " << std::endl
                << "   " << msgid << "|" << body << "|" << address << "|"
                << std::put_time(std::localtime(&epoch), "%F %T %z") << "|" << "|" << type << std::endl
                << "   in 'sms' table. Marking for deletion." << std::endl;
      ids_to_remove.push_back(msgid);
      continue;
    }


    if (body_is_null)
      d_database.exec("SELECT _id "
                      "FROM mms "
                      "WHERE (msg_box & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body IS NULL "
                      "AND address = ?", {date, address}, &res2);
    else // !body_is_null
      d_database.exec("SELECT _id "
                      "FROM mms "
                      "WHERE (msg_box & " + bepaald::toString(Types::SECURE_MESSAGE_BIT) + ") IS NOT 0 "
                      "AND date = ? "
                      "AND body = ? "
                      "AND address = ?", {date, body, address}, &res2);
    if (res2.rows() > 1)
    {
      std::cout << "Unexpectedley got multiple results when searching for duplicates... ignoring" << std::endl;
      continue;
    }
    else if (res2.rows() == 1)
    {
      std::time_t epoch = date / 1000;
      std::cout << " * Found duplicate of message: " << std::endl
                << "   " << msgid << "|" << body << "|" << address << "|"
                << std::put_time(std::localtime(&epoch), "%F %T %z") << "|" << "|" << type << std::endl
                << "   in 'mms' table. Marking for deletion." << std::endl;
      ids_to_remove.push_back(msgid);
      continue;
    }
  }

  std::string ids_to_remove_str;
  for (uint i = 0; i < ids_to_remove.size(); ++i)
    ids_to_remove_str += bepaald::toString(ids_to_remove[i]) + ((i < ids_to_remove.size() - 1) ? "," : "");

  std::cout << std::endl << std::endl << "About to remove messages from 'sms' table with _id's = " << std::endl;
  std::cout << ids_to_remove_str << std::endl << std::endl;

  std::cout << "Deleting " << ids_to_remove.size() << " duplicates..." << std::endl;
  d_database.exec("DELETE FROM sms WHERE _id IN (" + ids_to_remove_str + ")");
  std::cout << "Deleted " << d_database.changed() << " entries" << std::endl;
}
