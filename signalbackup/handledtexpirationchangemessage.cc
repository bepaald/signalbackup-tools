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

bool SignalBackup::handleDTExpirationChangeMessage(SqliteDB const &ddb [[maybe_unused]],
                                                   long long int rowid [[maybe_unused]],
                                                   long long int ttid [[maybe_unused]],
                                                   long long int address [[maybe_unused]]) const
{
  return true;

  //SqliteDB::QueryResults results;
  // ddb.exec("SELECT details FROM messages WHERE rowid = ?", rowid, results);

  // get details (who sent this, what's the new timer value

  // insertMessage(Types::INCOMING/OUTGOING | Types::EXPIRATION_TIMER_UPDATE_BIT), value
}
