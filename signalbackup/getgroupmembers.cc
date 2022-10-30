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

bool SignalBackup::getGroupMembers(std::vector<long long int> *members, std::string const &group_id) const
{
  if (!members)
    return false;
  SqliteDB::QueryResults r;
  d_database.exec("SELECT members FROM groups WHERE group_id = ?", group_id, &r);
  //r.prettyPrint();

  if (r.rows() != 1 || r.valueHasType<std::string>(0, "group_id"))
    return false;

  // tokenize
  std::string membersstring(r.valueAsString(0, "members"));
  std::regex comma(",");
  std::sregex_token_iterator iter(membersstring.begin(), membersstring.end(), comma, -1);

  std::transform(iter, std::sregex_token_iterator(), std::back_inserter(*members),
                 [](std::string const &m) -> long long int { return bepaald::toNumber<long long int>(m); });

  // std::cout << "=====" << std::endl;
  // std::cout << "Set group members:" << std::endl;
  // for (auto const &id : *members)
  //   std::cout << id << std::endl;
  // std::cout << "=====" << std::endl;

  return true;
}
