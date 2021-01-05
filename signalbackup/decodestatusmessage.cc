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



/*
 * This uses the old(?) V1(?) group status update protobuf

Signal-Android/libsignal/service/src/main/proto/SignalService.proto:

message AttachmentPointer {
  enum Flags {
    VOICE_MESSAGE = 1;
  }

  optional fixed64 id          = 1;
  optional string  contentType = 2;
  optional bytes   key         = 3;
  optional uint32  size        = 4;
  optional bytes   thumbnail   = 5;
  optional bytes   digest      = 6;
  optional string  fileName    = 7;
  optional uint32  flags       = 8;
  optional uint32  width       = 9;
  optional uint32  height      = 10;
}
message GroupContext {
  enum Type {
    UNKNOWN      = 0;
    UPDATE       = 1;
    DELIVER      = 2;
    QUIT         = 3;
    REQUEST_INFO = 4;
  }
  optional bytes             id      = 1;
  optional Type              type    = 2;
  optional string            name    = 3;
  repeated string            members = 4;
  optional AttachmentPointer avatar  = 5;
}


 */


std::string SignalBackup::decodeStatusMessage(std::string const &body, long long int expiration, long long int type, std::string const &contactname) const
{

  //std::cout << "DECODING: " << std::endl << "  " << body << std::endl << "  " << expiration << std::endl
  //          << "  " << type << std::endl << "  " << contactname << std::endl;

  if (Types::isGroupUpdate(type))
  {
    if (Types::isOutgoing(type))
      return "You updated the group.";

    std::string result = contactname + " updated the group.";

    // decode msg
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
    return contactname + "is on Signal!";
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

  return body;
}
