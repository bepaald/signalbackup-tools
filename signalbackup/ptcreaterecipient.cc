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

#if __cpp_lib_bitops >= 201907L
#include <bit>
#endif


long long int SignalBackup::ptCreateRecipient(std::unique_ptr<SignalPlaintextBackupDatabase> const &ptdb,
                                              std::map<std::string, long long int> *contactmap,
                                              bool *warned_createcontacts, std::string const &contact_name,
                                              std::string const &address, bool isgroup) const
{

  Logger::message("Creating recipient for address ", makePrintable(address), " (group: ", std::boolalpha, isgroup, ")");

  auto random_from_address = [](std::string const &a)
  {
    unsigned int result = 0;
    for (auto c : a)
#if __cpp_lib_bitops >= 201907L
      result = std::rotl(result, 3) ^ static_cast<int>(c);
#else
      result = ((result << 3) | (result >> (sizeof(int) - 3))) ^ static_cast<int>(c); // for (value in data) hash = hash.rotateLeft(3) xor value.toInt()
#endif
    return result;
  };

  // createcontact:
  if (*warned_createcontacts == false)
  {
    Logger::warning("Chat partner was not found in recipient-table. Attempting to create.");
    Logger::warning_indent(Logger::Control::BOLD, "NOTE THE RESULTING BACKUP CAN MOST LIKELY NOT BE RESTORED");
    Logger::warning_indent("ON SIGNAL ANDROID. IT IS ONLY MEANT TO EXPORT TO HTML.", Logger::Control::NORMAL);
    *warned_createcontacts = true;
  }

  if (contact_name.empty()) [[unlikely]]
    Logger::warning("Failed to get name for new contact (", makePrintable(address), ")");

  if (isgroup)
  {
    // std::cout << "GROUP: " << address << std::endl;
    // std::cout << "NAME:  " << contact_name << std::endl;
    // ptdb->d_database.prettyPrint(false, "SELECT DISTINCT sourceaddress FROM smses WHERE address = ?", address);
    // ptdb->d_database.prettyPrint(false, "SELECT DISTINCT value FROM smses, json_each(targetaddresses) WHERE smses.address = ?", address);

    std::set<std::string> group_members;
    SqliteDB::QueryResults group_members_res;
    ptdb->d_database.exec("SELECT DISTINCT sourceaddress FROM smses WHERE address = ?", address, &group_members_res);
    for (unsigned int i = 0; i < group_members_res.rows(); ++i)
    {
      if (group_members_res.isNull(i, "sourceaddress")) [[unlikely]]
        Logger::warning("Got 'NULL' sourceaddress in group '", makePrintable(address), "'");
      else
        group_members.insert(group_members_res(i, "sourceaddress"));
    }
    ptdb->d_database.exec("SELECT DISTINCT value FROM smses, json_each(targetaddresses) WHERE smses.address = ?", address, &group_members_res);
    for (unsigned int i = 0; i < group_members_res.rows(); ++i)
    {
      if (group_members_res.isNull(i, "value")) [[unlikely]]
        Logger::warning("Got 'NULL' targetaddress in group '", makePrintable(address), "'");
      else
        group_members.insert(group_members_res(i, "value"));
    }

    if (d_verbose) [[unlikely]]
    {
      Logger::message("ALL GROUP MEMBERS:");
      for (auto const &gm : group_members)
        Logger::message(makePrintable(gm));
    }

    // ensure all group members exist.
    for (auto const &gm : group_members)
    {
      if (!bepaald::contains(contactmap, gm))
      {
        std::string cn = ptdb->d_database.getSingleResultAs<std::string>("SELECT contact_name FROM smses WHERE address = ? LIMIT 1",
                                                                         gm, std::string());
        if (cn.empty()) [[unlikely]]
        {
          Logger::warning("Unexpectedly got empty contact name for group recipient '", gm, "'");//makePrintable(gm));
          cn = "(unknown)";
        }

        //std::cout << "Need to create group member(2): " << group_members(i, "address") << std::endl;
        if (ptCreateRecipient(ptdb, contactmap, warned_createcontacts, cn, gm, false) == -1)
        {
          Logger::error("Failed to create group member (", makePrintable(gm), ")");
          return -1;
        }
        // else
        //   std::cout << "Created contact: " << group_members(i, "address") << std::endl;
      }
      else
        if (d_verbose) [[unlikely]]
          Logger::message("Address already present in contactmap: ", makePrintable(gm));
    }

    d_database.exec("BEGIN TRANSACTION"); // things could still go bad...

    // create recipient for group
    std::any group_rid;
    std::string group_id = "__signal_group__fake__" + address;
    std::string color = s_html_random_colors[random_from_address(address) % s_html_random_colors.size()].first;
    if (!insertRow("recipient",
                   {{"group_id", group_id},
                    {d_recipient_avatar_color, color},
                    {d_recipient_type, 3}}, // group type
                   "_id", &group_rid))
    {
      Logger::error("Failed to insert new (group) recipient into database.");
      return -1;
    }
    if (group_rid.type() != typeid(long long int)) [[unlikely]]
    {
      Logger::error("New (group) recipient _id has unexpected type.");
      d_database.exec("ROLLBACK TRANSACTION");
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
    for (auto const &gm : group_members)
    {
      //std::cout << "Create group membership (" << (i + 1) << ")" << std::endl;
      long long int member_rid = contactmap->at(gm); // they should all exist at this point.
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

    contactmap->emplace(address, new_group_rid);
    return new_group_rid;
  }

  // else : NOT A GROUP

  std::any new_rid;
  long long int rid = -1;
  std::string color = s_html_random_colors[random_from_address(contact_name) % s_html_random_colors.size()].first;
  insertRow("recipient",
            {{d_recipient_profile_given_name, contact_name},
             {"profile_joined_name", contact_name},
             {d_recipient_avatar_color, color},
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
