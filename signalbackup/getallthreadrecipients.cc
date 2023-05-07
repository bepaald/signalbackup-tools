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

std::set<long long int> SignalBackup::getAllThreadRecipients(long long int t) const
{
  std::set<long long int> recipientlist;

  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT DISTINCT " + d_thread_recipient_id + " FROM thread WHERE _id = ? "
                       "UNION "
                       "SELECT DISTINCT " + d_mms_recipient_id + " FROM " + d_mms_table + " WHERE thread_id = ? " +
                       (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") ?
                        ("UNION "
                         "SELECT DISTINCT "s + "to_recipient_id" + " FROM " + d_mms_table + " WHERE thread_id = ? ") :
                        ""
                        ) +
                       "UNION "
                       "SELECT DISTINCT quote_author FROM " + d_mms_table + " WHERE thread_id = ? AND quote_id IS NOT 0 "
                       "UNION "
                       "SELECT DISTINCT author_id FROM reaction WHERE message_id IN (SELECT _id FROM " + d_mms_table + " WHERE thread_id = ?) "
                       "UNION "
                       "SELECT DISTINCT recipient_id FROM mention WHERE thread_id = ? "
                       , {t, t, t, t, t}, &results))
    std::cout << "error" << std::endl;

  //results.prettyPrint();

  // put results in vector...
  for (uint i = 0; i < results.rows(); ++i)
    if (!results.isNull(i, 0))
      recipientlist.insert(results.getValueAs<long long int>(i, 0));

  // check if thread is group
  std::string group_id;
  d_database.exec("SELECT group_id from recipient WHERE "
                  "_id IS (SELECT recipient_id FROM thread WHERE _id = ?) AND group_id IS NOT NULL", t, &results);
  if (results.rows() == 1)
    group_id = results.valueAsString(0, "group_id");

  if (!group_id.empty())
  {
    // get current group members and former v1 members
    std::vector<long long int> groupmembers;
    getGroupMembersOld(&groupmembers, group_id, "members");

    // for (long long int id : groupmembers)
    //   std::cout << "INSERTING (0): "  << id << std::endl;

    if (d_database.tableContainsColumn("groups", "former_v1_members"))
      getGroupMembersOld(&groupmembers, group_id, "former_v1_members");

    for (long long int id : groupmembers)
    {
      //std::cout << "INSERTING (1): "  << id << std::endl;
      recipientlist.insert(id);
    }

    // get other possible former group members by parsing group updates
    std::vector<long long int> group_update_recipients = getGroupUpdateRecipients(t);
    // append to recipientlist
    for (long long int id : group_update_recipients)
    {
      //std::cout << "INSERTING (2): " << id << std::endl;
      recipientlist.insert(id);
    }

    // get GV1 migration recipients...
    getGroupV1MigrationRecipients(&recipientlist, t);
  }
  return recipientlist;
}
