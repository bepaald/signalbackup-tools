/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

long long int SignalBackup::getThreadIdFromRecipient(std::string const &recipient) const
{

  // note: for d_database < 24, recipient == "+316xxxxxxxx" || "__text_secure__!..."
  //                      >= 24, recipient is just an int id

  long long int tid = -1;
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", recipient, &results);
  if (results.rows() == 1 && results.columns() == 1 && results.valueHasType<long long int>(0, 0))
    tid = results.getValueAs<long long int>(0, 0);
  return tid;
}
