/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

long long int SignalBackup::dtCreateRecipient(SqliteDB const &ddb,
                                              std::string const &id, std::string const &phone, std::string const &groupidb64,
                                              std::string const &databasedir,
                                              std::map<std::string, long long int> *recipient_info,
                                              bool *warn)
{

  //std::cout << "Creating new recipient for id: " << id << ", phone: " << phone << std::endl;

  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT "

                "type, TRIM(name) AS name, profileName, profileFamilyName, "
                "profileFullName, e164, " + d_dt_c_uuid + " AS uuid, json_extract(conversations.json,'$.color') AS color, "
                "COALESCE(json_extract(conversations.json, '$.profileAvatar.path'), json_extract(conversations.json, '$.avatar.path')) AS avatar, " // 'profileAvatar' for persons, 'avatar' for groups
                "IFNULL(COALESCE(json_extract(conversations.json, '$.profileAvatar.localKey'), json_extract(conversations.json, '$.avatar.localKey')), '') AS localKey, "
                "IFNULL(COALESCE(json_extract(conversations.json, '$.profileAvatar.size'), json_extract(conversations.json, '$.avatar.size')), 0) AS size, "
                "IFNULL(COALESCE(json_extract(conversations.json, '$.profileAvatar.version'), json_extract(conversations.json, '$.avatar.version')), 0) AS version, "
                "groupId, IFNULL(json_extract(conversations.json,'$.groupId'),'') AS 'json_groupId', "

                "IFNULL(json_extract(conversations.json, '$.expireTimer'), 0) AS 'expireTimer', "
                "IFNULL(json_extract(conversations.json, '$.expireTimerVersion'), 1) AS 'expireTimerVersion', "
                "json_extract(conversations.json, '$.storageID') AS 'storageId', "
                "json_extract(conversations.json, '$.pni') AS 'pni', "
                "IFNULL(json_extract(conversations.json, '$.profileSharing'), 'false') AS 'profileSharing', "
                "json_extract(conversations.json, '$.firstUnregisteredAt') AS 'firstUnregisteredAt', "

                "json_extract(identityKeys.json, '$.publicKey') AS 'publicKey', "
                "IFNULL(json_extract(identityKeys.json, '$.verified'), 0) AS 'verified', "
                "IFNULL(json_extract(identityKeys.json, '$.firstUse'), 'false') AS 'firstUse', "
                "IFNULL(json_extract(identityKeys.json, '$.timestamp'), 0) AS 'timestamp', "
                "IFNULL(json_extract(identityKeys.json, '$.nonblockingApproval'), 'false') AS 'nonblockingApproval', "

                "IFNULL(json_extract(conversations.json,'$.groupVersion'), 1) AS groupVersion, "
                "NULLIF(json_extract(conversations.json,'$.nicknameGivenName'), '') AS nick_first, "
                "NULLIF(json_extract(conversations.json,'$.nicknameFamilyName'), '') AS nick_last, "
                "TOKENCOUNT(members) AS nummembers, json_extract(conversations.json, '$.masterKey') AS masterKey "
                "FROM conversations "
                "LEFT JOIN identityKeys ON conversations. " + d_dt_c_uuid + " = identityKeys.id "
                "WHERE " + d_dt_c_uuid + " = ? OR e164 = ? OR groupId = ?",
                {id, phone, groupidb64}, &res))
  {
    // std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": ." << std::endl;
    return -1;
  }
  //res.prettyPrint(d_truncate);

  if (res.rows() != 1)
  {
    if (res.rows() > 1)
      Logger::error("Unexpected number of results getting new recipient data.");
    else // = 0
      Logger::error("No results trying to get new recipient data.");
    return -1;
  }

  if (*warn == false)
  {
    Logger::warning("Chat partner was not found in recipient-table. Attempting to create.");
    Logger::warning_indent(Logger::Control::BOLD, "NOTE THE RESULTING BACKUP CAN MOST LIKELY NOT BE RESTORED");
    Logger::warning_indent("ON SIGNAL ANDROID. IT IS ONLY MEANT TO EXPORT TO HTML.", Logger::Control::NORMAL);
    *warn = true;
  }

  if (res("type") == "group")
  {
    if (res.getValueAs<long long int>(0, "groupVersion") < 2)
    {
      // group v1 not yet....
      return -1;
    }
    std::pair<unsigned char *, size_t> groupid_data = Base64::base64StringToBytes(res("json_groupId"));
    if (!groupid_data.first || groupid_data.second == 0) // json data was not valid base64 string, lets try the other one
      groupid_data = Base64::base64StringToBytes(res("groupId"));
    if (!groupid_data.first || groupid_data.second == 0)
    {
      // maybe, just create out own id here? we don't care
      return -1;
    }
    std::string group_id = "__signal_group__v2__!" + bepaald::bytesToHexString(groupid_data, true);
    bepaald::destroyPtr(&groupid_data.first, &groupid_data.second);

    if (res("name").empty())
    {
      Logger::warning("Group name of new recipient is empty. Here is the data from the Desktop db:");
      ddb.printLineMode("SELECT * FROM conversations WHERE " + d_dt_c_uuid + " = ? OR e164 = ? OR groupId = ?", {id, phone, groupidb64});
    }

    d_database.exec("BEGIN TRANSACTION"); // things could still go bad...

    std::any new_rid;
    if (!insertRow("recipient",
                   {{"group_id", group_id},
                    {d_recipient_type, 3}, // group type
                    {"storage_service_id", res.value(0, "storageId")},
                    {"message_expiration_time_version", res.value(0, "expireTimerVersion")},
                    {"message_expiration_time", res.value(0, "expireTimer")},
                    {d_recipient_avatar_color, res.value(0, "color")}}, "_id", &new_rid))
    {
      Logger::error("Failed to insert new (group) recipient into database.");
      return -1;
    }
    if (new_rid.type() != typeid(long long int))
    {
      Logger::error("New (group) recipient _id has unexpected type.");
      return -1;
    }
    long long int new_rec_id = std::any_cast<long long int>(new_rid);

    std::pair<unsigned char *, size_t>  masterkey = Base64::base64StringToBytes(res("masterKey"));
    if (!insertRow("groups",
                   {{"title", res.value(0, "name")},
                    {"group_id", group_id},
                    {"recipient_id", new_rec_id},
                    {"avatar_id", 0},
                    {"master_key", masterkey},
                    // {"decrypted_group",},
                    // {"distribution_id",},
                    {"revision", 0}}))
    {
      Logger::error("Failed to insert new group into database.");
      d_database.exec("ROLLBACK TRANSACTION");
      bepaald::destroyPtr(&masterkey.first, &masterkey.second);
      return -1;
    }
    bepaald::destroyPtr(&masterkey.first, &masterkey.second);

    // get group members:
    std::string oldstyle_members;
    for (unsigned int i = 0; i < res.getValueAs<long long int>(0, "nummembers"); ++i)
    {
      SqliteDB::QueryResults mem;
      if (!ddb.exec("SELECT members,TOKEN(members, ?) AS member FROM conversations WHERE groupId = ?", {i, groupidb64}, &mem))
        continue;

      //std::cout << "Got members: " << mem("member") << std::endl;

      long long int member_rid = getRecipientIdFromUuidMapped(mem("member"), recipient_info);
      if (member_rid == -1)
      {
        Logger::message("Trying to create");
        member_rid = dtCreateRecipient(ddb, mem("member"), std::string(), std::string(), databasedir, recipient_info, warn);
        if (member_rid == -1)
        {
          Logger::error("Failed to get new groups members uuid.");
          d_database.exec("ROLLBACK TRANSACTION");
          return -1;
        }
      }

      if (d_database.containsTable("group_membership"))
      {
        if (!insertRow("group_membership",
                       {{"group_id", group_id},
                        {"recipient_id", member_rid}}))
        {
          Logger::error("Failed to set new groups membership.");
          d_database.exec("ROLLBACK TRANSACTION");
          return -1;
        }
      }
      else
        oldstyle_members += ((oldstyle_members.empty() ? "" : ",") + bepaald::toString(member_rid));
    }

    // set old-style members
    if (d_database.tableContainsColumn("groups", "members"))
    {
      if (!d_database.exec("UPDATE groups SET members = ? WHERE _id = ?", {oldstyle_members, new_rec_id}))
      {
        Logger::error("Failed to set new groups membership (old style).");
        d_database.exec("ROLLBACK TRANSACTION");
        return -1;
      }
    }

    // set member roles:
    if (d_database.containsTable("group_membership") &&
        d_database.tableContainsColumn("groups", "decrypted_group"))
    {
      std::map<std::string, long long int> memberroles;
      for (unsigned int i = 0; i < res.getValueAs<long long int>(0, "nummembers"); ++i)
      {
      // get member role (0 = unknown, 1 = normal, 2 = admin)
        SqliteDB::QueryResults memberrole_results;
        if (ddb.exec("SELECT json_extract(json, '$.membersV2[" + bepaald::toString(i) + "].role') AS role, "
                     "COALESCE(json_extract(json, '$.membersV2[" + bepaald::toString(i) + "].aci'), json_extract(json, '$.membersV2[" + bepaald::toString(i) + "].uuid')) AS uuid "
                     "FROM conversations WHERE groupId = ?", groupidb64, &memberrole_results))
        {
          //memberrole_results.prettyPrint();
          for (unsigned int mr = 0; mr < memberrole_results.rows(); ++mr)
          {
            if (!memberrole_results.valueHasType<long long int>(mr, "role") ||
                !memberrole_results.valueHasType<std::string>(mr, "uuid"))
              continue;

            // check if uuid is an actual member:
            long long int uuidpresent = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM group_membership WHERE "
                                                                                    "recipient_id IS (SELECT _id FROM recipient WHERE " + d_recipient_aci + " = ?) AND "
                                                                                    "group_id = ?", {memberrole_results(mr, "uuid"), group_id}, 0);
            if (uuidpresent)
              memberroles[memberrole_results(mr, "uuid")] = memberrole_results.getValueAs<long long int>(mr, "role");
          }
        }
      }

      // now create a GroupV2Context and add it
      /*
        message DecryptedGroup {
        string                    title                     = 2;
        string                    avatar                    = 3;
        DecryptedTimer            disappearingMessagesTimer = 4;
        AccessControl             accessControl             = 5;
        uint32                    revision                  = 6;
        repeated DecryptedMember           members                   = 7;
        repeated DecryptedPendingMember    pendingMembers            = 8;
        repeated DecryptedRequestingMember requestingMembers         = 9;
        bytes                     inviteLinkPassword        = 10;
        string                    description               = 11;
        EnabledState              isAnnouncementGroup       = 12;
        repeated DecryptedBannedMember     bannedMembers             = 13;
        }
      */
      std::string title = res("name");
      // if available
        // std::string description = res("description");

      DecryptedGroup group_info;
      group_info.addField<2>(title);
      //group_info.print();

      for (auto const &m : memberroles)
      {

        /*
          message DecryptedMember {
          bytes       uuid             = 1;
          Member.Role role             = 2;
          bytes       profileKey       = 3;
          uint32      joinedAtRevision = 5;
          bytes       pni              = 6;
          }
          enum Role {
          UNKNOWN       = 0;
          DEFAULT       = 1;
          ADMINISTRATOR = 2;
          }
        */
        DecryptedMember mem;

        unsigned char rawuuid[16];
        uint64_t rawuuid_size = 16;
        if (bepaald::hexStringToBytes(m.first, rawuuid, rawuuid_size))
        {
          mem.addField<1>({rawuuid, rawuuid_size});
          mem.addField<2>(m.second);
          group_info.addField<7>(mem);
          //group_info.print();
        }
      }
      // add it
      std::pair<unsigned char *, uint64_t> groupdetails = {group_info.data(), group_info.size()};
      d_database.exec("UPDATE groups SET decrypted_group = ? WHERE recipient_id = ?", {groupdetails, new_rid});
    }

    d_database.exec("COMMIT TRANSACTION");
    (*recipient_info)[groupidb64] = new_rec_id;

    // set avatar
    if (!dtSetAvatar(res("avatar"), res("localKey"), res.valueAsInt(0, "size"), res.valueAsInt(0, "version"), new_rec_id, databasedir))
      Logger::warning("Failed to set avatar for new recipient.");

    Logger::message("Succesfully created new recipient for group (id: ", new_rec_id, ").");
    return new_rec_id; //-1;
  }




  // type != group

  if (res("profileName").empty() && res("profileFamilyName").empty() &&
      res("profileFullName").empty() && res("e164").empty() &&
      res("uuid").empty())
  {
    Logger::warning("All relevant info on new recipient is empty. Here is the data from the Desktop db:");
    ddb.printLineMode("SELECT * FROM conversations WHERE " + d_dt_c_uuid + " = ? OR e164 = ? OR groupId = ?", {id, phone, groupidb64});
  }

  std::any new_rid;
  if (!insertRow("recipient",
                 {{d_recipient_profile_given_name, res.value(0, "profileName")},
                  {"profile_family_name", res.value(0, "profileFamilyName")},
                  {"profile_joined_name", res.value(0, "profileFullName")},
                  {"nickname_given_name", res.value(0, "nick_first")},
                  {"nickname_family_name", res.value(0, "nick_last")},
                  {(!res.isNull(0, "nick_first") || !res.isNull(0, "nick_last")) ?
                   "nickname_joined_name" :
                   "", (res(0, "nick_first").empty() ? res(0, "nick_last") :
                        (res(0, "nick_last").empty() ? res(0, "nick_first") :
                         res(0, "nick_first") + " " + res(0, "nick_last")))},
                  {d_recipient_e164, res.value(0, "e164")},
                  {d_recipient_aci, res.value(0, "uuid")},

                  {"pni", res.value(0, "pni")},
                  {"message_expiration_time_version", res.value(0, "expireTimerVersion")},
                  {"message_expiration_time", res.value(0, "expireTimer")},
                  {"storage_service_id", res.value(0, "storageId")},
                  {"profile_sharing", res(0, "profileSharing") == "true" ? 1 : 0},
                  {"registered", res.isNull(0, "firstUnregisteredAt") ? 1 : 0},   // registered if no Unregister-timestamp is found, unknown otherwise

                  // {d_database.tableContainsColumn("recipient", "blocked") ? // blocked recipients do not exist in Desktop?
                  //  "blocked" : "", res.value(0, "blocked")},
                  {d_recipient_avatar_color, res.value(0, "color")}}, "_id", &new_rid))
  {
    Logger::error("Failed to insert new recipient into database.");
    return -1;
  }
  if (new_rid.type() != typeid(long long int))
  {
    Logger::error("New recipient _id has unexpected type.");
    return -1;
  }
  long long int new_rec_id = std::any_cast<long long int>(new_rid);
  (*recipient_info)[id.empty() ? phone : id] = new_rec_id;

  // set avatar
  dtSetAvatar(res("avatar"), res("localKey"), res.valueAsInt(0, "size"), res.valueAsInt(0, "version"), new_rec_id, databasedir);

  // set identity info
  if (!res.isNull(0, "uuid"))
  {
    if (!insertRow("identities",
                   {{"address", res.value(0, "uuid")},
                    {"identity_key", res.value(0, "publicKey")},
                    {"first_use", res("firstUse") == "true" ? 1 : 0},
                    {"timestamp", res.value(0, "timestamp")},
                    {"verified", res.value(0, "verified")},
                    {"nonblocking_approval", res("nonblockingApproval") == "true" ? 1 : 0}}))
      Logger::warning("Failed to insert identity key for newly created recipient entry.");
  }
  else
    Logger::warning("Newly created contact has no UUID");

  Logger::message("Succesfully created new recipient (id: ", new_rec_id, ").");
  //d_database.printLineMode("SELECT * FROM recipient WHERE _id = ?", new_rec_id);
  return new_rec_id;
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
