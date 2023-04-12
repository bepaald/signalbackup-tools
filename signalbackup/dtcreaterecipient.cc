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

long long int SignalBackup::dtCreateRecipient(SqliteDB const &ddb, std::string const &id, std::string const &phone,
                                              std::map<std::string, long long int> *recipient_info) const
{
  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT type, name, profileName, profileFamilyName, "
                "profileFullName, e164, uuid, json_extract(json,'$.color') AS color, "
                "json_extract(json, '$.profileAvatar.path') AS avatar, "
                "groupId, members FROM conversations WHERE uuid = ? OR e164 = ?",
                {id, phone}, &res))
    return -1;
  //res.prettyPrint();

  if (res.rows() != 1)
    return -1;

  if (res("type") == "group")
  {
    // not yet
    return -1;
  }

  std::any new_rid;
  if (!insertRow("recipient",
                 {{"signal_profile_name", res.value(0, "profileName")},
                  {"profile_family_name", res.value(0, "profileFamilyName")},
                  {"profile_joined_name", res.value(0, "profileFullName")},
                  {"phone", res.value(0, "e164")},
                  {"uuid", res.value(0, "uuid")},
                  {"color", res.value(0, "color")}}, "_id", &new_rid))
  {
    // message
    return -1;
  }
  else
  {
    if (new_rid.type() != typeid(long long int))
      return -1;
    long long int new_rec_id = std::any_cast<long long int>(new_rid);
    (*recipient_info)[id.empty() ? phone : id] = new_rec_id;

    // set avatar
    std::string avatarpath = res("avatar");
    //if (!avatarpath.empty() &&
    // not starts with "images/"?? ")
    //{
    //  d_avatars.something...
    //}

    return new_rec_id;
  }

  return -1;
}

/*
ANDROID

system_display_name =
signal_profile_name = John
profile_family_name = Doe
profile_joined_name = John Doe
              title =
              phone = +316xxxxxxxx
               uuid = 9372xxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
           username =
              color = A110


DESKTOP

                type = private
         profileName = John
   profileFamilyName = Doe
     profileFullName = John Doe
                e164 = +316xxxxxxxx
                uuid = 9372xxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
       json'$.color' = A130



ANDROID

system_display_name =
profile_joined_name =
signal_profile_name =
              title = Test group
              phone =
               uuid =
           username =
              color = A120
           group_id = __signal_group__v2__!e57ccxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


DESKTOP

                type = group
                name = Test group
         profileName =
   profileFamilyName =
     profileFullName =
                e164 =
                uuid =
       json'$.color' = A130
             groupId = 5XzCxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
             members = 0d70xxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 9372xxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 09efxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx ???
             (note: $ echo 5XzCxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | base64 -d | xxd -plain -c 0
                    e57cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx)
*/
