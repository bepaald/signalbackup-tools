/*
    Copyright (C) 2021  Selwin van Dijk

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

/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

void SignalBackup::mergeRecipients(std::vector<std::string> const &addresses, bool editgroupmembers)
{
  std::cout << __FUNCTION__ << std::endl;

  if (addresses.size() < 2)
  {
    std::cout << "Too few addresses" << std::endl;
    return;
  }

  std::vector<std::string> r_ids = addresses;
  std::vector<std::string> phonenumbers = addresses;

  // for database version < 24, addresses = recipient_ids, for dataversion >= 24 addresses = recipient.phone
  // so convert to recipient._ids
  if (d_databaseversion >= 24)
  {
    for (uint i = 0; i < r_ids.size(); ++i)
    {
      SqliteDB::QueryResults res;
      d_database.exec("SELECT _id FROM recipient WHERE phone = ?", r_ids[i], &res);
      if (res.rows() != 1 || res.columns() != 1 ||
          !res.valueHasType<long long int>(0, 0))
      {
        std::cout << "Failed to find recipient._id matching phone/group_id in target database" << std::endl;
        return;
      }
      r_ids[i] = bepaald::toString(res.getValueAs<long long int>(0, 0));
    }
  }

  std::string targetaddr = r_ids.back();
  std::string targetphone = phonenumbers.back();

  // deal with one-on-one conversations:
  // get thread of target address
  long long int tid = getThreadIdFromRecipient(targetaddr);
  if (tid != -1)
  {

    // update all messages from this r_ids[i] to belong to that same thread and change address in new number
    for (uint i = 0; i < r_ids.size() - 1; ++i)
    {
      long long int oldtid = getThreadIdFromRecipient(r_ids[i]);
      if (oldtid == -1)
      {
        std::cout << "Failed to find thread for old address: " << r_ids[i] << std::endl;
        continue;
      }

      std::cout << "Dealing with address: " << r_ids[i] << std::endl;
      d_database.exec("UPDATE sms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?",
                      {tid, targetaddr, oldtid, r_ids[i]});
      std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
      d_database.exec("UPDATE mms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?",
                      {tid, targetaddr, oldtid, r_ids[i]});
      std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;

      if (d_database.containsTable("mention"))
      {
        d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {tid, oldtid});
        std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;
        d_database.exec("UPDATE mention SET recipient_id = ? WHERE recipient_id = ?", {targetaddr, r_ids[i]});
        std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;
      }

      d_database.exec("DELETE FROM thread WHERE " + d_thread_recipient_id + " = ?", r_ids[i]);
    }
  }
  else
  {
    std::cout << "Warning: no (one-on-one) thread with " << targetaddr << " found" << std::endl;
  }

  // change quote authors:
  for (uint i = 0; i < r_ids.size() - 1; ++i)
  {
    d_database.exec("UPDATE mms SET quote_author = ? WHERE quote_author = ?", {targetaddr, r_ids[i]});
    std::cout << "Updated " << d_database.changed() << " quotes in 'mms' table" << std::endl;
  }

  // update reaction authors
  if (d_database.tableContainsColumn("sms", "reactions"))
  {
    SqliteDB::QueryResults results;
    d_database.exec("SELECT _id, reactions FROM sms WHERE reactions IS NOT NULL", &results);
    bool changed = false;
    for (uint i = 0; i < results.rows(); ++i)
    {
      ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
      for (uint k = 0; k < reactions.numReactions(); ++k)
      {
        for (uint j = 0; j < r_ids.size() - 1; ++j)
          if (reactions.getAuthor(k) == bepaald::toNumber<uint64_t>(r_ids[j]))
          {
            reactions.setAuthor(k, bepaald::toNumber<uint64_t>(targetaddr));
            changed = true;
          }
        if (changed)
          d_database.exec("UPDATE sms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())), results.getValueAs<long long int>(i, "_id")});
      }
    }
  }
  if (d_database.tableContainsColumn("mms", "reactions"))
  {
    SqliteDB::QueryResults results;
    d_database.exec("SELECT _id, reactions FROM mms WHERE reactions IS NOT NULL", &results);
    bool changed = false;
    for (uint i = 0; i < results.rows(); ++i)
    {
      ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
      for (uint k = 0; k < reactions.numReactions(); ++k)
      {
        for (uint j = 0; j < r_ids.size() - 1; ++j)
          if (reactions.getAuthor(k) == bepaald::toNumber<uint64_t>(r_ids[j]))
          {
            reactions.setAuthor(k, bepaald::toNumber<uint64_t>(targetaddr));
            changed = true;
          }
        if (changed)
          d_database.exec("UPDATE mms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())), results.getValueAs<long long int>(i, "_id")});
      }
    }
  }

  // deal with groups
  SqliteDB::QueryResults results;
  d_database.exec("SELECT group_id,members,title FROM groups", &results); // get id,members and title from all groups
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (results.columns() != 3 ||
        !results.valueHasType<std::string>(i, 0) ||
        !results.valueHasType<std::string>(i, 1) ||
        !results.valueHasType<std::string>(i, 2))
    {
      std::cout << ":(" << std::endl;
      continue;
    }

    std::string id = results.getValueAs<std::string>(i, 0);
    std::string members = results.getValueAs<std::string>(i, 1);
    std::string title = results.getValueAs<std::string>(i, 2);

    std::cout << "Dealing with group: " << id << " (title: '" << title << "', members: " << members << ")" << std::endl;

    std::string recipient_id = id;
    if (d_databaseversion >= 24)
    {
      SqliteDB::QueryResults res;
      d_database.exec("SELECT _id FROM recipient WHERE group_id = ?", id, &res);
      if (res.rows() != 1 || res.columns() != 1 ||
          !res.valueHasType<long long int>(0, 0))
      {
        std::cout << "Failed to find recipient._id matching phone/group_id in target database" << std::endl;
        return;
      }
      recipient_id = bepaald::toString(res.getValueAs<long long int>(0, 0));
    }

    // get thread id for this group:
    tid = getThreadIdFromRecipient(recipient_id);
    if (tid == -1)
    {
      std::cout << "Failed to find thread for groupchat" << std::endl;
      continue;
    }

    // for all incoming messages of this group(= this thread), if the (originating) address = oldaddress, change it to target
    for (uint j = 0; j < r_ids.size() - 1; ++j)
    {
      d_database.exec("UPDATE sms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, r_ids[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
      d_database.exec("UPDATE mms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, r_ids[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;
    }



    if (editgroupmembers)
    {
      for (uint j = 0; j < r_ids.size() - 1; ++j)
      {
        // change current member list in group database:
        //std::cout << "  GROUP MEMBERS BEFORE: " << members << std::endl;
        std::string::size_type pos = std::string::npos;
        if ((pos = members.find(r_ids[j])) != std::string::npos)
        {
          std::cout << "  GROUP MEMBERS BEFORE: " << members << std::endl;
          //std::cout << "  FOUND ADDRESS TO CHANGE" << std::endl;
          // remove address
          members.erase(pos, r_ids[j].length());

          // remove left over comma
          if (members[0] == ',') // if removed was first
            members.erase(0, 1);
          else if (members.back() == ',') // if removed was last
            members.erase(members.size() - 1, 1);
          else // if removed was middle
            members.erase(std::unique(members.begin(), members.end(), [](char c1, char c2){ return c1 == ',' && c2 == ',';}), members.end());

          if (members.find(targetaddr) == std::string::npos) // else target already in memberlist
            members += "," + targetaddr;

          d_database.exec("UPDATE groups SET members = ? WHERE group_id = ?", {members, recipient_id});
          std::cout << "  GROUP MEMBERS AFTER : " << members << std::endl;
        }
      }
    }

    // get status message updates:
    SqliteDB::QueryResults results2;
    d_database.exec("SELECT type,body,_id FROM 'sms' WHERE thread_id = " + bepaald::toString(tid) + " AND (type & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + " IS NOT 0)", &results2);
    results2.prettyPrint();
    for (uint j = 0; j < results2.rows(); ++j)
    {
      std::string body = std::any_cast<std::string>(results2.value(j, "body"));
      long long int type = std::any_cast<long long int>(results2.value(j, "type"));
      long long int msgid = std::any_cast<long long int>(results2.value(j, "_id"));

      GroupContext statusmsg(body);

      if (Types::isGroupUpdate(type))
        std::cout << "Handling group update " << j + 1 << std::endl;

      bool targetpresent = false;
      auto field4 = statusmsg.getField<4>();
      for (uint k = 0; k < field4.size(); ++k)
      {
        std::cout << "memberlist: " << field4[k] << std::endl;
        if (field4[k] == targetphone)
          targetpresent = true;
      }

      int removed = 0;
      for (uint k = 0; k < phonenumbers.size() - 1; ++k)
      {
        removed = statusmsg.deleteFields(4, &phonenumbers[k]);
        std::cout << "deleted " << removed << " members from group update message" << std::endl;
      }

      if (removed)
      {
        if (!targetpresent) // add target if not present
          statusmsg.addField<4>(targetphone);
        // set body
        d_database.exec("UPDATE sms SET body = ? WHERE _id = ?", {statusmsg.getDataString(), msgid});
        std::cout << "Updated " << d_database.changed() << " group updates in 'sms' table" << std::endl;
      }
    }



    // same for status updates in mms database
    d_database.exec("SELECT msg_box,body,_id FROM 'mms' WHERE thread_id = " + bepaald::toString(tid) + " AND (msg_box & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + " IS NOT 0)", &results2);

    results2.prettyPrint();
    for (uint j = 0; j < results2.rows(); ++j)
    {
      std::string body = std::any_cast<std::string>(results2.value(j, "body"));
      long long int type = std::any_cast<long long int>(results2.value(j, "msg_box"));
      long long int msgid = std::any_cast<long long int>(results2.value(j, "_id"));

      GroupContext statusmsg(body);

      if (Types::isGroupUpdate(type))
        std::cout << "Handling group update " << j + 1 << std::endl;

      bool targetpresent = false;
      auto field4 = statusmsg.getField<4>();
      for (uint k = 0; k < field4.size(); ++k)
      {
        std::cout << "memberlist: " << field4[k] << std::endl;
        if (field4[k] == targetphone)
          targetpresent = true;
      }

      int removed = 0;
      for (uint k = 0; k < phonenumbers.size() - 1; ++k)
      {
        removed = statusmsg.deleteFields(4, &phonenumbers[k]);
        std::cout << "deleted " << removed << " members from group update message" << std::endl;
      }

      if (removed)
      {
        if (!targetpresent) // add target if not present
          statusmsg.addField<4>(targetphone);
        // set body
        d_database.exec("UPDATE mms SET body = ? WHERE _id = ?", {statusmsg.getDataString(), msgid});
        std::cout << "Updated " << d_database.changed() << " group updates in 'mms' table" << std::endl;
      }
    }
  }
}
