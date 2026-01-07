/*
  Copyright (C) 2025-2026  Selwin van Dijk

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

bool SignalBackup::fixForeignKeyConstraintViolations() const
{
  while (true)
  {
    Logger::message("Checking foreign key constraint violations...");

    SqliteDB::QueryResults FKC_results;
    if (!d_database.exec("SELECT [table],[parent],[fkid],COUNT(*) count FROM pragma_foreign_key_check GROUP BY [table],[parent],[fkid]",
                         &FKC_results)) [[unlikely]]
      return false;

    if (FKC_results.rows() == 0)
    {
      Logger::message("No foreign key constraint violations found!");
      break;
    }

    //FKC_results.prettyPrint(false);

    Logger::message("Found ", FKC_results.rows(), " foreign key constraint violations");

    for (unsigned int fkc = 0; fkc < FKC_results.rows(); ++fkc)
    {
      int count = FKC_results.valueAsInt(fkc, "count");

      Logger::message(" - Deleting ", count, (count > 1 ? " entries" : " entry"), " from table '", FKC_results(fkc, "table"),
                      "' that ", (count > 1 ? "refer to non-existing entries" : "refers to a non-existing entry")," in the '",
                      FKC_results(fkc, "parent"), "' table...");

      SqliteDB::QueryResults FKC_rowids;
      if (!d_database.exec("SELECT rowid FROM pragma_foreign_key_check WHERE [table] = ? AND [parent] = ? AND [fkid] = ?",
                           {FKC_results.value(fkc, "table"), FKC_results.value(fkc, "parent"), FKC_results.value(fkc, "fkid")},
                           &FKC_rowids)) [[unlikely]]
        return false;

      for (unsigned int i = 0; i < FKC_rowids.rows(); ++i)
      {
        if (d_verbose)
        {
          Logger::message("Entry be deleted:");
          if (FKC_results(fkc, "table") == d_mms_table)
            d_database.prettyPrint(d_truncate, "SELECT _id, thread_id, " + d_mms_type + ", DATETIME(" + d_mms_date_sent + " / 1000, 'unixepoch', 'localtime') AS date_sent, "
                                   "DATETIME(date_received / 1000, 'unixepoch', 'localtime') AS date_received, body FROM " + d_mms_table + " WHERE rowid = ?", FKC_rowids.value(i, "rowid"));
          else if (FKC_results(fkc, "table") == "thread")
            d_database.prettyPrint(d_truncate, "SELECT _id, " + d_thread_recipient_id + ", DATETIME(date / 1000, 'unixepoch', 'localtime') AS date, snippet, "
                                   "snippet_type, archived, active FROM thread WHERE rowid = ?", FKC_rowids.value(i, "rowid"));
          else if (FKC_results(fkc, "table") == "recipient")
            d_database.prettyPrint(d_truncate,
                                   "SELECT recipient._id, "

                                   "COALESCE(" + (d_database.tableContainsColumn("recipient", "nickname_joined_name") ? "NULLIF(recipient.nickname_joined_name, ''),"s : ""s) +
                                   "NULLIF(recipient." + d_recipient_system_joined_name + ", ''), " +
                                   (d_database.tableContainsColumn("recipient", "profile_joined_name") ? "NULLIF(recipient.profile_joined_name, ''),"s : ""s) +
                                   "NULLIF(recipient." + d_recipient_profile_given_name + ", ''), "
                                   "NULLIF(groups.title, ''), " +
                                   (d_database.containsTable("distribution_list") ? "NULLIF(distribution_list.name, ''), " : "") +
                                   "NULLIF(recipient." + d_recipient_e164 + ", ''), " +
                                   (d_database.tableContainsColumn("recipient", "username") ? "NULLIF(recipient.username, '')," : "") +
                                   "NULLIF(recipient." + d_recipient_aci + ", ''), "
                                   " recipient._id) AS 'display_name', " +

                                   "recipient." + d_recipient_e164 + ", " +
                                   (d_database.tableContainsColumn("recipient", "blocked") ? "blocked, " : "") +
                                   (d_database.tableContainsColumn("recipient", "hidden") ? "hidden, " : "") +
                                   "IFNULL(COALESCE(" + d_recipient_profile_avatar + ", groups.avatar_id), 0) IS NOT 0 AS 'has_avatar', "

                                   "CASE recipient." + d_recipient_type + " WHEN 0 THEN 'Individual' ELSE "
                                   "  CASE recipient." + d_recipient_type + " WHEN 3 THEN 'Group (v2)' ELSE "
                                   "    CASE recipient." + d_recipient_type + " WHEN 4 THEN 'Group (story)' ELSE "
                                   "      CASE recipient." + d_recipient_type + " WHEN 1 THEN 'Group (mms)' ELSE "
                                   "        CASE recipient." + d_recipient_type + " WHEN 2 THEN 'Group (v1)' ELSE "
                                   "          CASE recipient." + d_recipient_type + " WHEN 5 THEN 'Group (call)' ELSE 'unknown' "
                                   "          END "
                                   "        END "
                                   "      END "
                                   "    END "
                                   "  END "
                                   "END AS 'type', "

                                   "CASE WHEN recipient." + d_recipient_type + " IS NOT 0 THEN '(n/a)' ELSE "
                                   "  CASE registered WHEN 1 THEN 'Yes' ELSE "
                                   "    CASE registered WHEN 2 THEN 'No' ELSE 'Unknown' "
                                   "    END "
                                   "  END "
                                   "END AS 'registered', "

                                   "COALESCE (recipient.group_id, " + d_recipient_aci + ") IS NOT NULL AS has_id, "

                                   "thread._id IS NOT NULL as has_thread "

                                   "FROM recipient "
                                   "LEFT JOIN groups ON recipient.group_id = groups.group_id " +
                                   "LEFT JOIN thread ON recipient._id = thread.recipient_id " +
                                   (d_database.containsTable("distribution_list") ? "LEFT JOIN distribution_list ON recipient._id = distribution_list.recipient_id " : " ") +
                                   "WHERE recipient.rowid = ?", FKC_rowids.value(i, "rowid"));
          else
            d_database.prettyPrint(d_truncate, "SELECT * FROM " + FKC_results(fkc, "table") + " WHERE rowid = ?", FKC_rowids.value(i, "rowid"));
        }

        // we purposely do not check if anything is actually deleted, some specific entries may appear in multiple FKC violations and
        // already be deleted in a previous iteration through this loop...
        if (!d_database.exec("DELETE FROM " + FKC_results(fkc, "table") + " WHERE rowid = ?", FKC_rowids.value(i, "rowid"))) [[unlikely]]
          return false;
      }
    }
  }
  return true;
}
