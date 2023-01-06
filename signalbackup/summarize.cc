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

bool SignalBackup::summarize() const
{
  std::cout << "Database version: " << d_databaseversion << std::endl;
  if (d_fd)
    std::cout << "Filesize: " << d_fd->total() << " bytes" << std::endl;

  if (d_databaseversion < 25)
    return false;

  SqliteDB::QueryResults results;

  // daterange
  if (d_database.exec("SELECT DATETIME(ROUND((MIN(mindate)) / 1000), 'unixepoch') AS 'Min Date', DATETIME(ROUND(MAX(maxdate) / 1000), 'unixepoch') AS 'Max Date' FROM (SELECT MIN(sms." + d_sms_date_received + ") AS mindate, MAX(sms." + d_sms_date_received + ") AS maxdate FROM sms UNION ALL SELECT MIN(mms.date_received) AS mindate, MAX(mms.date_received) AS maxdate FROM mms)", &results))
    std::cout << "Period: " << results.valueAsString(0, "Min Date") << " - " << results.valueAsString(0, "Max Date") << std::endl;

  // tables + counts
  if (d_database.exec("SELECT name FROM sqlite_master WHERE type = 'table'", &results))
  {
    std::cout << "Tables:" << std::endl;
    for (uint i = 0; i < results.rows(); ++i)
    {
      SqliteDB::QueryResults results2;
      if (d_database.exec("SELECT COUNT(*) FROM " + results.valueAsString(i, "name"), &results2))
      {
        std::cout << results.valueAsString(i, "name") << " : " << results2.getValueAs<long long int>(0, 0);
        if (results.valueAsString(i, "name") == "sms" || results.valueAsString(i, "name") == "mms")
        {
          SqliteDB::QueryResults results3;
          if (d_database.exec("SELECT GROUP_CONCAT(counts, ',') FROM (SELECT COUNT(*) AS counts from " +
                              results.valueAsString(i, "name") +
                              " WHERE thread_id IN (SELECT _id FROM thread) GROUP BY thread_id ORDER BY thread_id ASC)", &results3))
            std::cout << " (per thread: " << results3.valueAsString(0, 0) << ")";
        }
        std::cout << std::endl;
      }
    }
  }
  return true;
}
