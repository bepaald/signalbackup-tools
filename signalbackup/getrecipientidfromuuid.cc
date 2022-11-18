/*
  Copyright (C) 2022  Selwin van Dijk

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

long long int SignalBackup::getRecipientIdFromUuid(std::string const &uuid) const
{
  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT _id FROM recipient WHERE uuid = ? OR group_id = ?", {uuid, uuid}, &res) ||
      res.rows() != 1 ||
      !res.valueHasType<long long int>(0, 0))
    return -1;
  return res.getValueAs<long long int>(0, 0);
}
