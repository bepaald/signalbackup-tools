/*
    Copyright (C) 2021-2022  Selwin van Dijk

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

bool SignalBackup::reorderMmsSmsIds() const
{
  std::cout << __FUNCTION__ << std::endl;

  // get all mms in the correct order
  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT _id FROM mms ORDER BY date_received ASC", &res)) // for sms table, use 'date'
    return false;

  // set all id's 'negatively ascending' (negative because of UNIQUE constraint)
  long long int negative_id_tmp = 0;
  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int oldid = res.getValueAs<long long int>(i, 0);
    ++negative_id_tmp;
    if (!d_database.exec("UPDATE mms SET _id = ? WHERE _id = ?", {-1 * negative_id_tmp, oldid}) ||
        !d_database.exec("UPDATE part SET mid = ? WHERE mid = ?", {-1 * negative_id_tmp, oldid}) ||
        !d_database.exec("UPDATE group_receipts SET mms_id = ? WHERE mms_id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (d_database.containsTable("mention"))
      if (!d_database.exec("UPDATE mention SET message_id = ? WHERE message_id = ?", {-1 * negative_id_tmp, oldid}))
        return false;
    if (d_database.containsTable("msl_message"))
      if (!d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ? AND is_mms IS 1", {-1 * negative_id_tmp, oldid}))
        return false;
    if (d_database.containsTable("reaction")) // dbv >= 121
      if (!d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ? AND is_mms IS 1", {-1 * negative_id_tmp, oldid}))
        return false;
  }

  // now make all id's positive again
  if (!d_database.exec("UPDATE mms SET _id = _id * -1 WHERE _id < 0") ||
      !d_database.exec("UPDATE part SET mid = mid * -1 WHERE mid < 0") ||
      !d_database.exec("UPDATE group_receipts SET mms_id = mms_id * -1 WHERE mms_id < 0"))
    return false;
  if (d_database.containsTable("mention"))
    if (!d_database.exec("UPDATE mention SET message_id = message_id * -1 WHERE message_id < 0"))
      return false;
  if (d_database.containsTable("msl_message"))
    if (!d_database.exec("UPDATE msl_message SET message_id = message_id * -1 WHERE message_id < 0 AND is_mms IS 1"))
      return false;
  if (d_database.containsTable("reaction")) // dbv >= 121
    if (!d_database.exec("UPDATE reaction SET message_id = message_id * -1 WHERE message_id < 0 AND is_mms IS 1"))
      return false;

  // SAME FOR SMS
  if (!d_database.exec("SELECT _id FROM sms ORDER BY date ASC", &res))
    return false;

  negative_id_tmp = 0;
  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int oldid = res.getValueAs<long long int>(i, 0);
    ++negative_id_tmp;
    if (!d_database.exec("UPDATE sms SET _id = ? WHERE _id = ?", {-1 * negative_id_tmp, oldid}))
      return false;
    if (d_database.containsTable("msl_message"))
      if (!d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ? AND is_mms IS NOT 1", {-1 * negative_id_tmp, oldid}))
        return false;
    if (d_database.containsTable("reaction")) // dbv >= 121
      if (!d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ? AND is_mms IS NOT 1", {-1 * negative_id_tmp, oldid}))
        return false;
  }

  if (!d_database.exec("UPDATE sms SET _id = _id * -1 WHERE _id < 0"))
    return false;
  if (d_database.containsTable("msl_message"))
    if (!d_database.exec("UPDATE msl_message SET message_id = message_id * -1 WHERE message_id < 0 AND is_mms IS NOT 1"))
      return false;
  if (d_database.containsTable("reaction")) // dbv >= 121
    if (!d_database.exec("UPDATE reaction SET message_id = message_id * -1 WHERE message_id < 0 AND is_mms IS NOT 1"))
      return false;

  return true;
}
