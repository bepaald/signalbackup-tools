/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SignalBackup::mergeGroups(std::vector<std::string> const &groupids)
{
  if (groupids.size() < 2)
  {
    Logger::error("Too few addresses");
    return;
  }

  Logger::message("\nTHIS FUNCTION MAY NEED UPDATING. PLEASE OPEN AN ISSUE\n"
                  "IF YOU NEED IT.\n");

  std::string targetgroup = groupids.back();

  SqliteDB::QueryResults res;
  std::set<std::string> targetmembersvec;
  if (d_database.tableContainsColumn("groups", "members"))
  {
    d_database.exec("SELECT members FROM groups WHERE group_id = ?", targetgroup, &res);
    std::string targetmembers = res.getValueAs<std::string>(0, 0);
    std::stringstream ss(targetmembers);
    while (ss.good())
    {
      std::string substr;
      std::getline(ss, substr, ',');
      targetmembersvec.insert(substr);
    }
  }
  else
  {
    d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id = ?", targetgroup, &res);
    for (uint i = 0; i < res.rows(); ++i)
      targetmembersvec.insert(res.valueAsString(i, 0));
  }


  // get the thread_id of the target
  long long int tid = getThreadIdFromRecipient(targetgroup);

  if (tid != -1)
  {

    // update all messages from this addresses[i] to belong to that same thread and change address in new number
    for (uint i = 0; i < groupids.size() - 1; ++i)
    {
      long long int oldtid = getThreadIdFromRecipient(groupids[i]);
      if (oldtid == -1)
      {
        Logger::error("Failed to find thread for old group: ", groupids[i]);
        continue;
      }

      Logger::message("Dealing with group: ", groupids[i]);
      if (d_database.containsTable("sms"))
      {
        d_database.exec("UPDATE sms SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
        Logger::message("Updated ", d_database.changed(), " entries in 'sms' table");
        d_database.exec("UPDATE sms SET " + d_sms_recipient_id + " = ? WHERE " + d_sms_recipient_id + " = ?",
                        {targetgroup, groupids[i]});
        Logger::message("Updated ", d_database.changed(), " entries in 'sms' table");
      }
      d_database.exec("UPDATE " + d_mms_table + " SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
      Logger::message("Updated ", d_database.changed(), " entries in 'mms' table");

      if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id")) // < dbv185
      {
        d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_recipient_id + " = ? WHERE " + d_mms_recipient_id + " = ?",
                        {targetgroup, groupids[i]});
        Logger::message("Updated ", d_database.changed(), " entries in 'mms' table");
      }
      else
      {
        // adjust to_recipient (= group id on outgoing messages)
        d_database.exec("UPDATE " + d_mms_table + " SET to_recipient_id = ? WHERE to_recipient_id = ?",
                        {targetgroup, groupids[i]});
        Logger::message("Updated ", d_database.changed(), " entries in 'mms' table");
      }

      if (d_database.containsTable("mention"))
      {
        d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
        Logger::message("Updated ", d_database.changed(), " entries in 'sms' table");
      }

      if (d_database.containsTable("msl_recipient"))
        d_database.exec("UPDATE msl_recipient SET recipient_id = ? WHERE recipient_id = ?", {targetgroup, groupids[i]});

      if (d_database.containsTable("reaction")) // dbv >= 121
        d_database.exec("UPDATE reaction SET author_id = ? WHERE author_id = ?", {targetgroup, groupids[i]});

      // delete old (now empty) thread
      d_database.exec("DELETE FROM thread WHERE " + d_thread_recipient_id + " = ?", groupids[i]);
      Logger::message("Removed ", d_database.changed(), " threads from table");

      // get members of groupids[i] and merge them into targetgroup
      if (d_database.tableContainsColumn("groups", "members"))
      {
        d_database.exec("SELECT members FROM groups WHERE group_id = ?", groupids[i], &res);
        std::string members = res.getValueAs<std::string>(0, 0);
        std::stringstream ss2(members);
        while (ss2.good())
        {
          std::string substr;
          std::getline(ss2, substr, ',');
          auto [it, inserted] = targetmembersvec.insert(substr);
          if (inserted)
            Logger::message("Added ", substr, " to memberlist of group");
          else
            Logger::message("Skipped adding ", substr, " to group: already a member");
        }
      }
      else
      {
        d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id = ?", groupids[i], &res);
        for (uint g = 0; g < res.rows(); ++g)
          d_database.exec("INSERT OR IGNORE INTO group_membership (group_id, recipient_id) VALUES (?, ?)", {targetgroup, res.getValueAs<long long int>(g, "recipient_id")});
      }

      // delete the merged group
      d_database.exec("DELETE FROM groups WHERE group_id = ?", groupids[i]);
      Logger::message("Removed ", d_database.changed(), " groups from table");

      if (d_database.containsTable("group_membership"))
        d_database.exec("DELETE FROM group_membership WHERE group_id = ?", groupids[i]);
    }

    // set new member list
    if (d_database.tableContainsColumn("groups", "members"))
    {
      Logger::message("Setting new memberlist");
      std::string newmemberlist;
      for (auto const &it : targetmembersvec)
        newmemberlist += it + ',';
      newmemberlist.pop_back(); // remove trailing comma...
      d_database.exec("UPDATE groups SET members = ? WHERE group_id = ?", {newmemberlist, targetgroup});
    }
  }
  else
  {
    Logger::warning("No group thread with id ", tid, " found");
  }
  updateThreadsEntries();
}
