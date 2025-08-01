/*
  Copyright (C) 2025  Selwin van Dijk

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

#include "../adbbackupdatabase/adbbackupdatabase.h"
#include "../adbbackupattachmentreader/adbbackupattachmentreader.h"

bool SignalBackup::importFromAdbBackup(std::unique_ptr<AdbBackupDatabase> const &adbdb, bool isdummy [[maybe_unused]])
{
  // get all conversations
  SqliteDB::QueryResults thread_results;
  if (!adbdb->d_db.exec("SELECT _id FROM thread", &thread_results))
    return false;

  for (unsigned int it = 0; it < thread_results.rows(); ++it)
  {
    Logger::message("Dealing with thread ", it + 1, "/", thread_results.rows());

    // match thread id in target backup, create if needed

    // get thread recipient (for to_/from_recipient_id)



    // get messages
    SqliteDB::QueryResults message_results;
    if (!adbdb->d_db.exec("SELECT _id, thread_id, body, date AS date_received, date_sent, read, type, delivery_receipt_count, 0 AS is_mms FROM sms WHERE thread_id = ?1"
                          "UNION ALL "
                          "SELECT _id, thread_id, body, date AS date_received, date AS date_sent, read, msg_box AS type, delivery_receipt_count, 1 AS is_mms FROM mms WHERE thread_id = ?1",
                          thread_results.value(it, 0),
                          &message_results))
      return false;

    if (message_results.rows() == 0)
    {
      Logger::message("No messages in thread");
      continue;
    }

    //message_results.printLineMode();

    for (unsigned int im = 0; im < message_results.rows(); ++im)
    {
      Logger::message("Importing message ", im + 1, "/", message_results.rows());
    }
  }

  return true;
}
