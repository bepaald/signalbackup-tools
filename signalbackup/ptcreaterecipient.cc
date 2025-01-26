/*
  Copyright (C) 2025  Selwin van Dijk

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

#include "../signalplaintextbackupdatabase/signalplaintextbackupdatabase.h"

long long int SignalBackup::ptCreateRecipient(std::unique_ptr<SignalPlaintextBackupDatabase> const &ptdb,
                                              std::map<std::string, long long int> *contactmap,
                                              bool *warned_createcontacts, std::string const &contact_name,
                                              std::string const &address, bool isgroup) const
{
  // createcontact:
  if (*warned_createcontacts == false)
  {
    Logger::warning("Chat partner was not found in recipient-table. Attempting to create.");
    Logger::warning_indent(Logger::Control::BOLD, "NOTE THE RESULTING BACKUP CAN MOST LIKELY NOT BE RESTORED");
    Logger::warning_indent("ON SIGNAL ANDROID. IT IS ONLY MEANT TO EXPORT TO HTML.", Logger::Control::NORMAL);
    *warned_createcontacts = true;
  }

  if (contact_name.empty()) [[unlikely]]
    Logger::warning("Failed to get name for new contact (", address, ")");

  if (isgroup)
  {
    SqliteDB::QueryResults group_members;
    ptdb->d_database.exec("WITH members AS "
                          "("
                          "  SELECT DISTINCT sourceaddress FROM smses WHERE address = ?"
                          ")"
                          "SELECT DISTINCT address,contact_name FROM smses WHERE address IN members", address, &group_members);
    //std::cout << "Address: " << address << std::endl;
    //group_members.prettyPrint(false);

    for (unsigned int i = 0; i < group_members.rows(); ++i)
    {
      if (!bepaald::contains(contactmap, group_members(i, "address")))
      {
        //std::cout << "Need to create : " << group_members(i, "address") << std::endl;
        if (ptCreateRecipient(ptdb, contactmap, warned_createcontacts, group_members(i, "contact_name"),
                              group_members(i, "address"), false) == -1)
          return -1;
        // else
        //   std::cout << "Created contact: " << group_members(i, "address") << std::endl;
      }
    }

    d_database.exec("BEGIN TRANSACTION"); // things could still go bad...

    // create recipient for group
    std::any group_rid;
    std::string group_id = "__signal_group__fake__" + address;
    if (!insertRow("recipient",
                   {{"group_id", group_id},
                    {d_recipient_type, 3}}, // group type
                   "_id", &group_rid))
    {
      Logger::error("Failed to insert new (group) recipient into database.");
      return -1;
    }
    if (group_rid.type() != typeid(long long int)) [[unlikely]]
    {
      Logger::error("New (group) recipient _id has unexpected type.");
      return -1;
    }
    long long int new_group_rid = std::any_cast<long long int>(group_rid);

    // create group
    if (!insertRow("groups",
                   {{"title", contact_name},
                    {"group_id", group_id},
                    {"recipient_id", new_group_rid},
                    {"avatar_id", 0},
                    {"revision", 0}}))
    {
      Logger::error("Failed to insert new group into database.");
      d_database.exec("ROLLBACK TRANSACTION");
      return -1;
    }

    // set group members
    for (unsigned int i = 0; i < group_members.rows(); ++i)
    {
      //std::cout << "Create group membership (" << (i + 1) << ")" << std::endl;
      long long int member_rid = contactmap->at(group_members(i, "address")); // they should all exist at this point.
      if (!insertRow("group_membership",
                     {{"group_id", group_id},
                      {"recipient_id", member_rid}}))
      {
        Logger::error("Failed to set new groups membership.");
        d_database.exec("ROLLBACK TRANSACTION");
        return -1;
      }
    }
    d_database.exec("COMMIT TRANSACTION");
    return new_group_rid;
  }

  // else : NOT A GROUP

  std::any new_rid;
  long long int rid = -1;
  insertRow("recipient",
            {{d_recipient_profile_given_name, contact_name},
             {"profile_joined_name", contact_name},
             {d_recipient_e164, address}}, "_id", &new_rid);
  if (new_rid.type() == typeid(long long int)) [[likely]]
    rid = std::any_cast<long long int>(new_rid);

  if (rid == -1) [[unlikely]]
  {
    Logger::warning("Failed to create missing recipient. Skipping message.");
    return rid;
  }
  contactmap->emplace(address, rid);
  return rid;
}
