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

bool SignalBackup::getGroupMembersModern(std::vector<long long int> *members, std::string const &group_id) const
{
  SqliteDB::QueryResults r;
  if (!d_database.containsTable("group_membership") ||
      !d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id = ?", group_id, &r))
    return false;

  for (uint i = 0; i < r.rows(); ++i)
    members->push_back(r.getValueAs<long long int>(i, "recipient_id"));

  return true;
}

bool SignalBackup::getGroupMembersOld(std::vector<long long int> *members, std::string const &group_id,
                                      std::string const &column) const
{
  if (!members)
    return false;
  if (!d_database.tableContainsColumn("groups", column))
  {
    if (column == "members")
      return getGroupMembersModern(members, group_id);
    else
      return false;
  }

  SqliteDB::QueryResults r;
  d_database.exec("SELECT " + column + " FROM groups WHERE group_id = ? AND " + column + " IS NOT NULL", group_id, &r);
  //r.prettyPrint();

  if (r.rows() == 0) // no results
    return true;

  if (r.rows() > 1 || !r.valueHasType<std::string>(0, column))
    return false;

  // tokenize
  std::string membersstring(r.valueAsString(0, column));
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
