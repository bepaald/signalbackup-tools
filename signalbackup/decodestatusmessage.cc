/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

std::string SignalBackup::decodeStatusMessage(std::string const &body, long long int expiration, long long int type, std::string const &contactname) const
{

  // std::cout << "DECODING: " << std::endl << "  " << body << std::endl << "  " << expiration << std::endl
  //           << "  " << type << std::endl << "  " << contactname << std::endl;
  // std::cout << Types::isGroupUpdate(type) << std::endl;
  // std::cout << Types::isGroupV2(type) << std::endl;

  if (Types::isGroupUpdate(type) && !Types::isGroupV2(type))
  {
    if (Types::isOutgoing(type))
      return "You updated the group.";

    std::string result = contactname + " updated the group.";

    GroupContext statusmsg(body);

    std::string members;
    auto field4 = statusmsg.getField<4>();
    if (field4.size())
    {
      for (uint k = 0; k < field4.size(); ++k)
      {
        // get name from members string
        SqliteDB::QueryResults res;
        if (d_databaseversion >= 24)
          d_database.exec("SELECT COALESCE(recipient.system_display_name, recipient.signal_profile_name) AS 'name' FROM recipient WHERE phone = ?", field4[k], &res);
        else
          d_database.exec("SELECT COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name) AS 'name' FROM recipient_preferences WHERE recipient_preferences.recipient_ids = ?", field4[k], &res);

        std::string name = field4[k];
        if (res.rows() == 1 && res.columns() == 1 && res.valueHasType<std::string>(0, "name"))
          name = res.getValueAs<std::string>(0, "name");

        members += name;
        if (k < field4.size() - 1)
          members += ", ";
      }
    }
    if (!members.empty())
      result += "\n" + members + " joined the group.";

    std::string title = statusmsg.getField<3>().value_or(std::string());
    if (!title.empty())
    {
      result += (!members.empty() ? ' ' : '\n');
      result += "Group name is now '" + title + "'.";
    }
    return result;
  }
  if (Types::isGroupQuit(type))
  {
    if (Types::isOutgoing(type))
      return "You have left the group.";
    return contactname + "has left the group.";
  }
  if (Types::isIncomingCall(type))
    return contactname + "called you";
  if (Types::isOutgoingCall(type))
    return "You called";
  if (Types::isMissedCall(type))
    return "Missed call";
  if (Types::isJoined(type))
    return contactname + " is on Signal!";
  if (Types::isExpirationTimerUpdate(type))
  {
    if (expiration <= 0)
    {
      if (Types::isOutgoing(type))
        return "You disabled disappearing messages.";
      return contactname + " disabled disappearing messages.";
    }

    std::string time;
    if (expiration < 60) // less than full minute
      time = bepaald::toString(expiration) + " seconds";
    else if (expiration < 60 * 60) // less than full hour
      time = bepaald::toString(expiration / 60) + " minutes";
    else if (expiration < 24 * 60 * 60) // less than full day
      time = bepaald::toString(expiration / (60 * 60)) + " hours";
    else if (expiration < 7 * 24 * 60 * 60) // less than full week
      time = bepaald::toString(expiration / (24 * 60 * 60)) + " days";
    else // show expiration in number of weeks
      time = bepaald::toString(expiration / (7 * 24 * 60 * 60)) + " weeks";

    if (Types::isOutgoing(type))
      return "You set the disappearing message timer to " + time;
    return contactname + " set the disappearing message timer to " + time;
  }
  if (Types::isIdentityUpdate(type))
    return "Your safety number with " + contactname + " has changed.";
  if (Types::isIdentityVerified(type))
  {
    if (Types::isOutgoing(type))
      return "You marked your safety number with " + contactname + " verified";
    return "You marked your safety number with " + contactname + " verified from another device";
  }
  if (Types::isIdentityDefault(type))
  {
    if (Types::isOutgoing(type))
      return "You marked your safety number with " + contactname + " unverified";
    return "You marked your safety number with " + contactname + " verified from another device";
  }
  if (Types::isProfileChange(type))
  {
    return decodeProfileChangeMessage(body, contactname);
  }
  if (Types::isGroupUpdate(type) && Types::isGroupV2(type))
  {
    /*
    //std::cout << body << std::endl;
    DecryptedGroupV2Context groupv2ctx(body);
    //groupv2ctx.print();

    if (groupv2ctx.getField<2>().has_value())
    {
      DecryptedGroupChange groupchange = groupv2ctx.getField<2>().value();
      std::cout << bepaald::bytesToHexString(groupchange.data(), groupchange.size()) << std::endl;
      groupchange.print();

      // check group title changed
      if (groupchange.getField<10>().has_value() &&
          groupchange.getField<10>().value().getField<1>().has_value())
      {
        std::cout << "NEW TITLE: " << groupchange.getField<10>().value().getField<1>().value() << std::endl; // string
        return "Group name is now '" + groupchange.getField<10>().value().getField<1>().value() + "'.";
      }

      // check group avatar changed
      if (groupchange.getField<11>().has_value() &&
          groupchange.getField<11>().value().getField<1>().has_value())
      {
        std::cout << "NEW AVATAR: " << groupchange.getField<11>().value().getField<1>().value() << std::endl; // string
        return "NEW AVATAR: " + groupchange.getField<11>().value().getField<1>().value();
      }

      // check group timer changed
      if (groupchange.getField<12>().has_value() &&
          groupchange.getField<12>().value().getField<1>().has_value())
      {
        std::cout << "NEW TIMER: " << groupchange.getField<12>().value().getField<1>().value() << std::endl; // uint32
        return "NEW TIMER: " + bepaald::toString(groupchange.getField<12>().value().getField<1>().value());
      }

      // check new member:
      auto newmembers = groupchange.getField<3>();
      for (uint i = 0; i < newmembers.size(); ++i)
      {
        auto [uuid, uuid_size] = newmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
        std::cout << "NEW MEMBER: " << bepaald::bytesToHexString(uuid, uuid_size) << std::endl;
        return "NEW MEMBER: " + bepaald::bytesToHexString(uuid, uuid_size);
      }

      // check members left:
      auto deletedmembers = groupchange.getField<4>();
      for (uint i = 0; i < deletedmembers.size(); ++i)
      {
        auto [uuid, uuid_size] = deletedmembers[i]; // bytes
        std::cout << "DELETED MEMBER: " << bepaald::bytesToHexString(uuid, uuid_size) << std::endl;
        return "DELETED MEMBER: " + bepaald::bytesToHexString(uuid, uuid_size);
      }

      std::cout << "" << std::endl;
    }
    */
    return "(group V2 update)";
  }


  return body;
}
