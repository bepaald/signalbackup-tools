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

bool SignalBackup::dtUpdateProfile(SqliteDB const &ddb, std::string const &dtid,
                                   long long int aid, std::string const &databasedir)
{
  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT type, name, profileName, IFNULL(profileFamilyName, '') AS profileFamilyName, profileFullName, "
                "IFNULL(json_extract(json,'$.groupVersion'), 1) AS groupVersion, "
                "COALESCE(json_extract(json, '$.profileAvatar.path'),json_extract(json, '$.avatar.path')) AS avatar " // 'profileAvatar' for persons, 'avatar' for groups
                "FROM conversations WHERE uuid = ? OR e164 = ? OR groupId = ?",
                {dtid, dtid, dtid}, &res))
    return false;

  // check if we have some data
  if (res.rows() != 1)
  {
    if (res.rows() > 1)
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Unexpected number of results getting recipient profile data." << std::endl;
    else // = 0
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": No results trying to get recipient profile data." << std::endl;
    return false;
  }

  // handle group
  if (res("type") == "group")
  {
    if (res.isNull(0, "name") || res("name").empty())
      return false;

    if (res.getValueAs<long long int>(0, "groupVersion") < 2)
    {
      // group v1 not yet....
      return false;
    }

    // get actual group id
    std::pair<unsigned char *, size_t> groupid_data = Base64::base64StringToBytes(res("json_groupId"));
    if (!groupid_data.first || groupid_data.second == 0) // json data was not valid base64 string, lets try the other one
      groupid_data = Base64::base64StringToBytes(res("groupId"));
    if (!groupid_data.first || groupid_data.second == 0)
      return false;
    std::string group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, true);
    bepaald::destroyPtr(&groupid_data.first, &groupid_data.second);

    if (!d_database.exec("UPDATE groups SET title = ? WHERE group_id = ?", {res("name"), group_id}))
      return false;
  }
  else // handle NOT group
  {
    if ((res.isNull(0, "profileName") || res("profileName").empty()) &&
        (res.isNull(0, "profileFamilyName") || res("profileFamilyName").empty()) &&  // not updating with empty info
        (res.isNull(0, "profileFullName") || res("profileFullName").empty()))
      return false;

    // update name info
    if (!d_database.exec("UPDATE recipient SET "
                         "signal_profile_name = ?, "
                         "profile_family_name = ?, "
                         "profile_joined_name = ? "
                         "WHERE _id = ?",
                         {res.value(0, "profileName"), res.value(0, "profileFamilyName"), res.value(0, "profileFullName"), aid}))
      return false;
  }

  // update avatar
  if (!res("avatar").empty())
  {
    // find current
    auto pos = std::find_if(d_avatars.begin(), d_avatars.end(),
                            [aid](auto const &p) { return p.first == bepaald::toString(aid); });
    std::unique_ptr<AvatarFrame> backup; // save the current in case something goes wrong...
    if (pos != d_avatars.end())
    {
      backup = std::move(pos->second);
      d_avatars.erase(pos);
    }

    if (!dtSetAvatar(res("avatar"), aid, databasedir))
      d_avatars.emplace_back(std::make_pair(bepaald::toString(aid), std::move(backup)));
  }

  return true;
}
