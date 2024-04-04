/*
  Copyright (C) 2024  Selwin van Dijk

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

void SignalBackup::scanMissingAttachments() const
{
  // get 'missing' attachments
  SqliteDB::QueryResults res;
  d_database.exec("SELECT _id," +
                  (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) +
                  " FROM " + d_part_table, &res);
  std::vector<std::pair<long long int, long long int>> missing;
  for (uint i = 0; i < res.rows(); ++i)
    if (/*true || */d_attachments.find({res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")}) == d_attachments.end())
      missing.emplace_back(std::make_pair(res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")));

  Logger::message("Got ", missing.size(), " attachments with data not found");

  for (uint i = 0; i < missing.size(); ++i)
  {
    Logger::message_start("Checking ", (i + 1), " of ", missing.size(), ": ",  missing[i].first, ",", missing[i].second, "... ");

    SqliteDB::QueryResults isquote;
    d_database.exec("SELECT " + d_part_mid + " FROM " + d_part_table + " WHERE _id = ?" +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                     " AND unique_id = " + bepaald::toString(missing[i].second) : "") +
                    " AND quote = 1", missing[i].first, &isquote);
    if (isquote.rows())
    {
      long long int mid = isquote.getValueAs<long long int>(0, d_part_mid);

      d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE quote_missing = 1 AND _id = ?", mid, &res);
      if (res.rows() == 1)
      {
        if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
          Logger::message("OK, EXPECTED (quote missing)");
        else
          Logger::message("FALSE HIT! (quote missing)");
        continue;
      }

      // quote_missing is not always (often not?) set to 1 even if quote is missing, so manually check
      if (d_database.tableContainsColumn(d_mms_table, "remote_deleted"))
      {
        d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE remote_deleted IS 1 AND " + d_mms_date_sent + " IS (SELECT quote_id FROM " + d_mms_table + " WHERE _id = ?)",
                        mid, &res);
        if (res.rows()) // can be more than 1 row if messages were doubled (before date_sent (=quote_id) had UNIQUE constraint)
        {
          if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
            Logger::message("OK, EXPECTED (original message missing (remote deleted))");
          else
            Logger::message("FALSE HIT! (remote delete)");
          continue;
        }
      }

      if (d_database.getSingleResultAs<long long int>("SELECT IFNULL(quote_id, 0)_id FROM " + d_mms_table + " WHERE _id = ?", mid, 0) != 0)
      {
        d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE " +
                        d_mms_date_sent + " IS (SELECT quote_id FROM " + d_mms_table + " WHERE _id = ?)"
                        " AND "
                        "thread_id IS (SELECT thread_id FROM " + d_mms_table + " WHERE _id = ?)",
                        {mid, mid}, &res);
        if (res.rows() == 0)
        {
          if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
            Logger::message("OK, EXPECTED (original message missing (deleted))");
          else
            Logger::message("FALSE HIT! (delete)");
          continue;
        }
      }
    }

    d_database.exec("SELECT " + d_part_ct + " FROM " + d_part_table + " WHERE "
                    "quote = 1 "
                    "AND _id = ?" +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(missing[i].second) : ""s) +
                    " AND " + d_part_ct + " NOT LIKE 'image%' AND " + d_part_ct + " NOT LIKE 'video%'",
                    missing[i].first, &res);
    if (res.rows() == 1)
    {
      if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
        Logger::message("OK, EXPECTED (type = ",res.valueAsString(0, 0), ")");
      else
        Logger::message("FALSE HIT! (type)");
      continue;
    }

    d_database.exec("SELECT " + d_part_pending + " FROM " + d_part_table + " WHERE " + d_part_pending + " IS NOT 0 AND _id = ?" +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(missing[i].second) : ""s),
                    missing[i].first, &res);
    if (res.rows() == 1)
    {
      if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
        Logger::message("OK, EXPECTED (pending_push = ", res.valueAsString(0, 0), ")");
      else
        Logger::message("FALSE HIT! (pending_push)");
      continue;
    }

    d_database.exec("SELECT " + d_part_ct + " FROM " + d_part_table + " WHERE _id = ?" +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(missing[i].second) : ""s),
                    missing[i].first, &res);
    if (res.rows() == 1 && res(0, d_part_ct) == "application/x-signal-view-once")
    {
      if (d_attachments.find({missing[i].first, missing[i].second}) == d_attachments.end())
        Logger::message("OK, EXPECTED (content_type = application/x-signal-view-once)");
      else
        Logger::message("FALSE HIT! (view_once)");
      continue;
    }

    if (d_attachments.find({missing[i].first, missing[i].second}) != d_attachments.end())
    {
      Logger::message("OK, EXPECTED (no special circumstances, but not missing)");
      continue;
    }

    Logger::message("UNEXPECTED! details:");
    d_database.exec("SELECT quote," + d_part_ct + "," + d_part_pending + " FROM " + d_part_table + " WHERE _id = ?" +
                    (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(missing[i].second) : ""s),
                    missing[i].first, &res);
    res.prettyPrint();
  }

}
