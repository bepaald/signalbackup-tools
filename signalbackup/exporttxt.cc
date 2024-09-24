/*
  Copyright (C) 2023-2024  Selwin van Dijk

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
#include "msgrange.h"

bool SignalBackup::exportTxt(std::string const &directory, std::vector<long long int> const &limittothreads,
                             std::vector<std::string> const &daterangelist, std::string const &selfphone [[maybe_unused]],
                             bool migrate, bool overwrite)
{
  bool databasemigrated = false;
  MemSqliteDB backup_database;

  // >= 168 will work already? (not sure if 168 and 169 were ever in production, I don't have them at least)
  if (d_databaseversion == 167)
  {
    SqliteDB::copyDb(d_database, backup_database);
    if (!migrateDatabase(167, 170))
    {
      Logger::error("Failed to migrate currently unsupported database version (", d_databaseversion, ")."
                    " Please upgrade your database");
      SqliteDB::copyDb(backup_database, d_database);
      return false;
    }
    else
      databasemigrated = true;
  }
  else if (d_databaseversion < 167)
  {
    if (!migrate)
    {
      Logger::error("Currently unsupported database version (", d_databaseversion, ").");
      Logger::error_indent("Please upgrade your database or append the `--migratedb' option to attempt to");
      Logger::error_indent("migrate this database to a supported version.");
      return false;
    }
    SqliteDB::copyDb(d_database, backup_database);
    if (!migrateDatabase(d_databaseversion, 170)) // migrate == TRUE, but migration fails
    {
      Logger::error("Failed to migrate currently unsupported database version (", d_databaseversion, ")."
                    " Please upgrade your database");
      SqliteDB::copyDb(backup_database, d_database);
      return false;
    }
    else
      databasemigrated = true;
  }

  // check if dir exists, create if not
  if (!prepareOutputDirectory(directory, overwrite))
  {
    if (databasemigrated)
      SqliteDB::copyDb(backup_database, d_database);
    return false;
  }

  // if (!bepaald::fileOrDirExists(directory))
  // {
  //   // try to create
  //   if (!bepaald::createDir(directory))
  //   {
  //     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
  //               << ": Failed to create directory `" << directory << "'"
  //               << " (errno: " << std::strerror(errno) << ")" << std::endl; // note: errno is not required to be set by std
  //     // temporary !!
  //     {
  //       std::error_code ec;
  //       std::filesystem::space_info const si = std::filesystem::space(directory, ec);
  //       if (!ec)
  //       {
  //         std::cout << "Available  : " << static_cast<std::intmax_t>(si.available) << std::endl;
  //         std::cout << "Backup size: " << d_fd->total() << std::endl;
  //       }
  //     }
  //     if (databasemigrated)
  //       SqliteDB::copyDb(backup_database, d_database);
  //     return false;
  //   }
  // }

  // // directory exists, but
  // // is it a dir?
  // if (!bepaald::isDir(directory))
  // {
  //   std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
  //             << ": `" << directory << "' is not a directory." << std::endl;
  //   if (databasemigrated)
  //     SqliteDB::copyDb(backup_database, d_database);
  //   return false;
  // }

  // // and is it empty?
  // if (!bepaald::isEmpty(directory))
  // {
  //   if (!overwrite)
  //   {
  //     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
  //               << ": Directory '" << directory << "' is not empty. Use --overwrite to clear directory before export." << std::endl;
  //     if (databasemigrated)
  //       SqliteDB::copyDb(backup_database, d_database);
  //     return false;
  //   }
  //   std::cout << "Clearing contents of directory '" << directory << "'..." << std::endl;
  //   if (!bepaald::clearDirectory(directory))
  //   {
  //     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
  //               << ": Failed to empty directory '" << directory << "'" << std::endl;
  //     if (databasemigrated)
  //       SqliteDB::copyDb(backup_database, d_database);
  //     return false;
  //   }
  // }

  // check and warn about selfid & note-to-self thread
  d_selfid = selfphone.empty() ? scanSelf() : d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
  if (d_selfid == -1)
  {
    if (!selfphone.empty())
      Logger::warning("Failed to determine id of 'self'.");
    else
      Logger::warning("Failed to determine id of 'self'. Consider passing `--setselfid \"[phone]\"' to set it manually");
  }
  else
    d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));

  std::vector<long long int> threads = ((limittothreads.empty() || (limittothreads.size() == 1 && limittothreads[0] == -1)) ?
                                        threadIds() : limittothreads);

  std::map<long long int, RecipientInfo> recipient_info;

  // set where-clause for date requested
  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (unsigned int i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});
  std::string datewhereclause;
  for (unsigned int i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      Logger::error("Skipping range: '", dateranges[i].first, " - ", dateranges[i].second, "'. Failed to parse or invalid range.");
      Logger::error_indent(startrange, " ", endrange);
      continue;
    }
    Logger::message("  Using range: ", dateranges[i].first, " - ", dateranges[i].second, " (", startrange, " - ", endrange, ")");

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    dateranges[i].first = bepaald::toString(startrange);
    dateranges[i].second = bepaald::toString(endrange);

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "date_received BETWEEN "s + dateranges[i].first + " AND " + dateranges[i].second;
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }
  std::sort(dateranges.begin(), dateranges.end());


  // handle each thread
  for (int t : threads)
  {

    Logger::message("Dealing with thread ", t);

    //bool is_note_to_self = false;//(t == note_to_self_thread_id);

    // get recipient_id for thread;
    SqliteDB::QueryResults recid;
    long long int thread_recipient_id = -1;
    if (!d_database.exec("SELECT _id," + d_thread_recipient_id + " FROM thread WHERE _id = ?", t, &recid) ||
        recid.rows() != 1 || (thread_recipient_id = recid.valueAsInt(0, d_thread_recipient_id)) == -1)
    {
      Logger::error("Failed to find recipient_id for thread (", t, ")... skipping");
      continue;
    }
    long long int thread_id = recid.getValueAs<long long int>(0, "_id");

    bool isgroup = false;
    SqliteDB::QueryResults groupcheck;
    d_database.exec("SELECT group_id FROM recipient WHERE _id = ? AND group_id IS NOT NULL", thread_recipient_id, &groupcheck);
    if (groupcheck.rows())
      isgroup = true;

    // now get all messages
    SqliteDB::QueryResults messages;
    if (!d_database.exec("SELECT "s
                         "_id, " + d_mms_recipient_id + ", body, "
                         "date_received, " + d_mms_type + ", "
                         //"quote_id, quote_author, quote_body, quote_mentions, "
                         // + d_mms_delivery_receipts + ", " + d_mms_read_receipts + ", "
                         "IFNULL(remote_deleted, 0) AS remote_deleted, "
                         "IFNULL(view_once, 0) AS view_once, " +
                         (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras, " : "") +
                         "expires_in"
                         //, message_ranges, "
                         //+ (d_database.tableContainsColumn(d_mms_table, "original_message_id") ? "original_message_id, " : "") +
                         //+ (d_database.tableContainsColumn(d_mms_table, "revision_number") ? "revision_number, " : "") +
                         //"json_extract(link_previews, '$[0].title') AS link_preview_title, "
                         //"json_extract(link_previews, '$[0].description') AS link_preview_description "
                         " FROM " + d_mms_table + " "
                         "WHERE thread_id = ?"
                         + datewhereclause +
                         + (d_database.tableContainsColumn(d_mms_table, "latest_revision_id") ? " AND latest_revision_id IS NULL" : "") +
                         " ORDER BY date_received ASC", t, &messages))
    {
      Logger::error("Failed to query database for messages");
      if (databasemigrated)
        SqliteDB::copyDb(backup_database, d_database);
      return false;
    }
    if (messages.rows() == 0)
      continue;

    // get all recipients in thread (group member (past and present), quote/reaction authors, mentions)
    std::set<long long int> all_recipients_ids = getAllThreadRecipients(t);

    //try to set any missing info on recipients
    setRecipientInfo(all_recipients_ids, &recipient_info);

    // get conversation name, sanitize it and set outputfilename
    if (recipient_info.find(thread_recipient_id) == recipient_info.end())
    {
      Logger::error("Failed set recipient info for thread (", t, ")... skipping");
      continue;
    }

    std::string filename = /*(is_note_to_self ? "Note to self (_id"s + bepaald::toString(thread_id) + ")"
                             : */sanitizeFilename(recipient_info[thread_recipient_id].display_name + " (_id" + bepaald::toString(thread_id) + ").txt")/*)*/;

    if (bepaald::fileOrDirExists(directory + "/" + filename))
    {
      Logger::error("Refusing to overwrite existing file");
      if (databasemigrated)
        SqliteDB::copyDb(backup_database, d_database);
      return false;
    }

    std::ofstream txtoutput(directory + "/" + filename, std::ios_base::binary);
    if (!txtoutput.is_open())
    {
      Logger::error("Failed to open '", directory, "/", filename, " for writing.");
      if (databasemigrated)
        SqliteDB::copyDb(backup_database, d_database);
      return false;
    }

    for (unsigned int i = 0; i < messages.rows(); ++i)
    {
      bool is_deleted = messages.getValueAs<long long int>(i, "remote_deleted") == 1;
      bool is_viewonce = messages.getValueAs<long long int>(i, "view_once") == 1;
      if (is_deleted || is_viewonce)
        continue;
      long long int type = messages.getValueAs<long long int>(i, d_mms_type);
      long long int msg_id = messages.getValueAs<long long int>(i, "_id");
      //bool incoming = !Types::isOutgoing(messages.getValueAs<long long int>(i, d_mms_type));
      long long int msg_recipient_id = messages.valueAsInt(i, d_mms_recipient_id);
      if (isgroup && Types::isOutgoing(type))
        msg_recipient_id = d_selfid;
      if (msg_recipient_id == -1) [[unlikely]]
      {
        Logger::warning("Failed to get message recipient id. Skipping.");
        continue;
      }
      std::string body = messages.valueAsString(i, "body");
      std::string readable_date = bepaald::toDateString(messages.getValueAs<long long int>(i, "date_received") / 1000,
                                                          "%b %d, %Y %H:%M:%S");
      SqliteDB::QueryResults attachment_results;
      d_database.exec("SELECT "
                      "_id, " +
                      (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id") + ", " +
                      d_part_ct + ", "
                      "file_name, "
                      + d_part_pending + ", " +
                      (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()) +
                      "sticker_pack_id "
                      "FROM " + d_part_table + " WHERE " + d_part_mid + " IS ? AND quote IS 0", msg_id, &attachment_results);
      // check attachments for long message body -> replace cropped body & remove from attachment results
      setLongMessageBody(&body, &attachment_results);

      SqliteDB::QueryResults mention_results;
      if (d_database.containsTable("mention"))
        d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

      SqliteDB::QueryResults reaction_results;
      d_database.exec("SELECT emoji, author_id, DATETIME(ROUND(date_sent / 1000), 'unixepoch', 'localtime') AS 'date_sent', "
                      "DATETIME(ROUND(date_received / 1000), 'unixepoch', 'localtime') AS 'date_received' "
                      "FROM reaction WHERE message_id IS ?", msg_id, &reaction_results);

      if (Types::isStatusMessage(type) || Types::isCallType(type))
      {
        std::string statusmsg;
        if (!body.empty() ||
            !(d_database.tableContainsColumn(d_mms_table, "message_extras") &&
              messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras")))
          statusmsg = decodeStatusMessage(body, messages.getValueAs<long long int>(i, "expires_in"), type,
                                          getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name);
        else if (d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                 messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"))
          statusmsg = decodeStatusMessage(messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "message_extras"),
                                          messages.getValueAs<long long int>(i, "expires_in"), type,
                                          getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name);

        txtoutput << "[" << readable_date << "] " << "***" << " " << statusmsg <<  '\n';
      }
      else
      {
        // get originating username
        std::string user = getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name;

        for (unsigned int a = 0; a < attachment_results.rows(); ++a)
        {
          std::string content_type = attachment_results.valueAsString(a, d_part_ct);
          if (content_type == "text/x-signal-plain") [[unlikely]]
            continue;

          std::string attachment_filename;
          if (!attachment_results.isNull(a, "file_name") && !attachment_results(a, "file_name").empty())
            attachment_filename = '"' + attachment_results(a, "file_name") + '"';
          else if (!content_type.empty())
            attachment_filename = "of type " + content_type;

          txtoutput << "[" << readable_date << "] *** <" << user << "> sent file"
                    << (attachment_filename.empty() ? "" : " " + attachment_filename);
          if (body.empty())
            TXTaddReactions(&reaction_results, &txtoutput);
          txtoutput << '\n';
        }
        if (!body.empty())
        {
          // prep body for mentions...
          std::vector<Range> ranges;
          for (unsigned int m = 0; m < mention_results.rows(); ++m)
          {
            std::string displayname = getNameFromRecipientId(mention_results.getValueAs<long long int>(m, "recipient_id"));
            if (displayname.empty())
              continue;
            ranges.emplace_back(Range{mention_results.getValueAs<long long int>(m, "range_start"),
                                      mention_results.getValueAs<long long int>(m, "range_length"),
                                      "",
                                      "@" + displayname,
                                      "",
                                      false});
          }
          applyRanges(&body, &ranges, nullptr);

          txtoutput << "[" << readable_date << "] <" << user << "> " << body;
          TXTaddReactions(&reaction_results, &txtoutput);
          txtoutput << '\n';
        }
      }
    }
  }

  Logger::message("All done!");
  if (databasemigrated)
  {
    Logger::message("restoring migrated database...");
    SqliteDB::copyDb(backup_database, d_database);
  }
  return true;
}
