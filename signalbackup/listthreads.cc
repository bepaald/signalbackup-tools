/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

void SignalBackup::listThreads() const
{
  using namespace std::string_literals;

  std::cout << "Database version: " << d_databaseversion << std::endl;

  SqliteDB::QueryResults results;

  d_database.exec("SELECT MIN(mindate) AS 'Min Date', MAX(maxdate) AS 'Max Date' FROM "
                  "(SELECT MIN(sms." + d_sms_date_received + ") AS mindate, MAX(sms." + d_sms_date_received + ") AS maxdate FROM sms "
                  "UNION ALL SELECT MIN(mms.date_received) AS mindate, MAX(mms.date_received) AS maxdate FROM mms)", &results);
  results.prettyPrint();

  if (!d_database.containsTable("recipient"))
    d_database.exec("SELECT thread._id, thread." + d_thread_recipient_id + ", thread.snippet, COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient_preferences ON thread." + d_thread_recipient_id + " = recipient_preferences.recipient_ids LEFT JOIN groups ON thread." + d_thread_recipient_id + " = groups.group_id ORDER BY thread._id ASC", &results);
  else // has recipient table
  {
    bool uuid = d_database.tableContainsColumn("recipient", "uuid");
    bool profile_joined_name = d_database.tableContainsColumn("recipient", "profile_joined_name");

    // std::cout << d_thread_recipient_id << std::endl;
    // std::cout << d_sms_recipient_id << std::endl;
    // std::cout << d_mms_recipient_id << std::endl;

    d_database.exec("SELECT thread._id, COALESCE(recipient.phone, recipient.group_id" + (uuid ? ", recipient.uuid"s : ""s) + ") AS 'recipient_ids', thread.snippet, COALESCE(recipient.system_display_name, " + (profile_joined_name ? "recipient.profile_joined_name,"s : ""s) + "recipient.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id ORDER BY thread._id ASC", &results);

    // results.prettyPrint();

    // if (d_database.tableContainsColumn("recipient", "profile_joined_name"))
    //   d_database.exec("SELECT thread._id, COALESCE(recipient.phone, recipient.group_id, recipient.uuid) AS 'recipient_ids', thread.snippet, COALESCE(recipient.system_display_name, recipient.profile_joined_name, recipient.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id ORDER BY thread._id ASC", &results);
    // else
    //   d_database.exec("SELECT thread._id, COALESCE(recipient.phone, recipient.group_id, recipient.uuid) AS 'recipient_ids', thread.snippet, COALESCE(recipient.system_display_name, recipient.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id ORDER BY thread._id ASC", &results);
  }
  results.prettyPrint();
}
