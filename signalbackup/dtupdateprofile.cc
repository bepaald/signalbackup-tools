/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

  if (d_verbose) [[unlikely]]
    Logger::message("Updating profile for id: ", dtid);

  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT type, name, profileName, IFNULL(profileFamilyName, '') AS profileFamilyName, profileFullName, "
                "IFNULL(json_extract(json,'$.groupVersion'), 1) AS groupVersion, "
                "COALESCE(json_extract(json, '$.profileAvatar.path'),json_extract(json, '$.avatar.path')) AS avatar, " // 'profileAvatar' for persons, 'avatar' for groups
                "IFNULL(COALESCE(json_extract(json, '$.profileAvatar.localKey'), json_extract(json, '$.avatar.localKey')), '') AS localKey, "
                "IFNULL(COALESCE(json_extract(json, '$.profileAvatar.size'), json_extract(json, '$.avatar.size')), 0) AS size, "
                "IFNULL(COALESCE(json_extract(json, '$.profileAvatar.version'), json_extract(json, '$.avatar.version')), 0) AS version "
                "FROM conversations WHERE " + d_dt_c_uuid + " = ?1 OR e164 = ?1 OR groupId = ?1",
                dtid, &res))
    return false;

  // check if we have some data
  if (res.rows() != 1)
  {
    if (res.rows() > 1)
      Logger::error("Unexpected number of results getting recipient profile data.");
    else // = 0
      Logger::error("No results trying to get recipient profile data.");
    return false;
  }

  // handle group
  if (res("type") == "group")
  {
    if (res.getValueAs<long long int>(0, "groupVersion") < 2)
    {
      // group v1 not yet....
      Logger::warning("Updating profile data for groupV1 not yet supported.");
      return false;
    }

    if (res.isNull(0, "name") || res("name").empty())
    {
      Logger::warning("Profile data empty. Not updating group recipient.");
      return false;
    }

    // get actual group id
    std::pair<unsigned char *, size_t> groupid_data = Base64::base64StringToBytes(res("json_groupId"));
    if (!groupid_data.first || groupid_data.second == 0) // json data was not valid base64 string, lets try the other one
      groupid_data = Base64::base64StringToBytes(res("groupId"));
    if (!groupid_data.first || groupid_data.second == 0)
    {
      Logger::warning("Failed to deteremine group_id when trying to update profile.");
      return false;
    }
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
    {
      Logger::warning("Profile data empty. Not updating group recipient.");
      return false;
    }

    // if (d_verbose) [[unlikely]]
    // {
    //   std::cout << "Updating profile:" << std::endl;
    //   res.prettyPrint();
    // }

    // update name info
    if (!d_database.exec("UPDATE recipient SET "
                         + d_recipient_profile_given_name + " = ?, "
                         "profile_family_name = ?, "
                         "profile_joined_name = ? "
                         "WHERE _id = ?",
                         {res.value(0, "profileName"), res.value(0, "profileFamilyName"), res.value(0, "profileFullName"), aid}))
      return false;
  }

  // update avatar
  if (!res("avatar").empty())
  {
    if (d_verbose) [[unlikely]]
      Logger::message_overwrite("Updating avatar...");

    // find current
    auto pos = std::find_if(d_avatars.begin(), d_avatars.end(),
                            [aid](auto const &p) { return p.first == bepaald::toString(aid); });
    DeepCopyingUniquePtr<AvatarFrame> backup; // save the current in case something goes wrong...
    if (pos != d_avatars.end())
    {
      backup = std::move(pos->second);
      d_avatars.erase(pos);
    }

    if (!dtSetAvatar(res("avatar"), res("localKey"), res.valueAsInt(0, "size"), res.valueAsInt(0, "version"), aid, databasedir))
    {
      if (d_verbose && !backup) [[unlikely]]
        Logger::message_overwrite("Updating avatar... Failed to set new avatar", Logger::Control::ENDOVERWRITE);
      if (backup)
      {
        Logger::message_overwrite("Updating avatar... Failed, restoring previous...", Logger::Control::ENDOVERWRITE);
        d_avatars.emplace_back(bepaald::toString(aid), std::move(backup));
      }
    }
    else
    {
      if (d_verbose) [[unlikely]]
      {
        Logger::message("Set new avatar. Info:");
        for (auto const &a : d_avatars)
          if (a.first == bepaald::toString(aid))
            a.second->printInfo();
      }
    }
  }

  return true;
}
