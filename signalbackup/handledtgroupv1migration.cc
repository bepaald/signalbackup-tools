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

bool SignalBackup::handleDTGroupV1Migration(SqliteDB const &ddb, long long int rowid,
                                            long long int thread_id, long long int timestamp, long long int address,
                                            std::map<std::string, long long int> *recipientmap, bool createcontacts,
                                            std::string const &databasedir, bool *warned_createcontacts)
{
  // get a list of dropped members (I _think_ these are not recipient uuid's but conversationUuid's...)
  std::string dropped_members;
  SqliteDB::QueryResults results_droppedmembers;
  if (ddb.exec("SELECT value AS droppedmember FROM messages, json_each(messages.json, '$.groupMigration.droppedMemberIds') WHERE messages.rowid = ?", rowid, &results_droppedmembers))
  {
    for (uint dm = 0; dm < results_droppedmembers.rows(); ++dm)
    {
      std::string convuuid = results_droppedmembers.valueAsString(dm, "droppedmember");
      SqliteDB::QueryResults dm_id;
      if (!ddb.exec("SELECT COALESCE(" + d_dt_c_uuid + ",e164) AS rid FROM conversations WHERE id IS ?", convuuid, &dm_id) ||
          dm_id.rows() != 1)
        continue;
      long long int recid = getRecipientIdFromUuid(dm_id.valueAsString(0, "rid"), recipientmap, createcontacts);
      if (recid < 0)
        recid = getRecipientIdFromPhone(dm_id.valueAsString(0, "rid"), recipientmap, createcontacts);
      if (recid < 0)
      {
        // let's just check the uuid's aren't recipient uuid's to make sure
        // this can go when we know it's working
        SqliteDB::QueryResults test_results;
        if (ddb.exec("SELECT " + d_dt_c_uuid + " FROM conversations WHERE " + d_dt_c_uuid + " IS ?", dm_id.valueAsString(0, "rid"), &test_results))
          if (test_results.rows())
            Logger::message(" *** NOTE FOR DEV: id was not found as conversationId but does appear as recipientUuid (droppedMembers) ***");

        if (createcontacts)
          recid = dtCreateRecipient(ddb, dm_id.valueAsString(0, "rid"), dm_id.valueAsString(0, "rid"), std::string(),
                                    databasedir, recipientmap, warned_createcontacts);
        if (recid < 0)
          continue;
      }


      dropped_members += (dropped_members.empty() ? bepaald::toString(recid) : ("," + bepaald::toString(recid)));
    }
  }

  // get a list of invited members
  //
  // invited members looks like this:
  // invitedMembers = [{"addedByUserId":"2e2axxxx-xxxx-xxxx-xxxx-xxxxxxxxxxbe","conversationId":"d608xxxx-xxxx-xxxx-xxxx-xxxxxxxxxx6a","timestamp":1614770366146,"role":2},{...]
  // or this:
  // invitedMembers = [{"addedByUserId":"000bxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxe6","uuid":"40d2xxxx-xxxx-xxxx-xxxx-xxxxxxxxxx71","timestamp":1651801628714,"role":2}]
  //
  // I'm assuming both are conversationUuid, but uuid might actually be recipients' uuid directly?
  //
  // SELECT json_extract(value, '$.conversationId'), json_extract(value, '$.uuid') FROM messages, json_each(messages.json, '$.groupMigration.invitedMembers') AS TREE WHERE messages.rowid = ?;
  std::string invited_members;
  SqliteDB::QueryResults results_invitedmembers;
  //if (ddb.exec("SELECT json_extract(value, '$.conversationId') AS conversationId, json_extract(value, '$.uuid') AS uuid"
  //             " FROM messages, json_each(messages.json, '$.groupMigration.invitedMembers') AS TREE WHERE messages.rowid = ?", rowid, &results_invitedmembers))
  if (ddb.exec("SELECT COALESCE(json_extract(value, '$.conversationId'), COALESCE(json_extract(value, '$.aci'), json_extract(value, '$.uuid'))) AS convuuid, "
               "json_extract(value, '$.conversationId') IS NULL AS is_uuid " // just to remember if this was gotten from "conversationId' or 'uuid' for testing
               "FROM messages, json_each(messages.json, '$.groupMigration.invitedMembers') AS TREE WHERE messages.rowid = ?", rowid, &results_invitedmembers))
  {
    for (uint im = 0; im < results_invitedmembers.rows(); ++im)
    {
      std::string convuuid = results_invitedmembers.valueAsString(im, "convuuid");
      if (!convuuid.empty())
      {
        SqliteDB::QueryResults im_id;
        if (!ddb.exec("SELECT COALESCE(" + d_dt_c_uuid + ", e164) AS rid FROM conversations WHERE id IS ?", convuuid, &im_id) ||
            im_id.rows() != 1)
          continue;
        long long int recid = getRecipientIdFromUuid(im_id.valueAsString(0, "rid"), recipientmap, createcontacts);
        if (recid < 0)
          recid = getRecipientIdFromPhone(im_id.valueAsString(0, "rid"), recipientmap, createcontacts);
        if (recid < 0)
        {
          // let's just check the uuid's aren't recipient uuid's to make sure
          // this can go when we know it's working (and the SELECT can be shortened!)
          SqliteDB::QueryResults test_results;
          if (ddb.exec("SELECT " + d_dt_c_uuid + " FROM conversations WHERE " + d_dt_c_uuid + " IS ?", im_id.valueAsString(0, "rid"), &test_results))
            if (test_results.rows())
              Logger::message(" *** NOTE FOR DEV: id was not found as conversationId but does appear as recipientUuid (invitedMembers, uuid: ",
                              im_id.valueAsString(0, "is_uuid"), ") ***");

          if (createcontacts)
            recid = dtCreateRecipient(ddb, im_id.valueAsString(0, "rid"), im_id.valueAsString(0, "rid"), std::string(),
                                      databasedir, recipientmap, warned_createcontacts);
          if (recid < 0)
            continue;
        }

        invited_members += (invited_members.empty() ? bepaald::toString(recid) : ("," + bepaald::toString(recid)));
      }
    }
  }

  std::string body;
  if (!invited_members.empty() || !dropped_members.empty())
    body = invited_members + '|' + dropped_members;

  if (d_database.containsTable("sms"))
  {
    if (!insertRow("sms", {{"thread_id", thread_id},
                           {"date_sent", timestamp},
                           {d_sms_date_received, timestamp},
                           {"type", Types::GV1_MIGRATION_TYPE},
                           {d_sms_recipient_id, address},
                           {"body", body},
                           {"read", 1}}))
    {
      Logger::error("Inserting group-v1-migration into sms");
      return false;
    }
  }
  else
  {
    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
    {
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, timestamp},
                                   {"date_received", timestamp},
                                   {d_mms_type, Types::GV1_MIGRATION_TYPE},
                                   {d_mms_recipient_id, address},
                                   {"body", body},
                                   {d_mms_recipient_device_id, 1},
                                   {"read", 1}}))
      {
        Logger::error("Inserting group-v1-migration into mms");
        return false;
      }
    }
    else
    {
      // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
      // we try to get the first free date_sent
      long long int freedate = getFreeDateForMessage(timestamp, thread_id, Types::isOutgoing(Types::GV1_MIGRATION_TYPE) ? d_selfid : address);
      if (freedate == -1)
      {
        Logger::error("Getting free date for inserting group-v1-migration message into mms");
        return false;
      }

      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, freedate},
                                   {"date_received", freedate},
                                   {d_mms_type, Types::GV1_MIGRATION_TYPE},
                                   {d_mms_recipient_id, Types::isOutgoing(Types::GV1_MIGRATION_TYPE) ? d_selfid : address},
                                   {"to_recipient_id", Types::isOutgoing(Types::GV1_MIGRATION_TYPE) ? address : d_selfid},
                                   {"body", body},
                                   {d_mms_recipient_device_id, 1},
                                   {"read", 1}}))
      {
        Logger::error("Inserting group-v1-migration into mms");
        return false;
      }
    }
  }
  return true;
}
