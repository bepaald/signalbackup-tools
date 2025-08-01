/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

#include "../common_filesystem.h"
#include "../scopeguard/scopeguard.h"
#include <cerrno>
#include <chrono>

bool SignalBackup::exportHtml(std::string const &directory, std::vector<long long int> const &limittothreads,
                              std::vector<std::string> const &daterangelist, std::string const &splitby,
                              long long int split, std::string const &selfphone, bool calllog, bool searchpage,
                              bool stickerpacks, bool migrate, bool overwrite, bool append, bool lighttheme,
                              bool themeswitching, bool addexportdetails, bool blocked, bool fullcontacts,
                              bool settings, bool receipts, bool originalfilenames, bool linkify, bool chatfolders,
                              bool compact, bool pagemenu, bool aggressive_sanitizing, bool excludeexpiring,
                              bool focusend, std::vector<std::string> const &ignoremediatypes)
{
  Logger::message("Starting HTML export to '", directory, "'");

  // v170 and above should work. Anything below will first migrate (I believe anything down to ~23 should more or less work)
  bool databasemigrated = false;
  MemSqliteDB backup_database;
  if (d_databaseversion < 170 || migrate)
  {
    SqliteDB::copyDb(d_database, backup_database);
    if (!migrateDatabase(d_databaseversion, 170)) // migrate == TRUE, but migration fails
    {
      Logger::error("Failed to migrate currently unsupported database version (", d_databaseversion, ")."
                    " Please upgrade your database");
      SqliteDB::copyDb(backup_database, d_database);
      return false;
    }
    databasemigrated = true;
  }
  ScopeGuard restore_migrated_database([&]() { if (databasemigrated) SqliteDB::copyDb(backup_database, d_database); });

  if (originalfilenames && append) [[unlikely]]
    Logger::warning("Options 'originalfilenames' and 'append' are incompatible");

  // // check if dir exists, create if not
  if (!prepareOutputDirectory(directory, overwrite, !originalfilenames /*allowappend only allowed when not using original filenames*/, append))
    return false;

  // set sql statement cache size
  d_database.setCacheSize(5); // empirically determined
  ScopeGuard reset_cache_size([&]() { d_database.setCacheSize(); });

  // see if we need aggressive filename sanitizing
  d_aggressive_filename_sanitizing = aggressive_sanitizing || !specialCharsSupported(directory);

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

  std::map<long long int, RecipientInfo> rid_recipientinfo_map;

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
  long long int searchidx_page_idx = 0;
  std::map<std::string, long long int> searchidx_page_idx_map;

  // start search index page
  if (searchpage)
  {
    searchidx.open(WIN_LONGPATH(directory + "/" + "searchidx.js"), std::ios_base::binary);
    if (!searchidx.is_open())
    {
      Logger::error("Failed to open 'searchidx.js' for writing");
      return false;
    }
    searchidx << "message_idx = [\n";
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
    if (focusend)
      options += "<br>--htmlfocusend";
    if (receipts)
      options += "<br>--includereceipts";
    if (calllog)
      options += "<br>--includecalllog";
    if (blocked)
      options += "<br>--includeblockedlist";
    if (settings)
      options += "<br>--includesettings";
    if (excludeexpiring)
      options += "<br>--excludeexpiring";
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
  std::string readablesplitformat;
  if (!splitby.empty())
  {
    auto icasecompare = [](std::string const &a, std::string const &b) STATICLAMBDA
    {
      return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char ca, char cb) STATICLAMBDA { return std::tolower(ca) == std::tolower(cb); });
    };

    if (icasecompare(splitby, "year"))
    {
      periodsplitformat = "%Y";
      readablesplitformat = "%Y";
    }
    else if (icasecompare(splitby, "month"))
    {
      periodsplitformat = "%Y%m";
      readablesplitformat = "%b, %Y";
    }
    else if (icasecompare(splitby, "week"))
    {
      periodsplitformat = "%Y%W";
      readablesplitformat = "week %W, %Y";
    }
    else if (icasecompare(splitby, "day"))
    {
      periodsplitformat = "%Y%j";
      readablesplitformat = "%b %d, %Y";
    }
    else
      Logger::warning("Ignoring invalid 'split-by'-value ('", splitby, "')");
  }

  std::map<int, int> thread_pagecount_map; // maps the number of pages for each thread.

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

    // check if this is releasechannel:
    bool is_releasechannel = false;
    for (auto const &skv : d_keyvalueframes)
      if (skv->key() == "releasechannel.recipient_id")
        is_releasechannel =
          (t == d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?",
                                                            bepaald::toNumber<int>(skv->value()), -1));

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

    std::map<int64_t, std::pair<std::string, int64_t>> quoteid_page_and_msgid_map; // maps date_sent (=quote_id) of quoted message to page it is on and _id of message.

    // now get all messages
    SqliteDB::QueryResults messages;
    d_database.exec(bepaald::concat("SELECT "
                                    "_id, ", d_mms_recipient_id, ", ",
                                    (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") ? "to_recipient_id" : "-1"), " AS to_recipient_id, body, "
                                    "MIN(date_received, ", d_mms_date_sent, ") AS bubble_date, "
                                    "date_received, ", d_mms_date_sent, ", ", d_mms_type, ", ",
                                    (!periodsplitformat.empty() ? bepaald::concat("strftime('", periodsplitformat, "', IFNULL(date_received, 0) / 1000, 'unixepoch', 'localtime')") : "''"), " AS periodsplit, "
                                    "quote_id, quote_author, quote_body, quote_mentions, quote_missing, "
                                    "attcount, reactioncount, mentioncount, "
                                    "IFNULL(", d_mms_date_sent, " IN (SELECT DISTINCT quote_id FROM ", d_mms_table, " WHERE thread_id = ?1), 0) AS is_quoted, ",
                                    d_mms_delivery_receipts, ", ", d_mms_read_receipts, ", IFNULL(remote_deleted, 0) AS remote_deleted, "
                                    "IFNULL(view_once, 0) AS view_once, expires_in, ", d_mms_ranges, ", shared_contacts, ",
                                    (d_database.tableContainsColumn(d_mms_table, "original_message_id") ? "original_message_id, " : ""),
                                    (d_database.tableContainsColumn(d_mms_table, "revision_number") ? "revision_number, " : ""),
                                    (d_database.tableContainsColumn(d_mms_table, "parent_story_id") ? "parent_story_id, " : ""),
                                    (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras, " : ""),
                                    (d_database.tableContainsColumn(d_mms_table, "receipt_timestamp") ? "receipt_timestamp, " : "-1 AS receipt_timestamp, "), // introduced in 117
                                    "json_extract(link_previews, '$[0].url') AS link_preview_url, "
                                    "json_extract(link_previews, '$[0].title') AS link_preview_title, "
                                    "json_extract(link_previews, '$[0].description') AS link_preview_description "
                                    "FROM ", d_mms_table, " "
                                    // get attachment count for message:
                                    "LEFT JOIN (SELECT ", d_part_mid, " AS message_id, COUNT(*) AS attcount FROM ", d_part_table, " GROUP BY message_id) AS attmnts ON ", d_mms_table, "._id = attmnts.message_id "
                                    // get reaction count for message:
                                    "LEFT JOIN (SELECT message_id, COUNT(*) AS reactioncount FROM reaction GROUP BY message_id) AS rctns ON ", d_mms_table, "._id = rctns.message_id "
                                    // get mention count for message:
                                    "LEFT JOIN (SELECT message_id, COUNT(*) AS mentioncount FROM mention GROUP BY message_id) AS mntns ON ", d_mms_table, "._id = mntns.message_id "
                                    "WHERE thread_id = ?1",
                                    (excludeexpiring ? " AND (expires_in == 0 OR (" + d_mms_type + " & 0x40000) != 0)" : ""),
                                    datewhereclause,
                                    (d_database.tableContainsColumn(d_mms_table, "latest_revision_id") ? " AND latest_revision_id IS NULL " : " "),
                                    (d_database.tableContainsColumn(d_mms_table, "story_type") ? " AND story_type = 0 OR story_type IS NULL " : ""), // storytype NONE(0), STORY_WITH(OUT)_REPLIES(1/2), TEXT_...(3/4)
                                    " ORDER BY date_received ASC"), t, &messages);
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
    setRecipientInfo(all_recipients_ids, &rid_recipientinfo_map);

    //for (auto const &ri : rid_recipientinfo_map)
    //  std::cout << ri.first << ": " << ri.second.display_name << std::endl;

    // get conversation name, sanitize it and create dir
    if (rid_recipientinfo_map.find(thread_recipient_id) == rid_recipientinfo_map.end())
    {
      Logger::error("Failed set recipient info for thread (", t, ")... skipping");
      continue;
    }

    std::string basethreaddir(is_note_to_self ? "Note to Self" : rid_recipientinfo_map[thread_recipient_id].display_name);
    WIN_LIMIT_FILENAME_LENGTH(basethreaddir);
    std::string threaddir(sanitizeFilename(basethreaddir, d_aggressive_filename_sanitizing) + " (_id"s + bepaald::toString(thread_id) + ")");
    if (compact) [[unlikely]]
      threaddir = "id" + bepaald::toString(thread_id);

    if (bepaald::fileOrDirExists(bepaald::concat(directory, "/", threaddir)))
    {
      if (!bepaald::isDir(bepaald::concat(directory, "/", threaddir)))
      {
        Logger::error("dir is regular file");
        return false;
      }
      if (!append && !overwrite) // should be impossible at this point....
      {
        Logger::error("Refusing to overwrite existing directory");
        return false;
      }
    }
    else if (!bepaald::createDir(bepaald::concat(directory, "/", threaddir))) // try to create it
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
      return false;
    }

    // now append messages to html
    std::map<long long int, std::string> rid_writtenavatarpath_map; // maps recipient_ids to the path of a written avatar file.
    unsigned int messagecount = 0; // current message
    unsigned int max_msg_per_page = messages.rows();
    int pagenumber = 0; // current page
    int totalpages = 1;
    if (split > 0)
    {
      totalpages = (messages.rows() / split) + (messages.rows() % split > 0 ? 1 : 0);
      max_msg_per_page = messages.rows() / totalpages + ((messages.rows() % totalpages) ? 1 : 0);
    }
    std::vector<std::string> split_page_names;
    if (!periodsplitformat.empty())
    {
      SqliteDB::QueryResults pagenames;
      if (d_database.exec(bepaald::concat("SELECT "
                                          "strftime('", periodsplitformat, "', IFNULL(date_received, 0) / 1000, 'unixepoch', 'localtime') AS splitdate, date_received / 1000 AS date_secs "
                                          "FROM ", d_mms_table, " WHERE thread_id = ? ", datewhereclause, " GROUP BY splitdate"), t, &pagenames))
      {
        totalpages = pagenames.rows();
        for (unsigned int p = 0; p < pagenames.rows(); ++p)
          split_page_names.emplace_back(bepaald::toDateString(pagenames.valueAsInt(p, "date_secs", 0), readablesplitformat));
      }
    }
    else if (totalpages > 1)
      for (int p = 0; p < totalpages; ++p)
        split_page_names.emplace_back("Page " + bepaald::toString(p + 1));

    if (focusend)
      thread_pagecount_map.emplace_hint(thread_pagecount_map.end(), t, totalpages);

    // std::cout << "Split: " << split << std::endl;
    // std::cout << "N MSG: " << messages.rows() << std::endl;
    // std::cout << "MAX PER PAGE: " << max_msg_per_page << std::endl;
    // std::cout << "N PAGES: " << totalpages << std::endl;

    unsigned int daterangeidx = 0;

    if (d_verbose) [[unlikely]]
      Logger::message("Starting importing ", messages.rows(), " messages...");

    while (true)
    {
      std::string previous_period_split_string(messages(messagecount, "periodsplit"));
      std::string previous_day_change;
      // create output-file
      std::string raw_base_filename = (is_note_to_self ? "Note to Self" : rid_recipientinfo_map[thread_recipient_id].display_name);
      WIN_LIMIT_FILENAME_LENGTH(raw_base_filename);
      std::string sanitized_base_filename(sanitizeFilename(raw_base_filename, d_aggressive_filename_sanitizing));
      std::string filename(sanitized_base_filename + (pagenumber > 0 ? "_" + bepaald::toString(pagenumber) : "") + ".html");
      if (compact) [[unlikely]]
      {
        sanitized_base_filename.clear();
        filename = bepaald::toString(pagenumber) + ".html";
      }

      WIN_CHECK_PATH_LENGTH(bepaald::concat(directory, "/", threaddir, "/", filename));

      std::ofstream htmloutput(WIN_LONGPATH(bepaald::concat(directory, "/", threaddir, "/", filename)), std::ios_base::binary);
      if (!htmloutput.is_open())
      {
        Logger::error("Failed to open '", directory, "/", threaddir, "/", filename, "' for writing.");
        return false;
      }

      // create start of html (css, head, start of body
      HTMLwriteStart(htmloutput, thread_recipient_id, directory, threaddir, isgroup, is_note_to_self,
                     is_releasechannel, all_recipients_ids, &rid_recipientinfo_map, &rid_writtenavatarpath_map, overwrite,
                     append, lighttheme, themeswitching, searchpage, addexportdetails, pagemenu && totalpages > 1);
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
        long long int quote_id = messages.valueAsInt(messagecount, "quote_id", 0);
        bool quote_missing = messages.valueAsInt(messagecount, "quote_missing", 0) != 0;
        bool story_reply = (d_database.tableContainsColumn(d_mms_table, "parent_story_id") ? messages.valueAsInt(messagecount, "parent_story_id", 0) : 0);
        //long long int attachmentcount = messages.valueAsInt(messagecount, "attcount", 0);
        //long long int reactioncount = messages.valueAsInt(messagecount, "reactioncount", 0);
        //long long int mentioncount = messages.valueAsInt(messagecount, "mentioncount", 0);

        SqliteDB::QueryResults attachment_results;
        SqliteDB::QueryResults quote_attachment_results;
        if (messages.valueAsInt(messagecount, "attcount", 0) > 0)
        {
          d_database.exec(bepaald::concat("SELECT ",
                                          d_part_table, "._id, ",
                                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"), ", ",
                                          d_part_ct, ", "
                                          "file_name, ",
                                          d_part_pending, ", ",
                                          (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()),
                                          "sticker_pack_id, ",
                                          d_mms_table, ".date_received AS date_received "
                                          "FROM ", d_part_table, " "
                                          "LEFT JOIN ", d_mms_table, " ON ", d_mms_table, "._id = ", d_part_table, ".", d_part_mid, " "
                                          "WHERE ", d_part_mid, " IS ? "
                                          "AND quote IS ? "
                                          " ORDER BY display_order ASC, ", d_part_table, "._id ASC"), {msg_id, 0}, &attachment_results);

          d_database.exec(bepaald::concat("SELECT ",
                                          d_part_table, "._id, ",
                                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"), ", ",
                                          d_part_ct, ", "
                                          "file_name, ",
                                          d_part_pending, ", ",
                                          (d_database.tableContainsColumn(d_part_table, "caption") ? "caption, "s : std::string()),
                                          "sticker_pack_id, ",
                                          d_mms_table, ".date_received AS date_received "
                                          "FROM ", d_part_table, " "
                                          "LEFT JOIN ", d_mms_table, " ON ", d_mms_table, "._id = ", d_part_table, ".", d_part_mid, " "
                                          "WHERE ", d_part_mid, " IS ? "
                                          "AND quote IS ? "
                                          " ORDER BY display_order ASC, ", d_part_table, "._id ASC"), {msg_id, 1}, &quote_attachment_results);
        }
        // check attachments for long message body -> replace cropped body & remove from attachment results
        bool haslongbody = setLongMessageBody(&body, &attachment_results);

        SqliteDB::QueryResults mention_results;
        if (messages.valueAsInt(messagecount, "mentioncount", 0) > 0)
          d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

        SqliteDB::QueryResults reaction_results;
        if (messages.valueAsInt(messagecount, "reactioncount", 0) > 0)
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
          // identityVerified and identityDefault ("You marked your safety number with XXX (un)verified") are
          // a little different from other (nongroup) statusmessages, in that the name to be filled in is not
          // the FROM_recipient_id (as is the case with others: "XXX reset the secure session", "XXX set the
          // disappearing message timer to ..."), but the TO_recipient. For these message FROM is always self
          // ("YOU set YOUR safety number..."), but we need the TO_id to fill in the name.
          // Note this is even true in group threads, where outgoing messages are usually always from you and
          // to the group, but not for these two status messages.
          // But all of the above is only true for (newer) databases that _have_ from_ and to_recipient_ids,
          // in older databases the target-name is recipient_id in all cases...
          long long int target_rid = msg_recipient_id;
          if ((Types::isIdentityVerified(type) || Types::isIdentityDefault(type)) &&
              messages.valueAsInt(messagecount, "to_recipient_id") != -1) [[unlikely]]
            target_rid = messages.valueAsInt(messagecount, "to_recipient_id");

          // decode from body if (body not empty) OR (message_extras not available)
          if (!body.empty() ||
              !(d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras")))
            body = decodeStatusMessage(body, messages.getValueAs<long long int>(messagecount, "expires_in"),
                                       type, getRecipientInfoFromMap(&rid_recipientinfo_map, target_rid).display_name, &icon);
          else if (d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                   messages.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras"))
            body = decodeStatusMessage(messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "message_extras"),
                                       messages.getValueAs<long long int>(messagecount, "expires_in"), type,
                                       getRecipientInfoFromMap(&rid_recipientinfo_map, target_rid).display_name, &icon);
        }

        // prep body (scan emoji? -> in <span>) and handle mentions...
        // if (prepbody)
        std::vector<std::tuple<long long int, long long int, long long int>> mentions;
        for (unsigned int mi = 0; mi < mention_results.rows(); ++mi)
          mentions.emplace_back(mention_results.getValueAs<long long int>(mi, "recipient_id"),
                                mention_results.getValueAs<long long int>(mi, "range_start"),
                                mention_results.getValueAs<long long int>(mi, "range_length"));
        std::pair<std::shared_ptr<unsigned char []>, size_t> brdata(nullptr, 0);
        if (!messages.isNull(messagecount, d_mms_ranges))
          brdata = messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, d_mms_ranges);

        bool only_emoji = HTMLprepMsgBody(&body, mentions, &rid_recipientinfo_map, incoming, brdata, linkify, false /*isquote*/);

        bool nobackground = false;
        if ((only_emoji && quote_id == 0 && !attachment_results.rows()) ||  // if no quote etc
            issticker) // or sticker
          nobackground = true;

        // same for quote_body!
        mentions.clear();
        std::pair<std::shared_ptr<unsigned char []>, size_t> quote_mentions{nullptr, 0};
        if (!messages.isNull(messagecount, "quote_mentions"))
          quote_mentions = messages.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(messagecount, "quote_mentions");
        HTMLprepMsgBody(&quote_body, mentions, &rid_recipientinfo_map, incoming, quote_mentions, false /*linkify*/, true);

        // insert date-change message
        if (readable_date_day != previous_day_change)
        {
          htmloutput <<
            "          <div class=\"msg msg-date-change\">\n"
            "            <p>\n"
            "              " << readable_date_day << "\n"
            "            </p>\n"
            "          </div>\n\n";
        }
        previous_day_change = readable_date_day;
        previous_period_split_string = messages(messagecount, "periodsplit");

        // collect data needed by writeMessage()
        HTMLMessageInfo msg_info({body,
                                  quote_body,
                                  readable_date,
                                  directory,
                                  threaddir,
                                  filename,
                                  messages(messagecount, "link_preview_url"),
                                  messages(messagecount, "link_preview_title"),
                                  messages(messagecount, "link_preview_description"),
                                  shared_contacts,

                                  &messages,
                                  &quote_attachment_results,
                                  &attachment_results,
                                  &reaction_results,
                                  &edit_revisions,

                                  type,
                                  expires_in,
                                  msg_id,
                                  msg_recipient_id,
                                  original_message_id,
                                  quote_id,

                                  icon,
                                  messagecount,

                                  only_emoji,
                                  messages.valueAsInt(messagecount, "is_quoted", 0) != 0,
                                  is_deleted,
                                  is_viewonce,
                                  isgroup,
                                  incoming,
                                  nobackground,
                                  quote_missing,
                                  originalfilenames,
                                  overwrite,
                                  append,
                                  story_reply});
        HTMLwriteMessage(htmloutput, msg_info, quoteid_page_and_msgid_map, &rid_recipientinfo_map, searchpage, receipts, ignoremediatypes);

        if (msg_info.is_quoted)
          quoteid_page_and_msgid_map.emplace(messages.valueAsInt(messagecount, d_mms_date_sent, 0), std::pair(msg_info.filename, msg_info.msg_id));

        if (searchpage && (!Types::isStatusMessage(msg_info.type) && !msg_info.body.empty()))
        {
          if (auto it = searchidx_page_idx_map.find(msg_info.threaddir + "/" + sanitized_base_filename); it != searchidx_page_idx_map.end())
            searchidx_page_idx = it->second;
          else
            searchidx_page_idx_map.emplace(msg_info.threaddir + "/" + sanitized_base_filename, ++searchidx_page_idx);

          // because the body is already escaped for html at this point, we get it fresh from database (and have sqlite do the json formatting)
          if (!d_database.exec("SELECT json_object("
                               "'i', ?1, "
                               "'b', " + d_mms_table + ".body, "
                               "'f', ?2, "
                               "'t', ?3, "
                               "'o', ?4, "
                               "'d', ?5, "
                               "'p', ?6, "
                               "'n', ?7) AS line," +
                               (haslongbody ?
                                d_part_table + "._id AS rowid, " +
                                (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                 d_part_table + ".unique_id AS uniqueid" : "-1 AS uniqueid") :
                                " -1 AS rowid, -1 AS uniqueid") +
                               " FROM " + d_mms_table + " "
                               "LEFT JOIN thread ON thread._id IS ?8 " +
                               (haslongbody ?
                                "LEFT JOIN " + d_part_table + " ON " + d_part_table + "." + d_part_mid + " IS ?1 AND " + d_part_table + "." + d_part_ct + " = 'text/x-signal-plain' AND " + d_part_table + ".quote = 0 " : "") +
                               "WHERE " + d_mms_table + "._id = ?1",
                               {msg_id, msg_recipient_id, thread_recipient_id, incoming ? 0 : 1,
                                messages.valueAsInt(messagecount, "date_received", 0) / 1000 - 1404165600, // lose the last three digits (miliseconds, they are never displayed anyway).
                                                                                                           // subtract "2014-07-01". Signals initial release was 2014-07-29, negative
                                                                                                           // numbers should work otherwise anyway.
                                searchidx_page_idx, pagenumber, t},
                               &search_idx_results) ||
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
              if (haslongbody && search_idx_results.valueAsInt(0, "rowid") != -1
                  /* && search_idx_results.valueAsInt(0, "uniqueid") != -1*/)
              {
                long long int rowid = search_idx_results.valueAsInt(0, "rowid");
                long long int uniqueid = search_idx_results.valueAsInt(0, "uniqueid");
                AttachmentFrame *a = d_attachments.at({rowid, uniqueid}).get();
                std::string longbody = std::string(reinterpret_cast<char *>(a->attachmentData(d_verbose)), a->attachmentSize());
                a->clearData();

                longbody = d_database.getSingleResultAs<std::string>("SELECT json_set(?, '$.b', ?)", {line, longbody}, std::string());
                if (!longbody.empty()) [[likely]]
                  line = std::move(longbody);
              }

              if (searchidx_write_started) [[likely]]
                searchidx << ",\n";

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
            htmloutput <<
              "        </div>\n"
              "        <div class=\"conversation-box\">\n"
              "\n";
          }
        }
      }

      htmloutput <<
        "        </div>\n"                      // closes conversation-box
        "        <a id=\"pagebottom\"></a>\n"
        "      </div>\n"                        // closes conversation-wrapper
        "\n";

      if (totalpages > 1)
      {
        std::string sanitized_filename(sanitized_base_filename);
        HTMLescapeUrl(&sanitized_filename);
        htmloutput <<
          "      <div class=\"conversation-link conversation-link-left\">\n"
          "        <div title=\"First page\">\n"
          "          <a href=\"" << (compact ? "0" : sanitized_filename) << ".html" << "\">\n"
          "            <div class=\"menu-icon nav-max" << (pagenumber > 0 ? "" : " nav-disabled") << "\"></div>\n"
          "          </a>\n"
          "        </div>\n"
          "        <div title=\"Previous page\">\n"
          "          <a href=\"" << sanitized_filename << (compact ? bepaald::toString(pagenumber - 1) : (pagenumber - 1 > 0 ? ("_" + bepaald::toString(pagenumber - 1)) : "")) << ".html" << "\">\n"
          "            <div class=\"menu-icon nav-one" << (pagenumber > 0 ? "" : " nav-disabled") << "\"></div>\n"
          "          </a>\n"
          "        </div>\n"
          "      </div>\n"
          "      <div class=\"conversation-link conversation-link-right\">\n"
          "        <div title=\"Next page\">\n"
          "          <a href=\"" << sanitized_filename << (compact ? "" : "_") << (pagenumber + 1 <= totalpages - 1 ?  bepaald::toString(pagenumber + 1) : bepaald::toString(totalpages - 1)) << ".html" << "\">\n"
          "            <div class=\"menu-icon nav-one nav-fwd" << (pagenumber < totalpages - 1 ? "" : " nav-disabled") << "\"></div>\n"
          "          </a>\n"
          "        </div>\n"
          "        <div title=\"Last page\">\n"
          "          <a href=\"" << sanitized_filename << (compact ? "" : "_") << bepaald::toString(totalpages - 1) << ".html" << "\">\n"
          "            <div class=\"menu-icon nav-max nav-fwd" << (pagenumber < totalpages - 1 ? "" : " nav-disabled") << "\"></div>\n"
          "          </a>\n"
          "        </div>\n"
          "      </div>\n"
          "\n";
      }
      htmloutput <<
        "     </div>\n" // closes controls-wrapper
        "\n"
        "       <div id=\"bottom\">\n"
        "         <a href=\"#pagebottom\" title=\"Jump to bottom\">\n"
        "           <div class=\"menu-item-bottom\">\n"
        "             <span class=\"menu-icon nav-one nav-bottom\">\n"
        "             </span>\n"
        "           </div>\n"
        "         </a>\n"
        "      </div>\n"
        "      <div id=\"menu\">\n"
        "        <a href=\"../index.html\">\n"
        "          <div class=\"menu-item\">\n"
        "            <div class=\"menu-icon nav-up\">\n"
        "            </div>\n"
        "            <div>\n"
        "              index\n"
        "            </div>\n"
        "          </div>\n"
        "        </a>\n"
        "      </div>\n"
        "\n";
      if (themeswitching || searchpage || (pagemenu && totalpages > 1))
      {
        htmloutput << "      <div id=\"theme\">\n";
        if (pagemenu && totalpages > 1)
        {
          std::string sanitized_filename(sanitized_base_filename);
          HTMLescapeUrl(&sanitized_filename);

          htmloutput <<
            "        <div class=\"menu-item\">\n"
            "          <div class=\"expandable-menu-item\">\n"
            "            <div class=\"menu-header\">\n"
            "              <span id=\"jump-to-page-icon\"></span>\n"
            "            </div>\n"
            "            <div class=\"expandedmenu\">\n"
            "              <div class=\"expandedmenu-container\">\n";
          for (int pn = 0; pn < static_cast<int>(split_page_names.size()); ++pn)
          {
            if (pn == pagenumber)
              htmloutput <<
                "                <div class=\"menu-item currentpage\">\n"
                "                  <div>" << split_page_names[pn] << "</div>\n"
                "                </div>\n";
            else
              htmloutput <<
                "                <a href=\"" << sanitized_filename << (compact ? bepaald::toString(pn) : (pn > 0 ? "_" + bepaald::toString(pn) : "")) << ".html\">\n"
                "                  <div class=\"menu-item\">\n"
                "                    <div>" << split_page_names[pn] << "</div>\n"
                "                  </div>\n"
                "                </a>\n";
          }
          htmloutput <<
            "              </div>\n"
            "            </div>\n"
            "          </div>\n"
            "        </div>\n";
        }
        if (searchpage)
        {
          htmloutput <<
            "        <div class=\"menu-item\">\n"
            "          <a href=\"../searchpage.html?recipient=" << thread_recipient_id << "\" title=\"Search\">\n"
            "            <span class=\"menu-icon searchbutton\">\n"
            "            </span>\n"
            "          </a>\n"
            "        </div>\n";
        }
        if (themeswitching)
        {
          htmloutput <<
            "        <div class=\"menu-item\">\n"
            "          <label for=\"theme-switch\">\n"
            "            <span class=\"menu-icon themebutton\">\n"
            "            </span>\n"
            "          </label>\n"
            "        </div>\n";
        }
        htmloutput <<
          "      </div>\n"
          "\n";
      }
      htmloutput << "  </div>\n"; // closes div id=page (I think)

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
      htmloutput << "  </body>\n";
      htmloutput << "</html>\n";

      ++pagenumber;
      if (messagecount >= messages.rows())
        break;
    }
  }

  if (searchpage)
  {
    if (searchidx_write_started) [[likely]]
      searchidx << "\n];\n";

    // write recipient info, maps recipient_ids to display name.
    //std::map<long long int, RecipientInfo> rid_recipientinfo_map;
    searchidx << "recipient_idx = [\n";
    for (auto r = rid_recipientinfo_map.begin(); r != rid_recipientinfo_map.end(); ++r)
    {
      std::string line = d_database.getSingleResultAs<std::string>("SELECT json_object('i', ?, 'dn', ?)", {r->first, r->second.display_name}, std::string());
      if (line.empty()) [[unlikely]]
        continue;
      searchidx << "  " << line;
      if (std::next(r) != rid_recipientinfo_map.end()) [[likely]]
        searchidx << ",\n";
      else
        searchidx << "\n];\n";
    }

    // write page info, maps threads to base filename (html without ' (NN).html')
    searchidx << "page_idx = [\n";
    for (auto pi = searchidx_page_idx_map.begin() ; pi != searchidx_page_idx_map.end(); ++pi)
    {
      std::string line = d_database.getSingleResultAs<std::string>("SELECT json_object('i', ?, 'f', ?)", {pi->second, pi->first}, std::string());
      if (line.empty()) [[unlikely]]
        continue;
      searchidx << "  " << line;
      if (std::next(pi) != searchidx_page_idx_map.end()) [[likely]]
        searchidx << ",\n";
      else
        searchidx << "\n];\n";
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
        HTMLwriteChatFolder(chatfolder_threads, maxdate, directory, filename, &rid_recipientinfo_map, note_to_self_thread_id,
                            calllog, searchpage, stickerpacks, blocked, fullcontacts, settings, overwrite,
                            append, lighttheme, themeswitching, exportdetails_html, cf_results.valueAsInt(i, "_id"),
                            chatfolders_list, excludeexpiring, thread_pagecount_map, compact);
      }
    }
  }
  HTMLwriteIndex(indexedthreads, maxdate, directory, &rid_recipientinfo_map, note_to_self_thread_id,
                 calllog, searchpage, stickerpacks, blocked, fullcontacts, settings, overwrite,
                 append, lighttheme, themeswitching, exportdetails_html, chatfolders_list,
                 excludeexpiring, thread_pagecount_map, compact);

  if (calllog)
    HTMLwriteCallLog(threads, directory, datewhereclausecalllog, &rid_recipientinfo_map, note_to_self_thread_id,
                     overwrite, append, lighttheme, themeswitching, exportdetails_html, compact);

  if (searchpage)
    HTMLwriteSearchpage(directory, lighttheme, themeswitching, compact);

  if (stickerpacks)
    HTMLwriteStickerpacks(directory, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  if (blocked)
    HTMLwriteBlockedlist(directory, &rid_recipientinfo_map, overwrite, append, lighttheme, themeswitching,
                         exportdetails_html, compact);

  if (fullcontacts)
    HTMLwriteFullContacts(directory, &rid_recipientinfo_map, overwrite, append, lighttheme, themeswitching,
                          exportdetails_html, compact);

  if (settings)
    HTMLwriteSettings(directory, overwrite, append, lighttheme, themeswitching, exportdetails_html);

  Logger::message("All done!");
  return true;
}
