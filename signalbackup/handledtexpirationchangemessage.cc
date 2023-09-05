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

bool SignalBackup::handleDTExpirationChangeMessage(SqliteDB const &ddb,
                                                   long long int rowid,
                                                   long long int ttid,
                                                   long long int sent_at,
                                                   long long int address) const
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
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Querying database" << std::endl;
    return false;
  }
  // 'from sync' timer updates do not have any info on who set the timer.
  // On Android, the message must be either incoming or outgoing, but I can
  // only guess. 50-50 of having correct or incorrect info in the database,
  // let's just skip.
  if (timer_results.valueAsString(0, "fromsync") != "0")
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
              << ": Unsupported message type 'timer-notification (fromSync=true)'. Skipping..." << std::endl;
    return true; // non-fatal error
  }

  // get details (who sent this, what's the new timer value

  long long int timer = timer_results.getValueAs<long long int>(0, "expiretimer");
  bool incoming = (timer_results.valueAsString(0, "type") == "incoming");
  if (!incoming)
  {
    // source is often uuid/phone of whoever set the timer? (maybe not on old messages
    std::string source = timer_results.valueAsString(0, "source");

    SqliteDB::QueryResults convresults;
    if (ddb.exec("SELECT id FROM conversations WHERE e164 = ? OR " + d_dt_c_uuid + " = ?", {source, source}, &convresults) &&
        convresults.rows() == 1)
      if (convresults.valueAsString(0, "id") == timer_results.valueAsString(0, "conversationId"))
      {
        //std::cout << convresults(0, "id") << "=" << timer_results(0, "conversationId") << std::endl;
        incoming = true;
      }

    // if it is outgoing, source would be a conversationId in conversations
    SqliteDB::QueryResults sourceresults;
    if (ddb.exec("SELECT " + d_dt_c_uuid + " FROM conversations WHERE id IS ? OR e164 = ?", {source, source}, &sourceresults) &&
        sourceresults.rows() != 1)
      incoming = true;
  }

  // 10747927 (outgoing type) =  PUSH_MESSAGE_BIT | SECURE_MESSAGE_BIT | EXPIRATION_TIMER_UPDATE_BIT | BASE_SENT_TYPE
  // 10747924 (incoming type) =  PUSH_MESSAGE_BIT | SECURE_MESSAGE_BIT | EXPIRATION_TIMER_UPDATE_BIT | BASE_INBOX_TYPE

  // std::cout << rowid
  //           << "|" << (incoming ? "incoming" : "outgoing")
  //           << "|" << ttid
  //           << "|" << address
  //           << "|" << timer
  //           << std::endl;

  if (d_database.containsTable("sms"))
  {
    if (!insertRow("sms", {{"thread_id", ttid},
                           {"date_sent", sent_at},
                           {d_sms_date_received, sent_at},
                           {"type", Types::PUSH_MESSAGE_BIT | Types::SECURE_MESSAGE_BIT | Types::EXPIRATION_TIMER_UPDATE_BIT |
                            (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                           {"expires_in", timer},
                           {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                           {d_sms_recipient_id, address}}))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting expiration-timer-update into sms" << std::endl;
      return false;
    }
  }
  else
  {
    if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id"))
    {
      if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                   {d_mms_date_sent, sent_at},
                                   {"date_received", sent_at},
                                   {d_mms_type, Types::PUSH_MESSAGE_BIT | Types::SECURE_MESSAGE_BIT | Types::EXPIRATION_TIMER_UPDATE_BIT |
                                    (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                   {"m_type", (incoming ? 132 : 128)},
                                   {"expires_in", timer},
                                   {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                   {d_mms_recipient_id, address}}))
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting expiration-timer-update into mms" << std::endl;
        return false;
      }
    }
    else
    {
      // newer tables have a unique constraint on date_sent/thread_id/from_recipient_id, so
      // we try to get the first free date_sent
      long long int freedate = getFreeDateForMessage(sent_at, ttid, incoming ? address : d_selfid);
      if (freedate == -1)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Getting free date for inserting expiration-timer-update message into mms" << std::endl;
        return false;
      }
      if (!insertRow(d_mms_table, {{"thread_id", ttid},
                                   {d_mms_date_sent, freedate},//sent_at},
                                   {"date_received", freedate},//sent_at},
                                   {d_mms_type, Types::PUSH_MESSAGE_BIT | Types::SECURE_MESSAGE_BIT | Types::EXPIRATION_TIMER_UPDATE_BIT |
                                    (incoming ? Types::BASE_INBOX_TYPE : Types::BASE_SENT_TYPE)},
                                   {"m_type", (incoming ? 132 : 128)},
                                   {"expires_in", timer},
                                   {"read", 1}, // hardcoded to 1 in Signal Android (for profile-change)
                                   {d_mms_recipient_id, incoming ? address : d_selfid},
                                   {"to_recipient_id", incoming ? d_selfid : address}}))
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting expiration-timer-update into mms" << std::endl;
        return false;
      }
    }
  }

  return true;
}

/*
          (in group converstations recipient uuid of contact setting the timer = sourceUuid, but
           this is group-update message on Android, so not handled here)

          in 1-on-1: json$.expirationTimerUpdate.source == conversationId of person setting the timer (IF SELF!)
                     json$.expirationTimerUpdate.source == recipientUuid of person setting the timer (IF OTHER!)

          json$.expirationTimerUpdate = not null
          json$.expirationTimerUpdate.expireTimer = some value (in seconds or milliseconds?) or not present when disabling timer

          IF json$.expirationTimerUpdate.fromSync = true -> SOURCE WILL BE UNKNOWN
*/
