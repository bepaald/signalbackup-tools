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

/*
  Many thanks to Gertjan van den Burg (https://github.com/GjjvdBurg) for his
  original project (used with permission) without which this function would
  not have come together so quickly (if at all).
*/

#include "signalbackup.ih"

#include <cerrno>

bool SignalBackup::exportHtml(std::string const &directory, std::vector<long long int> const &limittothreads,
                              std::vector<std::string> const &daterangelist, std::string const &splitby,
                              long long int split, std::string const &selfphone, bool calllog, bool searchpage,
                              bool stickerpacks, bool migrate, bool overwrite, bool append, bool lighttheme,
                              bool themeswitching, bool addexportdetails, bool blocked, bool fullcontacts,
                              bool settings, bool receipts, bool originalfilenames, bool linkify, bool chatfolders)
{
  Logger::message("Starting HTML export to '", directory, "'");

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

  if (originalfilenames && append) [[unlikely]]
    Logger::warning("Options 'originalfilenames' and 'append' are incompatible");

  // // check if dir exists, create if not
  if (!prepareOutputDirectory(directory, overwrite, !originalfilenames /*allowappend only allowed when not using original filenames*/, append))
  {
    if (databasemigrated)
      SqliteDB::copyDb(backup_database, d_database);
    return false;
  }

  // check and warn about selfid & note-to-self thread
  long long int note_to_self_thread_id = -1;
  d_selfid = selfphone.empty() ? scanSelf() : d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE " + d_recipient_e164 + " = ?", selfphone, -1);
  if (d_selfid == -1)
  {
    if (!selfphone.empty())
      Logger::warning("Failed to determine id of 'self'.");
    else // if (selfphone.empty())
      Logger::warning("Failed to determine Note-to-self thread. Consider passing `--setselfid \"[phone]\"' to set it manually");
  }
  else
  {
    note_to_self_thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", d_selfid, -1);
    d_selfuuid = bepaald::toLower(d_database.getSingleResultAs<std::string>("SELECT " + d_recipient_aci + " FROM recipient WHERE _id = ?", d_selfid, std::string()));
  }

  std::vector<long long int> threads = ((limittothreads.empty() || (limittothreads.size() == 1 && limittothreads[0] == -1)) ?
                                        threadIds() : limittothreads);
  std::vector<long long int> excludethreads; // threads excluded by limittodates...

  std::map<long long int, RecipientInfo> recipient_info;

  // set where-clause for date requested
  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (unsigned int i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});
  std::string datewhereclause;
  std::string datewhereclausecalllog;
  long long int maxdate = -1;
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

    if (endrange > maxdate)
      maxdate = endrange;

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    dateranges[i].first = bepaald::toString(startrange);
    dateranges[i].second = bepaald::toString(endrange);

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "date_received BETWEEN "s + dateranges[i].first + " AND " + dateranges[i].second;
    datewhereclausecalllog += (datewhereclausecalllog.empty() ? " AND (" : " OR ") + "timestamp BETWEEN "s + dateranges[i].first + " AND " + dateranges[i].second;
    if (i == dateranges.size() - 1)
    {
      datewhereclause += ')';
      datewhereclausecalllog += ')';
    }
  }
  std::sort(dateranges.begin(), dateranges.end());

  // // get releasechannel thread, to skip
  // int releasechannel = -1;
  // for (auto const &skv : d_keyvalueframes)
  //   if (skv->key() == "releasechannel.recipient_id")
  //     releasechannel = bepaald::toNumber<int>(skv->value());

  SqliteDB::QueryResults search_idx_results;
  std::ofstream searchidx;
  bool searchidx_write_started = false;
  // start search index page
  if (searchpage)
  {
    searchidx.open(directory + "/" + "searchidx.js", std::ios_base::binary);
    if (!searchidx.is_open())
    {
      Logger::error("Failed to open 'searchidx.js' for writing");
      return false;
    }
    searchidx << "message_idx = [" << std::endl;
  }

  std::string exportdetails_html;
  if (addexportdetails)
  {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filename = d_filename;
    HTMLescapeString(&filename);

    std::string options = "--exporthtml " + directory;
    HTMLescapeString(&options);
    if (!limittothreads.empty())
    {
      options += "<br>--limittothreads ";
      for (unsigned int i = 0; i < limittothreads.size(); ++i)
        options += bepaald::toString(limittothreads[i]) + (i < limittothreads.size() - 1  ? "," : "");
    }
    if (!daterangelist.empty())
    {
      options += "<br>--limittodates ";
      for (unsigned int i = 0; i < daterangelist.size(); i += 2)
        options += daterangelist[i] + "&ndash;" + daterangelist[i + 1] + (i < daterangelist.size() - 1  ? "," : "");
    }
    if (split > -1)
      options += "<br>--split " + bepaald::toString(split);
    if (!splitby.empty())
      options += "<br>--split-by " + splitby;
    if (receipts)
      options += "<br>--includereceipts";
    if (calllog)
      options += "<br>--includecalllog";
    if (blocked)
      options += "<br>--includeblockedlist";
    if (settings)
      options += "<br>--includesettings";
    if (addexportdetails)
      options += "<br>--addexportdetails";
    if (searchpage)
      options += "<br>--searchpage";
    if (stickerpacks)
      options += "<br>--stickerpacks";
    if (overwrite)
      options += "<br>--overwrite";
    if (append)
      options += "<br>--append";
    if (d_verbose)
      options += "<br>--verbose";
    if (lighttheme)
      options += "<br>--light";
    if (themeswitching)
      options += "<br>--themeswitching";
    if (originalfilenames)
      options += "<br>--originalfilenames";
    if (linkify)
      options += "<br>--linkify";
    if (chatfolders)
      options += "<br>--chatfolders";

    SqliteDB::QueryResults res;
    d_database.exec("SELECT MIN(" + d_mms_table + ".date_received) AS 'mindate', MAX(" + d_mms_table + ".date_received) AS 'maxdate' FROM " + d_mms_table, &res);
    std::string date_range = bepaald::toDateString(res.valueAsInt(0, "mindate") / 1000, "%Y-%m-%d %H:%M:%S") + "&ndash;" +
      bepaald::toDateString(res.valueAsInt(0, "maxdate") / 1000, "%Y-%m-%d %H:%M:%S");

    exportdetails_html =
      "    <div class=\"export-details\">\n"
      "      <div class=\"export-details-fullwidth\"><u>Export details</u></div>\n"
      "      <div>Exported by signalbackup-tools version:</div><div>"s + VERSIONDATE + "</div>\n"
      "      <div>Exported date:</div><div>" + bepaald::toDateString(now, "%Y-%m-%d %H:%M:%S") + "</div>\n"
      "      <div>Export options:</div><div>" + options + "</div>\n"
      "      <div>Backup file:</div> <div>" + filename + "</div>\n" +
      (d_fd ? ("      <div>Backup file size:</div><div>" + bepaald::toString(d_fd->total()) + " bytes</div>\n") : "") +
      "      <div>Backup range:</div><div>" + date_range + "</div>\n"
      "      <div>Backup file version:</div><div>" + bepaald::toString(d_backupfileversion) + "</div>\n"
      "      <div>Database version:</div><div>" + bepaald::toString(d_databaseversion) + "</div>\n"
      "    </div>\n";
  }

  std::string periodsplitformat;
  if (!splitby.empty())
  {
    auto icasecompare = [](std::string const &a, std::string const &b)
    {
      return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char ca, char cb) { return std::tolower(ca) == std::tolower(cb); });
    };

    if (icasecompare(splitby, "year"))
      periodsplitformat = "%Y";
    else if (icasecompare(splitby, "month"))
      periodsplitformat = "%Y%m";
    else if (icasecompare(splitby, "week"))
      periodsplitformat = "%Y%W";
    else if (icasecompare(splitby, "day"))
      periodsplitformat = "%Y%j";
    else
      Logger::warning("Ignoring invalid 'split-by'-value ('", splitby, "')");
  }

  for (unsigned int t_idx = 0; t_idx < threads.size(); ++t_idx)
  {
    int t = threads[t_idx];

    // if (t == releasechannel)
    // {
    //   std::cout << "INFO: Skipping releasechannel thread..." << std::endl;
    //   continue;
    // }

    Logger::message("Dealing with thread ", t);

    bool is_note_to_self = (t == note_to_self_thread_id);

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
    d_database.exec("SELECT "s
                    "_id, " + d_mms_recipient_id + ", body, "
                    "MIN(date_received, " + d_mms_date_sent + ") AS bubble_date, "
                    "date_received, " + d_mms_date_sent + ", " + d_mms_type + ", "
                    + (!periodsplitformat.empty() ? "strftime('" + periodsplitformat + "', IFNULL(date_received, 0) / 1000, 'unixepoch', 'localtime')" : "''") + " AS periodsplit, "
                    "quote_id, quote_author, quote_body, quote_mentions, quote_missing, "
                    "attcount, reactioncount, mentioncount, "
                    + d_mms_delivery_receipts + ", " + d_mms_read_receipts + ", IFNULL(remote_deleted, 0) AS remote_deleted, "
                    "IFNULL(view_once, 0) AS view_once, expires_in, " + d_mms_ranges + ", shared_contacts, "
                    + (d_database.tableContainsColumn(d_mms_table, "original_message_id") ? "original_message_id, " : "") +
                    + (d_database.tableContainsColumn(d_mms_table, "revision_number") ? "revision_number, " : "") +
                    + (d_database.tableContainsColumn(d_mms_table, "parent_story_id") ? "parent_story_id, " : "") +
                    + (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras, " : "") +
                    + (d_database.tableContainsColumn(d_mms_table, "receipt_timestamp") ? "receipt_timestamp, " : "-1 AS receipt_timestamp, ") + // introduced in 117
                    "json_extract(link_previews, '$[0].title') AS link_preview_title, "
                    "json_extract(link_previews, '$[0].description') AS link_preview_description "
                    "FROM " + d_mms_table + " "
                    // get attachment count for message:
                    "LEFT JOIN (SELECT " + d_part_mid + " AS message_id, COUNT(*) AS attcount FROM " + d_part_table + " GROUP BY message_id) AS attmnts ON " + d_mms_table + "._id = attmnts.message_id "
                    // get reaction count for message:
                    "LEFT JOIN (SELECT message_id, COUNT(*) AS reactioncount FROM reaction GROUP BY message_id) AS rctns ON " + d_mms_table + "._id = rctns.message_id "
                    // get mention count for message:
                    "LEFT JOIN (SELECT message_id, COUNT(*) AS mentioncount FROM mention GROUP BY message_id) AS mntns ON " + d_mms_table + "._id = mntns.message_id "
                    "WHERE thread_id = ?"
                    + datewhereclause +
                    + (d_database.tableContainsColumn(d_mms_table, "latest_revision_id") ? " AND latest_revision_id IS NULL " : " ") +
                    + (d_database.tableContainsColumn(d_mms_table, "story_type") ? " AND story_type = 0 OR story_type IS NULL " : "") + // storytype NONE(0), STORY_WITH(OUT)_REPLIES(1/2), TEXT_...(3/4)
                    " ORDER BY date_received ASC", t, &messages);
    if (messages.rows() == 0)
    {
      if (d_verbose) [[unlikely]]
        Logger::message("Thread appears empty. Skipping...");
      excludethreads.push_back(t);
      continue;
    }

    // get all recipients in thread (group member (past and present), quote/reaction authors, mentions)
    std::set<long long int> all_recipients_ids = getAllThreadRecipients(t);

    //try to set any missing info on recipients
    setRecipientInfo(all_recipients_ids, &recipient_info);

    //for (auto const &ri : recipient_info)
    //  std::cout << ri.first << ": " << ri.second.display_name << std::endl;

    // get conversation name, sanitize it and create dir
    if (recipient_info.find(thread_recipient_id) == recipient_info.end())
    {
      Logger::error("Failed set recipient info for thread (", t, ")... skipping");
      continue;
    }

    std::string threaddir = (is_note_to_self ? "Note to self (_id"s + bepaald::toString(thread_id) + ")"
                             : sanitizeFilename(recipient_info[thread_recipient_id].display_name + " (_id" + bepaald::toString(thread_id) + ")"));

    //if (!append)
    //  makeFilenameUnique(directory, &threaddir);

    if (bepaald::fileOrDirExists(directory + "/" + threaddir))
    {
      if (!bepaald::isDir(directory + "/" + threaddir))
      {
        Logger::error("dir is regular file");
        if (databasemigrated)
          SqliteDB::copyDb(backup_database, d_database);
        return false;
      }
      if (!append && !overwrite) // should be impossible at this point....
      {
        Logger::error("Refusing to overwrite existing directory");
        if (databasemigrated)
          SqliteDB::copyDb(backup_database, d_database);
        return false;
      }
    }
    else if (!bepaald::createDir(directory + "/" + threaddir)) // try to create it
    {
      Logger::error("Failed to create directory `", directory, "/", threaddir, "'",
                    " (errno: ", std::strerror(errno), ")"); // note: errno is not required to be set by std
      // temporary !!
      {
        std::error_code ec;
        std::filesystem::space_info const si = std::filesystem::space(directory, ec);
        if (!ec)
        {
          Logger::message("Available: ", static_cast<std::intmax_t>(si.available));
          Logger::message(" Filesize: ", d_fd->total());
        }
      }
      if (databasemigrated)
        SqliteDB::copyDb(backup_database, d_database);
      return false;
    }

    // now append messages to html
    std::map<long long int, std::string> written_avatars; // maps recipient_ids to the path of a written avatar file.
    unsigned int messagecount = 0; // current message
    unsigned int max_msg_per_page = messages.rows();
    int pagenumber = 0; // current page
    int totalpages = 1;
    if (split > 0)
    {
      totalpages = (messages.rows() / split) + (messages.rows() % split > 0 ? 1 : 0);
      max_msg_per_page = messages.rows() / totalpages + (messages.rows() % totalpages ? 1 : 0);
    }
    if (!periodsplitformat.empty())
      totalpages = d_database.getSingleResultAs<long long int>("SELECT COUNT(DISTINCT strftime('" + periodsplitformat +  "', IFNULL(date_received, 0) / 1000, 'unixepoch', 'localtime')) "
                                                               "FROM message WHERE thread_id = ?" + datewhereclause, t, 1);

    // std::cout << "Split: " << split << std::endl;
    // std::cout << "N MSG: " << messages.rows() << std::endl;
    // std::cout << "MAX PER PAGE: " << max_msg_per_page << std::endl;
    // std::cout << "N PAGES: " << totalpages << std::endl;

    unsigned int daterangeidx = 0;

    while (true)
    {
      std::string previous_period_split_string(messages(messagecount, "periodsplit"));
      std::string previous_day_change;
      // create output-file
      std::string raw_base_filename = (is_note_to_self ? "Note to self" : recipient_info[thread_recipient_id].display_name);
      std::string filename = sanitizeFilename(raw_base_filename + (pagenumber > 0 ? "_" + bepaald::toString(pagenumber) : "") + ".html");
      std::ofstream htmloutput(directory + "/" + threaddir + "/" + filename, std::ios_base::binary);
      if (!htmloutput.is_open())
      {
        Logger::error("Failed to open '", directory, "/", threaddir, "/", filename, " for writing.");
        if (databasemigrated)
          SqliteDB::copyDb(backup_database, d_database);
        return false;
      }

      // create start of html (css, head, start of body
      HTMLwriteStart(htmloutput, thread_recipient_id, directory, threaddir, isgroup, is_note_to_self,
                     all_recipients_ids, &recipient_info, &written_avatars, overwrite, append,
                     lighttheme, themeswitching, searchpage, addexportdetails);
      while (messagecount < (max_msg_per_page * (pagenumber + 1)) &&
             messages(messagecount, "periodsplit") == previous_period_split_string)
      {
        long long int msg_id = messages.getValueAs<long long int>(messagecount, "_id");
        long long int msg_recipient_id = messages.valueAsInt(messagecount, d_mms_recipient_id);
        if (msg_recipient_id == -1) [[unlikely]]
        {
          Logger::warning("Failed to get message recipient id. Skipping.");
          continue;
        }
        long long int original_message_id = (d_database.tableContainsColumn(d_mms_table, "original_message_id") ?
                                             messages.valueAsInt(messagecount, "original_message_id") :
                                             -1);
        std::string readable_date =
          bepaald::toDateString(messages.getValueAs<long long int>(messagecount, "bubble_date") / 1000, //(/*(original_message_id != -1) ? "date_received" : */d_mms_date_sent)) / 1000,
                                "%b %d, %Y %H:%M:%S");
        std::string readable_date_day =
          bepaald::toDateString(messages.getValueAs<long long int>(messagecount, "bubble_date") / 1000, //(/*(original_message_id != -1) ? "date_received" : */d_mms_date_sent)) / 1000,
                                "%b %d, %Y");
        bool incoming = !Types::isOutgoing(messages.getValueAs<long long int>(messagecount, d_mms_type));
        bool is_deleted = messages.getValueAs<long long int>(messagecount, "remote_deleted") == 1;
        bool is_viewonce = messages.getValueAs<long long int>(messagecount, "view_once") == 1;
        std::string body = messages.valueAsString(messagecount, "body");
        std::string shared_contacts = messages.valueAsString(messagecount, "shared_contacts");
        std::string quote_body = messages.valueAsString(messagecount, "quote_body");
        long long int expires_in = messages.getValueAs<long long int>(messagecount, "expires_in");
        long long int type = messages.getValueAs<long long int>(messagecount, d_mms_type);
        bool hasquote = !messages.isNull(messagecount, "quote_id") && messages.getValueAs<long long int>(messagecount, "quote_id");
        bool quote_missing = messages.valueAsInt(messagecount, "quote_missing", 0) != 0;
        bool story_reply = (d_database.tableContainsColumn(d_mms_table, "parent_story_id") ? messages.valueAsInt(messagecount, "parent_story_id", 0) : 0);
        long long int attachmentcount = messages.valueAsInt(messagecount, "attcount", 0);
        long long int reactioncount = messages.valueAsInt(messagecount, "reactioncount", 0);
        long long int mentioncount = messages.valueAsInt(messagecount, "mentioncount", 0);

        SqliteDB::QueryResults attachment_results;
        if (attachmentcount > 0)
          d_database.exec("SELECT " +
                          d_part_table + "._id, " +
                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id") + ", " +
                          d_part_ct + ", "
                          "file_name, "
                          + d_part_pending + ", " +
                          (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()) +
                          "sticker_pack_id, " +
                          d_mms_table + ".date_received AS date_received "
                          "FROM " + d_part_table + " "
                          "LEFT JOIN " + d_mms_table + " ON " + d_mms_table + "._id = " + d_part_table + "." + d_part_mid + " "
                          "WHERE " + d_part_mid + " IS ? "
                          "AND quote IS ? "
                          " ORDER BY display_order ASC, " + d_part_table + "._id ASC", {msg_id, 0}, &attachment_results);

        // check attachments for long message body -> replace cropped body & remove from attachment results
        setLongMessageBody(&body, &attachment_results);

        SqliteDB::QueryResults quote_attachment_results;
        if (attachmentcount > 0)
          d_database.exec("SELECT " +
                          d_part_table + "._id, " +
                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id") + ", " +
                          d_part_ct + ", "
                          "file_name, "
                          + d_part_pending + ", " +
                          (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()) +
                          "sticker_pack_id, " +
                          d_mms_table + ".date_received AS date_received "
                          "FROM " + d_part_table + " "
                          "LEFT JOIN " + d_mms_table + " ON " + d_mms_table + "._id = " + d_part_table + "." + d_part_mid + " "
                          "WHERE " + d_part_mid + " IS ? "
                          "AND quote IS ? "
                          " ORDER BY display_order ASC, " + d_part_table + "._id ASC", {msg_id, 1}, &quote_attachment_results);

        SqliteDB::QueryResults mention_results;
        if (mentioncount > 0)
          d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

        SqliteDB::QueryResults reaction_results;
        if (reactioncount > 0)
          d_database.exec("SELECT emoji, author_id, DATETIME(date_sent / 1000, 'unixepoch', 'localtime') AS 'date_sent', DATETIME(date_received / 1000, 'unixepoch', 'localtime') AS 'date_received' "
                          "FROM reaction WHERE message_id IS ?", msg_id, &reaction_results);

        SqliteDB::QueryResults edit_revisions;
        if (original_message_id != -1 && d_database.tableContainsColumn(d_mms_table, "revision_number"))
          d_database.exec("SELECT _id,body,date_received," + d_mms_date_sent + ",revision_number FROM " + d_mms_table +
                          " WHERE _id = ?1 OR original_message_id = ?1 ORDER BY " + d_mms_date_sent + " ASC", // skip actual current message
                          original_message_id, &edit_revisions);

        bool issticker = (attachment_results.rows() == 1 && !attachment_results.isNull(0, "sticker_pack_id"));

        IconType icon = IconType::NONE;
        if (Types::isStatusMessage(type))
        {
          // decode from body if (body not empty) OR (message_extras not available)
          if (!body.empty() ||
              !(d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras")))
            body = decodeStatusMessage(body, messages.getValueAs<long long int>(messagecount, "expires_in"),
                                       type, getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name, &icon);
          else if (d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                   messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras"))
            body = decodeStatusMessage(messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras"),
                                       messages.getValueAs<long long int>(messagecount, "expires_in"), type,
                                       getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name, &icon);
        }

        // prep body (scan emoji? -> in <span>) and handle mentions...
        // if (prepbody)
        std::vector<std::tuple<long long int, long long int, long long int>> mentions;
        for (unsigned int mi = 0; mi < mention_results.rows(); ++mi)
          mentions.emplace_back(std::make_tuple(mention_results.getValueAs<long long int>(mi, "recipient_id"),
                                                mention_results.getValueAs<long long int>(mi, "range_start"),
                                                mention_results.getValueAs<long long int>(mi, "range_length")));
        std::pair<std::shared_ptr<unsigned char []>, size_t> brdata(nullptr, 0);
        if (!messages.isNull(messagecount, d_mms_ranges))
          brdata = messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, d_mms_ranges);

        bool only_emoji = HTMLprepMsgBody(&body, mentions, &recipient_info, incoming, brdata, linkify, false /*isquote*/);

        bool nobackground = false;
        if ((only_emoji && !hasquote && !attachment_results.rows()) ||  // if no quote etc
            issticker) // or sticker
          nobackground = true;

        // same for quote_body!
        mentions.clear();
        std::pair<std::shared_ptr<unsigned char []>, size_t> quote_mentions{nullptr, 0};
        if (!messages.isNull(messagecount, "quote_mentions"))
          quote_mentions = messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "quote_mentions");
        HTMLprepMsgBody(&quote_body, mentions, &recipient_info, incoming, quote_mentions, linkify, true);

        // insert date-change message
        if (readable_date_day != previous_day_change)
        {
          htmloutput << R"(          <div class="msg msg-date-change">
            <p>
              )" << readable_date_day << R"(
            </p>
          </div>)" << std::endl << std::endl;
        }
        previous_day_change = readable_date_day;
        previous_period_split_string = messages(messagecount, "periodsplit");

        /*

          LINKIFY?

          Notes:
          - currently this matches 'yes.combine them please' as 'yes.com'. (maybe try to match per word?)
          - dont copy entire body, just match on stringview, and update it from suffix start?
          - this interacts with prepbody/escapehtml

        std::regex url_regex("(?:(?:(?:(?:(?:http|ftp|https|localhost):\\/\\/)|(?:www\\.)|(?:xn--)){1}(?:[\\w_-]+(?:(?:\\.[\\w_-]+)+))(?:[\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])?)|(?:(?:[\\w_-]{2,200}(?:(?:\\.[\\w_-]+)*))(?:(?:\\.[\\w_-]+\\/(?:[\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])?)|(?:\\.(?:(?:org|com|net|edu|gov|mil|int|arpa|biz|info|unknown|one|ninja|network|host|coop|tech)|(?:jp|br|it|cn|mx|ar|nl|pl|ru|tr|tw|za|be|uk|eg|es|fi|pt|th|nz|cz|hu|gr|dk|il|sg|uy|lt|ua|ie|ir|ve|kz|ec|rs|sk|py|bg|hk|eu|ee|md|is|my|lv|gt|pk|ni|by|ae|kr|su|vn|cy|am|ke))))))(?!(?:(?:(?:ttp|tp|ttps):\\/\\/)|(?:ww\\.)|(?:n--)))");
        std::smatch url_match_result;
        std::string body2 = body;
        while (std::regex_search(body2, url_match_result, url_regex))
        {
          for (const auto &res : url_match_result)
            std::cout << "FOUND URL: " << res << std::endl;
          body2 = url_match_result.suffix();
        }
         */

        // collect data needed by writeMessage()
        HTMLMessageInfo msg_info({only_emoji,
                                  is_deleted,
                                  is_viewonce,
                                  isgroup,
                                  incoming,
                                  nobackground,
                                  hasquote,
                                  quote_missing,
                                  originalfilenames,
                                  overwrite,
                                  append,
                                  story_reply,
                                  type,
                                  expires_in,
                                  msg_id,
                                  msg_recipient_id,
                                  original_message_id,
                                  messagecount,

                                  &messages,
                                  &quote_attachment_results,
                                  &attachment_results,
                                  &reaction_results,
                                  &edit_revisions,

                                  body,
                                  quote_body,
                                  readable_date,
                                  directory,
                                  threaddir,
                                  filename,
                                  messages(messagecount, "link_preview_title"),
                                  messages(messagecount, "link_preview_description"),
                                  shared_contacts,

                                  icon
          });
        HTMLwriteMessage(htmloutput, msg_info, &recipient_info, searchpage, receipts);

        if (searchpage && (!Types::isStatusMessage(msg_info.type) && !msg_info.body.empty()))
        {
          // because the body is already escaped for html at this point, we get it fresh from database (and have sqlite do the json formatting)
          if (!d_database.exec("SELECT json_object("
                               "'id', " + d_mms_table + "._id, "
                               "'b', " + d_mms_table + ".body, "
                               "'f', " + d_mms_table + "." + d_mms_recipient_id + ", "
                               "'tr', thread." + d_thread_recipient_id + ", "
                               "'o', (" + d_mms_table + "." + d_mms_type + " & 0x1F) IN (2,11,21,22,23,24,25,26), "
                               "'d', (" + d_mms_table + ".date_received / 1000 - 1404165600), " // loose the last three digits (miliseconds, they are never displayed anyway).
                                                                                                // subtract "2014-07-01". Signals initial release was 2014-07-29, negative numbers should work otherwise anyway.
                               "'p', " + "SUBSTR(\"" + msg_info.threaddir + "/" + msg_info.filename + "\", 1, LENGTH(\"" + msg_info.threaddir + "/" + msg_info.filename + "\") - 5)" + ") AS line, " // all pages end in ".html", slice it off
                               + d_part_table + "._id AS rowid, " +
                               (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                d_part_table + ".unique_id AS uniqueid" : "-1 AS uniqueid") +
                               " FROM " + d_mms_table + " "
                               "LEFT JOIN thread ON thread._id IS " + d_mms_table + ".thread_id "
                               "LEFT JOIN " + d_part_table + " ON " + d_part_table + "." + d_part_mid + " IS " + d_mms_table + "._id AND " + d_part_table + "." + d_part_ct + " = 'text/x-signal-plain' AND " + d_part_table + ".quote = 0 "
                               "WHERE " + d_mms_table + "._id = ?",
                               msg_info.msg_id, &search_idx_results) ||
              search_idx_results.rows() < 1) [[unlikely]]
          {
            Logger::warning("Search_idx query failed or no results");
          }
          else
          {
            if (search_idx_results.rows() > 1) [[unlikely]]
              Logger::warning("Unexpected number of results from search_idx query (",
                              search_idx_results.rows(), " results, using first)");

            std::string line = search_idx_results("line");
            if (!line.empty()) [[likely]]
            {
              if (search_idx_results.valueAsInt(0, "rowid") != -1
                  /* && search_idx_results.valueAsInt(0, "uniqueid") != -1*/)
              {
                long long int rowid = search_idx_results.valueAsInt(0, "rowid");
                long long int uniqueid = search_idx_results.valueAsInt(0, "uniqueid");
                AttachmentFrame *a = d_attachments.at({rowid, uniqueid}).get();
                std::string longbody = std::string(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
                a->clearData();

                longbody = d_database.getSingleResultAs<std::string>("SELECT json_set(?, '$.b', ?)", {line, longbody}, std::string());
                if (!longbody.empty()) [[likely]]
                  line = longbody;
              }

              if (searchidx_write_started) [[likely]]
                searchidx << "," << std::endl;

              searchidx << "  " << line;
              searchidx_write_started = true;
            }
          }
        }

        // set daterangeidx (which range were we in for the just written message...)
        for (unsigned int dri = 0; dri < dateranges.size(); ++dri)
          if (messages.getValueAs<long long int>(messagecount, "date_received") > bepaald::toNumber<long long int>(dateranges[dri].first) &&
              messages.getValueAs<long long int>(messagecount, "date_received") <= bepaald::toNumber<long long int>(dateranges[dri].second))
          {
            daterangeidx = dri;
            break;
          }

        if (++messagecount >= messages.rows())
          break;

        // // BREAK THE CONVERSATION BOX BETWEEN SEPARATE DATE RANGES ON THE SAME PAGE

        // std::cout << daterangeidx << std::endl;
        // std::cout << "curm: " << messages.getValueAs<long long int>(messagecount, "date_received") << std::endl;
        // std::cout << "rhig: " << bepaald::toNumber<long long int>(dateranges[daterangeidx].second) << std::endl;
        // std::cout << "rlow: " << bepaald::toNumber<long long int>(dateranges[daterangeidx + 1].first) << std::endl;
        // std::cout << (messages.getValueAs<long long int>(messagecount, "date_received") > bepaald::toNumber<long long int>(dateranges[daterangeidx].second) &&
        //               messages.getValueAs<long long int>(messagecount, "date_received") <= bepaald::toNumber<long long int>(dateranges[daterangeidx + 1].first)) << std::endl;
        if (!dateranges.empty() &&
            daterangeidx < dateranges.size() - 1 && // dont split if it's the last range
            messages.getValueAs<long long int>(messagecount, "date_received") > bepaald::toNumber<long long int>(dateranges[daterangeidx].second))
        {
          if (messagecount < (max_msg_per_page * (pagenumber + 1))) // dont break convo-box if we are moving to a new page (because of --split)
          {
            // std::cout << "SPLITTING! (rangeend(" << daterangeidx << "): " << dateranges[daterangeidx].second << ")" << std::endl;
            // std::cout << "         ! (rangeend(" << daterangeidx << "): " << dateranges[daterangeidx + 1].first << ")" << std::endl;
            // std::cout << "         ! " << messages.getValueAs<long long int>(messagecount, "date_received") << std::endl;
            htmloutput << "        </div>" << std::endl;
            htmloutput << "        <div class=\"conversation-box\">" << std::endl;
            htmloutput << std::endl;
          }
        }
      }

      htmloutput << "        </div>" << '\n'; // closes conversation-box
      htmloutput << "        <a id=\"pagebottom\"></a>" << '\n';
      htmloutput << "      </div>" << '\n'; // closes conversation-wrapper
      htmloutput << '\n';

      if (totalpages > 1)
      {
        std::string sanitized_filename = sanitizeFilename(raw_base_filename);
        HTMLescapeUrl(&sanitized_filename);
        htmloutput << "      <div class=\"conversation-link conversation-link-left\">" << '\n';
        htmloutput << "        <div title=\"First page\">" << '\n';
        htmloutput << "          <a href=\"" << sanitized_filename << ".html" << "\">" << '\n';
        htmloutput << "            <div class=\"menu-icon nav-max" << (pagenumber > 0 ? "" : " nav-disabled") << "\"></div>" << '\n';
        htmloutput << "          </a>" << '\n';
        htmloutput << "        </div>" << '\n';
        htmloutput << "        <div title=\"Previous page\">" << '\n';
        htmloutput << "          <a href=\"" << sanitized_filename << (pagenumber - 1 > 0 ? ("_" + bepaald::toString(pagenumber - 1)) : "") << ".html" << "\">" << '\n';
        htmloutput << "            <div class=\"menu-icon nav-one" << (pagenumber > 0 ? "" : " nav-disabled") << "\"></div>" << '\n';
        htmloutput << "          </a>" << '\n';
        htmloutput << "        </div>" << '\n';
        htmloutput << "      </div>" << '\n';
        htmloutput << "      <div class=\"conversation-link conversation-link-right\">" << '\n';
        htmloutput << "        <div title=\"Next page\">" << '\n';
        htmloutput << "          <a href=\"" << sanitized_filename << "_" << (pagenumber + 1 <= totalpages - 1 ?  bepaald::toString(pagenumber + 1) : bepaald::toString(totalpages - 1)) << ".html" << "\">" << '\n';
        htmloutput << "            <div class=\"menu-icon nav-one nav-fwd" << (pagenumber < totalpages - 1 ? "" : " nav-disabled") << "\"></div>" << '\n';
        htmloutput << "          </a>" << '\n';
        htmloutput << "        </div>" << '\n';
        htmloutput << "        <div title=\"Last page\">" << '\n';
        htmloutput << "          <a href=\"" << sanitized_filename << "_" << bepaald::toString(totalpages - 1) << ".html" << "\">" << '\n';
        htmloutput << "            <div class=\"menu-icon nav-max nav-fwd" << (pagenumber < totalpages - 1 ? "" : " nav-disabled") << "\"></div>" << '\n';
        htmloutput << "          </a>" << '\n';
        htmloutput << "        </div>" << '\n';
        htmloutput << "      </div>" << '\n';
        htmloutput << '\n';
      }
      htmloutput << "     </div>" << '\n'; // closes controls-wrapper
      htmloutput << '\n';
      htmloutput << "       <div id=\"bottom\">" << '\n';
      htmloutput << "         <a href=\"#pagebottom\" title=\"Jump to bottom\">" << '\n';
      htmloutput << "           <div class=\"menu-item-bottom\">" << '\n';
      htmloutput << "             <span class=\"menu-icon nav-one nav-bottom\">" << '\n';
      htmloutput << "             </span>" << '\n';
      htmloutput << "           </div>" << '\n';
      htmloutput << "         </a>" << '\n';
      htmloutput << "      </div>" << '\n';
      htmloutput << "      <div id=\"menu\">" << '\n';
      htmloutput << "        <a href=\"../index.html\">" << '\n';
      htmloutput << "          <div class=\"menu-item\">" << '\n';
      htmloutput << "            <div class=\"menu-icon nav-up\">" << '\n';
      htmloutput << "            </div>" << '\n';
      htmloutput << "            <div>" << '\n';
      htmloutput << "              index" << '\n';
      htmloutput << "            </div>" << '\n';
      htmloutput << "          </div>" << '\n';
      htmloutput << "        </a>" << '\n';
      htmloutput << "      </div>" << '\n';
      htmloutput << '\n';
      if (themeswitching || searchpage)
      {
        htmloutput << "      <div id=\"theme\">" << '\n';
        if (searchpage)
        {
          htmloutput << "        <div class=\"menu-item\">" << '\n';
          htmloutput << "          <a href=\"../searchpage.html?recipient=" << thread_recipient_id << "\" title=\"Search\">" << '\n';
          htmloutput << "            <span class=\"menu-icon searchbutton\">" << '\n';
          htmloutput << "            </span>" << '\n';
          htmloutput << "          </a>" << '\n';
          htmloutput << "        </div>" << '\n';
        }
        if (themeswitching)
        {
          htmloutput << "        <div class=\"menu-item\">" << '\n';
          htmloutput << "          <label for=\"theme-switch\">" << '\n';
          htmloutput << "            <span class=\"menu-icon themebutton\">" << '\n';
          htmloutput << "            </span>" << '\n';
          htmloutput << "          </label>" << '\n';
          htmloutput << "        </div>" << '\n';
        }
        htmloutput << "      </div>" << '\n';
        htmloutput << '\n';
      }
      htmloutput << "  </div>" << '\n'; // closes div id=page (I think)

      if (addexportdetails)
        htmloutput << '\n' << exportdetails_html << '\n';

      if (themeswitching)
      {
        htmloutput << R"(  <script>
    const themeSwitch = document.querySelector('#theme-switch');
    themeSwitch.addEventListener('change', function(e)
    {
      if (e.currentTarget.checked === true)
      {
        //alert('Setting theme light');
        setCookie('theme', 'light');
        document.documentElement.dataset.theme = 'light';
      }
      else
      {
        //alert('Setting theme dark');
        setCookie('theme', 'dark');
        document.documentElement.dataset.theme = 'dark';
      }
    });
  </script>

)";
      }
      htmloutput << "  </body>" << '\n';
      htmloutput << "</html>" << '\n';

      ++pagenumber;
      if (messagecount >= messages.rows())
        break;
    }
  }

  if (searchpage)
  {
    if (searchidx_write_started) [[likely]]
      searchidx << std::endl << "];" << std::endl;

    // write recipient info:
    //std::map<long long int, RecipientInfo> recipient_info;
    searchidx << "recipient_idx = [" << std::endl;
    for (auto r = recipient_info.begin(); r != recipient_info.end(); ++r)
    {
      std::string line = d_database.getSingleResultAs<std::string>("SELECT json_object('_id', ?, 'display_name', ?)", {r->first, r->second.display_name}, std::string());
      if (line.empty()) [[unlikely]]
        continue;

      searchidx << "  " << line;
      if (std::next(r) != recipient_info.end()) [[likely]]
        searchidx << "," << std::endl;
      else
        searchidx << std::endl << "];" << std::endl;
    }
  }

  // write chat folders
  std::vector<long long int> indexedthreads;
  std::set_difference(threads.begin(), threads.end(),
                      excludethreads.begin(), excludethreads.end(),
                      std::back_inserter(indexedthreads));
  std::vector<std::tuple<long long int, std::string, std::string>> chatfolders_list; // {_id, chatfolder name, filename(link)}
  if (chatfolders && d_database.containsTable("chat_folder"))
  {
    // get all folders
    /* FolderType:
       ALL(0),
       // Folder containing all 1:1 chats
       INDIVIDUAL(1),
       // Folder containing group chats
       GROUP(2),
       // Folder containing unread chats.
       UNREAD(3),
       // Folder containing custom chosen chats
       CUSTOM(4);
    */
    SqliteDB::QueryResults cf_results;
    if (d_database.exec("SELECT _id, name, show_individual, show_groups FROM chat_folder "
                        "WHERE folder_type IS NOT 0 ORDER BY position ASC",
                        &cf_results)) [[likely]]
    {
      for (unsigned int i = 0; i < cf_results.rows(); ++i)
      {
        std::string filename = "chatfolder_" + cf_results(i, "_id") + "_" + cf_results(i, "name");
        chatfolders_list.emplace_back(cf_results.valueAsInt(i, "_id"), cf_results(i, "name"), filename);
      }

      for (unsigned int i = 0; i < cf_results.rows(); ++i)
      {
        std::vector<long long int> chatfolder_threads;

        // add 1-on-1
        if (cf_results.valueAsInt(i, "show_individual"))
        {
          SqliteDB::QueryResults individual_threads;
          if (!d_database.exec("SELECT _id FROM thread WHERE recipient_id IN "
                               "(SELECT _id FROM recipient WHERE type = 0)", &individual_threads)) [[unlikely]]
            continue;
          for (unsigned int j = 0; j < individual_threads.rows(); ++j)
          {
            long long int individual_thread_id = individual_threads.valueAsInt(j, "_id");
            if (bepaald::contains(indexedthreads, individual_thread_id))
              chatfolder_threads.push_back(individual_thread_id);
          }
        }

        // add groups
        if (cf_results.valueAsInt(i, "show_groups"))
        {
          SqliteDB::QueryResults group_threads;
          if (!d_database.exec("SELECT _id FROM thread WHERE recipient_id IN "
                               "(SELECT _id FROM recipient WHERE type = 3)", &group_threads)) [[unlikely]] // consider type = 1,2,3 ?
            continue;
          for (unsigned int j = 0; j < group_threads.rows(); ++j)
          {
            long long int group_thread_id = group_threads.valueAsInt(j, "_id");
            if (bepaald::contains(indexedthreads, group_thread_id))
              chatfolder_threads.push_back(group_thread_id);
          }
        }

        /* membership_type
        // Chat that should be included in the chat folder
        INCLUDED(0),
        // Chat that should be excluded from the chat folder
        EXCLUDED(1)
        */
        SqliteDB::QueryResults membership_results;
        if (!d_database.exec("SELECT thread_id FROM chat_folder_membership WHERE chat_folder_id = ? AND membership_type = 0",
                             cf_results.value(i, "_id"), &membership_results)) [[unlikely]]
          continue;
        // add threads manually included
        for (unsigned int j = 0; j < membership_results.rows(); ++j)
        {
          long long int cf_thread_id = membership_results.valueAsInt(j, "thread_id");
          if (bepaald::contains(indexedthreads, cf_thread_id))
            chatfolder_threads.push_back(cf_thread_id);
        }

        if (!d_database.exec("SELECT thread_id FROM chat_folder_membership WHERE chat_folder_id = ? AND membership_type = 1",
                             cf_results.value(i, "_id"), &membership_results)) [[unlikely]]
          continue;
        // remove threads manually excluded
#if __cpp_lib_erase_if >= 202002L
        std::erase_if(chatfolder_threads, [&](long long int tid) { return membership_results.contains<long long int>(tid); });
#else // I think I support c++17...
        auto it = std::remove_if(chatfolder_threads.begin(), chatfolder_threads.end(),
                                 [&](long long int tid) { return membership_results.contains<long long int>(tid); });
        chatfolder_threads.erase(it, chatfolder_threads.end());
#endif

        std::string filename = "chatfolder_" + cf_results(i, "_id") + "_" + cf_results(i, "name");
        HTMLwriteChatFolder(chatfolder_threads, maxdate, directory, filename, &recipient_info, note_to_self_thread_id,
                            calllog, searchpage, stickerpacks, blocked, fullcontacts, settings, overwrite,
                            append, lighttheme, themeswitching, exportdetails_html, cf_results.valueAsInt(i, "_id"),
                            chatfolders_list);
      }
    }
  }
  HTMLwriteIndex(indexedthreads, maxdate, directory, &recipient_info, note_to_self_thread_id,
                 calllog, searchpage, stickerpacks, blocked, fullcontacts, settings, overwrite,
                 append, lighttheme, themeswitching, exportdetails_html, chatfolders_list);

  if (calllog)
    HTMLwriteCallLog(threads, directory, datewhereclausecalllog, &recipient_info, note_to_self_thread_id,
                     overwrite, append, lighttheme, themeswitching, exportdetails_html);

  if (searchpage)
    HTMLwriteSearchpage(directory, lighttheme, themeswitching);

  if (stickerpacks)
    HTMLwriteStickerpacks(directory, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  if (blocked)
    HTMLwriteBlockedlist(directory, &recipient_info, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  if (fullcontacts)
    HTMLwriteFullContacts(directory, &recipient_info, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  if (settings)
    HTMLwriteSettings(directory, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  Logger::message("All done!");
  if (databasemigrated)
  {
    Logger::message("restoring migrated database...");
    SqliteDB::copyDb(backup_database, d_database);
  }
  return true;
}
