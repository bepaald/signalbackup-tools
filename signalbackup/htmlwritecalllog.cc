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

void SignalBackup::HTMLwriteCallLog(std::vector<long long int> const &threads, std::string const &directory,
                                    std::string const &datewhereclause,
                                    std::map<long long int, RecipientInfo> *recipientinfo,
                                    long long int notetoself_tid [[maybe_unused]],
                                    bool overwrite, bool append, bool light, bool themeswitching,
                                    std::string const &exportdetails) const
{
  Logger::message("Writing calllog.html...");

  if (bepaald::fileOrDirExists(directory + "/calllog.html"))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", directory, "/calllog.html' exists. Use --overwrite to overwrite.");
      return;
    }
  }
  std::ofstream outputfile(directory + "/calllog.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", directory, "/calllog.html' for writing.");
    return;
  }

  // build string of requested threads
  std::string threadlist;
  for (unsigned int i = 0; i < threads.size(); ++i)
  {
    threadlist += bepaald::toString(threads[i]);
    if (i < threads.size() - 1)
      threadlist += ",";
  }
  SqliteDB::QueryResults results;
  if (d_database.containsTable("call"))
  {
    if (!d_database.exec("SELECT "
                         //"_id, "
                         "message_id, peer, type, direction, event, "
                         + (d_database.tableContainsColumn("call", "timestamp") ? "timestamp" :
                            "(SELECT " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE " + d_mms_table + "._id = call.message_id)") + " AS timestamp "
                         //", ringer, deletion_timestamp, "
                         //"datetime((timestamp / 1000), 'unixepoch', 'localtime') "
                         "FROM call WHERE "
                         "message_id IN (SELECT DISTINCT _id FROM " + d_mms_table + " WHERE thread_id IN (" + threadlist + ")) " +
                         datewhereclause + " "
                         "ORDER BY timestamp DESC",
                         &results))
    {
      Logger::error("Failed to query database for call data.");
      return;
    }
  }
  else
  {
    Logger::warning("Call table not found in database");
    return;
  }

  bool listempty = false;
  if (results.rows() == 0)
    listempty = true;

  //results.prettyPrint(d_truncate);

  /*
    CALL LOG:

    enum class Direction(private val code: Int) {
    INCOMING(0),
    OUTGOING(1);

    enum class Type(private val code: Int) {
    AUDIO_CALL(0),
    VIDEO_CALL(1),
    GROUP_CALL(3),
    AD_HOC_CALL(4);

    enum class Event(private val code: Int) {
    ONGOING(0), // 1:1 Calls only.
    ACCEPTED(1), // 1:1 and Group Calls
    NOT_ACCEPTED(2), // 1:1 Calls only.
    MISSED(3), // 1:1 and Group/Ad-Hoc Calls. Group calls: The remote ring has expired or was cancelled by the ringer.
    DELETE(4), // 1:1 and Group/Ad-Hoc Calls.
    GENERIC_GROUP_CALL(5), // Group/Ad-Hoc Calls only. Initial state.
    JOINED(6), // Group Calls: User has joined the group call.
    RINGING(7), // Group Calls: If a ring was requested by another user.
    DECLINED(8), // Group Calls: If you declined a ring.
    OUTGOING_RING(9); // Group Calls: If you are ringing a group.
  */

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%F %T") // %F an d%T do not work on minGW
  outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
             << " by signalbackup-tools (" << VERSIONDATE << "). "
             << "Input database version: " << d_databaseversion << ". -->" << '\n'
             << "<!DOCTYPE html>" << '\n'
             << "<html>" << '\n'
             << "  <head>" << '\n'
             << "    <meta charset=\"utf-8\">" << '\n'
             << "    <title>Signal call log</title>" << '\n';

  // STYLE
  outputfile << "    <style>" << '\n'
             << "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << '\n'
             << "        /* " << (light ? "light" : "dark") << "*/" << '\n'
             << "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << '\n'
             << "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n'
             << "        --conversationlistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n'
             << "        --conversationlist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << '\n'
             << "        --conversationlist-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n'
             << "        --avatar-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << '\n'
             << "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << '\n'
             << "        --icon-f: " << (light ? "brightness(0);" : "none;") << '\n'
             << "      }" << '\n'
             << '\n';

  if (themeswitching)
  {
    outputfile
      << "    :root[data-theme=\"" + (!light ? "light"s : "dark") + "\"] {" << '\n'
      << "        /* " << (!light ? "light" : "dark") << "*/" << '\n'
      << "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << '\n'
      << "        --body-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n'
      << "        --conversationlistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n'
      << "        --conversationlist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << '\n'
      << "        --conversationlist-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n'
      << "        --avatar-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << '\n'
      << "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << '\n'
      << "        --icon-f: " << (!light ? "brightness(0);" : "none;") << '\n'
      << "      }" << '\n';
  }
  outputfile
    << "      body {" << '\n'
    << "        margin: 0px;" << '\n'
    << "        padding: 0px;" << '\n'
    << "        width: 100%;" << '\n'
    << "        background-color: var(--body-bgc);" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      #theme-switch {" << '\n'
    << "        display: none;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      #page {" << '\n'
    << "        background-color: var(--body-bgc);" << '\n'
    << "        padding: 8px;" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: column;" << '\n'
    << "        transition: color .2s, background-color .2s;" << '\n'
    << "      }" << '\n'
    << '\n';

  if (!exportdetails.empty())
    outputfile
      << "      .export-details {" << '\n'
      << "        display: none;" << '\n'
      << "        grid-template-columns: repeat(2 , 1fr);" << '\n'
      << "        color: var(--body-c);" << '\n'
      << "        margin-left: auto;" << '\n'
      << "        margin-right: auto;" << '\n'
      << "        margin-bottom: 10px;" << '\n'
      << "        grid-gap: 0px 15px;" << '\n'
      << "        width: fit-content;" << '\n'
      << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << '\n'
      << "      }" << '\n'
      << "      .export-details-fullwidth {" << '\n'
      << "        text-align: center;" << '\n'
      << "        font-weight: bold;" << '\n'
      << "        grid-column: 1 / 3;" << '\n'
      << "      }" << '\n'
      << "      .export-details div:nth-child(odd of :not(.export-details-fullwidth)) {" << '\n'
      << "        text-align: right;" << '\n'
      << "        font-style: italic;" << '\n'
      << "      }" << '\n'
    << '\n';

  outputfile
    << "      #menu {" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: column;" << '\n'
    << "        position: fixed;" << '\n'
    << "        top: 20px;" << '\n'
    << "        left: 20px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      #menu a:link," << '\n'
    << "      #menu a:visited," << '\n'
    << "      #menu a:hover," << '\n'
    << "      #menu a:active {" << '\n'
    << "        color: #FFFFFF;" << '\n'
    << "        text-decoration: none;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .menu-icon {" << '\n'
    << "        margin-right: 0px;" << '\n'
    << "        width: 30px;" << '\n'
    << "        aspect-ratio: 1 / 1;" << '\n'
    << "        background-position: center;" << '\n'
    << "        background-repeat: no-repeat;" << '\n'
    << "        background-size: cover;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .menu-item {" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: row;" << '\n'
    << "        color: var(--menuitem-c);" << '\n'
    << "        align-items: center;" << '\n'
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << '\n'
    << "        padding: 5px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .menu-item > div {" << '\n'
    << "        margin-right: 5px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      #theme {" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: column;" << '\n'
    << "        position: fixed;" << '\n'
    << "        top: 20px;" << '\n'
    << "        right: 20px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .themebutton {" << '\n'
    << "        display: block;" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M11.5 7.75c0-0.4 0.34-0.77 0.78-0.74C14.9 7.15 17 9.33 17 12c0 2.67-2.09 4.85-4.72 5-0.44 0.02-0.78-0.34-0.78-0.75v-8.5Z\"/><path d=\"M12.97 0.73c-0.53-0.53-1.4-0.53-1.94 0L8.39 3.38H4.75c-0.76 0-1.37 0.61-1.37 1.37v3.64l-2.65 2.64c-0.53 0.53-0.53 1.4 0 1.94l2.65 2.64v3.64c0 0.76 0.61 1.38 1.37 1.38h3.64l2.64 2.64c0.53 0.53 1.4 0.53 1.94 0l2.64-2.63 3.64-0.01c0.76 0 1.38-0.62 1.38-1.38v-3.64l2.63-2.64c0.54-0.53 0.54-1.4 0-1.94l-2.62-2.61-0.01-3.67c0-0.76-0.62-1.38-1.38-1.38h-3.64l-2.64-2.64Zm-3.45 4L12 2.22l2.48 2.5c0.26 0.25 0.61 0.4 0.98 0.4h3.42v3.45c0.01 0.36 0.16 0.71 0.41 0.97L21.76 12l-2.48 2.48c-0.26 0.26-0.4 0.61-0.4 0.98v3.42h-3.43c-0.36 0.01-0.7 0.15-0.96 0.4L12 21.77l-2.48-2.48c-0.26-0.26-0.61-0.4-0.98-0.4H5.13v-3.42c0-0.37-0.15-0.72-0.4-0.98L2.22 12l2.5-2.48c0.25-0.26 0.4-0.61 0.4-0.98V5.13h3.41c0.37 0 0.72-0.15 0.98-0.4Z\"/></g></svg>');" << '\n'
    << "        filter: var(--icon-f);" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .nav-up {" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"/></svg>');" << '\n'
    << "        filter: var(--icon-f);" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .conversation-list-header {" << '\n'
    << "        text-align: center;" << '\n'
    << "        font-size: xx-large;" << '\n'
    << "        color: var(--conversationlistheader-c);" << '\n'
    << "        padding: 10px;" << '\n'
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .conversation-list {" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: column;" << '\n'
    << "        width: fit-content;" << '\n'
    << "        margin-top: 10px;" << '\n'
    << "        margin-bottom: 100px;" << '\n'
    << "        margin-right: auto;" << '\n'
    << "        margin-left: auto;" << '\n'
    << "        padding: 30px;" << '\n'
    << "        background-color: var(--conversationlist-bc);" << '\n'
    << "        color: var(--conversationlist-c);" << '\n'
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << '\n'
    << "        border-radius: 10px;" << '\n'
    << "        min-width: 503px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .conversation-list-item {" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: row;" << '\n'
    << "        padding: 10px;" << '\n'
    << "        margin: auto;" << '\n'
    << "        justify-content: center;" << '\n'
    << "        align-items: center;" << '\n'
    << "        align-content: center;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .avatar {" << '\n'
    << "        position: relative;" << '\n'
    << "        display: flex;" << '\n'
    << "        border-radius: 50%;" << '\n'
    << "        width: 60px;" << '\n'
    << "        height: 60px;" << '\n'
    << "        line-height: 60px;" << '\n'
    << "        text-align: center;" << '\n'
    << "        justify-content: center;" << '\n'
    << "        font-size: 38px;" << '\n'
    << "        color: var(--avatar-c);" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .avatar-emoji-initial {" << '\n'
    << "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .note-to-self-icon {" << '\n'
    << "        background: #315FF4;" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"80\" height=\"80\" viewBox=\"0 0 80 80\" fill=\"white\"><path d=\"M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z\"/></svg>');" << '\n'
    << "        background-position: center;" << '\n'
    << "        background-repeat: no-repeat;" << '\n'
    << "        background-size: 80%;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .group-avatar-icon {" << '\n'
    << "        background: #315FF4;" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" idth=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"/></svg>');" << '\n'
    << "        background-position: center;" << '\n'
    << "        background-repeat: no-repeat;" << '\n'
    << "        background-size: 80%;" << '\n'
    << "      }" << '\n'
    << '\n';

  std::set<long long int> peers;
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    long long int peer = results.valueAsInt(i, "peer");
    if (peer > -1 && !bepaald::contains(peers, peer))
    {
      if (getRecipientInfoFromMap(recipientinfo, peer).hasavatar)
      {
        long long int threadid = d_database.getSingleResultAs<long long int>("SELECT thread_id FROM " + d_mms_table + " WHERE _id = ?", results(i, "message_id"), -1);
        if (threadid == -1)
          continue;
        std::string avatar_path = (sanitizeFilename(getRecipientInfoFromMap(recipientinfo, peer).display_name + " (_id" + bepaald::toString(threadid) + ")"));
        bepaald::replaceAll(&avatar_path, '\"', R"(\")");
        std::string avatar_extension = getAvatarExtension(peer);

        // actually write avatar! (it may not exist....)

        outputfile
          << "      .avatar-" << peer << " {" << '\n'
          << "        background-image: url(\"" << avatar_path << "/media/Avatar_" << peer << "." << avatar_extension << "\");" << '\n'
          << "        background-position: center;" << '\n'
          << "        background-repeat: no-repeat;" << '\n'
          << "        background-size: cover;" << '\n'
          << "      }" << '\n'
          << '\n';
      }
      else
      {
        if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", peer, -1) == 0)   // no avatar, no group
        {
          outputfile
            << "      .avatar-" << peer << " {" << '\n'
            << "        background: #" << getRecipientInfoFromMap(recipientinfo, peer).color << ";" << '\n'
            << "      }" << '\n'
            << '\n';
        }
      }
      peers.insert(peer);
    }
  }

  outputfile
    << "      .name-and-snippet {" << '\n'
    << "        position: relative;" << '\n'
    << "        display: flex;" << '\n'
    << "        flex-direction: column;" << '\n'
    << "        padding-left: 30px;" << '\n'
    << "        justify-content: center;" << '\n'
    << "        align-content: center;" << '\n'
    << "        width: 350px;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .name {" << '\n'
    << "        font-weight: bold;" << '\n'
    << "        font-size: 18px;" << '\n'
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << '\n'
    << "        margin: 0px;" << '\n'
    << "        padding: 0px;" << '\n'
    << "      }" << '\n'
    << '\n'
    // << "      .groupsender {" << '\n'
    // << "        font-weight: 500;" << '\n'
    // << "      }" << '\n'
    // << '\n'
    // << "      .snippet {" << '\n'
    // << "        display: -webkit-box;" << '\n'
    // << "        -webkit-line-clamp: 2;" << '\n'
    // << "/*        line-clamp: 2; This is still in working draft, though the vendor extension version is well supported */" << '\n'
    // << "        -webkit-box-orient: vertical;" << '\n'
    // << "/*        box-orient: vertical; */" << '\n'
    // << "        overflow: hidden;" << '\n'
    // << "        text-overflow: ellipsis;" << '\n'
    // << "      }" << '\n'
    // << '\n'
    << "      .snippet2 {" << '\n'
    << "        display: flex;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .snippet-missed {" << '\n'
    << "        color: red;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .calltype-video-icon {" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 16 16\" stroke=\"none\" fill=\"white\"><path d=\"M14.23,4.16a1.23,1.23 0,0 0,-1.36 0.27L11,6.29L11,4.75A1.76,1.76 0,0 0,9.25 3L2.75,3A1.76,1.76 0,0 0,1 4.75v6.5A1.76,1.76 0,0 0,2.75 13h6.5A1.76,1.76 0,0 0,11 11.25L11,9.71l1.87,1.86a1.23,1.23 0,0 0,0.88 0.37,1.18 1.18,0 0,0 0.48,-0.1A1.23,1.23 0,0 0,15 10.69L15,5.31A1.23,1.23 0,0 0,14.23 4.16ZM10,11.25a0.76,0.76 0,0 1,-0.75 0.75L2.75,12A0.76,0.76 0,0 1,2 11.25L2,4.75A0.76,0.76 0,0 1,2.75 4h6.5a0.76,0.76 0,0 1,0.75 0.75ZM14,10.69a0.25,0.25 0,0 1,-0.15 0.23,0.26 0.26,0 0,1 -0.28,-0.05L11,8.29L11,7.71l2.57,-2.58a0.26,0.26 0,0 1,0.28 0,0.25 0.25,0 0,1 0.15,0.23Z\"/></svg>');" << '\n'
    << "        filter: var(--icon-f);" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .calltype-audio-icon {" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M17.21 22a8.08 8.08 0 0 1-2.66-.51 20.79 20.79 0 0 1-7.3-4.73 21 21 0 0 1-4.74-7.3c-.78-2.22-.67-4 .35-5.45h0a5 5 0 0 1 2-1.67 2.72 2.72 0 0 1 3.51.81l2.11 3a2.69 2.69 0 0 1-.35 3.49l-.93.85c-.09.08-.15.22-.08.31A20 20 0 0 0 11 13a20 20 0 0 0 2.21 1.91.24.24 0 0 0 .3-.08l.85-.93a2.68 2.68 0 0 1 3.49-.35l3 2.11a2.68 2.68 0 0 1 .85 3.43 5.22 5.22 0 0 1-1.71 2 4.69 4.69 0 0 1-2.78.91zM4.09 4.87c-.46.64-1 1.77-.16 4.08a19.28 19.28 0 0 0 4.38 6.74A19.49 19.49 0 0 0 15 20.07c2.31.81 3.44.3 4.09-.16a3.55 3.55 0 0 0 1.2-1.42A1.21 1.21 0 0 0 20 16.9l-3-2.12a1.18 1.18 0 0 0-1.53.15l-.82.9a1.72 1.72 0 0 1-2.33.29 21.9 21.9 0 0 1-2.37-2.05 22.2 22.2 0 0 1-2-2.37 1.71 1.71 0 0 1 .3-2.32l.89-.82A1.19 1.19 0 0 0 9.21 7L7.1 4a1.19 1.19 0 0 0-1.51-.38 3.72 3.72 0 0 0-1.5 1.25z\"/></svg>');" << '\n'
    << "        filter: var(--icon-f);" << '\n'
    << "    }" << '\n'
    << '\n'
    << "    .calltype-video-icon," << '\n'
    << "    .calltype-audio-icon {" << '\n'
    << "      display: inline-block;" << '\n'
    << "      height: 35px;" << '\n'
    << "      aspect-ratio: 1 / 1;" << '\n'
    << "      margin-right: 8px;" << '\n'
    << "      top: 2px;" << '\n'
    << "      position: relative;" << '\n'
    << "    }" << '\n'
    << '\n'
    << "    .callstatus-group-icon {" << '\n'
    << "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M13 7.148c0-2.133 1.434-4.023 3.5-4.023S20 5.015 20 7.148c0 1.082-.353 2.11-.952 2.885s-1.49 1.342-2.548 1.342-1.949-.568-2.548-1.342S13 8.23 13 7.148zm3.5-2.273c-.833 0-1.75.817-1.75 2.273 0 .713.235 1.36.586 1.814s.771.663 1.164.663.813-.21 1.164-.663.586-1.1.586-1.814c0-1.456-.917-2.273-1.75-2.273z\" fill-rule=\"evenodd\"/><path d=\"M7.5 12.625c1.115 0 2.186.242 3.136.676a7.726 7.726 0 0 0-1.237 1.382 5.887 5.887 0 0 0-1.899-.308c-2.675 0-4.72 1.705-5.072 3.75h5.723a7.34 7.34 0 0 0 .061 1.75H1.757A1.128 1.128 0 0 1 .625 18.75c0-3.489 3.191-6.125 6.875-6.125z\"/><path d=\"M16.5 12.625c-3.684 0-6.875 2.636-6.875 6.125 0 .654.54 1.125 1.132 1.125h11.486c.593 0 1.132-.471 1.132-1.125 0-3.489-3.191-6.125-6.875-6.125zm0 1.75c2.675 0 4.72 1.705 5.072 3.75H11.428c.351-2.045 2.397-3.75 5.072-3.75zm-9-11.25c-2.066 0-3.5 1.89-3.5 4.023 0 1.082.353 2.11.952 2.885s1.49 1.342 2.548 1.342 1.949-.568 2.548-1.342S11 8.23 11 7.148c0-2.133-1.434-4.023-3.5-4.023zM5.75 7.148c0-1.456.917-2.273 1.75-2.273s1.75.817 1.75 2.273c0 .713-.235 1.36-.586 1.814s-.771.663-1.164.663-.813-.21-1.164-.663-.586-1.1-.586-1.814z\" fill-rule=\"evenodd\"/></svg>');" << '\n'
    << "      filter: var(--icon-f);" << '\n'
    << "    }" << '\n'
    << "    .callstatus-missed-icon {" << '\n'
    << "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"red\" stroke=\"red\" stroke-width=\"0.5\"><path d=\"M4.13 9.61v6.14a.89.89 0 0 1-.88.88.89.89 0 0 1-.88-.88v-8a.89.89 0 0 1 .88-.88h8a.89.89 0 0 1 .88.88.89.89 0 0 1-.88.88H5.11l1.5 1.25 6.14 6.13 8.63-8.63a.88.88 0 0 1 1.24 0 .88.88 0 0 1 0 1.24l-8.72 8.72c-.64.63-1.66.63-2.3 0l-6.22-6.22-1.25-1.5z\"/></svg>');" << '\n'
    << "    }" << '\n'
    << "    .callstatus-incoming-icon {" << '\n'
    << "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\" stroke-width=\"0.5\"><path d=\"M18.37 6.87l-8.25 8.25-1.5 1.26h7.63a.88.88 0 0 1 .88.87.89.89 0 0 1-.88.88h-9.5a.89.89 0 0 1-.88-.88v-9.5a.89.89 0 0 1 .88-.88.89.89 0 0 1 .88.88v7.64l1.25-1.5 8.25-8.26a.88.88 0 0 1 1.24 0 .88.88 0 0 1 0 1.24z\"/></svg>');" << '\n'
    << "      filter: var(--icon-f);" << '\n'
    << "    }" << '\n'
    << "    .callstatus-outgoing-icon {" << '\n'
    << "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\" stroke-width=\"0.5\"><path d=\"M16.38 8.61v7.64a.88.88 0 0 0 .87.88.89.89 0 0 0 .88-.88v-9.5a.89.89 0 0 0-.88-.88h-9.5a.89.89 0 0 0-.88.88.89.89 0 0 0 .88.88h7.64l-1.5 1.25-8.26 8.25a.88.88 0 0 0 0 1.24.88.88 0 0 0 1.24 0l8.25-8.25 1.26-1.5z\"/></svg>');" << '\n'
    << "      filter: var(--icon-f);" << '\n'
    << "    }" << '\n'
    << '\n'
    << "    .callstatus-missed-icon," << '\n'
    << "    .callstatus-incoming-icon," << '\n'
    << "    .callstatus-outgoing-icon," << '\n'
    << "    .callstatus-group-icon {" << '\n'
    << "      display: inline-block;" << '\n'
    << "      height: 22px;" << '\n'
    << "      aspect-ratio: 1 / 1;" << '\n'
    << "      margin-right: 8px;" << '\n'
    << "      position: relative;" << '\n'
    << "    }" << '\n'
    << '\n'
    << "    .middot {" << '\n'
    << "      font-weight: bold;" << '\n'
    << "      margin-left: 5px;" << '\n'
    << "      margin-right: 5px;" << '\n'
    << "    }" << '\n'
    << '\n';

  outputfile
    << "      @media print {" << '\n'
    << "        .conversation-list-header {" << '\n'
    << "          padding: 0;" << '\n'
    << "        }" << '\n'
    << '\n'
    << "        .conversation-list-item {" << '\n'
    << "          break-inside: avoid;" << '\n'
    << "        }" << '\n'
    << '\n'
    << "        .conversation-list {" << '\n'
    << "          margin: 0 auto;" << '\n'
    << "          display: block;" << '\n'
    << "          border-radius: 0;" << '\n'
    << "        }" << '\n'
    << '\n'
    << "        .avatar {" << '\n'
    << "          -webkit-print-color-adjust: exact;" << '\n'
    << "          color-adjust: exact;" << '\n'
    << "          print-color-adjust: exact;" << '\n'
    << "          flex-shrink: 0;" << '\n'
    << "        }" << '\n'
    << '\n';

  if (!exportdetails.empty())
    outputfile
      << "        .export-details {" << '\n'
      << "          display: grid;" << '\n'
      << "        }" << '\n'
      << '\n';

  outputfile
    << "        #menu {" << '\n'
    << "          display: none;" << '\n'
    << "        }" << '\n'
    << '\n'
    << "        #theme {" << '\n'
    << "          display: none;" << '\n'
    << "        }" << '\n'
    << '\n'
    << "        .callstatus-group-icon, .callstatus-missed-icon," << '\n'
    << "        .callstatus-incoming-icon, .callstatus-outgoing-icon, " << '\n'
    << "        .calltype-audio-icon, .calltype-video-icon {" << '\n'
    << "          -webkit-print-color-adjust: exact;" << '\n'
    << "          print-color-adjust: exact;" << '\n'
    << "          filter: brightness(0.5);" << '\n'
    << "        }" << '\n'
    << "      }" << '\n'
    << '\n';

  outputfile
    << "    </style>" << '\n'
    << "  </head>" << '\n';

  // BODY
  outputfile
    << "  <body>" << '\n';
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
  outputfile
    << '\n'
    << "    <input type=\"checkbox\" id=\"theme-switch\">" << '\n'
    << '\n'
    << "    <div id=\"page\">" << '\n'
    << '\n'
    << "      <div class=\"conversation-list-header\">" << '\n'
    << "        Signal call log" << '\n'
    << "      </div>" << '\n'
    << '\n'
    << "      <div class=\"conversation-list\">" << '\n'
    << '\n';
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    long long int peer = results.valueAsInt(i, "peer");
    if (peer == -1)
      continue;

    bool isgroup = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", peer, -1) == 1;
    bool hasavatar = getRecipientInfoFromMap(recipientinfo, peer).hasavatar;
    bool emoji_initial = getRecipientInfoFromMap(recipientinfo, peer).initial_is_emoji;
    long long int datetime = results.getValueAs<long long int>(i, "timestamp");
    std::string date_date = bepaald::toDateString(datetime / 1000, "%H:%M %b %d, %Y");
    long long int type = results.valueAsInt(i, "type");
    long long int event = results.valueAsInt(i, "event");
    long long int direction = results.valueAsInt(i, "direction");

    outputfile
      << "        <div class=\"conversation-list-item\">" << '\n'
      << "          <div class=\"avatar"
      << ((hasavatar || !isgroup) ? " avatar-" + bepaald::toString(peer) : "")
      << ((isgroup && !hasavatar) ? " group-avatar-icon" : "")
      << ((emoji_initial && !hasavatar) ? " avatar-emoji-initial" : "") << "\">" << '\n'
      << ((!hasavatar && !isgroup) ? "            <span>" + getRecipientInfoFromMap(recipientinfo, peer).initial + "</span>\n" : "")
      << "          </div>" << '\n'
      << "          <div class=\"name-and-snippet\">" << '\n'
      << "            <pre class=\"name\">" << HTMLescapeString(getRecipientInfoFromMap(recipientinfo, peer).display_name) << "</pre>" << '\n'
      << "            <div class=\"snippet2" << (event == 3 /*missed*/? " snippet-missed" : "") << "\">" << '\n';

    /*
      to display the correct status icon and text, 'messagetype' is used. But while it's value is set to
      an existing message type, it is not taken from the actual message table (message.type), but set
      as follows:
     */
    long long int messagetype;
    if (type == 3) // 'GROUP_CALL'
      messagetype = Types::GROUP_CALL_TYPE;
    else if (direction == 0 /* incoming */ && event == 3 /* missed */)
      messagetype = (type == 1 /* VIDEO_CALL */ ? Types::MISSED_VIDEO_CALL_TYPE : Types::MISSED_AUDIO_CALL_TYPE);
    else if (direction == 0) // incoming
      messagetype = (type == 1 /* VIDEO_CALL */ ? Types::INCOMING_VIDEO_CALL_TYPE : Types::INCOMING_AUDIO_CALL_TYPE);
    else // outgoing
      messagetype = (type == 1 /* VIDEO_CALL */ ? Types::OUTGOING_VIDEO_CALL_TYPE : Types::OUTGOING_AUDIO_CALL_TYPE);

    // output status icon
    if (messagetype == Types::MISSED_VIDEO_CALL_TYPE || messagetype == Types::MISSED_AUDIO_CALL_TYPE)
      outputfile << "              <div class=\"callstatus-missed-icon\"></div>" << '\n';
    else if (messagetype == Types::INCOMING_AUDIO_CALL_TYPE || messagetype == Types::INCOMING_VIDEO_CALL_TYPE)
      outputfile << "              <div class=\"callstatus-incoming-icon\"></div>" << '\n';
    else if (messagetype == Types::OUTGOING_AUDIO_CALL_TYPE || messagetype == Types::OUTGOING_VIDEO_CALL_TYPE)
      outputfile << "              <div class=\"callstatus-outgoing-icon\"></div>" << '\n';
    else if (messagetype == Types::GROUP_CALL_TYPE)
    {
      if (event == 3) // missed
        outputfile << "              <div class=\"callstatus-missed-icon\"></div>" << '\n';
      else if (event == 5 || event == 6) // 'generic group call', 'joined'
        outputfile << "              <div class=\"callstatus-group-icon\"></div>" << '\n';
      else if (direction == 0) // incoming
        outputfile << "              <div class=\"callstatus-incoming-icon\"></div>" << '\n';
      else if (direction == 1) // outgoing
        outputfile << "              <div class=\"callstatus-outgoing-icon\"></div>" << '\n';
    }

    outputfile
      << "              <div>";

    // output status text
    if (messagetype == Types::MISSED_VIDEO_CALL_TYPE || messagetype == Types::MISSED_AUDIO_CALL_TYPE)
      outputfile << "Missed";
    else if (messagetype == Types::INCOMING_AUDIO_CALL_TYPE || messagetype == Types::INCOMING_VIDEO_CALL_TYPE)
      outputfile << "Incoming";
    else if (messagetype == Types::OUTGOING_AUDIO_CALL_TYPE || messagetype == Types::OUTGOING_VIDEO_CALL_TYPE)
      outputfile << "Outgoing";
    else if (messagetype == Types::GROUP_CALL_TYPE)
    {
      if (event == 3) // missed
        outputfile << "Missed";
      else if (event == 5 || event == 6) // 'generic group call', 'joined'
        outputfile << "Group call";
      else if (direction == 0) // incoming
        outputfile << "Incoming";
      else if (direction == 1) // outgoing
        outputfile << "Outgoing";
    }

    outputfile << "<span class=\"middot\">&middot;</span>" << date_date << "</div>" << '\n'
               << "            </div>" << '\n'
               << "          </div>" << '\n';
    if (type == 0) // 'audio call'
      outputfile << "          <div class=\"calltype-audio-icon\"></div>" << '\n';
    else if (type == 1 || type == 3 || type == 4) // 'video'/'group'/'ad hoc'
      outputfile << "          <div class=\"calltype-video-icon\"></div>" << '\n';
    outputfile
      << "        </div>" << '\n'
      << '\n';
  }

  if (listempty)
  {
    outputfile
      << "        <div class=\"conversation-list-item\">" << '\n'
      << "            <span style=\"font-weight: bold; font-size: 18px\">(none)</span>" << '\n'
      << "        </div>" << '\n';
  }


  outputfile
    << "        <div id=\"menu\">" << '\n'
    << "          <a href=\"index.html\">" << '\n'
    << "            <div class=\"menu-item\">" << '\n'
    << "              <div class=\"menu-icon nav-up\">" << '\n'
    << "              </div>" << '\n'
    << "              <div>" << '\n'
    << "                index" << '\n'
    << "              </div>" << '\n'
    << "            </div>" << '\n'
    << "          </a>" << '\n'
    << "        </div>" << '\n'
    << '\n';

  if (themeswitching)
  {
    outputfile
      << "      <div id=\"theme\">" << '\n'
      << "        <div class=\"menu-item\">" << '\n'
      << "          <label for=\"theme-switch\">" << '\n'
      << "            <span class=\"menu-icon themebutton\">" << '\n'
      << "           </span>" << '\n'
      << "         </label>" << '\n'
      << "        </div>" << '\n'
      << "      </div>" << '\n'
      << '\n';
  }
  outputfile
    << "    </div>" << '\n'
    << "  </div>" << '\n';

  if (!exportdetails.empty())
    outputfile << '\n' << exportdetails << '\n';

  if (themeswitching)
  {
    outputfile << R"(  <script>
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

  // END
  outputfile
    << "  </body>" << '\n'
    << "</html>" << '\n';
}
