/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

bool SignalBackup::missingAttachmentExpected(uint64_t rowid, uint64_t unique_id) const
{
  // if the attachment was never successfully completely downloaded, the data is expected to be missing.
  // this is shown by the 'pending_push' field in the part table which can have the following values:
  // public static final int TRANSFER_PROGRESS_DONE    = 0;
  // public static final int TRANSFER_PROGRESS_STARTED = 1;
  // public static final int TRANSFER_PROGRESS_PENDING = 2;
  // public static final int TRANSFER_PROGRESS_FAILED  = 3;
  SqliteDB::QueryResults results;
  if (d_database.exec("SELECT pending_push FROM part WHERE _id = ? AND unique_id = ? AND pending_push != 0",
                      {rowid, unique_id}, &results))
    if (results.rows())
      return true;

  // if the attachment is in a quote and the original quote is missing, attachment is expected to be missing
  if (d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE quote_missing = 1 AND _id IN (SELECT mid FROM part WHERE quote IS 1 AND mid = ?)",
                      rowid, &results))
    if (results.rows())
      return true;

  // quote_missing is not always (often not?) set to 1 even if quote is missing, so manually check
  if (d_database.exec("SELECT _id FROM " + d_mms_table + " WHERE remote_deleted IS 1 AND " + d_mms_date_sent + " IN (SELECT quote_id FROM " + d_mms_table + " WHERE _id IN "
                      "(SELECT mid FROM part WHERE _id = ? AND unique_id = ? AND quote = 1))",
                      {rowid, unique_id}, &results))
    if (results.rows())
      return true;

  // if the attachment is in a quote, but required no preview (is not an image or video), attachment is expected to be missing
  // NOTE
  // I have seen this fail for a 'image/webp' type, maybe because that particular image type was not supported? (for that phone??)
  if (d_database.exec("SELECT ct FROM part WHERE quote = 1 AND _id = ? AND unique_id = ? AND ct NOT LIKE 'image%' AND ct NOT LIKE 'video%'",
                      {rowid, unique_id}, &results))
    if (results.rows())
      return true;

  return false;
}
