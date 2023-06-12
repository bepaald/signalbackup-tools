/*
  Copyright (C) 2022-2023  Selwin van Dijk

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
                                              bool istimermessage) const
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
                  "json_extract(json,'$.expirationTimerUpdate.sourceUuid') AS sourceuuid "
                  "FROM messages WHERE rowid = ?", rowid, &timer_results))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Querying database" << std::endl;
      return;
    }

    bool incoming = timer_results("sourceuuid") != d_selfuuid;
    long long int timer = timer_results.getValueAs<long long int>(0, "expiretimer");
    long long int groupv2type = Types::SECURE_MESSAGE_BIT | Types::PUSH_MESSAGE_BIT | Types::GROUP_V2_BIT |
      Types::GROUP_UPDATE_BIT | (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENDING_TYPE);
    // at this point address is the group_recipient. This is good for outgoing messages,
    // but incoming should have individual_recipient
    if (timer_results("sourceuuid").empty())
      return;
    if (incoming)
      address = getRecipientIdFromUuid(timer_results("sourceuuid"), savedmap);

    if (address == -1)
    {
      // print wrn
      return;
    }

    //std::cout << "Got timer message: " << timer << std::endl;

    DecryptedTimer dt;
    dt.addField<1>(timer);
    DecryptedGroupChange groupchange;
    groupchange.addField<12>(dt);
    DecryptedGroupV2Context groupv2ctx;
    groupv2ctx.addField<2>(groupchange);
    std::pair<unsigned char *, size_t> groupchange_data(groupv2ctx.data(), groupv2ctx.size());

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
                                   {"body", groupchange_data},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}))              // hardcoded to 1 in Signal Android
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting verified-change into mms" << std::endl;
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
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                  << ": Getting free date for inserting verified-change message into mms" << std::endl;
        return;
      }
      if (date != freedate)
        (*adjusted_timestamps)[date] = freedate;

      std::any newmms_id;
      if (!insertRow(d_mms_table, {{"thread_id", thread_id},
                                   {d_mms_date_sent, freedate},
                                   {"date_received", freedate},
                                   {"body", groupchange_data},
                                   {d_mms_type, groupv2type},
                                   {d_mms_recipient_id, incoming ? address : d_selfid},
                                   {"to_recipient_id", incoming ? d_selfid : address},
                                   {"m_type", incoming ? 132 : 128},
                                   {"read", 1}}, "_id", &newmms_id))              // hardcoded to 1 in Signal Android
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting verified-change into mms" << std::endl;
        return;
      }
    }
    return;
  }

  SqliteDB::QueryResults res;
  if (!ddb.exec("SELECT "
                "json_extract(json, '$.groupV2Change.from') AS source,"
                "IFNULL(json_array_length(json, '$.groupV2Change.details'), 0) AS numchanges"
                " FROM messages WHERE rowid = ?", rowid, &res))
    return;

  //res.prettyPrint();
  long long int numchanges = res.getValueAs<long long int>(0, "numchanges");

  for (uint i = 0; i < numchanges; ++i)
  {
    if (!ddb.exec("SELECT "
                  "json_extract(json, '$.groupV2Change.details[" + bepaald::toString(i) + "].type') AS type,"
                  "json_extract(json, '$.groupV2Change.details[" + bepaald::toString(i) + "].uuid') AS uuid"
                  " FROM messages WHERE rowid = ?", rowid, &res))
      continue;

    //res.prettyPrint();
  }
}


/*
  IN MMS:
  CREATE!

                   _id = 15
             thread_id = 4
                  date = 1668710597007
         date_received = 1668710597010
           date_server = -1
               msg_box = 11075607
                  read = 1
                  body = CiQKIGRjL3ssfpiGCUIJYuhuKQq2t9V3lTF8++oKdHVcM8PfEAASlgEKEA1wt/T+SkGvn9XnQmjRP24aNgoQDXC39P5KQa+f1edCaNE/bhACGiCrcRhBUNNqqwmJk4gGMZ7uMfnPS0/nUvcBDFDQDL2bDxo2ChCTciJzeONBNoZAyCYZaXFMEAEaIPY/j3uppGuBSiGCOwJCCEioNDmq2eH6dBYbPx4odt1uUgsKCVRlc3Rncm91cGgCcAKoAQIahQESCVRlc3Rncm91cCIAKgQIAhACOjYKEA1wt/T+SkGvn9XnQmjRP24QAhogq3EYQVDTaqsJiZOIBjGe7jH5z0tP51L3AQxQ0Ay9mw86NgoQk3Iic3jjQTaGQMgmGWlxTBABGiD2P497qaRrgUohgjsCQghIqDQ5qtnh+nQWGz8eKHbdbmAC

                  Field #1: 0A String Length = 36, Hex = 24, UTF8 = " dc/{,~�� B b�n ..." (total 36 chars)                           // GroupContextV2
                  |  As sub-object :
                  |  Field #1: 0A String Length = 32, Hex = 20, UTF8 = "dc/{,~�� B b�n) ..." (total 32 chars)                        //   masterKey (bytes)
                  \  Field #2: 10 Varint Value = 0, Hex = 00                                                                         //   revision (uint32)
                  Field #2: 12 String Length = 150, Hex = 96-01, UTF8 = "  p���JA����Bh� ..." (total 150 chars)                    // DecryptedGroupChange
                  |  As sub-object :
                  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = " p���JA����Bh�?n"                                            //   editor (bytes)
                  |  Field #3: 1A String Length = 54, Hex = 36, UTF8 = "  p���JA����Bh� ..." (total 54 chars)                      //   DecryptedMember
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = " p���JA����Bh�?n"                                         //     uuid (bytes)
                  |  |  Field #2: 10 Varint Value = 2, Hex = 02                                                                      //     role (enum?)
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�qAP�j� ���1�� ..." (total 32 chars)                  //     profilekey (bytes)
                  |  Field #3: 1A String Length = 54, Hex = 36, UTF8 = " �r"sx�A6�@�&i ..." (total 54 chars)                     //   DecryptedMember
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = "�r"sx�A6�@�&iqL"                                        //     uuid (bytes)
                  |  |  Field #2: 10 Varint Value = 1, Hex = 01                                                                      //     role (enum?)
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�?�{��k�J!�;BH ..." (total 32 chars)                  //     profilekey (bytes)
                  |  Field #10: 52 String Length = 11, Hex = 0B, UTF8 = " Testgroup"                                                 //   DecrytedString
                  |  |  As sub-object :
                  |  \  Field #1: 0A String Length = 9, Hex = 09, UTF8 = "Testgroup"                                                 //     title (string)
                  |  Field #13: 68 Varint Value = 2, Hex = 02                                                                        //   AccessControl (enum)
                  |  Field #14: 70 Varint Value = 2, Hex = 02                                                                        //   AccessControl (enum)
                  \  Field #21: A8-01 Varint Value = 2, Hex = 02                                                                     //   EnabledState (enum)
                  Field #3: 1A String Length = 133, Hex = 85-01, UTF8 = " Testgroup"* ..." (total 133 chars)                 // DecryptedGroup
                  |  As sub-object :
                  |  Field #2: 12 String Length = 9, Hex = 09, UTF8 = "Testgroup"                                                    //   title (string)
                  |  Field #4: 22 String Length = 0, Hex = 00, UTF8 = ""                                                             //   DecryptedTimer
                  |  Field #5: 2A String Length = 4, Hex = 04, UTF8 = ""                                                     //   AccessControl
                  |  |  As sub-object :
                  |  |  Field #1: 08 Varint Value = 2, Hex = 02                                                                      //     AccessRequired (enum)
                  |  \  Field #2: 10 Varint Value = 2, Hex = 02                                                                      //     AccessRequired (enum)
                  |  Field #7: 3A String Length = 54, Hex = 36, UTF8 = "  p���JA����Bh� ..." (total 54 chars)                      //   DecryptedMember
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = " p���JA����Bh�?n"                                         //     uuid (bytes)
                  |  |  Field #2: 10 Varint Value = 2, Hex = 02                                                                      //     role (enum?)
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�qAP�j� ���1�� ..." (total 32 chars)                  //     profilekey (bytes)
                  |  Field #7: 3A String Length = 54, Hex = 36, UTF8 = " �r"sx�A6�@�&i ..." (total 54 chars)                     //   DecryptedMember
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = "�r"sx�A6�@�&iqL"                                        //     uuid (bytes)
                  |  |  Field #2: 10 Varint Value = 1, Hex = 01                                                                      //     role (enum?)
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�?�{��k�J!�;BH ..." (total 32 chars)                  //     profilekey (bytes)
                  \  Field #12: 60 Varint Value = 2, Hex = 02                                                                        //   EnabledState (enum)

            part_count = 0
                  ct_l =
               address = 6 // == group!
     address_device_id =
                   exp =
                m_type = 128
                m_size =
                    st =
                 tr_id =
delivery_receipt_count = 0
 mismatched_identities =
      network_failures =
       subscription_id = -1
            expires_in = 0
        expire_started = 0
              notified = 0
    read_receipt_count = 0
              quote_id = 0
          quote_author =
            quote_body =
      quote_attachment = -1
         quote_missing = 0
        quote_mentions =
            quote_type = 0
       shared_contacts =
          unidentified = 0
              previews =
       reveal_duration = 0
      reactions_unread = 0
   reactions_last_seen = -1
        remote_deleted = 0
         mentions_self = 0
    notified_timestamp = 0
  viewed_receipt_count = 0
           server_guid =
     receipt_timestamp = -1
                ranges =
              is_story = 0
       parent_story_id = 0
          export_state =
              exported = 0


  IN SMS:
  OTHER REMOVES THEMSELVES AS MEMBER

                   _id = 7
             thread_id = 4
               address = 4 // = MAINPHONE
     address_device_id = -1
                person =
                  date = 1668710722642
             date_sent = 1668710722642
           date_server = 1668710722642
              protocol = 31337
                  read = 1
                status = -1
                  type = 11206676
    reply_path_present = 1
delivery_receipt_count = 0
               subject =
                  body = CiQKIGRjL3ssfpiGCUIJYuhuKQq2t9V3lTF8++oKdHVcM8PfEAESJgoQk3Iic3jjQTaGQMgmGWlxTBABIhCTciJzeONBNoZAyCYZaXFMGk8SCVRlc3Rncm91cCIAKgQIAhACMAE6NgoQDXC39P5KQa+f1edCaNE/bhACGiCrcRhBUNNqqwmJk4gGMZ7uMfnPS0/nUvcBDFDQDL2bD2ACIoUBEglUZXN0Z3JvdXAiACoECAIQAjo2ChANcLf0/kpBr5/V50Jo0T9uEAIaIKtxGEFQ02qrCYmTiAYxnu4x+c9LT+dS9wEMUNAMvZsPOjYKEJNyInN440E2hkDIJhlpcUwQARog9j+Pe6mka4FKIYI7AkIISKg0OarZ4fp0Fhs/Hih23W5gAg==

                  Field #1: 0A String Length = 36, Hex = 24, UTF8 = " dc/{,~�� B b�n ..." (total 36 chars)
                  |  As sub-object :
                  |  Field #1: 0A String Length = 32, Hex = 20, UTF8 = "dc/{,~�� B b�n) ..." (total 32 chars)
                  \  Field #2: 10 Varint Value = 1, Hex = 01
                  Field #2: 12 String Length = 38, Hex = 26, UTF8 = " �r"sx�A6�@�&i ..." (total 38 chars)
                  |  As sub-object :
                  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = "�r"sx�A6�@�&iqL"
                  |  Field #2: 10 Varint Value = 1, Hex = 01
                  \  Field #4: 22 String Length = 16, Hex = 10, UTF8 = "�r"sx�A6�@�&iqL"
                  Field #3: 1A String Length = 79, Hex = 4F, UTF8 = " Testgroup"* ..." (total 79 chars)
                  |  As sub-object :
                  |  Field #2: 12 String Length = 9, Hex = 09, UTF8 = "Testgroup"
                  |  Field #4: 22 String Length = 0, Hex = 00, UTF8 = ""
                  |  Field #5: 2A String Length = 4, Hex = 04, UTF8 = ""
                  |  |  As sub-object :
                  |  |  Field #1: 08 Varint Value = 2, Hex = 02
                  |  \  Field #2: 10 Varint Value = 2, Hex = 02
                  |  Field #6: 30 Varint Value = 1, Hex = 01
                  |  Field #7: 3A String Length = 54, Hex = 36, UTF8 = "  p���JA����Bh� ..." (total 54 chars)
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = " p���JA����Bh�?n"
                  |  |  Field #2: 10 Varint Value = 2, Hex = 02
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�qAP�j� ���1�� ..." (total 32 chars)
                  \  Field #12: 60 Varint Value = 2, Hex = 02
                  Field #4: 22 String Length = 133, Hex = 85-01, UTF8 = " Testgroup"* ..." (total 133 chars)
                  |  As sub-object :
                  |  Field #2: 12 String Length = 9, Hex = 09, UTF8 = "Testgroup"
                  |  Field #4: 22 String Length = 0, Hex = 00, UTF8 = ""
                  |  Field #5: 2A String Length = 4, Hex = 04, UTF8 = ""
                  |  |  As sub-object :
                  |  |  Field #1: 08 Varint Value = 2, Hex = 02
                  |  \  Field #2: 10 Varint Value = 2, Hex = 02
                  |  Field #7: 3A String Length = 54, Hex = 36, UTF8 = "  p���JA����Bh� ..." (total 54 chars)
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = " p���JA����Bh�?n"
                  |  |  Field #2: 10 Varint Value = 2, Hex = 02
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�qAP�j� ���1�� ..." (total 32 chars)
                  |  Field #7: 3A String Length = 54, Hex = 36, UTF8 = " �r"sx�A6�@�&i ..." (total 54 chars)
                  |  |  As sub-object :
                  |  |  Field #1: 0A String Length = 16, Hex = 10, UTF8 = "�r"sx�A6�@�&iqL"
                  |  |  Field #2: 10 Varint Value = 1, Hex = 01
                  |  \  Field #3: 1A String Length = 32, Hex = 20, UTF8 = "�?�{��k�J!�;BH ..." (total 32 chars)
                  \  Field #12: 60 Varint Value = 2, Hex = 02


 mismatched_identities =
        service_center = GCM
       subscription_id = -1
            expires_in = 0
        expire_started = 0
              notified = 0
    read_receipt_count = 0
          unidentified = 0
      reactions_unread = 0
   reactions_last_seen = -1
        remote_deleted = 0
    notified_timestamp = 0
           server_guid =
     receipt_timestamp = -1
          export_state =
              exported = 0
*/
