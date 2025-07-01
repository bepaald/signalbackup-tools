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

#include "signalbackup.ih"

#include <chrono>
#include "../common_filesystem.h"

bool SignalBackup::HTMLwriteIndexImpl(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                                      std::string const &basename, std::map<long long int, RecipientInfo> *recipient_info,
                                      long long int note_to_self_tid, bool calllog, bool searchpage, bool stickerpacks, bool blocked,
                                      bool fullcontacts, bool settings,  bool overwrite, bool append, bool light, bool themeswitching,
                                      std::string const &exportdetails, long long int chatfolder_idx,
                                      std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders,
                                      bool excludeexpiring, bool compact) const
{
  std::string filename(sanitizeFilename(basename, d_aggressive_filename_sanitizing) + ".html");

  Logger::message("Writing ", filename, "...");
  if (bepaald::fileOrDirExists(directory + "/" + filename))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", directory, "/", filename, "' exists. Use --overwrite to overwrite.");
      return false;
    }
  }
  std::ofstream outputfile(WIN_LONGPATH(directory + "/" + filename), std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", directory, "/", filename, "' for writing.");
    return false;
  }

  // build string of requested threads
  std::string threadlist;
  for (unsigned int i = 0; i < threads.size(); ++i)
  {
    threadlist += bepaald::toString(threads[i]);
    if (i < threads.size() - 1)
      threadlist += ",";
  }

  // get thread_id of release channel
  int releasechannel = -1;
  for (auto const &skv : d_keyvalueframes)
    if (skv->key() == "releasechannel.recipient_id")
      releasechannel = bepaald::toNumber<int>(skv->value());
  long long int releasechannel_tid =
    d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", releasechannel, -1);

  int menuitems = 0;
  for (bool o : {calllog, stickerpacks, blocked, fullcontacts, settings})
    if (o)
      ++menuitems;

  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT "
                       "thread._id, "
                       "thread." + d_thread_recipient_id + ", "
                       "thread.snippet, "
                       "thread.snippet_type, "
                       "thread.expires_in AS thread_expires_in, " // for groups, this is not set even if there are disappearing message
                       "recipient.message_expiration_time AS recipient_expires_in, " // this one should always be set I think, but it's in seconds as opposed to milliseconds
                       "IFNULL(thread.date, 0) AS date, "
                       "json_extract(thread.snippet_extras, '$.individualRecipientId') AS 'group_sender_id', "
                       "json_extract(thread.snippet_extras, '$.bodyRanges') AS 'snippet_ranges', "
                       "json_extract(thread.snippet_extras, '$.isRemoteDelete') AS 'deleted', " +
                       (d_database.tableContainsColumn("thread", "snippet_message_extras") ? "snippet_message_extras," : "NULL AS snippet_message_extras,") +
                       (d_database.tableContainsColumn("thread", d_thread_pinned) ? "IFNULL(" + d_thread_pinned + ", 0) AS pinned," : "0 AS pinned,") +
                       (d_database.tableContainsColumn("thread", "archived") ? "archived," : "") +
                       //"IFNULL(recipient.mute_until, 0) AS mute_until, " // dont think this is ever NULL
                       //"recipient.blocked, "
                       //"(SELECT COUNT(" + d_mms_table + "._id) FROM " + d_mms_table + " WHERE " + d_mms_table + ".thread_id = thread._id) AS message_count, "
                       "recipient.group_id "
                       "FROM thread "
                       "LEFT JOIN recipient ON recipient._id IS thread." + d_thread_recipient_id + " "
                       "WHERE thread._id IN (" + threadlist + ") AND " + d_thread_message_count + " > 0 ORDER BY "
                       "(pinned != 0) DESC, pinned ASC, " // before 266 pinned == 0 meant 'not pinned', after pinned = NULL is not pinned. But in the code, pinned = 0 is still not possible (when pinning something, even the first thread, the value is set to 1 to start)
                       + (d_database.tableContainsColumn("thread", "archived") ? "archived ASC, " : "") +
                       "date DESC", &results))
  {
    Logger::error("Failed to query database for thread snippets.");
    return false;
  }
  //results.prettyPrint(d_truncate);

  //maxtimestamp = 9999999999999;
  if (maxtimestamp != -1) [[unlikely]]
  {
    if (!d_database.exec("WITH partitioned_messages AS ("
                         "SELECT "
                         "ROW_NUMBER() OVER (PARTITION BY thread_id ORDER BY " + d_mms_table + ".date_received DESC) AS partition_idx, " +
                         d_mms_table + ".thread_id AS _id, "
                         "thread." + d_thread_recipient_id + ", "
                         "NULLIF(" + d_mms_table + ".body, '') AS snippet, " +
                         d_part_table + "." + d_part_ct + " AS partct, "
                         + d_mms_table + "." + d_mms_type + " AS snippet_type, "
                         "thread.expires_in AS thread_expires_in, " // for groups, this is not set even if there are disappearing message
                         "recipient.message_expiration_time AS recipient_expires_in, " // this one should always be set I think, but it's in seconds as opposed to milliseconds
                         "IFNULL(" + d_mms_table + "." + d_mms_date_sent + ", 0) AS date, "
                         "CAST(" + d_mms_table + "." + d_mms_recipient_id + " AS text) AS 'group_sender_id', " +
                         (d_database.tableContainsColumn("thread", "snippet_message_extras") ? "snippet_message_extras," : "NULL AS snippet_message_extras,") +
                         d_mms_ranges + " AS 'snippet_ranges', " +
                         (d_database.tableContainsColumn(d_mms_table, "remote_deleted") ? "remote_deleted AS 'deleted', " : "0 AS 'deleted', ") +
                         (d_database.tableContainsColumn("thread", d_thread_pinned) ? "IFNULL(" + d_thread_pinned + ", 0) AS pinned," : "0 AS pinned,") +
                         (d_database.tableContainsColumn("thread", "archived") ? "thread.archived, " : "") +
                         //"IFNULL(recipient.mute_until, 0) AS mute_until, "
                         //"recipient.blocked, "
                         //"-1 AS message_count, "
                         "recipient.group_id "
                         "FROM " + d_mms_table + " "
                         "LEFT JOIN thread ON thread._id IS " + d_mms_table + ".thread_id "
                         "LEFT JOIN recipient ON recipient._id IS thread." + d_thread_recipient_id + " "
                         "LEFT JOIN " + d_part_table + " ON " + d_part_table + "." + d_part_mid + " IS " + d_mms_table + "._id "
                         "WHERE thread._id IN (" + threadlist + ") AND " + d_thread_message_count + " > 0 "
                         "AND " + d_mms_table + ".date_received <= ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ? "
                         "AND (" + d_mms_table + "." + d_mms_type + " & ?) IS NOT ?) SELECT * FROM partitioned_messages WHERE partition_idx = 1 "
                         "ORDER BY "
                         "(pinned != 0) DESC, pinned ASC, " // before 266 pinned == 0 meant 'not pinned', after pinned = NULL is not pinned. But in the code, pinned = 0 is still not possible (when pinning something, even the first thread, the value is set to 1 to start)
                         + (d_database.tableContainsColumn("thread", "archived") ? "archived ASC, " : "") + "date DESC",
                         {maxtimestamp,
                          Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                          Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                          Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                          Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                          Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                          Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE},
                         &results))
    {
      Logger::error("Failed to query database for thread snippets.");
      return false;
    }
    //results.prettyPrint(true);
  }

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%F %T") // %F and %T do not work on minGW
  outputfile <<
    "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") <<
    " by signalbackup-tools (" << VERSIONDATE << "). "
    "Input database version: " << d_databaseversion << ". -->\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "  <head>\n"
    "    <meta charset=\"utf-8\">\n"
    "    <title>Signal conversation list</title>\n"
    "    <style>\n"
    "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {\n"
    "        /* " << (light ? "light" : "dark") << " */\n"
    "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << "\n"
    "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --conversationlistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --conversationlist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
    "        --conversationlist-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --spoiler-b: " << (light ? "rgba(0, 0, 0, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
    // "        --avatar-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << "\n"
    "        --avatar-c: #FFFFFF;\n"
    "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --icon-f: " << (light ? "brightness(0);" : "none;") << "\n"
    "      }\n"
    "\n";

  if (themeswitching)
  {
    outputfile <<
      "    :root[data-theme=\"" + (!light ? "light"s : "dark") + "\"] {\n"
      "        /* " << (!light ? "light" : "dark") << " */\n"
      "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << "\n"
      "        --body-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --conversationlistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --conversationlist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
      "        --conversationlist-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --spoiler-b: " << (!light ? "rgba(0, 0, 0, 0.5);" : "rgba(255, 255, 255, 0.5);") << "\n"
      // "        --avatar-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << "\n"
      "        --avatar-c: #FFFFFF;\n"
      "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --icon-f: " << (!light ? "brightness(0);" : "none;") << "\n"
      "      }\n";
  }

  outputfile <<
    "      body {\n"
    "        margin: 0px;\n"
    "        padding: 0px;\n"
    "        width: 100%;\n"
    "        background-color: var(--body-bgc);\n"
    "      }\n"
    "\n";

  outputfile <<
    "      #theme-switch {\n"
    "        display: none;\n"
    "      }\n"
    "\n"
    "      #page {\n"
    "        background-color: var(--body-bgc);\n"
    "        padding: 8px;\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        transition: color .2s, background-color .2s;\n"
    "      }\n"
    "\n";

  if (!exportdetails.empty())
    outputfile <<
      "      .export-details {\n"
      "        display: none;\n"
      "        grid-template-columns: repeat(2 , 1fr);\n"
      "        color: var(--body-c);\n"
      "        margin-left: auto;\n"
      "        margin-right: auto;\n"
      "        margin-bottom: 10px;\n"
      "        grid-gap: 0px 15px;\n"
      "        width: fit-content;\n"
      "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
      "      }\n"
      "      .export-details-fullwidth {\n"
      "        text-align: center;\n"
      "        font-weight: bold;\n"
      "        grid-column: 1 / 3;\n"
      "      }\n"
      "      .export-details div:nth-child(odd of :not(.export-details-fullwidth)) {\n"
      "        text-align: right;\n"
      "        font-style: italic;\n"
      "      }\n"
      "\n";

  outputfile <<
    "      .conversation-list-header {\n"
    "        text-align: center;\n"
    "        font-size: xx-large;\n"
    "        color: var(--conversationlistheader-c);\n"
    "        padding: 10px;\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "      }\n"
    "\n"
    "      .header {\n"
    "        margin-top: 5px;\n"
    "        margin-bottom: 5px;\n"
    "        margin-left: 10px;\n"
    "        font-weight: bold;\n"
    "      }\n"
    "\n"
    "      .conversation-list {\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        width: fit-content;\n"
    "        margin-top: 10px;\n"
    "        margin-bottom: 100px;\n"
    "        margin-right: auto;\n"
    "        margin-left: auto;\n"
    "        padding: 30px;\n"
    "        background-color: var(--conversationlist-bc);\n"
    "        color: var(--conversationlist-c);\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        border-radius: 10px;\n"
    "      }\n"
    "\n"
    "      .conversation-list-item {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "        padding: 10px;\n"
    "        margin: auto;\n"
    "        justify-content: center;\n"
    "        align-items: center;\n"
    "        align-content: center;\n"
    "      }\n"
    "\n"
    "      .avatar {\n"
    "        position: relative;\n"
    "        display: flex;\n"
    "        border-radius: 50%;\n"
    "        width: 60px;\n"
    "        height: 60px;\n"
    "        line-height: 60px;\n"
    "        text-align: center;\n"
    "        justify-content: center;\n"
    "        font-size: 38px;\n"
    "        color: var(--avatar-c);\n"
    "      }\n"
    "\n"
    "      .avatar-emoji-initial {\n"
    "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;\n"
    "      }\n"
    "\n"
    "      .note-to-self-icon {\n"
    "        background: #315FF4;\n"
    "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"80\" height=\"80\" viewBox=\"0 0 80 80\" fill=\"white\"><path d=\"M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z\"/></svg>');\n"
    "        background-position: center;\n"
    "        background-repeat: no-repeat;\n"
    "        background-size: 75%;\n"
    "      }\n"
    "\n"
    "      .group-avatar-icon,\n"
    "      .fullcontacts-icon {\n"
    "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"/></svg>');\n"
    "      }\n"
    "\n"
    "      .group-avatar-icon {\n"
    "        background-color: #315FF4;\n"
    "        background-position: center;\n"
    "        background-repeat: no-repeat;\n"
    "        background-size: 80%;\n"
    "      }\n"
    "\n";

  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, d_thread_recipient_id);
    if (rec_id == -1) [[unlikely]]
    {
      Logger::warning("Failed to get thread recipient id. Skipping.");
      continue;
    }

    if (getRecipientInfoFromMap(recipient_info, rec_id).hasavatar)
    {
      std::string raw_avatar_path(results.getValueAs<long long int>(i, "_id") == note_to_self_tid ?
                                  "Note to Self" : getRecipientInfoFromMap(recipient_info, rec_id).display_name);
      WIN_LIMIT_FILENAME_LENGTH(raw_avatar_path);
      std::string avatar_path(sanitizeFilename(raw_avatar_path, d_aggressive_filename_sanitizing) + " (_id" + results(i, "_id") + ")");
      if (compact) [[unlikely]]
        avatar_path = "id" + results(i, "_id");

      std::string avatar_extension = getAvatarExtension(rec_id);
      HTMLescapeUrl(&avatar_path);
      bepaald::replaceAll(&avatar_path, '\"', R"(\")");

      outputfile <<
        "      .avatar-" << rec_id << " {\n"
        "        background-image: url(\"" << avatar_path << "/media/Avatar_" << rec_id << "." << avatar_extension << "\");\n"
        //         "        background-image: url(\"" << avatar_path << "/" << avatar << "\");\n"
        "        background-position: center;\n"
        "        background-repeat: no-repeat;\n"
        "        background-size: cover;\n"
        "      }\n"
        "\n";
    }
    else if (results.isNull(i, "group_id")) // no avatar, no group
    {
      outputfile <<
        "      .avatar-" << rec_id << " {\n"
        "        background: #" << getRecipientInfoFromMap(recipient_info, rec_id).color << ";\n"
        "      }\n"
        "\n";
    }
  }

  outputfile <<
    "      .name-and-snippet {\n"
    "        position: relative;\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        padding-left: 30px;\n"
    "        justify-content: center;\n"
    "        align-content: center;\n"
    "        width: 350px;\n"
    "      }\n"
    "\n"
    "      .name-and-status {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "      }\n"
    "\n"
    "      .blocked {\n"
    "         background-image: url('data:image/svg+xml;utf-8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\" stroke-width=\"1\" ><path d=\"M12 1a11 11 0 1 0 11 11A11 11 0 0 0 12 1zm0 1.5a9.448 9.448 0 0 1 6.159 2.281L4.781 18.159A9.488 9.488 0 0 1 12 2.5zm0 19a9.448 9.448 0 0 1-6.159-2.281L19.219 5.841A9.488 9.488 0 0 1 12 21.5z\"/></svg>');\n"
    "        filter: var(--icon-f);\n"
    "        display: inline-block;\n"
    "        height: 18px;\n"
    "        aspect-ratio: 1 / 1;\n"
    "        margin-right: 8px;\n"
    "        margin-top: 3px;\n"
    "      }\n"
    "\n"
    "      .official {\n"
    "         background-image: url('data:image/svg+xml;utf-8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"3 3 18 18\"><path d=\"M10.662 3.455a1.82 1.82 0 0 1 2.676 0l.37.402a1.82 1.82 0 0 0 1.697.551l.535-.108a1.82 1.82 0 0 1 2.165 1.573l.063.542a1.82 1.82 0 0 0 1.049 1.444l.496.227a1.82 1.82 0 0 1 .827 2.545l-.268.476a1.82 1.82 0 0 0 0 1.785l.268.476a1.82 1.82 0 0 1-.827 2.545l-.496.227a1.82 1.82 0 0 0-1.049 1.444l-.063.542A1.82 1.82 0 0 1 15.94 19.7l-.535-.108a1.82 1.82 0 0 0-1.697.551l-.37.402a1.82 1.82 0 0 1-2.676 0l-.37-.402a1.82 1.82 0 0 0-1.697-.551l-.535.108a1.82 1.82 0 0 1-2.165-1.573l-.063-.542a1.82 1.82 0 0 0-1.049-1.444l-.496-.227a1.82 1.82 0 0 1-.827-2.545l.268-.476a1.82 1.82 0 0 0 0-1.785l-.268-.476a1.82 1.82 0 0 1 .827-2.545l.496-.227a1.82 1.82 0 0 0 1.049-1.444l.063-.542A1.82 1.82 0 0 1 8.06 4.3l.535.108a1.82 1.82 0 0 0 1.697-.551l.37-.402z\" fill=\"%232c6bed\"/><path d=\"M16.524 9.948l-.999-.999-4.55 4.551-2.164-2.164-.999.999 3.162 3.162 5.549-5.549z\" fill=\"white\"/></svg>');\n"
    "        display: inline-block;\n"
    "        height: 18px;\n"
    "        aspect-ratio: 1 / 1;\n"
    "        margin-left: 8px;\n"
    "        margin-top: 3px;\n"
    "      }\n"
    "\n"
    // only showing the muted icon for threads muted 'forever', for others it gets a bit
    // difficult since the mute_until is never cleared (contains past timestamps) and the
    // value is check against 'current time' normally.
    "      .muted {\n"
    "         background-image: url('data:image/svg+xml;utf-8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 1 16 16\" fill=\"white\"><path d=\"M6.163 14h3.674a1.875 1.875 0 0 1-3.674 0zM8 2a3.233 3.233 0 0 1 3.041 2.171l.113.322L3.5 10.507a8.079 8.079 0 0 0 .335-1.136L4.84 4.548A3.25 3.25 0 0 1 8 2m0-1a4.236 4.236 0 0 0-4.138 3.337l-1.007 4.83a5.83 5.83 0 0 1-1.785 3.25l-.879.69.618.786 14-11-.618-.786-2.206 1.734A4.225 4.225 0 0 0 8 1zm6.54 10.035a2.846 2.846 0 0 1-1.395-1.868l-.662-3.176h0l-.878.689.564 2.7a3.954 3.954 0 0 0 1.89 2.558A.059.059 0 0 1 14 12H4.834l-1.272 1H14a1.056 1.056 0 0 0 .54-1.965z\"/></svg>');\n"
    "        display: inline-block;\n"
    "        height: 18px;\n"
    "        aspect-ratio: 1 / 1;\n"
    "        margin-left: 8px;\n"
    "        margin-top: 4px;\n"
    "        filter: var(--icon-f);\n"
    "      }\n"
    "\n"
    "      .name {\n"
    "        font-weight: bold;\n"
    "        font-size: 18px;\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        margin: 0px;\n"
    "        padding: 0px;\n"
    "        overflow: hidden;\n"
    "        text-overflow: ellipsis;\n"
    "      }\n"
    "\n"
    "      .groupsender {\n"
    "        font-weight: 500;\n"
    "      }\n"
    "\n"
    "      .snippet {\n"
    "        display: -webkit-box;\n"
    "        -webkit-line-clamp: 2;\n"
    "/*        line-clamp: 2; This is still in working draft, though the vendor extension version is well supported */\n"
    "        -webkit-box-orient: vertical;\n"
    "/*        box-orient: vertical; */\n"
    "        overflow: hidden;\n"
    "        text-overflow: ellipsis;\n"
    "      }\n"
    "\n"
    "      .monospace\n"
    "      {\n"
    "        font-family: 'Roboto Mono', 'Noto Mono', \"Liberation Mono\", OpenMono,  monospace;\n"
    "      }\n"
    "\n"
    "      .spoiler {\n"
    "        transition: background .2s, filter .2s;\n"
    "        filter: blur(5px) saturate(0%) contrast(0);\n"
    "        background: var(--spoiler-b);\n"
    "      }\n"
    "\n"
    "      .spoiler:hover,\n"
    "      .spoiler:active {\n"
    "        background: transparent;\n"
    "        filter: none;\n"
    "        transition: background .2s, filter .2s;\n"
    "      }\n"
    "\n"
    "      .msg-emoji {\n"
    "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;\n"
    "      }\n"
    "\n"
    "      .index-date {\n"
    "        position: relative;\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        padding-left: 20px;\n"
    "        font-size: small;\n"
    "      /*font-style: italic;*/\n"
    "        text-align: right;\n"
    "        max-width: 100px;\n"
    "      }\n"
    "\n"
    "      .main-link::before {\n"
    "        content: \" \";\n"
    "        position: absolute;\n"
    "        top: 0;\n"
    "        left: 0;\n"
    "        width: 100%;\n"
    "        height: 100%;\n"
    "      }\n"
    "\n"
    "      .menu-item > div {\n"
    "        margin-right: 5px;\n"
    "      }\n"
    "\n";
  if (menuitems > 0 ||
      chatfolders.size() > 0 ||
      chatfolder_idx >= 0)
  {
    outputfile <<
      "      #menu {\n"
      "        display: flex;\n"
      "        flex-direction: column;\n"
      "        position: fixed;\n"
      "        top: 20px;\n"
      "        left: 20px;\n"
      "        background: var(--body-bgc);\n"
      "        cursor: pointer;\n"
      "      }\n"
      "\n"
      "      #menu a:link,\n"
      "      #menu a:visited,\n"
      "      #menu a:hover,\n"
      "      #menu a:active {\n"
      "        color: #FFFFFF;\n"
      "        text-decoration: none;\n"
      "      }\n"
      "\n";
  }
  outputfile <<
    "      .menu-icon {\n"
    "        margin-right: 0px;\n"
    "        width: 30px;\n"
    "        aspect-ratio: 1 / 1;\n"
    "        background-position: center;\n"
    "        background-repeat: no-repeat;\n"
    "        background-size: cover;\n"
    "      }\n"
    "\n"
    "      .menu-item {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "        color: var(--menuitem-c);\n"
    "        align-items: center;\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        padding: 5px;\n"
    "      }\n"
    "\n";
  if (chatfolder_idx >= 0)
    outputfile <<
      "      .current-menu-item {\n"
      "        filter: opacity(0.5);\n"
      "      }\n"
      "\n";

  if (menuitems > 1 ||
      chatfolders.size() > 0 ||
      (chatfolder_idx >= 0 && menuitems > 0)) // we need an expandable menu
  {
    outputfile <<
      "       .menu-separator {\n"
      "         width: 90%;\n"
      "         height: 5px;\n"
      "       }\n"
      "\n"
      "       .menu-separator hr {\n"
      "         width: 100%;\n"
      "       }\n"
      "\n"
      "       #expandedmenu .menu-item {\n"
      "         padding-left: 0px;\n"
      "         padding-bottom: 0px;\n"
      "       }\n"
      "\n"
      "       .expandable-menu-item {\n"
      "         display: flex;\n"
      "         flex-direction: column;\n"
      "         margin-right: 0px;\n"
      "         cursor: pointer;\n"
      "       }  \n"
      "\n"
      "      #expandedmenu {\n"
      "        display: flex;\n"
      "        flex-direction: column;\n"
      "        align-items: flex-start;\n"
      "        width: 0px;\n"
      "        max-height: 0px;\n"
      "        overflow: hidden;\n"
      "        padding: 0px;\n"
      "        opacity: 0%;\n"
      "        border: none; \n"
      "        background: var(--body-bgc);\n"
      "        transition: max-height .25s ease-out, padding .25s ease-out, opacity .25s ease-out, width 0s 0.25s, overflow 0s 0.25s;\n"
      "      }\n"
      "\n"
      "     #menu:hover #expandedmenu {\n"
      "       max-height: " << (menuitems + (chatfolder_idx >= 0 ? 1 : 0)) * 35 + chatfolders.size() * 35 + (chatfolders.size() ? 5 : 0) << "px;\n"
      "       width: 100%;\n"
      "       padding-top: 10px;\n"
      "       opacity: 100%;\n"
      "       transition: max-height .25s ease-out, padding .25s ease-out, opacity .15s ease-out;\n"
      "     }\n"
      "\n"
      "     .menu-header .label-text {\n"
      "       display: inline-block;\n"
      "       height: 100%;\n"
      "       vertical-align: middle;\n"
      "     }\n"
      "\n"
      "     #menu #hamburger-icon {\n"
      "       display: inline-block;\n"
      "       width:30px;\n"
      "       height: 30px;\n"
      "       margin-right: 5px;\n"
      "       vertical-align: middle;\n"
      "       background: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M13.5 5.5A1.5 1.5 0 1 1 12 4a1.5 1.5 0 0 1 1.5 1.5zm-1.5 5a1.5 1.5 0 1 0 1.5 1.5 1.5 1.5 0 0 0-1.5-1.5zm0 6.5a1.5 1.5 0 1 0 1.5 1.5A1.5 1.5 0 0 0 12 17z\"/></svg>');\n"
    "       filter: var(--icon-f);\n"
      "       transition: background 0.25s ease-out, transform 0.25s ease-out;\n"
      "     }\n"
      "\n"
      "     #menu:hover #hamburger-icon {\n"
      "       background: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"30\" height=\"30\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M21 11.25h-8.25V3h-1.5v8.25H3v1.5h8.25V21h1.5v-8.25H21v-1.5z\"/></g></svg>');\n"
    "       transform: rotate(45deg);\n"
      "       transition: background 0.25s ease-out, transform 0.25s ease-out;\n"
      "     }\n"
      "\n";
  }

  if (chatfolder_idx >= 0)
  {
    outputfile <<
      "    #chatfolder-name {\n"
      "      font-size: large;\n"
      "      max-width: 50%;\n"
      "      margin-left: auto;\n"
      "      margin-right: auto;\n"
      "    }\n"
      "\n"
      "    #chatfolder-name input[type=checkbox] {\n"
      "      display: none;\n"
      "    }\n"
      "\n"
      "    #chatfolder-name input[type=checkbox]:checked ~ label > #chatfolder-details {\n"
      "      max-height: none;\n"
      "      padding-top: 5px;\n"
      "      padding-bottom: 5px;\n"
      "      overflow: visible;\n"
      "      transition: padding-top 0.2s ease, padding-bottom 0.2s ease, max-height 0.4s ease;\n"
      "    }\n"
      "\n"
      "    #chatfolder-name input[type=checkbox] ~ label > small::before {\n"
      "      content: '(show';\n"
      "    }\n"
      "\n"
      "    #chatfolder-name input[type=checkbox]:checked ~ label >  small::before {\n"
      "      content: '(hide';\n"
      "    }\n"
      "\n"
      "    #chatfolder-details {\n"
      "      display: block;\n"
      "      font-size: small;\n"
      "      max-height: 0px;\n"
      "      max-width: 90%;\n"
      "      margin-left: auto;\n"
      "      margin-right: auto;\n"
      "      overflow: hidden;\n"
      "      padding-top: 0px;\n"
      "      padding-bottom: 0px;\n"
      "      transition: padding-top 0.05s ease, padding-bottom 0.05s ease, max-height 0.25s ease;\n"
      "    }\n"
      "\n"
      "    .columnview {\n"
      "      display: flex;\n"
      "      flex-flow: row wrap;\n"
      "      justify-content: space-between;\n"
      "      overflow-wrap: anywhere;\n"
      "    }\n"
      "\n"
      "    .column-left-align,\n"
      "    .column-right-align {\n"
      "      flex: 0 0 49%;\n"
      "    }\n"
      "\n"
      "    .column-right-align {\n"
      "      padding-right: 1%;\n"
      "      text-align: right;\n"
      "    }\n"
      "\n"
      "    .column-left-align {\n"
      "      padding-left: 1%;\n"
      "      text-align: left;\n"
      "    }\n"
      "\n"
      "    .columnview-header {\n"
      "      flex: 0 0 100%;\n"
      "      text-align: center;\n"
      "    }\n"
      "\n"
      "    #chatfolder-details .columnview-header {\n"
      "      font-style: italic;\n"
      "    }\n"
      "\n";
  }

  outputfile <<
    "      #theme {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "        position: fixed;\n"
    "        top: 20px;\n"
    "        right: 20px;\n"
    "      }\n"
    "\n";
  if (themeswitching)
  {
    outputfile <<
      "      .themebutton {\n"
      "        display: block;\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 25 25\" fill=\"white\" stroke=\"white\"><defs><path id=\"s\" d=\"m12.5,2.5 2.5,2.5c0.27,0.27 0.63,0.43 1,0.43L19.62,5.42h1.79l-3.8e-5,-0.4C21.41,4.24 20.77,3.61 20,3.6l-3.8,0 -2.64,-2.65C13,0.4 12,0.4 11.5,0.96l-0.28,0.28z\"/></defs><path d=\"m12,8.13c0,-0.4 0.35,-0.8 0.8,-0.76 2.7,0.14 4.86,2.4 4.86,5.13 0,2.75 -2.15,5 -4.86,5.14 -0.45,0 -0.8,-0.35 -0.8,-0.77z\"/><use href=\"%23s\"/><use href=\"%23s\" transform=\"rotate(45 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(90 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(135 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(180 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(225 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(270 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(315 12.5 12.5)\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "      }\n"
      "\n";
  }

  if (searchpage)
  {
    outputfile <<
      "    .searchbutton {\n"
      "      display: block;\n"
      "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M10 2.125a7.875 7.875 0 1 0 4.716 14.182l4.989 4.989a1.125 1.125 0 0 0 1.59-1.591l-4.988-4.989A7.875 7.875 0 0 0 10 2.125zM3.875 10a6.125 6.125 0 1 1 12.25 0 6.125 6.125 0 0 1-12.25 0z\"/></g></svg>');\n"
      "      filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (calllog)
  {
    outputfile <<
      "      .calllog-icon {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M17.21 22a8.08 8.08 0 0 1-2.66-.51 20.79 20.79 0 0 1-7.3-4.73 21 21 0 0 1-4.74-7.3c-.78-2.22-.67-4 .35-5.45h0a5 5 0 0 1 2-1.67 2.72 2.72 0 0 1 3.51.81l2.11 3a2.69 2.69 0 0 1-.35 3.49l-.93.85c-.09.08-.15.22-.08.31A20 20 0 0 0 11 13a20 20 0 0 0 2.21 1.91.24.24 0 0 0 .3-.08l.85-.93a2.68 2.68 0 0 1 3.49-.35l3 2.11a2.68 2.68 0 0 1 .85 3.43 5.22 5.22 0 0 1-1.71 2 4.69 4.69 0 0 1-2.78.91zM4.09 4.87c-.46.64-1 1.77-.16 4.08a19.28 19.28 0 0 0 4.38 6.74A19.49 19.49 0 0 0 15 20.07c2.31.81 3.44.3 4.09-.16a3.55 3.55 0 0 0 1.2-1.42A1.21 1.21 0 0 0 20 16.9l-3-2.12a1.18 1.18 0 0 0-1.53.15l-.82.9a1.72 1.72 0 0 1-2.33.29 21.9 21.9 0 0 1-2.37-2.05 22.2 22.2 0 0 1-2-2.37 1.71 1.71 0 0 1 .3-2.32l.89-.82A1.19 1.19 0 0 0 9.21 7L7.1 4a1.19 1.19 0 0 0-1.51-.38 3.72 3.72 0 0 0-1.5 1.25z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (blocked)
  {
    outputfile <<
      "      .blocked-icon {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"-2 -1 29 29\" fill=\"white\" stroke=\"white\"><path d=\"M12 1a11 11 0 1 0 11 11A11 11 0 0 0 12 1zm0 1.5a9.448 9.448 0 0 1 6.159 2.281L4.781 18.159A9.488 9.488 0 0 1 12 2.5zm0 19a9.448 9.448 0 0 1-6.159-2.281L19.219 5.841A9.488 9.488 0 0 1 12 21.5z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (fullcontacts)
  {
    outputfile <<
      "      .fullcontacts-icon {\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (stickerpacks)
  {
    outputfile <<
      "      .stickerpacks-icon {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M21.2 5.072A5.55 5.55 0 0 0 18.928 2.8c-.977-.522-1.947-.8-4.62-.8H9.692c-2.673 0-3.643.278-4.62.8A5.55 5.55 0 0 0 2.8 5.072c-.522.977-.8 1.947-.8 4.62v4.616c0 2.673.278 3.643.8 4.62A5.55 5.55 0 0 0 5.072 21.2c1.118.567 2.363.837 3.616.785h.1a3 3 0 0 0 1.7-.53L20.734 14.4A3 3 0 0 0 22 11.949V9.692c0-2.673-.278-3.643-.8-4.62zM8.739 20.485a5.82 5.82 0 0 1-2.96-.608 4.02 4.02 0 0 1-1.656-1.656c-.365-.683-.623-1.363-.623-3.913V9.692c0-2.55.258-3.231.623-3.913a4.02 4.02 0 0 1 1.656-1.656c.683-.365 1.363-.623 3.913-.623h4.616c2.55 0 3.231.258 3.913.623a4.02 4.02 0 0 1 1.656 1.656c.365.683.623 1.363.623 3.913v2.257c-.002.101-.014.201-.036.3h-3.273c-2.8 0-3.872.3-4.975.89a6.17 6.17 0 0 0-2.575 2.575c-.575 1.074-.872 2.132-.888 4.769h-.014zm1.525-.7a6.63 6.63 0 0 1 .7-3.362 4.7 4.7 0 0 1 1.96-1.961c.755-.4 1.549-.712 4.268-.712h1.837z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (settings)
  {
    outputfile <<
      "      .settings-icon {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M12 8.5A3.5 3.5 0 1 1 8.5 12 3.5 3.5 0 0 1 12 8.5M12 7a5 5 0 1 0 5 5 5 5 0 0 0-5-5zm0-4.49a9.83 9.83 0 0 1 1.21.08l.21 2.49.91.33a5.72 5.72 0 0 1 .68.28l.88.42 1.91-1.62a9.23 9.23 0 0 1 1.71 1.71l-1.62 1.91.42.88a5.72 5.72 0 0 1 .28.68l.33.91 2.49.21a8.91 8.91 0 0 1 0 2.42l-2.49.21-.33.91a5.72 5.72 0 0 1-.28.68l-.42.88 1.62 1.91a9.23 9.23 0 0 1-1.71 1.71l-1.91-1.62-.88.42a5.72 5.72 0 0 1-.68.28l-.91.33-.21 2.49a9.19 9.19 0 0 1-2.42 0l-.21-2.49-.91-.33a5.72 5.72 0 0 1-.67-.28l-.88-.42-1.92 1.62a9.23 9.23 0 0 1-1.71-1.71l1.62-1.91-.42-.89a5.72 5.72 0 0 1-.28-.68l-.33-.91-2.49-.21a8.91 8.91 0 0 1 0-2.42l2.49-.21.33-.91A5.72 5.72 0 0 1 5.69 9l.42-.88L4.49 6.2A9.23 9.23 0 0 1 6.2 4.49l1.91 1.62.89-.42a5.72 5.72 0 0 1 .68-.28l.91-.33.21-2.49a9.83 9.83 0 0 1 1.2-.08h0M12 1a10.93 10.93 0 0 0-1.88.16 1 1 0 0 0-.79.9L9.17 4a7.64 7.64 0 0 0-.83.35L6.87 3.09a1 1 0 0 0-.66-.24 1 1 0 0 0-.54.15 11 11 0 0 0-2.62 2.62 1 1 0 0 0 0 1.25l1.29 1.47a7.64 7.64 0 0 0-.34.83l-1.92.16a1 1 0 0 0-.9.79 11 11 0 0 0 0 3.76 1 1 0 0 0 .9.79l1.92.16a7.64 7.64 0 0 0 .35.83l-1.26 1.47a1 1 0 0 0-.09 1.2A11 11 0 0 0 5.62 21a1 1 0 0 0 .61.19 1 1 0 0 0 .64-.23l1.47-1.25a7.64 7.64 0 0 0 .83.35l.16 1.92a1 1 0 0 0 .79.9A11.83 11.83 0 0 0 12 23a10.93 10.93 0 0 0 1.88-.16 1 1 0 0 0 .79-.9l.16-1.94a7.64 7.64 0 0 0 .83-.35l1.47 1.25a1 1 0 0 0 .66.24 1 1 0 0 0 .54-.16 11 11 0 0 0 2.67-2.6 1 1 0 0 0 0-1.25l-1.25-1.47a7.64 7.64 0 0 0 .35-.83l1.92-.16a1 1 0 0 0 .9-.79 11 11 0 0 0 0-3.76 1 1 0 0 0-.9-.79L20 9.17a7.64 7.64 0 0 0-.35-.83l1.25-1.47a1 1 0 0 0 .1-1.2 11 11 0 0 0-2.61-2.62 1 1 0 0 0-.61-.19 1 1 0 0 0-.64.23l-1.48 1.25a7.64 7.64 0 0 0-.83-.34l-.16-1.92a1 1 0 0 0-.79-.9A11.83 11.83 0 0 0 12 1z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (chatfolders.size() > 0)
  {
    outputfile <<
      "      .chatfolders-icon {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"white\"><path d=\"M4.063 20.397C2.046 20.397 1 19.361 1 17.362V5.979C1 4.018 2.027 3 3.773 3h2.643c.971 0 1.457.177 2.111.719l.541.43c.514.42.906.579 1.578.579h8.685c2.017 0 3.063 1.036 3.063 3.035v9.599c0 1.999-1.036 3.035-2.82 3.035H4.063zM2.83 6.138v2.325h17.734v-.542c0-.915-.476-1.363-1.335-1.363h-9.068c-.971 0-1.476-.178-2.12-.71L7.5 5.409c-.523-.429-.906-.579-1.569-.579H4.138c-.841 0-1.308.449-1.308 1.308zm1.345 12.429h15.054c.859 0 1.335-.448 1.335-1.354v-7.135H2.83v7.135c0 .906.477 1.354 1.345 1.354z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  if (chatfolder_idx >= 0)
  {
    outputfile <<
      "      .nav-up {\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "    }\n"
      "\n";
  }

  outputfile <<
    "      @media print {\n"
    "        .conversation-list-header {\n"
    "          padding: 0;\n"
    "        }\n"
    "\n"
    "        .conversation-list-item {\n"
    "          break-inside: avoid;\n"
    "        }\n"
    "\n"
    "        .conversation-list {\n"
    "          margin: 0 auto;\n"
    "          display: block;\n"
    "          border-radius: 0;\n"
    "        }\n"
    "\n"
    "        .avatar {\n"
    "          -webkit-print-color-adjust: exact;\n"
    "          color-adjust: exact;\n"
    "          print-color-adjust: exact;\n"
    "          flex-shrink: 0;\n"
    "        }\n"
    "\n";

  if (!exportdetails.empty())
    outputfile <<
      "        .export-details {\n"
      "          display: grid;\n"
      "        }\n"
      "\n";

  if (chatfolder_idx >= 0)
    outputfile <<
      "        #chatfolder-details {\n"
      "          max-height: none;\n"
      "          padding-top: 5px;\n"
      "          padding-bottom: 5px;\n"
      "          overflow: visible;\n"
      "        }\n"
      "\n";

  outputfile <<
    "        #menu {\n"
    "          display: none;\n"
    "        }\n"
    "\n"
    "        #theme {\n"
    "          display: none;\n"
    "        }\n"
    "      }\n"
    "\n"
    "    </style>\n"
    "  </head>\n"
    "  <body>\n";
  if (themeswitching)
  {
    outputfile << R"(    <script>
      function setCookie(name, value, days)
      {
        var expires = "";
        if (days)
        {
          var date = new Date();
          date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
          expires = "; expires=" + date.toUTCString();
        }
        document.cookie = name + "=" + (value || "")  + expires + "; SameSite=None; Secure; path=/";
      }

      function getCookie(name)
      {
        var nameEQ = name + "=";
        var ca = document.cookie.split(';');
        for (var i = 0; i < ca.length; ++i)
        {
          var c = ca[i];
          while (c.charAt(0) == ' ')
            c = c.substring(1, c.length);
          if (c.indexOf(nameEQ) == 0)
            return c.substring(nameEQ.length, c.length);
        }
        return null;
      }

      function eraseCookie(name)
      {
        document.cookie = name + '=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/'
      }

      // Important to be 1st in the DOM
      const theme = getCookie('theme') || ')" << (light ? "light" : "dark") << R"(';
      //alert(theme);

      document.documentElement.dataset.theme = theme;
    </script>)";
  }

  outputfile <<
    "\n"
    "  <input type=\"checkbox\" id=\"theme-switch\">\n"
    "  <div id=\"page\">\n"
    "\n"
    "    <div class=\"conversation-list-header\">\n"
    "      Signal conversation list\n";

  if (chatfolder_idx >= 0)
  {
    SqliteDB::QueryResults cf_details;
    SqliteDB::QueryResults cf_details_members_included;
    SqliteDB::QueryResults cf_details_members_excluded;
    if (d_database.exec("SELECT name, show_unread, show_muted, show_individual, show_groups, is_muted, folder_type FROM chat_folder WHERE _id = ?", chatfolder_idx, &cf_details) &&
        d_database.exec("SELECT thread.recipient_id FROM chat_folder_membership "
                        "LEFT JOIN thread ON thread._id = chat_folder_membership.thread_id "
                        "WHERE chat_folder_id = ? AND membership_type = 0", chatfolder_idx, &cf_details_members_included) &&
        d_database.exec("SELECT thread.recipient_id FROM chat_folder_membership "
                        "LEFT JOIN thread ON thread._id = chat_folder_membership.thread_id "
                        "WHERE chat_folder_id = ? AND membership_type = 1", chatfolder_idx, &cf_details_members_excluded))
    {
      outputfile <<
        "      <div id=\"chatfolder-name\">\n"
        "        " << HTMLescapeString(cf_details("name")) << "\n"
        "        <input type=\"checkbox\" id=\"show-chatfolder-details\">\n"
        "        <label for=\"show-chatfolder-details\">\n"
        "        <small> details)</small>\n"
        "        <span id=\"chatfolder-details\">\n"
        "          <span class=\"columnview\">\n"
        "            <span class=\"column-right-align\">Show 1:1 chats:</span>\n"
        "            <span class=\"column-left-align\">" << (cf_details.valueAsInt(0, "show_individual") ? "true" : "false") << "</span>\n"
        "            <span class=\"column-right-align\">Show group chats:</span>\n"
        "            <span class=\"column-left-align\">" << (cf_details.valueAsInt(0, "show_groups") ? "true" : "false") << "</span>\n"
        "            <span class=\"column-right-align\">Included chats:</span>\n"
        "            <span class=\"column-left-align\">";
      if (cf_details_members_included.rows() == 0)
        outputfile << "(none)";
      else
        for (unsigned int i = 0; i < cf_details_members_included.rows(); ++i)
        {
          if (i > 0)
            outputfile << ", ";
          outputfile << HTMLescapeString(getRecipientInfoFromMap(recipient_info, cf_details_members_included.valueAsInt(i, "recipient_id")).display_name);
        }

      outputfile <<
        "</span>\n"
        "            <span class=\"column-right-align\">Excluded chats:</span>\n"
        "            <span class=\"column-left-align\">";

      if (cf_details_members_excluded.rows() == 0)
        outputfile << "(none)";
      else
        for (unsigned int i = 0; i < cf_details_members_excluded.rows(); ++i)
        {
          if (i > 0)
            outputfile << ", ";
          outputfile << HTMLescapeString(getRecipientInfoFromMap(recipient_info, cf_details_members_excluded.valueAsInt(i, "recipient_id")).display_name);
        }

      outputfile <<
        "</span>\n"
        "            <span class=\"column-right-align\">Show only unread:</span>\n"
        "            <span class=\"column-left-align\">" << (cf_details.valueAsInt(0, "show_unread") ? "true" : "false") << "</span>\n"
        "            <span class=\"column-right-align\">Include muted chats:</span>\n"
        "            <span class=\"column-left-align\">" << (cf_details.valueAsInt(0, "show_muted") ? "true" : "false") << "</span>\n"
        "            <span class=\"column-right-align\">Muted:</span>\n"
        "            <span class=\"column-left-align\">" << (cf_details.valueAsInt(0, "is_muted") ? "true" : "false") << "</span>\n"
        "          </span>\n"
        "        </span>\n"
        "        </label>\n"
        "      </div>\n";
    }
  }
  outputfile <<
    "    </div>\n"
    "\n"
    "    <div class=\"conversation-list\">\n"
    "\n";

  // for item in threads
  bool pinnedheader = false;
  bool archivedheader = false;
  bool chatsheader = false;
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    bool archived = false;
    if (d_database.tableContainsColumn("thread", "archived"))
      archived = (results.getValueAs<long long int>(i, "archived") != 0);
    if (archived && !archivedheader)
    {
      outputfile << "      <div class=\"header\">Archived conversations</div>\n";
      archivedheader = true;
    }

    bool pinned = (results.valueAsInt(i, "pinned", 0) != 0); // before 266 pinned == 0 meant 'not pinned', after pinned = NULL is not pinned. But in the code, pinned = 0 is still not possible (when pinning something, even the first thread, the value is set to 1 to start)
    if (pinned && !pinnedheader)
    {
      outputfile << "      <div class=\"header\">Pinned</div>\n";
      pinnedheader = true;
    }

    if (pinnedheader && !pinned && !chatsheader && !archived) // this message is not pinned, but pinnedheader was previously shown
    {
      outputfile << "      <div class=\"header\">Chats</div>\n";
      chatsheader = true;
    }

    long long int rec_id = results.valueAsInt(i, d_thread_recipient_id);
    if (rec_id == -1) [[unlikely]]
    {
      Logger::warning("Failed to get thread recipient id. Skipping.");
      continue;
    }

    if (!results.valueHasType<long long int>(i, "_id"))
      continue;
    long long int t_id = results.getValueAs<long long int>(i, "_id");

    if (!results.valueHasType<long long int>(i, "snippet_type"))
      continue;
    long long int snippet_type = results.getValueAs<long long int>(i, "snippet_type");

    bool isblocked = getRecipientInfoFromMap(recipient_info, rec_id).blocked;
    bool ismuted = getRecipientInfoFromMap(recipient_info, rec_id).mute_until == 0x7FFFFFFFFFFFFFFF;
    bool isgroup = !results.isNull(i, "group_id");
    bool isnotetoself = (t_id == note_to_self_tid);
    bool emoji_initial = getRecipientInfoFromMap(recipient_info, rec_id).initial_is_emoji;
    bool hasavatar = getRecipientInfoFromMap(recipient_info, rec_id).hasavatar;
    bool isreleasechannel = (t_id == releasechannel_tid);

    long long int groupsender = -1;
    if (results.valueHasType<std::string>(i, "group_sender_id"))
      groupsender = bepaald::toNumber<long long int>(results.valueAsString(i, "group_sender_id"));

    std::string snippet = results.valueAsString(i, "snippet");
    //HTMLescapeString(&snippet);
    std::string snippet_ranges = results.valueAsString(i, "snippet_ranges");
    if (!snippet_ranges.empty())
    {
      if (maxtimestamp == -1) // ranges were taken from thread table, here they are base64 encoded
      {
        auto [data, length] = Base64::base64StringToBytes(snippet_ranges);
        std::pair<std::shared_ptr<unsigned char []>, size_t> brdata(data, length);
        HTMLprepMsgBody(&snippet, std::vector<std::tuple<long long int, long long int, long long int>>(), recipient_info,
                        !Types::isOutgoing(snippet_type), brdata, false /*linkify*/, false);
      }
      else // range from message, here range is in binary format
      {
        std::pair<std::shared_ptr<unsigned char []>, size_t> brdata = results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "snippet_ranges");
        HTMLprepMsgBody(&snippet, std::vector<std::tuple<long long int, long long int, long long int>>(), recipient_info,
                        !Types::isOutgoing(snippet_type), brdata, false /*linkify*/, false);
      }
    }

    if (maxtimestamp != -1) // we are creating a new snippet, add attachment icon if needed
    {                       // needs to be done _after_ applying ranges.
      std::string attachmenttype = results.valueAsString(i, "partct");
      if (STRING_STARTS_WITH(attachmenttype, "image/gif"))
        snippet = "<span class=\"msg-emoji\">\xF0\x9F\x8E\xA1</span> " + (snippet.empty() ? "GIF" : snippet); // ferris wheel emoji for some reason
      else if (STRING_STARTS_WITH(attachmenttype, "image"))
        snippet = "<span class=\"msg-emoji\">\xF0\x9F\x93\xB7</span> " + (snippet.empty() ? "Photo" : snippet); // (still) camera emoji
      else if (STRING_STARTS_WITH(attachmenttype, "audio"))
        snippet = "<span class=\"msg-emoji\">\xF0\x9F\x8E\xA4</span> " + (snippet.empty() ? "Voice message" : snippet); // microphone emoji
      else if (STRING_STARTS_WITH(attachmenttype, "video"))
        snippet = "<span class=\"msg-emoji\">\xF0\x9F\x8E\xA5</span> " + (snippet.empty() ? "Video" : snippet); //  (movie) camera emoji
      else if (!attachmenttype.empty()) // if binary file
        snippet = "<span class=\"msg-emoji\">\xF0\x9F\x93\x8E</span> " + (snippet.empty() ? "File" : snippet); // paperclip
    }

    if (results.valueAsInt(i, "deleted", 0) == 1)
    {
      if (Types::isOutgoing(snippet_type))
        snippet = "<i>You deleted this message.</i>";
      else
        snippet = "<i>This message was deleted.</i>";
    }

    bool isstatusmsg = Types::isStatusMessage(snippet_type);
    if (isstatusmsg)
    {
      // if snippet message extras does not exist, or was empty, decode from body
      if (!(d_database.tableContainsColumn("thread", "snippet_message_extras") &&
            results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "snippet_message_extras")))
        snippet = decodeStatusMessage(snippet, results.valueAsInt(i, "recipient_expires_in", 0), snippet_type,
                                      isgroup ?
                                      (groupsender > 0 ? getRecipientInfoFromMap(recipient_info, groupsender).display_name : "") :
                                      getRecipientInfoFromMap(recipient_info, rec_id).display_name,
                                      nullptr);
      else if (results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "snippet_message_extras"))
        snippet = decodeStatusMessage(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "snippet_message_extras"), results.valueAsInt(i, "recipient_expires_in", 0), snippet_type,
                                      isgroup ?
                                      (groupsender > 0 ? getRecipientInfoFromMap(recipient_info, groupsender).display_name : "") :
                                      getRecipientInfoFromMap(recipient_info, rec_id).display_name,
                                      nullptr);

      if (snippet.empty())
        snippet = "(status message)";
    }

    if (excludeexpiring && results.valueAsInt(i, "recipient_expires_in", 0) > 0) [[unlikely]]
    {
      /*
        ThreadTable::
        private fun isSilentType(type: Long): Boolean {
          return MessageTypes.isProfileChange(type) ||
          MessageTypes.isGroupV1MigrationEvent(type) ||
          MessageTypes.isChangeNumber(type) ||
          MessageTypes.isBoostRequest(type) ||
          MessageTypes.isGroupV2LeaveOnly(type) ||
          MessageTypes.isThreadMergeType(type)
        }
      */
      SqliteDB::QueryResults newsnippet_info;
      if (d_database.exec(bepaald::concat("SELECT "
                                          "body,"
                                          "expires_in,",
                                          (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "message_extras," : "NULL AS message_extras,"),
                                          "IFNULL(remote_deleted, 0) AS remote_deleted,",
                                          d_mms_type, " AS type "
                                          "FROM ", d_mms_table,
                                          " WHERE thread_id = ?",                    // get last body from this thread, where
                                          (maxtimestamp > 0 ? " AND " + d_mms_table + ".date_received <= " + bepaald::toString(maxtimestamp) : ""),
                                          " AND ("
                                          "(expires_in == 0"                         // it's not expiring (and not any
                                          " AND (", d_mms_type, " & ?) != ?"         // of the normally excluded snippet types,
                                          " AND (", d_mms_type, " & ?) != ?"         // like identity verifications or spam reports
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          " AND (", d_mms_type, " & ?) IS NOT ?"
                                          ")"
                                          " OR ("
                                          "expires_in != 0"                         // or it _is_ expiring, but
                                          " AND (", d_mms_type, " & ?) != 0)"       // it is the expiration timer update itself
                                          ")"
                                          " ORDER BY date_received DESC LIMIT 1"),
                          {t_id,
                           Types::SPECIAL_TYPES_MASK, Types::SPECIAL_TYPE_REPORTED_SPAM,
                           Types::SPECIAL_TYPES_MASK, Types::SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED,
                           Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                           Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                           Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                           Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                           Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                           Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE,
                           Types::EXPIRATION_TIMER_UPDATE_BIT}, &newsnippet_info) &&
          newsnippet_info.rows() == 1)
        {
          long long int newsnippet_type = newsnippet_info.valueAsInt(0, "type");
          if (newsnippet_info.valueAsInt(0, "remote_deleted") != 0)
          {
            if (Types::isOutgoing(newsnippet_type))
              snippet = "<i>You deleted this message.</i>";
            else
              snippet = "<i>This message was deleted.</i>";
          }
          else if (Types::isStatusMessage(newsnippet_type))
          {
            isstatusmsg = true;
            if (!(d_database.tableContainsColumn(d_mms_table, "message_extras") &&
                  newsnippet_info.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "message_extras")))
              snippet =
                decodeStatusMessage(newsnippet_info("body"), newsnippet_info.valueAsInt(0, "expires_in", 0), newsnippet_type,
                                    isgroup ?
                                    (groupsender > 0 ? getRecipientInfoFromMap(recipient_info, groupsender).display_name : "") :
                                    getRecipientInfoFromMap(recipient_info, rec_id).display_name,
                                    nullptr);
            else if (newsnippet_info.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "message_extras"))
              snippet =
                decodeStatusMessage(newsnippet_info.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(0, "message_extras"), newsnippet_info.valueAsInt(0, "expires_in", 0), newsnippet_type,
                                    isgroup ?
                                    (groupsender > 0 ? getRecipientInfoFromMap(recipient_info, groupsender).display_name : "") :
                                    getRecipientInfoFromMap(recipient_info, rec_id).display_name,
                                    nullptr);
            if (snippet.empty())
              snippet = "(status message)";
          }
          else
            snippet = newsnippet_info(0, "body");
        }
      else
        snippet.clear();
    }

    long long int datetime = results.getValueAs<long long int>(i, "date");
    std::string date_date = bepaald::toDateString(datetime / 1000, "%b %d, %Y");
    //std::string date_time = bepaald::toDateString(datetime / 1000, "%R"); // does not work with mingw
    std::string date_time = bepaald::toDateString(datetime / 1000, "%H:%M");

    std::string raw_convo_url_path(isnotetoself ? "Note to Self" : getRecipientInfoFromMap(recipient_info, rec_id).display_name);
    WIN_LIMIT_FILENAME_LENGTH(raw_convo_url_path);
    std::string convo_url_path(sanitizeFilename(raw_convo_url_path, d_aggressive_filename_sanitizing) + " (_id" + bepaald::toString(t_id) + ")");
    if (compact) [[unlikely]]
    {
      raw_convo_url_path.clear();
      convo_url_path = "id" + bepaald::toString(t_id);
    }
    HTMLescapeUrl(&convo_url_path);

    std::string convo_url_location(sanitizeFilename(raw_convo_url_path, d_aggressive_filename_sanitizing) + ".html");
    if (compact) [[unlikely]]
      convo_url_location = "0.html";
    HTMLescapeUrl(&convo_url_location);

    if (convo_url_location == ".html") [[unlikely]]
    {
      Logger::error("Sanitized+url encoded name was empty. This should never happen. Original display_name: '",
                    getRecipientInfoFromMap(recipient_info, rec_id).display_name, "'");
      return false;
    }


    // if (t_id == 11)
    // {
    //   std::cout << "Snippet: " << snippet << "\n";
    //   if (isgroup && groupsender > 0)
    //     std::cout << "GROUPSEND: " + getRecipientInfoFromMap(recipient_info, groupsender).display_name << "\n";
    //   else
    //     std::cout << "isgroup: " << isgroup << "\n" << "groupsender: " << groupsender << "\n";
    // }

    outputfile <<
      "      <div class=\"conversation-list-item\">\n"
      "        <div class=\"avatar" <<
      (((hasavatar || !isgroup) && !isnotetoself) ? " avatar-" + bepaald::toString(rec_id) : "") <<
      ((isgroup && !hasavatar) ? " group-avatar-icon" : "") <<
      ((emoji_initial && !hasavatar) ? " avatar-emoji-initial" : "") <<
      (isnotetoself ? " note-to-self-icon" : "") << "\">\n"

      "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>\n" <<
      ((!hasavatar && !isgroup && !isnotetoself) ? "          <span>" + getRecipientInfoFromMap(recipient_info, rec_id).initial + "</span>\n" : "") <<
      "        </div>\n"
      "        <div class=\"name-and-snippet\">\n"
      "          <div class=\"name-and-status\">\n";
    if (isblocked)
      outputfile << "            <div class=\"blocked\"></div>\n";
    outputfile <<
      "            <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>\n"
      "            <pre class=\"name\">" << (isnotetoself ? "Note to Self" : HTMLescapeString(getRecipientInfoFromMap(recipient_info, rec_id).display_name)) << "</pre>\n";
    if (isreleasechannel || isnotetoself)
      outputfile << "            <div class=\"official\"></div>\n";
    if (ismuted)
      outputfile << "            <div class=\"muted\"></div>\n";
    outputfile <<
      "          </div>\n"
      "          <span class=\"snippet\">" <<
      ((isgroup && groupsender > 0 && !isstatusmsg) ? "<span class=\"groupsender\">" + HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupsender).display_name) + "</span>: " : "")
               << (isstatusmsg ? "<i>" : "") << snippet << (isstatusmsg ? "</i>" : "") << "</span>\n"
      "        </div>\n"
      "        <div class=\"index-date\">\n"
      "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>\n"
      "          <span>" << date_date << "</span>\n"
      "          <span>" << date_time << "</span>\n"
      "        </div>\n"
      "      </div>\n"
      "\n";
  }

  if (menuitems > 0 ||
      chatfolders.size() > 0 ||
      chatfolder_idx >= 0)
    outputfile
      << "    <div id=\"menu\">\n";

  if (menuitems > 1 ||
      chatfolders.size() > 0 ||
      (chatfolder_idx >= 0 && menuitems > 0)) // collapsible menu
    outputfile <<
      "         <div class=\"menu-item\">\n"
      "           <div class=\"expandable-menu-item\">\n"
      "             <div class=\"menu-header\">\n"
      "               <span id=\"hamburger-icon\"></span><span class=\"label-text\">menu</span>\n"
      "             </div>\n"
      "             <div id=\"expandedmenu\">\n"
      "\n";

  if (chatfolder_idx >= 0) // back to index if current page is a chatfolder
  {
    outputfile <<
      "      <a href=\"index.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon nav-up\"></div>\n"
      "          <div>\n"
      "            index\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }

  if (calllog)
  {
    outputfile <<
      "      <a href=\"calllog.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon calllog-icon\"></div>\n"
      "          <div>\n"
      "            call log\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }
  if (stickerpacks)
  {
    outputfile <<
      "      <a href=\"stickerpacks.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon stickerpacks-icon\"></div>\n"
      "          <div>\n"
      "            stickerpacks\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }

  if (blocked)
  {
    outputfile <<
      "      <a href=\"blockedlist.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon blocked-icon\"></div>\n"
      "          <div>\n"
      "            blocked contacts\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }

  if (fullcontacts)
  {
    outputfile <<
      "      <a href=\"fullcontactslist.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon fullcontacts-icon\"></div>\n"
      "          <div>\n"
      "            all known contacts\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }

  if (settings)
  {
    outputfile <<
      "      <a href=\"settings.html\">\n"
      "        <div class=\"menu-item\">\n"
      "          <div class=\"menu-icon settings-icon\"></div>\n"
      "          <div>\n"
      "            settings\n"
      "          </div>\n"
      "        </div>\n"
      "      </a>\n"
      "\n";
  }

  if (menuitems && chatfolders.size())
    outputfile <<
      "      <div class=\"menu-item menu-separator\">\n"
      "        <hr>\n"
      "      </div>\n"
      "\n";

  if (chatfolders.size())
    for (auto const &cf : chatfolders)
      outputfile <<
        "      <a href=\"" << HTMLescapeUrl(std::get<2>(cf)) << ".html\">\n"
        "        <div class=\"menu-item" << (std::get<0>(cf) == chatfolder_idx ? " current-menu-item" : "") <<  "\">\n"
        "          <div class=\"menu-icon chatfolders-icon\"></div>\n"
        "          <div>\n"
        "            " << HTMLescapeString(std::get<1>(cf)) << "\n"
        "          </div>\n"
        "        </div>\n"
        "      </a>\n"
        "\n";

  if (menuitems > 1 ||
      chatfolders.size() ||
      (chatfolder_idx >= 0 && menuitems > 0)) // collapsible menu closing tags
    outputfile <<
      "             </div>\n"
      "           </div>\n"
      "         </div>\n";

  if (menuitems > 0 ||
      chatfolders.size() ||
      chatfolder_idx >= 0)
    outputfile <<
      "    </div>\n";

  if (themeswitching || searchpage)
  {
    outputfile << "    <div id=\"theme\">\n";
    if (searchpage)
    {
      outputfile <<
        "      <div class=\"menu-item\">\n"
        "        <a href=\"searchpage.html\">\n"
        "          <span class=\"menu-icon searchbutton\">\n"
        "          </span>\n"
        "        </a>\n"
        "      </div>\n";
    }
    if (themeswitching)
    {
      outputfile <<
        "      <div class=\"menu-item\">\n"
        "        <label for=\"theme-switch\">\n"
        "          <span class=\"menu-icon themebutton\">\n"
        "          </span>\n"
        "        </label>\n"
        "      </div>\n";
    }
    outputfile << "    </div>\n";
  }
  outputfile <<
    "    </div>\n"
    "  </div>\n";

  if (!exportdetails.empty())
    outputfile << '\n' << exportdetails << '\n';

  if (themeswitching)
  {
    outputfile << R"(<script>
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
  </script>)";
  }

  outputfile <<
    "  </body>\n"
    "</html>\n";

  //for (auto const &[k, v] : chatfolders)
  //  std::cout << "'" << k << "' : " << (sanitizeFilename(v) + ".html") << std::endl;

  return true;
}
