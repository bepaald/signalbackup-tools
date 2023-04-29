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
    return "You marked your safety number with " + contactname + " unverified from another device";
  }
  if (Types::isEndSession(type))
  {
    if (Types::isOutgoing(type))
      return "You reset the secure session.";
    return contactname + " reset the secure session.";
  }
  if (Types::isProfileChange(type))
  {
    return decodeProfileChangeMessage(body, contactname);
  }
  if (Types::isGroupUpdate(type) && Types::isGroupV2(type))
  {

    //std::cout << body << std::endl;
    DecryptedGroupV2Context groupv2ctx(body);
    //groupv2ctx.print();

    std::string statusmsg;

    if (groupv2ctx.getField<2>().has_value())
    {
      DecryptedGroupChange groupchange = groupv2ctx.getField<2>().value();
      //std::cout << bepaald::bytesToHexString(groupchange.data(), groupchange.size()) << std::endl;
      //groupchange.print();

      // check group title changed
      if (groupchange.getField<10>().has_value() &&
          groupchange.getField<10>().value().getField<1>().has_value())
      {
        statusmsg += (Types::isOutgoing(type) ? "You" : contactname) + " changed the group name to \"" + groupchange.getField<10>().value().getField<1>().value() + "\".";
      }

      // check group description changed
      if (groupchange.getField<20>().has_value()/* && groupchange.getField<20>().value().getField<1>().has_value()*/)
      {
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " changed the group description.";
      }

      // check group avatar changed
      if (groupchange.getField<11>().has_value()/* && groupchange.getField<11>().value().getField<1>().has_value()*/)
      {
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " changed the group avatar.";
      }

      // check group timer changed
      if (groupchange.getField<12>().has_value()/* && groupchange.getField<12>().value().getField<1>().has_value()*/)
      {
        uint32_t newexp = groupchange.getField<12>().value().getField<1>().value_or(0);
        std::string time;
        if (newexp == 0)
          time = "Off";
        else if (newexp < 60) // less than full minute
          time = bepaald::toString(newexp) + " second" + (newexp > 1 ? "s" : "");
        else if (newexp < 60 * 60) // less than full hour
          time = bepaald::toString(newexp / 60) + " minute" + ((newexp / 60) > 1 ? "s" : "");
        else if (newexp < 24 * 60 * 60) // less than full day
          time = bepaald::toString(newexp / (60 * 60)) + " hour" + ((newexp / (60 * 60)) > 1 ? "s" : "");
        else if (newexp < 7 * 24 * 60 * 60) // less than full week
          time = bepaald::toString(newexp / (24 * 60 * 60)) + " day" + ((newexp / (24 * 60 * 60)) > 1 ? "s" : "");
        else // show newexp in number of weeks
          time = bepaald::toString(newexp / (7 * 24 * 60 * 60)) + " week" + ((newexp / (7 * 24 * 60 * 60)) > 1 ? "s" : "");
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " set the disappearing message timer to " + time + ".";
      }

      // check new member:
      auto newmembers = groupchange.getField<3>();
      for (uint i = 0; i < newmembers.size(); ++i)
      {
        auto [uuid, uuid_size] = newmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
        std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        statusmsg += (Types::isOutgoing(type) ? "You" : contactname) + " added " + getNameFromUuid(uuidstr) + ".";
      }

      // check members left:
      auto deletedmembers = groupchange.getField<4>();
      for (uint i = 0; i < deletedmembers.size(); ++i) // I dont know how this can be more than size() == 1
      {
        auto [uuid, uuid_size] = deletedmembers[i]; // bytes
        std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " removed " + getNameFromUuid(uuidstr) + ".";
      }

      // check memberrole change
      auto memberrolechanges = groupchange.getField<5>();
      for (uint i = 0; i < memberrolechanges.size(); ++i) // I dont know how this can be more than size() == 1
      {
        DecryptedModifyMemberRole mr = memberrolechanges[i];

        std::string uuidstr;
        if (mr.getField<1>().has_value())
        {
          auto [uuid, uuid_size] = mr.getField<1>().value();
          uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
          uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        }
        /*
          message Member {
          enum Role {
          UNKNOWN       = 0;
          DEFAULT       = 1;
          ADMINISTRATOR = 2;
          }*/
        int newrole = mr.getField<2>().value_or(0);

        if (newrole == 2)
          statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " made " + getNameFromUuid(uuidstr) + " an admin.";
        else
          statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " revoked admin privileges from " + getNameFromUuid(uuidstr) + ".";
      }

      // // check members left:
      // auto deletedmembers = groupchange.getField<4>();
      // for (uint i = 0; i < deletedmembers.size(); ++i) // I dont know how this can be more than size() == 1
      // {
      //   auto [uuid, uuid_size] = deletedmembers[i]; // bytes
      //   std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      //   uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      //   statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " removed " + getNameFromUuid(uuidstr) + ".";
      // }
    }

    if (statusmsg.empty())
    {
      //std::cout << body << std::endl;
      //groupv2ctx.print();
      return "(group V2 update)";
    }
    return statusmsg;
  }

  if (type == Types::GV1_MIGRATION_TYPE)
  {
    if (body.empty())
      return "This group was updated to a New Group.";

    std::string b;
    // parse body, get number of id's before '|': if one
    //b = "A member couldn't be added to the New Group and has been invited to join."
    // if N
    //b = "N members couldn't be added to the New Group and have been invited to join."
    // parse body, get number of id's after '|': if one
    //b = "A member couldn't be added to the New Group and has been removed.";
    // if N
    //b = "N members couldn't be added to the New Group and have been removed.";
    unsigned int middlepos = body.find('|');
    int membersinvited = std::count(body.begin(), body.begin() + middlepos, ',') + (middlepos == 0 ? 0 : 1);
    int membersremoved = std::count(body.begin() + middlepos + 1, body.end(), ',') + (middlepos == body.size() - 1 ? 0 : 1);
    if (membersinvited == 1)
      b = "A member couldn't be added to the New Group and has been invited to join.";
    else if (membersinvited > 1)
      b = bepaald::toString(membersinvited) + " members couldn't be added to the New Group and have been invited to join.";
    if (membersremoved == 1)
      b = (b.empty() ? std::string() : "\n") + "A member couldn't be added to the New Group and has been removed.";
    else if (membersremoved > 1)
      b = (b.empty() ? "" : "\n") + bepaald::toString(membersremoved) + " members couldn't be added to the New Group and have been removed.";

    return b;
  }

  return body;
}

/*

type & 0x1f == 14 CHANGE_NUMBER_TYPE

689:app/src/main/res/values/strings.xml:1437:    <string name="MessageRecord_s_changed_their_phone_number">%1$s changed their phone number.</string>
701:app/src/main/res/values/strings.xml:3982:    <string name="ChangeNumber__your_phone_number_has_changed_to_s">Your phone number has been changed to %1$s</string>
*/
