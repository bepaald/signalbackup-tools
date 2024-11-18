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
                                              bool create_valid_contacts, bool *was_warned)
{
  std::string printable_uuid(makePrintable(id));
  Logger::message("Creating new recipient for id: ", printable_uuid);

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
                "IFNULL(json_extract(conversations.json, '$.profileSharing'), '0') AS 'profileSharing', "
                "json_extract(conversations.json, '$.firstUnregisteredAt') AS 'firstUnregisteredAt', "
                "IFNULL(json_extract(conversations.json, '$.sealedSender'), 0) AS 'sealedSender', "

                "json_extract(identityKeys.json, '$.publicKey') AS 'publicKey', "
                "IFNULL(json_extract(identityKeys.json, '$.verified'), 0) AS 'verified', "
                "IFNULL(json_extract(identityKeys.json, '$.firstUse'), 0) AS 'firstUse', "
                "IFNULL(json_extract(identityKeys.json, '$.timestamp'), 0) AS 'timestamp', "
                "IFNULL(json_extract(identityKeys.json, '$.nonblockingApproval'), 0) AS 'nonblockingApproval', "

                "IFNULL(json_extract(conversations.json,'$.groupVersion'), 1) AS groupVersion, "
                "NULLIF(json_extract(conversations.json,'$.nicknameGivenName'), '') AS nick_first, "
                "NULLIF(json_extract(conversations.json,'$.nicknameFamilyName'), '') AS nick_last, "
                "TOKENCOUNT(members) AS nummembers, json_extract(conversations.json, '$.masterKey') AS masterKey "
                "FROM conversations "
                "LEFT JOIN identityKeys ON conversations." + d_dt_c_uuid + " = identityKeys.id "
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

  if (*was_warned == false)
  {
    Logger::warning("Chat partner was not found in recipient-table. Attempting to create.");
    Logger::warning_indent(Logger::Control::BOLD, "NOTE THE RESULTING BACKUP CAN MOST LIKELY NOT BE RESTORED");
    Logger::warning_indent("ON SIGNAL ANDROID. IT IS ONLY MEANT TO EXPORT TO HTML.", Logger::Control::NORMAL);
    *was_warned = true;
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
    // I suspect members can occur double in the list? or maybe my tokenizer is no good?, this is just to check
    std::set<std::string> members_processed;
    // the actual UUID of the member returned by getRecipientIdFromUuidMapped may differ from the input id, since it may
    // exist in the Android database under a different uuid. Normally the database only referes to recipient._id so it's
    // no problem, but member roles use the uuid, so we need the correct one...
    std::map<std::string, std::string> member_uuids; // [ ddb_uuid -> android_uuid ]

    for (unsigned int i = 0; i < res.getValueAs<long long int>(0, "nummembers"); ++i)
    {
      SqliteDB::QueryResults mem;
      if (!ddb.exec("SELECT members,TOKEN(members, ?) AS member FROM conversations WHERE groupId = ?", {i, groupidb64}, &mem))
        continue;

      //std::cout << "Got members: " << mem("member") << std::endl;
      if (bepaald::contains(members_processed, mem("member")))
      {
        Logger::warning("Asked to process same member again. Skipping.");
        Logger::warning_indent("Here is is raw members-list:");
        Logger::warning_indent("'", mem("members"), "'");
        continue;
      }
      members_processed.insert(mem("member"));

      long long int member_rid = getRecipientIdFromUuidMapped(mem("member"), recipient_info, was_warned);

      if (member_rid == -1)
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Creating group member...");
        member_rid = dtCreateRecipient(ddb, mem("member"), std::string(), std::string(), databasedir, recipient_info, create_valid_contacts, was_warned);
        if (member_rid == -1)
        {
          Logger::error("Failed to get new groups members uuid.");
          d_database.exec("ROLLBACK TRANSACTION");
          return -1;
        }
      }

      // save the actual uuid of this recipient in map
      std::string android_uuid = d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", member_rid, std::string());
      if (!android_uuid.empty())
        member_uuids[mem("member")] = android_uuid;

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
      std::map<std::string, long long int> memberroles; // [ uuid -> Role ]
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

            std::string uuid = bepaald::contains(member_uuids, memberrole_results(mr, "uuid")) ?
              member_uuids[memberrole_results(mr, "uuid")] :
              memberrole_results(mr, "uuid");

            // check if uuid is an actual member:
            long long int uuidpresent = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM group_membership WHERE "
                                                                                    "recipient_id IS (SELECT _id FROM recipient WHERE " + d_recipient_aci + " = ?) AND "
                                                                                    "group_id = ?", {uuid, group_id}, 0);
            if (uuidpresent)
              memberroles[uuid] = memberrole_results.getValueAs<long long int>(mr, "role");
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

    Logger::message("Successfully created new recipient for group (id: ", new_rec_id, ").");
    return new_rec_id; //-1;
  }







  // type != group :

  long long int new_rec_id = -1;

  if (res("profileName").empty() && res("profileFamilyName").empty() &&
      res("profileFullName").empty() && res("e164").empty() &&
      res("uuid").empty())
  {
    Logger::warning("All relevant info on new recipient is empty. Here is the data from the Desktop db:");
    ddb.printLineMode("SELECT * FROM conversations WHERE " + d_dt_c_uuid + " = ? OR e164 = ? OR groupId = ?", {id, phone, groupidb64});
  }

  // it is possible the contacts exists already, but not as a valid Signal contact (with uuid and keys)
  // (maybe should check for username and email as well, both are also unique in the recipient table)
  SqliteDB::QueryResults existing_recipient;
  if (!d_database.exec("SELECT _id, " + d_recipient_aci + " FROM recipient WHERE pni = ? OR " + d_recipient_e164 + " = ?",
                       {res.value(0, "pni"), res.value(0, "e164")}, &existing_recipient))
    return -1;

  if (existing_recipient.rows() > 1)
  {
    Logger::error("Unexpected number of results for query (existing_recipient");
    return -1;
  }

  if (existing_recipient.rows() == 1) // update existing recipient
  {
    long long int existing_recipient_id = existing_recipient.getValueAs<long long int>(0, "_id");
    std::string existing_recipient_uuid = existing_recipient(d_recipient_aci);

    // if the existing recipient already has a uuid and a indentity key, just use it???
    if (!existing_recipient_uuid.empty())
    {
      if (d_database.getSingleResultAs<long long int>("SELECT _id FROM identities WHERE address = ? AND identity_key IS NOT NULL", existing_recipient_uuid, -1) != -1)
      {
        Logger::message("Found existing valid contact under different uuid [", printable_uuid, " -> ", makePrintable(existing_recipient_uuid), "] "
                        "(id: ", existing_recipient_id, ").");

        (*recipient_info)[id.empty() ? phone : id] = existing_recipient_id;
        return existing_recipient_id;
      }
      else
      {
        Logger::error("Contact already exists with a different uuid, but no valid identity key. not sure what to do here yet...");
        return -1;
      }
    }
    else // contact uuid == NULL in Android db, lets update it with Desktop data
    {
      if (!d_database.exec("UPDATE recipient SET " +
                           d_recipient_aci  + " = ?, " +
                           d_recipient_e164 + " = COALESCE(" + d_recipient_e164 + ", ?), " +
                           "pni = COALESCE(pni, ?), "
                           "storage_service_id = COALESCE(storage_service_id, ?), "
                           "registered = ? "
                           "WHERE _id = ?",
                           {res.value(0, "uuid"), res.value(0, "e164"), res.value(0, "pni"),
                            res.value(0, "storageId"), res.isNull(0, "firstUnregisteredAt") ? 1 : 0, existing_recipient_id}))
        return -1;
      Logger::message("Found existing contact without uuid, Updating... (id: ", existing_recipient_id, ").");
      new_rec_id = existing_recipient_id;
    }
  }
  else // insert new recipient
  {
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
                    {"profile_sharing", res.value(0, "profileSharing")},
                    {"registered", res.isNull(0, "firstUnregisteredAt") ? 1 : 0},   // registered if no Unregister-timestamp is found, unknown otherwise
                    {d_recipient_sealed_sender, res.value(0, "sealedSender")},

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
      d_database.exec("DELETE FROM recipient WHERE _id = ?", new_rid);
      return -1;
    }
    new_rec_id = std::any_cast<long long int>(new_rid);

    // set avatar
    dtSetAvatar(res("avatar"), res("localKey"), res.valueAsInt(0, "size"), res.valueAsInt(0, "version"), new_rec_id, databasedir);
  }

  std::string identity_key = res(0, "publicKey");

  if (identity_key.empty() && create_valid_contacts)
  {
    Logger::warning("No publicKey found for new recipient, inserting fake key...");
    identity_key = "BUZBS0VLRVlGQUtFS0VZRkFLRUtFWUZBS0VLRVlGQUtF";
  }

  // set identity info
  if (!res.isNull(0, "uuid"))
  {
    if (!insertRow("identities",
                   {{"address", res.value(0, "uuid")},
                    {"identity_key", identity_key},
                    {"first_use", res("firstUse")},
                    {"timestamp", res.value(0, "timestamp")},
                    {"verified", res.value(0, "verified")},
                    {"nonblocking_approval", res("nonblockingApproval")}}))
    {
      if (create_valid_contacts)
      {
        Logger::error("Failed to insert identity key for newly created recipient entry.");
        d_database.exec("DELETE FROM recipient WHERE _id = ?", new_rec_id);
        return -1;
      }
      else
        Logger::warning("Failed to insert identity key for newly created recipient entry.");
    }
    else
      Logger::message("Successfully updated identity-key for contact (", new_rec_id, ", ", makePrintable(res("uuid")), ")");
  }
  else
  {
    if (create_valid_contacts)
    {
      Logger::error("Newly created contact has no UUID");
      d_database.exec("DELETE FROM recipient WHERE _id = ?", new_rec_id);
      return -1;
    }
    else
      Logger::warning("Newly created contact has no UUID");
  }

  Logger::message("Successfully created new recipient (id: ", new_rec_id, ").");
  //d_database.printLineMode("SELECT * FROM recipient WHERE _id = ?", new_rec_id);

  (*recipient_info)[id.empty() ? phone : id] = new_rec_id;
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
