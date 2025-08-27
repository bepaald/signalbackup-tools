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
#include "../desktopdatabase/desktopdatabase.h"

bool SignalBackup::dtUpdateDatabase(std::unique_ptr<DesktopDatabase> const &dtdb, bool targetisdummy [[maybe_unused]])
{
  Logger::message("Attempting to get old Desktopdatabase in a compatible state for importing...");

  if (d_dt_c_uuid.empty() &&
      d_dt_s_uuid.empty() &&
      d_dt_m_sourceuuid.empty())
  {
    if (!dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN uuid TEXT DEFAULT NULL"))
      return false;
    d_dt_c_uuid = "uuid";

    if (!dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN sourceUuid TEXT DEFAULT NULL"))
      return false;
    d_dt_m_sourceuuid = "sourceUuid";

    // if !targetisdummy
    //   attempt to match to d_database on recipient.e164, set uuid from recipient.aci
    // else
    //   set to "FAKE_UUID_FOR_RECIPIENT_[dtdb.conversations.id]

    // ONLY USED IN dummybackup cstor
    // if (!dtdb->d_database.exec("ALTER TABLE sessions ADD COLUMN ourUuid TEXT DEFAULT NULL"))
    //   return false;
    // d_dt_c_uuid = "ourUuid";


  }

  // make sure column is present
  if (!dtdb->d_database.tableContainsColumn("messages", "received_at_ms") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN received_at_ms INTEGER DEFAULT NULL"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("messages", "isErased") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN isErased INTEGER DEFAULT 0"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("messages", "isViewOnce") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN isViewOnce INTEGER DEFAULT 0"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("messages", "isStory") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN isStory INTEGER DEFAULT 0"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("messages", "serverGuid") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN serverGuid INTEGER DEFAULT NULL"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("messages", "seenStatus") &&
      !dtdb->d_database.exec("ALTER TABLE messages ADD COLUMN seenStatus INTEGER DEFAULT 0"))
    return false;

  // make sure column is present
  if (!dtdb->d_database.tableContainsColumn("conversations", "profileFamilyName") &&
      !dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN profileFamilyName TEXT DEFAULT NULL"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("conversations", "profileFullName") &&
      !dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN profileFullName TEXT DEFAULT NULL"))
    return false;
  if (!dtdb->d_database.tableContainsColumn("conversations", "profileLastFetchedAt") &&
      !dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN profileLastFetchedAt INTEGER DEFAULT 0"))
    return false;

  // make sure column is present and has good value
  if (!dtdb->d_database.tableContainsColumn("conversations", "e164"))
  {
    if (!dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN e164 TEXT DEFAULT NULL"))
      return false;
    if (!dtdb->d_database.exec("UPDATE conversations SET e164 = ('+' || id)"))
      return false;
  }

  // make sure is present, for the current import (see #335), no groups exist in database.
  if (!dtdb->d_database.tableContainsColumn("conversations", "groupId"))
  {
    if (!dtdb->d_database.exec("ALTER TABLE conversations ADD COLUMN groupId TEXT DEFAULT NULL"))
      return false;
    // if (select 1 FROM conversations where type IS NOT 'private') -> warn! error?
  }

  // need message count!
  SqliteDB::QueryResults msgcount_results;
  if (!dtdb->d_database.exec("SELECT id, json_extract(json, '$.messageCount') AS msgcount FROM conversations", &msgcount_results))
    return false;
  for (unsigned int i = 0; i < msgcount_results.rows(); ++i)
  {
    if (msgcount_results.isNull(i, "msgcount"))
    {
      long long int msgcount = dtdb->d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM messages WHERE conversationId = ?", msgcount_results.value(i, "id"), -1);
      if (msgcount != -1)
      {
        if (d_verbose)
          Logger::message("Setting json.messageCount for conversation '", msgcount_results(i, "id"), " to ", msgcount);
        if (!dtdb->d_database.exec("UPDATE conversations SET json = json_set(json, '$.messageCount', ?) WHERE id = ?", {msgcount, msgcount_results.value(i, "id")}))
          return false;
      }
    }
  }

  // create sticker table
  if (!dtdb->d_database.containsTable("stickers") &&
      !dtdb->d_database.exec("CREATE TABLE stickers (id INTEGER NOT NULL, packId TEXT NOT NULL, emoji STRING, height INTEGER, isCoverOnly INTEGER, lastUsed INTEGER, path STRING, width INTEGER, version INTEGER NOT NULL DEFAULT 1, localKey TEXT, size INTEGER)"))
    return false;

  return true;
}
