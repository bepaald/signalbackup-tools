/*
  Copyright (C) 2021-2024  Selwin van Dijk

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
  Logger::message("Database version: ", d_databaseversion);
  if (d_fd)
    Logger::message("Filesize: ", d_fd->total(), " bytes");

  if (d_databaseversion < 25)
    return false;

  SqliteDB::QueryResults results;

  // daterange
  if (d_database.containsTable("sms"))
  {
    if (d_database.exec("SELECT DATETIME((MIN(mindate)) / 1000, 'unixepoch', 'localtime') AS 'Min Date', DATETIME(MAX((maxdate) / 1000), 'unixepoch', 'localtime') AS 'Max Date' FROM (SELECT MIN(sms." + d_sms_date_received + ") AS mindate, MAX(sms." + d_sms_date_received + ") AS maxdate FROM sms UNION ALL SELECT MIN(" + d_mms_table + ".date_received) AS mindate, MAX(" + d_mms_table + ".date_received) AS maxdate FROM " + d_mms_table + ")", &results))
      Logger::message("Period: ", results.valueAsString(0, "Min Date"), " - ", results.valueAsString(0, "Max Date"));
  }
  else
  {
    if (d_database.exec("SELECT DATETIME(mindate / 1000, 'unixepoch', 'localtime') AS 'Min Date', DATETIME(maxdate / 1000, 'unixepoch', 'localtime') AS 'Max Date' FROM (SELECT MIN(" + d_mms_table + ".date_received) AS mindate, MAX(" + d_mms_table + ".date_received) AS maxdate FROM " + d_mms_table + ")", &results))
      Logger::message("Period: ", results.valueAsString(0, "Min Date"), " - ", results.valueAsString(0, "Max Date"));
  }

  // tables + counts
  if (d_database.exec("SELECT name FROM sqlite_master WHERE type = 'table'", &results))
  {
    Logger::message("Tables:");
    for (unsigned int i = 0; i < results.rows(); ++i)
    {
      SqliteDB::QueryResults results2;
      if (d_database.exec("SELECT COUNT(*) FROM " + results.valueAsString(i, "name"), &results2))
      {
        Logger::message_start(results.valueAsString(i, "name"), " : ", results2.getValueAs<long long int>(0, 0));
        if (results.valueAsString(i, "name") == "sms" || results.valueAsString(i, "name") == d_mms_table)
        {
          SqliteDB::QueryResults results3;
          if (d_database.exec("SELECT GROUP_CONCAT(counts, ',') FROM (SELECT COUNT(*) AS counts from " +
                              results.valueAsString(i, "name") +
                              " WHERE thread_id IN (SELECT _id FROM thread) GROUP BY thread_id ORDER BY thread_id ASC)", &results3))
            Logger::message_start(" (per thread: ", results3.valueAsString(0, 0), ")");
        }
        Logger::message_end();
      }
    }
  }
  return true;
}
