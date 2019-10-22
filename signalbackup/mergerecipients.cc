/*
    Copyright (C) 2019  Selwin van Dijk

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
  if (addresses.size() < 2)
  {
    std::cout << "Too few addresses" << std::endl;
    return;
  }

  std::string targetaddr = addresses.back();

  // deal with one-on-one conversations:
  // get thread of target address
  long long int tid = getThreadIdFromRecipient(targetaddr);
  if (tid != -1)
  {

    // update all messages from this addresses[i] to belong to that same thread and change address in new number
    for (uint i = 0; i < addresses.size() - 1; ++i)
    {
      long long int oldtid = getThreadIdFromRecipient(addresses[i]);
      if (oldtid == -1)
      {
        std::cout << "Failed to find thread for old address: " << addresses[i] << std::endl;
        continue;
      }

      std::cout << "Dealing with address: " << addresses[i] << std::endl;
      d_database.exec("UPDATE sms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?", {tid, targetaddr, oldtid, addresses[i]});
      std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
      d_database.exec("UPDATE mms SET thread_id = ?, address = ? WHERE thread_id = ? AND address = ?", {tid, targetaddr, oldtid, addresses[i]});
      std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;

      d_database.exec("DELETE FROM thread WHERE recipient_ids = ?", addresses[i]);
    }
  }
  else
  {
    std::cout << "Warning: no (one-on-one) thread with " << targetaddr << " found" << std::endl;
  }

  // deal with groups
  SqliteDB::QueryResults results;
  d_database.exec("SELECT group_id,members,title FROM groups", &results);
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

    // get thread id for this group:
    tid = getThreadIdFromRecipient(id);
    if (tid == -1)
    {
      std::cout << "Failed to find thread for groupchat" << std::endl;
      continue;
    }

    for (uint j = 0; j < addresses.size() - 1; ++j)
    {
      d_database.exec("UPDATE sms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, addresses[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'sms' table" << std::endl;
      d_database.exec("UPDATE mms SET address = ? WHERE address = ? AND thread_id = ?", {targetaddr, addresses[j], tid});
      std::cout << "Updated " << d_database.changed() << " entries in 'mms' table" << std::endl;
    }



    if (editgroupmembers)
    {
      for (uint j = 0; j < addresses.size() - 1; ++j)
      {
        // change current member list in group database:
        //std::cout << "  GROUP MEMBERS BEFORE: " << members << std::endl;
        std::string::size_type pos = std::string::npos;
        if ((pos = members.find(addresses[j])) != std::string::npos)
        {
          std::cout << "  GROUP MEMBERS BEFORE: " << members << std::endl;
          //std::cout << "  FOUND ADDRESS TO CHANGE" << std::endl;
          // remove address
          members.erase(pos, addresses[j].length());

          // remove left over comma
          if (members[0] == ',') // if removed was first
            members.erase(0, 1);
          else if (members.back() == ',') // if removed was last
            members.erase(members.size() - 1, 1);
          else // if removed was middle
            members.erase(std::unique(members.begin(), members.end(), [](char c1, char c2){ return c1 == ',' && c2 == ',';}), members.end());

          if (members.find(targetaddr) == std::string::npos) // else target already in memberlist
            members += "," + targetaddr;

          d_database.exec("UPDATE groups SET members = ? WHERE group_id = ?", {members, id});
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

      ProtoBufParser<protobuffer::optional::BYTES,
                     protobuffer::optional::ENUM,
                     protobuffer::optional::STRING,
                     protobuffer::repeated::STRING,
                     ProtoBufParser<protobuffer::optional::FIXED64,
                                    protobuffer::optional::STRING,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::STRING,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::UINT32>> statusmsg(body);


      if (Types::isGroupUpdate(type))
        std::cout << j << " GROUP UPDATE" << std::endl;

      bool targetpresent = false;
      auto field4 = statusmsg.getField<4>();
      for (uint k = 0; k < field4.size(); ++k)
      {
        std::cout << "memberlist: " << field4[k] << std::endl;
        if (field4[k] == targetaddr)
          targetpresent = true;
      }

      int removed = 0;
      for (uint k = 0; k < addresses.size() - 1; ++k)
      {
        removed = statusmsg.deleteFields(4, &addresses[k]);
        std::cout << "deleted " << removed << " members from group update message" << std::endl;
      }

      if (removed)
      {
        if (!targetpresent) // add target if not present
          statusmsg.addField<4>(targetaddr);
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

      ProtoBufParser<protobuffer::optional::BYTES,
                     protobuffer::optional::ENUM,
                     protobuffer::optional::STRING,
                     protobuffer::repeated::STRING,
                     ProtoBufParser<protobuffer::optional::FIXED64,
                                    protobuffer::optional::STRING,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::BYTES,
                                    protobuffer::optional::STRING,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::UINT32,
                                    protobuffer::optional::UINT32>> statusmsg(body);


      if (Types::isGroupUpdate(type))
        std::cout << j << " GROUP UPDATE" << std::endl;

      bool targetpresent = false;
      auto field4 = statusmsg.getField<4>();
      for (uint k = 0; k < field4.size(); ++k)
      {
        std::cout << "memberlist: " << field4[k] << std::endl;
        if (field4[k] == targetaddr)
          targetpresent = true;
      }

      int removed = 0;
      for (uint k = 0; k < addresses.size() - 1; ++k)
      {
        removed = statusmsg.deleteFields(4, &addresses[k]);
        std::cout << "deleted " << removed << " members from group update message" << std::endl;
      }

      if (removed)
      {
        if (!targetpresent) // add target if not present
          statusmsg.addField<4>(targetaddr);
        // set body
        d_database.exec("UPDATE mms SET body = ? WHERE _id = ?", {statusmsg.getDataString(), msgid});
        std::cout << "Updated " << d_database.changed() << " group updates in 'mms' table" << std::endl;
      }
    }
  }
}
