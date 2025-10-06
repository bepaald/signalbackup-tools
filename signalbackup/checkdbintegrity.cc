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

bool SignalBackup::checkDbIntegrityInternal(bool warn) const
{
  SqliteDB::QueryResults results;

  // CHECKING FOREIGN KEY CONSTRAINTS
  if (!warn)
    Logger::message_start("Checking foreign key constraints... ");
  d_database.exec("SELECT DISTINCT [table],[parent],[fkid] FROM pragma_foreign_key_check", &results);
  if (results.rows())
  {
    if (!warn)
      Logger::error("Foreign key constraint violated. This will not end well, aborting."
                    "\n\n"
                    "Please report this error to the program author.");
    else
      Logger::warning("Foreign key constraint violated.");
    results.prettyPrint(d_truncate);
    return false;
  }
  if (!warn)
    Logger::message_end(Logger::Control::BOLD, "ok", Logger::Control::NORMAL);

  // std::cout << "Checking database integrity (quick)..." << std::flush;
  // d_database.exec("SELECT * FROM pragma_quick_check", &results);
  // if (results.rows() && results.valueAsString(0, "quick_check") != "ok")
  // {
  //   std::cout << std::endl << bepaald::bold_on << "ERROR" << bepaald::bold_off << " Database integrity check failed. This will not end well, aborting." << std::endl
  //             <<                                  "     "                         " Please report this error to the program author." << std::endl;
  //   results.prettyPrint();
  //   return false;
  // }
  // std::cout << " ok" << std::endl;

  // CHECKING DATABASE
  if (!warn)
    Logger::message_start("Checking database integrity (full)... ");
  d_database.exec("SELECT * FROM pragma_integrity_check", &results);
  if (results.rows() && results.valueAsString(0, "integrity_check") != "ok")
  {
    if (!warn)
      Logger::error("Database integrity check failed. This will not end well, aborting."
                    "\n\n"
                    "Please report this error to the program author.");
    else
      Logger::warning("Foreign key constraint violated.");
    results.prettyPrint(d_truncate);
    return false;
  }
  if (!warn)
    Logger::message_end(Logger::Control::BOLD, "ok", Logger::Control::NORMAL);

  return true;
}

bool SignalBackup::checkDbIntegrity() const
{
  bool ret = checkDbIntegrityInternal(false /* warnonly */);

  Logger::message("\nPerforming additional checks. The following checks are expected to\n"
                  "cause problems if not passed. However, this is not a certainty. Escpecially\n"
                  "if the database is an unaltered database straight from an official source,\n"
                  "it can be assumed any warnings are false positives\n");

  // it has been reported that group recipients without a storage_service_id can cause
  // Signal Android to crash (#341). This checks for that case. However, I expect
  // NULL storage_service_ids to actually be valid in some cases as long as the remote
  // and local storage_service db are synced and a group is new...
  if (d_database.tableContainsColumn("recipient", d_recipient_storage_service) && d_database.tableContainsColumn("recipient", d_recipient_type))
  {
    Logger::message_start("Checking for NULL values in storage_service_id for group recipients... ");
    long long int count = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM recipient WHERE " +
                                                                      d_recipient_storage_service + " IS NULL AND " + d_recipient_type + " = 3", -1);
    if (count == -1) [[unlikely]]
    {
      Logger::error("Query failed");
      ret = false;
    }
    else if (count == 0)
      Logger::message_end(Logger::Control::BOLD, "ok", Logger::Control::NORMAL);
    else
    {
      Logger::error("Found group recipients with storage_service_id = NULL. This has been known to cause crashes in Signal Android");
      ret = false;
    }
  }

  // a valid, active registered user with which one can communicate must have an identity key in
  // the identity database. This was an important part while developing the importcontacts function
  // in importfromdesktop. This attempts to check for that, though it is difficult.
  // I have one contact that is registerd ('1') (with e164, aci and pni), with an 'active' thread
  // with 'meaningful_messages' > 1. But _without_ an identity key. The only message from this contact
  // is "XXX is on Signal" (-> not a real message actively sent by the user).
  // Here I check for a contact with no identity key & no thread with outgoing messages. Maybe a better
  // check is possible...
  {
    Logger::message_start("Checking for registered, active recipients without indentity key... ");
    long long int count = -1;
    if (d_databaseversion >= 114) // before this version 'identites.address' referred to recipent._id, after it was uuid/e164(/pni)
      count = d_database.getSingleResultAs<long long int>("WITH idad AS (SELECT DISTINCT address FROM identities) "
                                                          "SELECT COUNT(*) FROM recipient "
                                                          "LEFT JOIN thread ON thread." + d_thread_recipient_id + " = recipient._id "
                                                          "LEFT JOIN " + d_mms_table + " ON thread._id = " + d_mms_table + ".thread_id "
                                                          "WHERE " +
                                                          d_recipient_aci + " NOT IN idad AND " +
                                                          d_recipient_e164 + " NOT in idad AND " +
                                                          (d_database.tableContainsColumn("recipient", "pni") ? "pni NOT in idad AND " : "") +
                                                          "registered = 1 AND "
                                                          "(" + d_mms_table + "." + d_mms_type + " & 0x1f) = ?", Types::BASE_SENT_TYPE, -1);
    else if (d_databaseversion >= 24)
      count = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM recipient "
                                                          "LEFT JOIN thread ON thread." + d_thread_recipient_id + " = recipient._id "
                                                          "LEFT JOIN " + d_mms_table + " ON thread._id = " + d_mms_table + ".thread_id "
                                                          "WHERE "
                                                          "recipient._id NOT IN (SELECT address FROM identities) AND "
                                                          "registered = 1 AND "
                                                          "(" + d_mms_table + "." + d_mms_type + " & 0x1f) = ?", Types::BASE_SENT_TYPE, -1);
    else // "recipient" table was "recipient_preferences"
      count = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM recipient_preferences "
                                                          "LEFT JOIN thread ON thread." + d_thread_recipient_id + " = recipient_preferences._id "
                                                          "LEFT JOIN " + d_mms_table + " ON thread._id = " + d_mms_table + ".thread_id "
                                                          "WHERE "
                                                          "recipient_preferences._id NOT IN (SELECT address FROM identities) AND "
                                                          "registered = 1 AND "
                                                          "(" + d_mms_table + "." + d_mms_type + " & 0x1f) = ?", Types::BASE_SENT_TYPE, -1);
    if (count == -1) [[unlikely]]
    {
      Logger::error("Query failed");
      ret = false;
    }
    else if (count == 0)
      Logger::message_end(Logger::Control::BOLD, "ok", Logger::Control::NORMAL);
    else
    {
      Logger::error("Found active recipients without an identity key");
      ret = false;
    }
  }

  return ret;
}
