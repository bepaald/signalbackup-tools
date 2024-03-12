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

bool SignalBackup::missingAttachmentExpected(uint64_t rowid, int64_t unique_id) const
{
  // if the attachment was never successfully completely downloaded, the data is expected to be missing.
  // this is shown by the 'pending_push' field in the part table which can have the following values:
  // public static final int TRANSFER_PROGRESS_DONE    = 0;
  // public static final int TRANSFER_PROGRESS_STARTED = 1;
  // public static final int TRANSFER_PROGRESS_PENDING = 2;
  // public static final int TRANSFER_PROGRESS_FAILED  = 3;
  SqliteDB::QueryResults results;
  if (d_database.exec("SELECT " + d_part_pending + " FROM " + d_part_table + " WHERE _id = ?" +
                      (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(unique_id) : "") +
                      " AND " + d_part_pending + " != 0",
                      rowid, &results))
    if (results.rows() == 1)
      return true;


  // if the attachment is a view once type, it is expected to be missing
  if (d_database.getSingleResultAs<std::string>("SELECT " + d_part_ct + " FROM " + d_part_table + " WHERE _id = ?" +
                                                (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(unique_id) : ""),
                                                rowid, std::string()) == "application/x-signal-view-once")
    return true;

  SqliteDB::QueryResults isquote;
  d_database.exec("SELECT " + d_part_mid + " FROM " + d_part_table + " WHERE _id = ?" +
                   (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(unique_id) : "") +
                  " AND quote = 1", rowid, &isquote);
  if (isquote.rows())
  {
    long long int mid = isquote.getValueAs<long long int>(0, d_part_mid);

    // if the attachment is in a quote and the original quote is missing, attachment is expected to be missing (NOT ALWAYS)
    if (d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE quote_missing = 1 AND _id = ?", mid, &results))
      if (results.rows() == 1)
        return true;

    // quote_missing is not always (often not?) set to 1 even if quote is missing, so manually check:

    // check for remote deleted
    if (d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE remote_deleted IS 1 AND " +
                        d_mms_date_sent + " IS (SELECT quote_id FROM " + d_mms_table + " WHERE _id = ?)",
                        mid, &results))
      if (results.rows()) // can be > 1 if message are doubled (and before date_sent had UNIQUE)
        return true;

    // check when self-deleted
    long long int quoteid = 0;
    if ((quoteid = d_database.getSingleResultAs<long long int>("SELECT IFNULL(quote_id, 0)_id FROM " + d_mms_table + " WHERE _id = ?", mid, 0)) != 0)
    {
      d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE " + d_mms_date_sent + " = ? AND "
                      "thread_id IS (SELECT thread_id FROM " + d_mms_table + " WHERE _id = ?)",
                      {quoteid, mid}, &results);
      if (results.rows() == 0)
        return true;
    }
  }

  // if the attachment is in a quote, but required no preview (is not an image or video), attachment
  // is expected to be missing (though not always)
  // NOTE
  // I have seen this fail for a 'image/webp' type, maybe because that particular image type was not supported? (for that phone??)
  if (d_database.exec("SELECT " + d_part_ct + " FROM " + d_part_table + " WHERE quote = 1 AND _id = ?" +
                      (d_database.tableContainsColumn(d_part_table, "unique_id") ? " AND unique_id = " + bepaald::toString(unique_id) : "") +
                      " AND " + d_part_ct + " NOT LIKE 'image%' AND " + d_part_ct + " NOT LIKE 'video%'", rowid, &results))
    if (results.rows() == 1)
      return true;

  return false;
}
