/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

long long int SignalBackup::getRecipientIdFromName(std::string const &name, bool withthread) const
{
  SqliteDB::QueryResults results;

  if (d_database.exec("SELECT recipient._id, thread._id FROM recipient LEFT JOIN groups ON recipient.group_id = groups.group_id LEFT JOIN thread ON recipient._id = thread." + d_thread_recipient_id + " WHERE "
                      "COALESCE(NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
                      (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                      "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), NULLIF(groups.title, ''), NULLIF(recipient." + d_recipient_aci + ", ''), NULLIF(recipient." + d_recipient_e164 + ", ''), "
                      " recipient._id) = ?" + (withthread ? " AND thread._id IS NOT NULL" : ""), name, &results))
  {
    //results.prettyPrint();

    // no results
    if (results.rows() == 0)
      return -1;

    // multiple hits for 'name'
    if (results.rows() > 1)
    {
      Logger::error("Got multiple results for recipient `", name, "'");
      return -1;
    }

    // ok
    return results.valueAsInt(0, "_id");
  }

  // some error executing query
  return -1;
}
