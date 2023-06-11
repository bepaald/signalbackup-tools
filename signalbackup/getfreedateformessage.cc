/*
  Copyright (C) 2023  Selwin van Dijk

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

long long int SignalBackup::getFreeDateForMessage(long long int targetdate, long long int thread_id,
                                                  long long int from_recipient_id) const
{

  // long long int freedate = d_database.getSingleResultAs<long long int>("SELECT min(unused_date) AS unused_date FROM "
  //                                                                      "(SELECT min(" + d_mms_date_sent + ") + 1 AS unused_date FROM " + d_mms_table + " AS t1 WHERE "
  //                                                                      "thread_id = ? AND "
  //                                                                      "from_recipient_id = ? "
  //                                                                      "AND " + d_mms_date_sent + " >= ? AND "
  //                                                                      "NOT EXISTS (SELECT * FROM message AS t2 WHERE t2." + d_mms_date_sent + " = t1." + d_mms_date_sent + " + 1 AND thread_id = ? AND from_recipient_id = ?) UNION "
  //                                                                      "SELECT ? FROM " + d_mms_table + " WHERE NOT EXISTS (SELECT * FROM " + d_mms_table + " WHERE " + d_mms_date_sent + " = ?))",
  //                                                                      {thread_id, from_recipient_id, targetdate, thread_id, from_recipient_id, targetdate, targetdate}, -1);
  int incr = 0;
  long long int freedate = -1;
  while ((freedate = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE thread_id = ? AND from_recipient_id = ? AND " + d_mms_date_sent + " = ?",
                                                                 {thread_id, from_recipient_id, targetdate + incr}, -1)) != -1 && incr < 1000)
  {
    //std::cout << "date: " << freedate << " was taken" << std::endl;
    ++incr;
  }

  if (freedate != -1)
    return -1;

  return targetdate + incr;
}
