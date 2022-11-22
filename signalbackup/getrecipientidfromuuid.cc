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

long long int SignalBackup::getRecipientIdFromUuid(std::string const &uuid,
                                                   std::map<std::string, long long int> *savedmap) const
{

  std::cout << "Finding recipient for uuid: " << uuid << std::endl;

  if (savedmap->find(uuid) == savedmap->end())
  {
    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT recipient._id FROM recipient WHERE uuid = ? OR group_id = ?", {uuid, uuid}, &res) ||
        res.rows() != 1 ||
        !res.valueHasType<long long int>(0, 0))
    {
      return -1;
    }
    //res.prettyPrint();
    (*savedmap)[uuid] = res.getValueAs<long long int>(0, 0);
  }
  //std::cout << "RETURNING " << (*savedmap)[uuid] << std::endl;
  return (*savedmap)[uuid];
}
