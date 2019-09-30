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

#include "signalbackup.ih"

void SignalBackup::mergeRecipients(std::vector<std::string> const &addresses)
{
  if (addresses.size() < 2)
  {
    std::cout << "Too few addresses" << std::endl;
    return;
  }

  std::string targetaddr = addresses.back();

  // deal with one-on-one conversations:
  // get thread of target address
  long long int tid = getThreadIdFromRecipient(targetaddr);
  if (tid == -1)
  {
    std::cout << "Fail :(" << std::endl;
    return;
  }

  // update all messages from this addresses[i] to belong to that same thread and change address in new number
  for (uint i = 0; i < addresses.size() - 1; ++i)
  {
    long long int oldtid = getThreadIdFromRecipient(addresses[i]);
    if (oldtid == -1)
    {
      std::cout << "Failed to find thread for old address: " << addresses[i] << std::endl;
      continue;
    }

    std::cout << "Dealing with address: " << addresses[i] << std::endl;
    d_database.exec("UPDATE sms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?", {tid, targetaddr, oldtid, addresses[i]});
    std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
    d_database.exec("UPDATE mms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?", {tid, targetaddr, oldtid, addresses[i]});
    std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;

    d_database.exec("DELETE FROM thread WHERE recipient_ids = ?", addresses[i]);
  }


  // deal with groups
  SqliteDB::QueryResults results;
  d_database.exec("SELECT group_id,members,title FROM groups", &results);
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (results.columns() != 3 ||
        !results.valueHasType<std::string>(i, 0) ||
        !results.valueHasType<std::string>(i, 1) ||
        !results.valueHasType<std::string>(i, 2))
    {
      std::cout << ":(" << std::endl;
      continue;
    }

    std::string id = results.getValueAs<std::string>(i, 0);
    std::string members = results.getValueAs<std::string>(i, 1);
    std::string title = results.getValueAs<std::string>(i, 2);

    std::cout << "Dealing with group: " << id << " (title: '" << title << "', members: " << members << ")" << std::endl;

    // get thread id for this group:
    tid = getThreadIdFromRecipient(id);
    if (tid == -1)
    {
      std::cout << "Failed to find thread for groupchat" << std::endl;
      continue;
    }

    for (uint j = 0; j < addresses.size() - 1; ++j)
    {
      d_database.exec("UPDATE sms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, addresses[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
      d_database.exec("UPDATE mms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, addresses[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;
    }


  }
}
