/*
  Copyright (C) 2022-2025  Selwin van Dijk

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
  It seems the desktop message does not contain most of the info of the phone message. For example the creation message:

  ("type":"group-v2-change","groupV2Change":{"from":"0d70b7f4-fe4a-41af-9fd5-e74268d13f6e","details":[{"type":"create"}]}"

  has no group title, no group memberlist (uuids, profilekeys, roles, accesscontrol...)

  Also, the same rules for sms/mms database apply for incoming/outgoing messages (since these are all group messages), but
  the desktop database does not say if the messages are incoming or outgoing. Only the source uuid ('from'), but we don't
  know without scanning other messages (and possibly cant know if unlucky), which uuid is self and which are others.

*/

void SignalBackup::handleDTGroupChangeMessage(SqliteDB const &ddb, long long int rowid,
                                              long long int thread_id, long long int address, long long int date,
                                              std::map<long long int, long long int> *adjusted_timestamps,
                                              std::map<std::string, long long int> *savedmap,
                                              std::string const &databasedir, bool istimermessage, bool createcontacts,
                                              bool createvalidcontacts, bool *warn)
{
  if (date == -1)
  {
    // print wrn
    return;
  }

  if (istimermessage)
  {

    SqliteDB::QueryResults timer_results;
    if (!ddb.exec("SELECT "
                  "type, "
                  "conversationId, "
                  "IFNULL(json_extract(json,'$.expirationTimerUpdate.fromGroupUpdate'), false) AS fromgroupupdate, "
                  "IFNULL(json_extract(json,'$.expirationTimerUpdate.fromSync'), false) AS fromsync, "
                  "IFNULL(json_extract(json,'$.expirationTimerUpdate.expireTimer'), 0) AS expiretimer, "
                  "json_extract(json,'$.expirationTimerUpdate.source') AS source, "
                  "COALESCE(json_extract(json,'$.expirationTimerUpdate.sourceServiceId'), json_extract(json,'$.expirationTimerUpdate.sourceUuid')) AS sourceuuid "
                  "FROM messages WHERE rowid = ?", rowid, &timer_results))
    {
      Logger::error("Querying database");
      return;
    }

    bool incoming = bepaald::toLower(timer_results("sourceuuid")) != d_selfuuid;
    long long int timer = timer_results.getValueAs<long long int>(0, "expiretimer");
    long long int groupv2type = Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | Types::GROUP_V2_BIT |
      Types::GROUP_UPDATE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENDING_TYPE);
    // at this point address is the group_recipient. This is good for outgoing messages,
    // but incoming should have individual_recipient
    if (timer_results("sourceuuid").empty())
      return;
    if (incoming)
    {
      address = getRecipientIdFromUuidMapped(timer_results("sourceuuid"), savedmap);

      if (address == -1)
      {
        if (createcontacts)
        {
          if ((address = dtCreateRecipient(ddb, timer_results("sourceuuid"), std::string(), std::string(), databasedir, savedmap, createvalidcontacts, warn)) == -1)
          {
            Logger::error("Failed to create group-v2-expiration-timer contact (1), skipping");
            return;
          }
        }
        else
        {
          Logger::error("Failed to create group-v2-expiration-timer contact (2), skipping");
          return;
        }
      }
    }
    //std::cout << "Got timer message: " << timer << std::endl;

    DecryptedTimer dt;
    dt.addField<1>(timer);
    DecryptedGroupChange groupchange;
    groupchange.addField<12>(dt);
    DecryptedGroupV2Context groupv2ctx;
    groupv2ctx.addField<2>(groupchange);
    std::pair<unsigned char *, size_t> groupchange_data(groupv2ctx.data(), groupv2ctx.size());
    std::string groupchange_data_b64 = Base64::bytesToBase64String(groupchange_data);
    // add message to database
    // if (d_database.containsTable("sms"))
    //   not going through the trouble
    // else
    // {
    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
    {
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, date},
                                   {"date_received", date},
                                   {"body", groupchange_data_b64},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}))              // hardcoded to 1 in Signal Android
      {
        Logger::error("Inserting verified-change into mms");
        return;
      }
    }
    else
    {
      //newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
      //we try to get the first free date_sent
      long long int freedate = getFreeDateForMessage(date, thread_id, Types::isOutgoing(groupv2type) ? d_selfid : address);
      if (freedate == -1)
      {
        Logger::error("Getting free date for inserting verified-change message into mms");
        return;
      }
      if (date != freedate)
        (*adjusted_timestamps)[date] = freedate;

      std::any newmms_id;
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, freedate},
                                   {"date_received", freedate},
                                   {"body", groupchange_data_b64},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, incoming ? address : d_selfid},
                                   {"to_recipient_id", incoming ? d_selfid : address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}, "_id", &newmms_id))              // hardcoded to 1 in Signal Android
      {
        Logger::error("Inserting verified-change into mms");
        return;
      }
    }
    return;
  }

  // !istimermessage

  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT "
                "LOWER(json_extract(json, '$.groupV2Change.from')) AS source,"
                "IFNULL(json_array_length(json, '$.groupV2Change.details'), 0) AS numchanges"
                " FROM messages WHERE rowid = ?", rowid, &res))
    return;

  //res.prettyPrint();
  long long int numchanges = res.getValueAs<long long int>(0, "numchanges");
  if (numchanges == 0)
    return;

  std::string source_uuid = res("source");
  bool incoming = source_uuid != d_selfuuid;
  long long int groupv2type = Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | Types::GROUP_V2_BIT |
    Types::GROUP_UPDATE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENDING_TYPE);
  if (incoming)
  {
    address = getRecipientIdFromUuidMapped(source_uuid, savedmap);
    if (address == -1)
    {
      if (createcontacts)
      {
        if ((address = dtCreateRecipient(ddb, source_uuid, std::string(), std::string(), databasedir, savedmap, createvalidcontacts, warn)) == -1)
        {
          Logger::error("Failed to create group-v2-update contact (1), skipping");
          return;
        }
      }
      else
      {
        Logger::error("Failed to create group-v2-update contact (2), skipping");
        return;
      }
    }
  }

  DecryptedGroupV2Context groupv2ctx;
  DecryptedGroupChange groupchange;
  bool addchange = false;

  // add each group change...
  for (unsigned int i = 0; i < numchanges; ++i)
  {
    if (!ddb.exec("SELECT "
                  "json_extract(json, '$.groupV2Change.details[' || ? || '].type') AS type,"
                  "COALESCE(json_extract(json, '$.groupV2Change.details[' || ? || '].aci'), json_extract(json, '$.groupV2Change.details[' || ? || '].uuid')) AS uuid,"
                  "json_extract(json, '$.groupV2Change.details[' || ? || '].newTitle') AS title,"
                  "json_extract(json, '$.groupV2Change.details[' || ? || '].description') AS description,"
                  "json_extract(json, '$.groupV2Change.details[' || ? || '].avatar') AS avatar,"
                  "json_extract(json, '$.groupV2Change.details[' || ? || '].removed') AS removed"
                  " FROM messages WHERE rowid = ?", {i, i, i, i, i, i, i, rowid}, &res))
      continue;

    std::string changetype = res("type");

    if (changetype == "title")
    {
      DecryptedString newtitle;
      newtitle.addField<1>(res("title"));
      groupchange.addField<10>(newtitle);
      addchange = true;
      //Logger::message("new title '", title, "'");
    }
    else if (changetype == "description")
    {
      //bool removed [[maybe_unused]] = res.valueAsInt(0, "removed");
      DecryptedString newdescription;
      newdescription.addField<1>(res("description"));
      groupchange.addField<20>(newdescription);
      addchange = true;
      //Logger::message("new description: '", description, "' (", removed, ")");
    }
    else if (changetype == "avatar")
    {
      //bool removed [[maybe_unused]] = res.valueAsInt(0, "removed");
      DecryptedString newavatar;
      newavatar.addField<1>("new_avatar");
      groupchange.addField<11>(newavatar);
      addchange = true;
      //Logger::message("new avatar (", removed, ")");
    }
    else if (changetype == "member-add")
    {
      std::string uuid = res("uuid");
      if (static_cast<int>(uuid.size()) != STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx") ||
          static_cast<int>(source_uuid.size()) != STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"))
        continue;

      unsigned int uuid_bytes_size = (STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx") - STRLEN("----")) / 2;
      std::unique_ptr<unsigned char[]> uuid_bytes(new unsigned char[uuid_bytes_size]);
      bepaald::hexStringToBytes(source_uuid, uuid_bytes.get(), uuid_bytes_size);
      groupchange.addField<1>(std::make_pair<unsigned char *, int>(uuid_bytes.get(), uuid_bytes_size));

      DecryptedMember newmember;
      uuid_bytes.reset(new unsigned char[uuid_bytes_size]);
      bepaald::hexStringToBytes(uuid, uuid_bytes.get(), uuid_bytes_size);
      newmember.addField<1>(std::make_pair<unsigned char *, int>(uuid_bytes.get(), uuid_bytes_size));
      groupchange.addField<3>(newmember);

      addchange = true;

      //Logger::message("member add: ", uuid);
    }
    else if (changetype == "member-remove")
    {
      std::string uuid = res("uuid");

      if (uuid == source_uuid) // xxx left the group
        groupv2type |= Types::GROUP_QUIT_BIT;
      //else // xxx removed yyy

      if (static_cast<int>(uuid.size()) != STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"))
        continue;

      unsigned int uuid_bytes_size = (STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx") - STRLEN("----")) / 2;
      std::unique_ptr<unsigned char[]> uuid_bytes(new unsigned char[uuid_bytes_size]);
      bepaald::hexStringToBytes(uuid, uuid_bytes.get(), uuid_bytes_size);
      groupchange.addField<4>(std::make_pair<unsigned char *, int>(uuid_bytes.get(), uuid_bytes_size));

      addchange = true;
      //Logger::message("member remove: ", uuid);
    }
    else if (changetype == "create")
    {
      if (static_cast<int>(source_uuid.size()) != STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"))
        continue;

      // set source = source_uuid
      unsigned int uuid_bytes_size = (STRLEN("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx") - STRLEN("----")) / 2;
      std::unique_ptr<unsigned char[]> uuid_bytes(new unsigned char[uuid_bytes_size]);
      bepaald::hexStringToBytes(source_uuid, uuid_bytes.get(), uuid_bytes_size);
      groupchange.addField<1>(std::make_pair<unsigned char *, int>(uuid_bytes.get(), uuid_bytes_size));

      // set new member = also source_uuid
      DecryptedMember newmember;
      newmember.addField<1>(std::make_pair<unsigned char *, int>(uuid_bytes.get(), uuid_bytes_size));
      groupchange.addField<3>(newmember);

      // explicitly set revision 0
      GroupContextV2 groupctx;
      groupctx.addField<2>(0);
      groupv2ctx.addField<1>(groupctx);

      addchange = true;

      //Logger::message("member add: ", uuid);
    }
    else
    {
      //warnOnce("Unhandled groupv2-update-type: '" + changetype + "' (this warning will be shown only once)");
      continue;
    }

    //res.prettyPrint(d_truncate);

  }

  if (addchange)
  {
    groupv2ctx.addField<2>(groupchange);
    std::pair<unsigned char *, size_t> groupchange_data(groupv2ctx.data(), groupv2ctx.size());
    std::string groupchange_data_b64 = Base64::bytesToBase64String(groupchange_data);
    // add message to database
    // if (d_database.containsTable("sms"))
    //   not going through the trouble
    // else
    // {
    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
    {
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, date},
                                   {"date_received", date},
                                   {"body", groupchange_data_b64},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}))              // hardcoded to 1 in Signal Android
      {
        Logger::error("Inserting verified-change into mms");
        return;
      }
    }
    else
    {
      //newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
      //we try to get the first free date_sent
      long long int freedate = getFreeDateForMessage(date, thread_id, Types::isOutgoing(groupv2type) ? d_selfid : address);
      if (freedate == -1)
      {
        Logger::error("Getting free date for inserting verified-change message into mms");
        return;
      }
      if (date != freedate)
        (*adjusted_timestamps)[date] = freedate;

      //std::cout << "ADDING NEW GROUPV2 MESSAGE AT DATE: " << bepaald::toDateString(freedate / 1000, "%Y-%m-%d %H:%M:%S") << std::endl;

      std::any newmms_id;
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, freedate},
                                   {"date_received", freedate},
                                   {"body", groupchange_data_b64},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, incoming ? address : d_selfid},
                                   {"to_recipient_id", incoming ? d_selfid : address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}, "_id", &newmms_id))              // hardcoded to 1 in Signal Android
      {
        Logger::error("Inserting verified-change into mms");
        return;
      }
    }
  }
  return;
}
