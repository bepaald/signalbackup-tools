/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

bool SignalBackup::exportBackupToFile(std::string const &filename, std::string const &passphrase, bool overwrite, bool keepattachmentdatainmemory)
{
  Logger::message("\nExporting backup to '", filename, "'");

  std::string newpw = passphrase;
  if (newpw == std::string())
    newpw = d_passphrase;
  if (newpw == std::string())
  {
    Logger::error("Need password to create encrypted backup file.");
    return false;
  }

  if (!overwrite && bepaald::fileOrDirExists(filename))
  {
    Logger::error("File '", filename, "' exists, use --overwrite to overwrite");
    return false;
  }

  if (!d_headerframe || !d_fe.init(newpw, d_headerframe->salt(), d_headerframe->salt_length(), d_headerframe->iv(), d_headerframe->iv_length(), d_headerframe->version(), d_verbose))
  {
    Logger::error("Failed to initialize FileEncryptor");
    return false;
  }

  std::ofstream outputfile(filename, std::ios_base::binary);

  // HEADER // Note: HeaderFrame is not encrypted.
  Logger::message("Writing HeaderFrame...");
  if (!d_headerframe)
  {
    Logger::error("HeaderFrame not found");
    return false;
  }
  std::pair<unsigned char *, uint64_t> framedata = d_headerframe->getData();
  if (!framedata.first)
  {
    Logger::error("Failed to get HeaderFrame data");
    return false;
  }
  bool writeok = writeFrameDataToFile(outputfile, framedata);
  delete[] framedata.first;
  if (!writeok)
    return false;

  // VERSION
  Logger::message("Writing DatabaseVersionFrame...");
  if (!d_databaseversionframe)
  {
    Logger::error("DataBaseVersionFrame not found");
    return false;
  }
  if (!writeEncryptedFrame(outputfile, d_databaseversionframe.get()))
    return false;

  // SQL DATABASE + ATTACHMENTS
  Logger::message("Writing SqlStatementFrame(s)...");

  // get and write schema
  SqliteDB::QueryResults results;
  d_database.exec("SELECT sql, name, type FROM sqlite_master WHERE sql NOT NULL", &results);
  std::vector<std::string> tables;
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    if (results.valueHasType<std::string>(i, 1) &&
        (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
         STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sms_fts")))
      continue;//std::cout << "Skipping " << results[i][1].second << " because it is sms_ftssecrettable" << std::endl;

    if (results.valueHasType<std::string>(i, 1) &&
        (results.getValueAs<std::string>(i, 1) != d_mms_table + "_fts" &&
         STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), d_mms_table + "_fts")))
      continue;//std::cout << "Skipping " << results[i][1].second << " because it is mms_ftssecrettable" << std::endl;

    if (results.valueHasType<std::string>(i, 1) &&
        (results.getValueAs<std::string>(i, 1) != "emoji_search" &&
         STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "emoji_search")))
      continue;//std::cout << "Skipping " << results[i][1].second << " because it is emoji_search_ftssecrettable" << std::endl;

    if (results.valueHasType<std::string>(i, 1) &&
        STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sqlite_"))
    {
      // this is normally skipped, but for testing purposes we won't skip if it was found in input
#ifdef BUILT_FOR_TESTING
      if (d_found_sqlite_sequence_in_backup)
        ;
      else
#endif
        continue;
    }

    if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
      tables.emplace_back(results.getValueAs<std::string>(i, 1));

    SqlStatementFrame newframe;
    newframe.setStatementField(results.getValueAs<std::string>(i, 0));

    //std::cout << "Writing SqlStatementFrame..." << std::endl;
    if (!writeEncryptedFrame(outputfile, &newframe))
      return false;
  }

  // write contents of tables
  for (std::string const &table : tables)
  {
    if (table == "signed_prekeys" ||
        table == "one_time_prekeys" ||
        table == "sessions" ||
        //table == "job_spec" ||           // this is in the official export. But it makes testing more difficult. it
        //table == "constraint_spec" ||    // should be ok to export these (if present in source), since we are only
        //table == "dependency_spec" ||    // dealing with exported backups (not from live installations) -> they should
        //table == "emoji_search" ||       // have been excluded + the official import should be able to deal with them
        //table == "sender_keys" ||
        //table == "sender_key_shared" ||
        //table == "pending_retry_receipts" ||
        //table == "avatar_picker" ||
        //table == "remapped_recipients" ||
        //table == "remapped_threads" ||
        STRING_STARTS_WITH(table, "sms_fts") ||
        STRING_STARTS_WITH(table, d_mms_table + "_fts") ||
        STRING_STARTS_WITH(table, "sqlite_"))
      continue;

    d_database.exec("SELECT * FROM " + table, &results);

    if (!d_showprogress)
      Logger::message_start("  Dealing with table '", table, "'... ");

    for (unsigned int i = 0; i < results.rows(); ++i)
    {
      if (d_showprogress)
        Logger::message_overwrite("  Dealing with table '", table, "'... ", i + 1, "/", results.rows(), " entries...");

      SqlStatementFrame newframe = buildSqlStatementFrame(table, results.row(i));

      //std::cout << "Writing SqlStatementFrame..." << std::endl;
      if (!writeEncryptedFrame(outputfile, &newframe))
        return false;

      if (table == d_part_table) // find corresponding attachment
      {
        bool needuniqqueid = d_database.tableContainsColumn(d_part_table, "unique_id");
        long long int rowid = 0, uniqueid = needuniqqueid ? 0 : -1;
        for (unsigned int j = 0; j < results.columns(); ++j)
        {
          if (results.header(j) == "_id" && results.valueHasType<long long int>(i, j))
          {
            rowid = results.getValueAs<long long int>(i, j);
            if (rowid && (uniqueid || !needuniqqueid))
              break;
          }
          else if (needuniqqueid &&
                   results.header(j) == "unique_id" && results.valueHasType<long long int>(i, j))
          {
           //std::cout << "UNIQUEID: " << std::any_cast<long long int>(results[i][j].second) << std::endl;
            uniqueid = results.getValueAs<long long int>(i, j);
            if (rowid && (uniqueid || !needuniqqueid))
              break;
          }
        }
        auto attachment = d_attachments.find({rowid, uniqueid});
        if (attachment != d_attachments.end()) [[likely]]
        {
          if (!writeEncryptedFrame(outputfile, attachment->second.get()))
            return false;
          if (!keepattachmentdatainmemory)
          {
            MEMINFO("BEFORE DROPPING ATTACHMENT DATA");
            attachment->second.get()->clearData();
            MEMINFO("AFTER DROPPING ATTACHMENT DATA");
          }
        }
        else [[unlikely]]
        {
          if (!missingAttachmentExpected(rowid, uniqueid))
          {
            Logger::warning("Attachment data not found (rowid: ", rowid, ", uniqueid: ", uniqueid, ")");
            if (d_showprogress)
              Logger::message_overwrite("  Dealing with table '", table, "'... ", i + 1, "/", results.rows(), " entries...");
          }
        }
      }
      else if (table == "sticker") // find corresponding sticker
      {
        uint64_t rowid = 0;
        for (unsigned int j = 0; j < results.columns(); ++j)
          if (results.header(j) == "_id" && results.valueHasType<long long int>(i, j))
          {
            rowid = results.getValueAs<long long int>(i, j);
            break;
          }
        auto sticker = d_stickers.find(rowid);
        if (sticker != d_stickers.end())
        {
          if (!writeEncryptedFrame(outputfile, sticker->second.get()))
            return false;
          if (!keepattachmentdatainmemory)
            sticker->second.get()->clearData();
        }
        else
        {
          Logger::warning("Sticker data not found (rowid: ", rowid, ")");
          if (d_showprogress)
            Logger::message_overwrite("  Dealing with table '", table, "'... ", i + 1, "/", results.rows(), " entries...");
        }
      }
    }
    if (d_showprogress)
      Logger::message_overwrite("  Dealing with table '", table, "'... ", results.rows(), "/", results.rows(), " entries...done", Logger::Control::ENDOVERWRITE);
    else
      Logger::message_end("done");
  }

  Logger::message("Writing SharedPrefFrame(s)...");
  // SHAREDPREFS
  for (unsigned int i = 0; i < d_sharedpreferenceframes.size(); ++i)
    if (!writeEncryptedFrame(outputfile, d_sharedpreferenceframes[i].get()))
      return false;

  Logger::message("Writing KeyValueFrame(s)...");
  // KEYVALUES
  for (unsigned int i = 0; i < d_keyvalueframes.size(); ++i)
    if (!writeEncryptedFrame(outputfile, d_keyvalueframes[i].get()))
      return false;

  // AVATAR
  Logger::message("Writing Avatars...");
  for (auto const &a : d_avatars)
  {

    if (d_verbose && !a.second.get()) [[unlikely]]
    {
      Logger::error("ASKED TO WRITE NULLPTR-AVATAR. THIS SHOULD BE AN ERROR");
      Logger::error_indent("BUT I'M PRETENDING IT DIDN'T HAPPEN TO FIND THE CAUSE OF IT");
      Logger::error_indent("THE PROGRAM WILL LIKELY CRASH NOW...");
    }

    if (!writeEncryptedFrame(outputfile, a.second.get()))
      return false;
  }

  // END
  Logger::message("Writing EndFrame...");
  if (!d_endframe)
  {
    Logger::error("EndFrame not found.");
    return false;
  }
  if (!writeEncryptedFrame(outputfile, d_endframe.get()))
    return false;

  outputfile.flush();

  Logger::message("Done! Wrote ", outputfile.tellp(), " bytes.");
  return true;
}
