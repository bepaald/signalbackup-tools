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

#include "../protobufparser/protobufparser.h"

bool SignalBackup::handleDTCallTypeMessage(SqliteDB const &ddb, std::string const &callid, long long int rowid,
                                           long long int ttid, long long int address, bool insertincompletedataforexport) const
{
  SqliteDB::QueryResults calldetails;
  uint64_t calltype = 0;
  std::any body = nullptr;
  if (ddb.containsTable("callsHistory"))
  {
    // new style! get details from this table
    /*
      sqlite> SELECT * from callsHistory;
    */

    if (!ddb.exec("SELECT "
                  "timestamp AS sent_at, "
                  "mode, "
                  "type, "
                  "peerId, "
                  "direction, "
                  "status"
                  " FROM callsHistory WHERE callId = ?", callid, &calldetails))
    {
      Logger::error("Failed to get call details from desktop database. Skipping.");
      return false;
    }

    if (calldetails.valueAsString(0, "mode") == "Direct")
    {
      /*
        sqlite> SELECT * from callsHistory;
        callId = 4690462596594407498
        peerId = 93722273-78e3-4136-8640-c8261969714c
        ringerId =
        mode = Direct
        type = Audio
        direction = Incoming
        status = Accepted
        timestamp = 1693839545545
      */
      if (calldetails.valueAsString(0, "type") == "Video")
      {
        if (calldetails.valueAsString(0, "direction") == "Incoming")
        {
          if (calldetails.valueAsString(0, "status") == "Accepted")
            calltype = Types::INCOMING_VIDEO_CALL_TYPE;
          else
            calltype = Types::MISSED_VIDEO_CALL_TYPE;
        }
        else // direction = outgoing
          calltype = Types::OUTGOING_VIDEO_CALL_TYPE;
      }
      else // type == "Audio"
      {
        if (calldetails.valueAsString(0, "direction") == "Incoming")
        {
          if (calldetails.valueAsString(0, "status") == "Accepted")
            calltype = Types::INCOMING_AUDIO_CALL_TYPE;
          else
            calltype = Types::MISSED_AUDIO_CALL_TYPE;
        }
        else// direction = outgoing
          calltype = Types::OUTGOING_AUDIO_CALL_TYPE;
      }
    }
    else if (calldetails.valueAsString(0, "mode") == "Group")
    {
      /*
         callId = 13523070632477333339
         peerId = 8FqrnHI/W7ZKnVmfmQEGX3oxRrFeriGy3KT2Ih/dAww=
         ringerId =
         mode = Group
         type = Group
         direction = Incoming
         status = Accepted
         timestamp = 1693840579538
      */


      // NOTE THE ANDROID BACKUP NEEDS THE CALL INITIATOR ADDRESS. THIS IS NOT AVAILABLE IN THE DESKTOP DATABASE ANYMORE
      if (!insertincompletedataforexport)
        return true;

      calltype = Types::GROUP_CALL_TYPE; // always video?
      ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING, protobuffer::optional::INT64, protobuffer::repeated::STRING, protobuffer::optional::BOOL> groupcallbody;
      groupcallbody.addField<3>(calldetails.getValueAs<long long int>(0, "sent_at")); // cheating
      body = groupcallbody.getDataString();
    }
  }
  else
  {
    if (!ddb.exec("SELECT "
                  "COALESCE(sent_at, json_extract(json, '$.sent_at'), json_extract(json, '$.received_at_ms'), received_at, json_extract(json, '$.received_at')) AS sent_at,"
                  "json_extract(json, '$.callHistoryDetails.callMode') AS mode,"
                  "json_extract(json, '$.callHistoryDetails.creatorUuid') AS creator_uuid,"
                  "json_extract(json, '$.callHistoryDetails.eraId') AS era_id,"
                  "json_extract(json, '$.callHistoryDetails.startedTime') AS started_time,"
                  "IFNULL(json_extract(json, '$.callHistoryDetails.wasIncoming'), false) AS incoming,"
                  "IFNULL(json_extract(json, '$.callHistoryDetails.wasVideoCall'), false) AS video,"
                  "IFNULL(json_extract(json, '$.callHistoryDetails.wasDeclined'), false) AS declined,"
                  "IFNULL(json_extract(json, '$.callHistoryDetails.acceptedTime'), -1) AS accepted"
                  " FROM messages WHERE rowid = ?", rowid, &calldetails))
    {
      Logger::error("Failed to get call details from desktop database. Skipping.");
      return false;
    }

    //calldetails.prettyPrint();

    if (calldetails.valueAsString(0, "mode") == "Direct")
    {
      if (calldetails.getValueAs<long long int>(0, "video"))
      {
        if (calldetails.getValueAs<long long int>(0, "incoming"))
        {
          if (calldetails.getValueAs<long long int>(0, "accepted") >= 0)
            calltype = Types::INCOMING_VIDEO_CALL_TYPE;
          else
            calltype = Types::MISSED_VIDEO_CALL_TYPE;
        }
        else
          calltype = Types::OUTGOING_VIDEO_CALL_TYPE;
      }
      else
      {
        if (calldetails.getValueAs<long long int>(0, "incoming"))
        {
          if (calldetails.getValueAs<long long int>(0, "accepted") >= 0)
            calltype = Types::INCOMING_AUDIO_CALL_TYPE;
          else
            calltype = Types::MISSED_AUDIO_CALL_TYPE;
        }
        else
          calltype = Types::OUTGOING_AUDIO_CALL_TYPE;
      }
    }
    else if (calldetails.valueAsString(0, "mode") == "Group")
    {
      calltype = Types::GROUP_CALL_TYPE; // always video?

      //"callHistoryDetails":{"callMode":"Group","creatorUuid":"93722273-78e3-4136-8640-c8261969714c","eraId":"5d36bc8b0d6a1c5d","startedTime":1669314425500}
      // ->
      //                   _id = 18
      //              thread_id = 4
      //                address = 4
      //      address_device_id = 1
      //                 person =
      //                   date = 1669314409536
      //              date_sent = 1669314409536
      //            date_server = -1
      //               protocol =
      //                   read = 1
      //                 status = -1
      //                   type = 12
      //     reply_path_present =
      // delivery_receipt_count = 0
      //                subject =
      //                   body = ChA1ZDM2YmM4YjBkNmExYzVkEiQ5MzcyMjI3My03OGUzLTQxMzYtODY0MC1jODI2MTk2OTcxNGMYwOiR18ow
      //  mismatched_identities =
      //         service_center =
      //        subscription_id = -1
      //             expires_in = 0
      //         expire_started = 0
      //               notified = 0
      //     read_receipt_count = 0
      //           unidentified = 0
      //       reactions_unread = 0
      //    reactions_last_seen = 1669314426630
      //         remote_deleted = 0
      //     notified_timestamp = 1669314422531
      //            server_guid =
      //      receipt_timestamp = -1
      //           export_state =
      //               exported = 0
      //
      // BODY PROTO
      // message GroupCallUpdateDetails {
      //              string eraId                = 1;
      //              string startedCallUuid      = 2;
      //              int64  startedCallTimestamp = 3;
      //     repeated string inCallUuids          = 4;
      //              bool   isCallFull           = 5;
      // }
      //
      // body =
      // Field #1: 0A String Length = 16, Hex = 10, UTF8 = "5d36bc8b0d6a1c5d"
      // Field #2: 12 String Length = 36, Hex = 24, UTF8 = "93722273-78e3-41 ..." (total 36 chars)
      // Field #3: 18 Varint Value = 1669314409536, Hex = C0-E8-91-D7-CA-30
      ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING, protobuffer::optional::INT64, protobuffer::repeated::STRING, protobuffer::optional::BOOL> groupcallbody;
      groupcallbody.addField<1>(calldetails.valueAsString(0, "era_id"));
      groupcallbody.addField<2>(calldetails.valueAsString(0, "creator_uuid"));
      groupcallbody.addField<3>(calldetails.getValueAs<long long int>(0, "started_time"));
      body = groupcallbody.getDataString();
    }
  }

  // if (d_databaseversion < 170)
  //{
  if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
  {
    if (!insertRow(d_database.containsTable("sms") ? "sms" : d_mms_table,
                   {{"thread_id", ttid},
                    {d_database.containsTable("sms") ? d_sms_recipient_id : d_mms_recipient_id, address},
                    {d_database.containsTable("sms") ? d_sms_date_received : "date_received", calldetails.value(0, "sent_at")},
                    {"date_sent", calldetails.value(0, "sent_at")},
                    {"type", calltype},
                    {"body", body}}))
    {
      Logger::warning("Failed inserting into ", (d_database.containsTable("sms") ? "sms" : d_mms_table), ": call type message.");
      return false;
    }
  }
  else
  {
    // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
    // we try to get the first free date_sent
    long long int freedate = getFreeDateForMessage(calldetails.getValueAs<long long int>(0, "sent_at"), ttid, Types::isOutgoing(calltype) ? d_selfid : address);
    if (freedate == -1)
    {
      Logger::error("Getting free date for call type message");
      return false;
    }

    if (!insertRow(d_mms_table,
                   {{"thread_id", ttid},
                    {d_mms_recipient_id, Types::isOutgoing(calltype) ? d_selfid : address},
                    {"to_recipient_id", Types::isOutgoing(calltype) ? address : d_selfid},
                    {"date_received", freedate},//calldetails.value(0, "sent_at")},
                    {"date_sent", freedate},//calldetails.value(0, "sent_at")},
                    {"type", calltype},
                    {"body", body}}))
    {
      Logger::warning("Failed inserting into ", d_mms_table, ": call type message.");
      return false;
    }
  }
  //}
  /*
  else // dbv >=170 -> call into 'call' table???, or also in mms -> just additional details?
  {
  insertRow("call",
            {{call_id, ???},
             {message_id, ???},
             {peer, address?},
             {type, calltype?},
             {direction, ...},
             {event, ???}
            }
  }
  */

  return true;
}
