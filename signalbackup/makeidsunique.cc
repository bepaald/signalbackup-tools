/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SignalBackup::makeIdsUnique(SignalBackup *source)
{
  Logger::message(__FUNCTION__);

  Logger::message("  Adjusting indexes in tables...");

  for (auto const &dbl : s_databaselinks)
  {
    // skip if table/column does not exist, or if skip is set
    if ((dbl.flags & SKIP) ||
        !d_database.containsTable(dbl.table) ||
        !source->d_database.containsTable(dbl.table) ||
        !source->d_database.tableContainsColumn(dbl.table, dbl.column))
      continue;

    if (dbl.flags & WARN)
    {
      SqliteDB::QueryResults results;
      if (source->d_database.exec("SELECT * FROM " + dbl.table, &results) &&
          results.rows() > 0)
        Logger::warning("Found entries in a usually empty table. Trying to deal with it, but problems may occur.");
    }

    long long int offsetvalue = getMaxUsedId(dbl.table, dbl.column) + 1 - source->getMinUsedId(dbl.table, dbl.column);
    source->setMinimumId(dbl.table, offsetvalue, dbl.column);

    for (auto const &c : dbl.connections)
    {
      if (source->d_databaseversion >= c.mindbvversion && source->d_databaseversion <= c.maxdbvversion)
      {
        if (!source->d_database.containsTable(c.table) || !source->d_database.tableContainsColumn(c.table, c.column))
          continue;

        Logger::message("  Adjusting '", c.table, ".", c.column, "' to match changes in '", dbl.table, "'");

        if (!c.json_path.empty())
        {
          source->d_database.exec("UPDATE " + c.table + " SET " + c.column +
                                  " = json_replace(" + c.column + ", " + c.json_path + ", json_extract(" + c.column + ", " + c.json_path + ") + ?) "
                                  "WHERE json_extract(" + c.column + ", " + c.json_path + ") IS NOT NULL", offsetvalue);
        }
        else if ((c.flags & SET_UNIQUELY))
        {
          // set all values negative
          source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " * -1" +
                                  (c.whereclause.empty() ? "" : " WHERE " + c.whereclause));
          // set to wanted value
          source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " * -1 + ?"
                                  + (c.whereclause.empty() ? "" : " WHERE " + c.whereclause), offsetvalue);
        }
        else
          source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " + ? "
                                  + (c.whereclause.empty() ? "" : " WHERE " + c.whereclause), offsetvalue);
      }
    }

    if (dbl.table == d_part_table)
    {
      // update rowid's in attachments
      std::map<std::pair<uint64_t, int64_t>, DeepCopyingUniquePtr<AttachmentFrame>> newattdb;
      for (auto &att : source->d_attachments)
      {
        AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(att.second.release());
        a->setRowId(a->rowId() + offsetvalue);

        int64_t attachmentid = a->attachmentId();
        newattdb.emplace(std::make_pair(a->rowId(), attachmentid ? attachmentid : -1), a);
      }
      source->d_attachments.clear();
      source->d_attachments = std::move(newattdb);

      /*
        REPLACED WITH JSON OPTION IN DatabaseConnections

        // update rowid in previews (mms.previews contains a json string referencing the 'rowId' == part._id)
      SqliteDB::QueryResults results;
      source->d_database.exec("SELECT _id,previews FROM mms WHERE previews IS NOT NULL", &results);
      std::regex rowid_in_json(".*\"rowId\":([0-9]*)[,}].*");
      std::smatch sm;
      for (uint i = 0; i < results.rows(); ++i)
      {
        std::string line = results.valueAsString(i, "previews");
        if (std::regex_match(line, sm, rowid_in_json))
          if (sm.size() == 2) // 0 is full match, 1 is first submatch (which is what we want)
          {
            line.replace(sm.position(1), sm.length(1), bepaald::toString(bepaald::toNumber<unsigned long>(sm[1]) + offsetvalue));
            source->d_database.exec("UPDATE mms SET previews = ? WHERE _id = ?", {line, results.getValueAs<long long int>(i, "_id")});
          }
      }
      */
    }

    if (dbl.table == "recipient")
    {
      source->updateGroupMembers(offsetvalue);

      // in groups, during the v1 -> v2 update, members may have been removed from the group, these messages
      // are of type "GV1_MIGRATION_TYPE" and have a body that looks like '_id,_id,...|_id,_id,_id,...' (I think, I have
      // not seen one with more than 1 id). These id_s must also be updated.
      source->updateGV1MigrationMessage(offsetvalue);

      //update (old-style)reaction authors
      source->updateReactionAuthors(offsetvalue);
    }

    // compact table if requested
    if (!(dbl.flags & NO_COMPACT))
      source->compactIds(dbl.table, dbl.column);
  }

  /*

    CHECK!

    These are the tables that are imported by importThread(),
    check if they are all handled properly

  */

  // get tables
  std::string q("SELECT sql, name, type FROM sqlite_master");
  SqliteDB::QueryResults results;
  source->d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.isNull(i, 0))
    {
      //Logger::message("Dealing with: ", results.getValueAs<std::string>(i, 1));
      if (results.valueHasType<std::string>(i, 1) &&
          (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
           STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sms_fts")))
        ;//Logger::message("Skipping ", results[i][1].second, " because it is sms_ftssecrettable");
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != d_mms_table + "_fts" &&
                STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), d_mms_table + "_fts")))
        ;//Logger::message("Skipping ", results[i][1].second, " because it is mms_ftssecrettable");
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "emoji_search" &&
                STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "emoji_search")))
        ;//Logger::message("Skipping ", results.getValueAs<std::string>(i, 1), " because it is emoji_search_ftssecrettable");
      else if (results.valueHasType<std::string>(i, 1) &&
               STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sqlite_"))
        ;
      else
        if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
          tables.emplace_back(results.getValueAs<std::string>(i, 1));
    }
  }

  for (std::string const &table : tables)
  {
    if (table == "signed_prekeys" ||
        table == "one_time_prekeys" ||
        table == "sessions" ||
        //table == "job_spec" ||           // this is in the official export. But it makes testing more difficult. it
        //table == "constraint_spec" ||    // should be ok to export these (if present in source), since we are only
        //table == "dependency_spec" ||    // dealing with exported backups (not from live installations) -> they should
        //table == "emoji_search" ||       // have been excluded + the official import should be able to deal with them
        STRING_STARTS_WITH(table, "sms_fts") ||
        STRING_STARTS_WITH(table, d_mms_table + "_fts") ||
        STRING_STARTS_WITH(table, "sqlite_"))
      continue;

    if (std::find_if(s_databaselinks.begin(), s_databaselinks.end(), [table](DatabaseLink const &d){ return d.table == table; }) == s_databaselinks.end())
      Logger::warning("Found table unhandled by ", __FUNCTION__ , " : ", table);
  }
}
