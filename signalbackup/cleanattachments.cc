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

bool SignalBackup::cleanAttachments()
{
  //std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> d_attachments; //maps <rowid,uniqueid> to attachment
  // remove unused attachments
  Logger::message("  Deleting unused attachments...");
  SqliteDB::QueryResults results;
  int constexpr INVALID_ID = -10;
  d_database.exec("SELECT _id,"
                  + (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) +
                  " FROM " + d_part_table, &results);
  for (auto it = d_attachments.begin(); it != d_attachments.end();)
  {
    bool found = false;
    for (uint i = 0; i < results.rows(); ++i)
    {
      long long int rowid = INVALID_ID;
      if (results.valueHasType<long long int>(i, "_id"))
        rowid = results.getValueAs<long long int>(i, "_id");
      long long int uniqueid = INVALID_ID;
      if (results.valueHasType<long long int>(i, "unique_id"))
        uniqueid = results.getValueAs<long long int>(i, "unique_id");

      if (rowid != INVALID_ID && uniqueid != INVALID_ID &&
          it->first.first == static_cast<uint64_t>(rowid) && it->first.second == static_cast<int64_t>(uniqueid))
      {
        found = true;
        break;
      }
    }
    if (!found)
      it = d_attachments.erase(it);
    else
      ++it;
  }
  return true;
}
