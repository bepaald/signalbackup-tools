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

#include "../sqlcipherdecryptor/sqlcipherdecryptor.h"

bool SignalBackup::importFromDesktop(std::string const &dir, bool ignorewal)
{
  if (dir.empty())
  {
    // try to set dir automatically
  }

  // check if a wal (Write-Ahead Logging) file is present in path, and warn user to (cleanly) shut Signal Desktop down
  if (!ignorewal &&
      bepaald::fileOrDirExists(dir + "/db.sqlite-wal"))
  {
    // warn
    return false;
  }

  SqlCipherDecryptor sqlcipherdecryptor(dir);
  if (!sqlcipherdecryptor.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }

  auto [data, size] = sqlcipherdecryptor.data(); // unsigned char *, uint64_t

  // disable WAL (Write-Ahead Logging) on database, reading from memory otherwise will not work
  // see https://www.sqlite.org/fileformat.html
  if (data[0x12] == 2)
    data[0x12] = 1;
  if (data[0x13] == 2)
    data[0x13] = 1;

  std::pair<unsigned char *, uint64_t> desktopdata = {data, size};
  SqliteDB ddb(&desktopdata);
  if (!ddb.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }

  // actual functionality comes here :)
  // ...

  // get all conversations (conversationpartners) from ddb
  SqliteDB::QueryResults results;
  if (!ddb.exec("SELECT id,uuid,groupId FROM conversations WHERE json_extract(json, '$.messageCount') > 0", &results))
    return false;

  results.prettyPrint();

  // for each conversation
  for (uint i = 0; i < results.rows(); ++i)
  {
    // get the actual id
    std::string person_or_group_id = results.valueAsString(i, "uuid");
    if (person_or_group_id.empty())
    {
      auto [groupid_data, groupid_data_length] = Base64::base64StringToBytes(results.valueAsString(i, "groupId"));
      if (groupid_data && groupid_data_length != 0)
      {
        //std::cout << bepaald::bytesToHexString(groupid_data, groupid_data_length, true) << std::endl;
        person_or_group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, groupid_data_length, true);
        bepaald::destroyPtr(&groupid_data, &groupid_data_length);
      }
    }

    if (person_or_group_id.empty())
    {
      //std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << "Useful error message" << std::endl;
      continue;
    }

    // get matching thread id from android database
    SqliteDB::QueryResults results2;
    if (!d_database.exec("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE (uuid = ? OR group_id = ?))",
                         {person_or_group_id, person_or_group_id}, &results2) ||
        results2.rows() != 1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << " : Failed to find matching thread for conversation, skipping. (id: " << person_or_group_id << ")" << std::endl;
      continue;
    }

    std::cout << "Match for " << person_or_group_id << std::endl;
    results2.prettyPrint();

    long long int ttid = results2.getValueAs<long long int>(0, "_id"); // ttid : target thread id
    std::cout << ttid << std::endl;
  }

  return false;
}
