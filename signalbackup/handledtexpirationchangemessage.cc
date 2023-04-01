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

bool SignalBackup::handleDTExpirationChangeMessage(SqliteDB const &ddb [[maybe_unused]],
                                                   long long int rowid [[maybe_unused]],
                                                   long long int ttid [[maybe_unused]],
                                                   long long int address [[maybe_unused]]) const
{
  return true;

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
    return false;
  }

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
    // source is conversationId if timer was set by self (=outgoing)
    // else it is uuid/phone of conversation partner
    std::string source = timer_results.valueAsString(0, "source");

    SqliteDB::QueryResults convresults;
    if (ddb.exec("SELECT id FROM conversations WHERE e164 = ? OR uuid = ?", {source, source}, &convresults) &&
        convresults.rows() == 1)
      if (convresults.valueAsString(0, "id") == timer_results.valueAsString(0, "conversationId"))
      {
        //std::cout << convresults(0, "id") << "=" << timer_results(0, "conversationId") << std::endl;
        incoming = true;
      }

    // if it is outgoing, source would be a conversationId in conversations
    SqliteDB::QueryResults sourceresults;
    if (ddb.exec("SELECT uuid FROM conversations WHERE id IS ?", source, &sourceresults) && // source is conversationId if timer was set by self (=outgoing)
        sourceresults.rows() != 1)                                                          // else it is uuid/phone of conversation partner
      incoming = true;
  }

  // 10747927 (outgoing type) =  PUSH_MESSAGE_BIT | SECURE_MESSAGE_BIT | EXPIRATION_TIMER_UPDATE_BIT | BASE_SENT_TYPE
  // 10747924 (incoming type) =  PUSH_MESSAGE_BIT | SECURE_MESSAGE_BIT | EXPIRATION_TIMER_UPDATE_BIT | BASE_INBOX_TYPE


  std::cout << rowid
            << "|" << (incoming ? "incoming" : "outgoing")
            << "|" << ttid
            << "|" << address
            << "|" << timer
            << std::endl;


  // insertMessage(Types::INCOMING/OUTGOING | Types::EXPIRATION_TIMER_UPDATE_BIT), value

  return true;
}
