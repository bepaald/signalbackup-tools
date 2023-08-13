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

std::string SignalBackup::getNameFromRecipientId(long long int rid) const
{
  SqliteDB::QueryResults results;
  if (d_database.exec("SELECT COALESCE(NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
                      (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                      "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), NULLIF(recipient." + d_recipient_aci + ", ''), NULLIF(recipient." + d_recipient_e164 + ", ''), "
                      " recipient._id) AS 'display_name',recipient." + d_recipient_e164 + ",recipient.username,recipient." + d_recipient_aci + " "
                      "FROM recipient LEFT JOIN groups ON recipient.group_id = groups.group_id WHERE recipient._id = ?", rid, &results) &&
      results.rows() == 1 &&
      results.valueHasType<std::string>(0, "display_name"))
    return results.valueAsString(0, "display_name");
  return std::string();
}
