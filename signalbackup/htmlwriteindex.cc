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

void SignalBackup::HTMLwriteIndex(std::vector<long long int> const &threads, std::string const &directory,
                                  std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid,
                                  bool calllog, bool searchpage, bool stickerpacks, bool blocked, bool settings,  bool overwrite,
                                  bool append, bool light, bool themeswitching, std::string const &exportdetails) const
{

  Logger::message("Writing index.html...");

  if (bepaald::fileOrDirExists(directory + "/index.html"))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", directory, "/index.html' exists. Use --overwrite to overwrite.");
      return;
    }
  }
  std::ofstream outputfile(directory + "/index.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", directory, "/index.html' for writing.");
    return;
  }

  // build string of requested threads
  std::string threadlist;
  for (uint i = 0; i < threads.size(); ++i)
  {
    threadlist += bepaald::toString(threads[i]);
    if (i < threads.size() - 1)
      threadlist += ",";
  }

  int collapsiblemenu = 0;
  for (auto const &o : {blocked, stickerpacks, calllog, settings})
    if (o)
      ++collapsiblemenu;

  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT "
                       "thread._id, "
                       "thread." + d_thread_recipient_id + ", "
                       "thread.snippet, "
                       "thread.snippet_type, "
                       "IFNULL(thread.date, 0) AS date, "
                       "json_extract(thread.snippet_extras, '$.individualRecipientId') AS 'group_sender_id', "
                       + (d_database.tableContainsColumn("thread", "pinned") ? "pinned," : "") +
                       + (d_database.tableContainsColumn("thread", "archived") ? "archived," : "") +
                       "recipient.group_id, "
                       "recipient.blocked, "
                       "(SELECT COUNT(" + d_mms_table + "._id) FROM " + d_mms_table + " WHERE " + d_mms_table + ".thread_id = thread._id) AS message_count "
                       "FROM thread "
                       "LEFT JOIN recipient ON recipient._id IS thread." + d_thread_recipient_id + " "
                       "WHERE thread._id IN (" + threadlist + ") AND " + d_thread_message_count + " > 0 ORDER BY "
                       + (d_database.tableContainsColumn("thread", "pinned") ? "(pinned != 0) DESC, " : "") +
                       + (d_database.tableContainsColumn("thread", "archived") ? "archived ASC, " : "") +
                       "date DESC", &results)) // order by pinned DESC archived ASC date DESC??
  {
    Logger::error("Failed to query database for thread snippets.");
    return;
  }
  //results.prettyPrint();

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%F %T") // %F an d%T do not work on minGW
  outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
             << " by signalbackup-tools (" << VERSIONDATE << "). "
             << "Input database version: " << d_databaseversion << ". -->" << std::endl
             << "<!DOCTYPE html>" << std::endl
             << "<html>" << std::endl
             << "  <head>" << std::endl
             << "    <meta charset=\"utf-8\">" << std::endl
             << "    <title>Signal conversation list</title>" << std::endl
             << "    <style>" << std::endl
             << "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl
             << "        /* " << (light ? "light" : "dark") << " */" << std::endl
             << "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << std::endl
             << "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
             << "        --conversationlistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
             << "        --conversationlist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << std::endl
             << "        --conversationlist-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
             << "        --avatar-c: " << (light ? "#FFFFFF;" : "#FFFFFF;") << std::endl
             << "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
             << "        --icon-f: " << (light ? "brightness(0);" : "none;") << std::endl
             << "      }" << std::endl
             << std::endl;

  if (themeswitching)
  {
    outputfile
      << "    :root[data-theme=\"" + (!light ? "light"s : "dark") + "\"] {" << std::endl
      << "        /* " << (!light ? "light" : "dark") << " */" << std::endl
      << "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << std::endl
      << "        --body-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
      << "        --conversationlistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
      << "        --conversationlist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << std::endl
      << "        --conversationlist-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
      << "        --avatar-c: " << (!light ? "#FFFFFF;" : "#FFFFFF;") << std::endl
      << "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
      << "        --icon-f: " << (!light ? "brightness(0);" : "none;") << std::endl
      << "      }" << std::endl;
  }

  outputfile
    << "      body {" << std::endl
    << "        margin: 0px;" << std::endl
    << "        padding: 0px;" << std::endl
    << "        width: 100%;" << std::endl
    << "        background-color: var(--body-bgc);" << std::endl
    << "      }" << std::endl
    << "" << std::endl;

  outputfile
    << "      #theme-switch {" << std::endl
    << "        display: none;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      #page {" << std::endl
    << "        background-color: var(--body-bgc);" << std::endl
    << "        padding: 8px;" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        transition: color .2s, background-color .2s;" << std::endl
    << "      }" << std::endl
    << std::endl;

  if (!exportdetails.empty())
    outputfile
      << "      .export-details {" << std::endl
      << "        display: none;" << std::endl
      << "        grid-template-columns: repeat(2 , 1fr);" << std::endl
      << "        color: var(--body-c);" << std::endl
      << "        margin-left: auto;" << std::endl
      << "        margin-right: auto;" << std::endl
      << "        margin-bottom: 10px;" << std::endl
      << "        grid-gap: 0px 15px;" << std::endl
      << "        width: fit-content;" << std::endl
      << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
      << "      }" << std::endl
      << "      .export-details-fullwidth {" << std::endl
      << "        text-align: center;" << std::endl
      << "        font-weight: bold;" << std::endl
      << "        grid-column: 1 / 3;" << std::endl
      << "      }" << std::endl
      << "      .export-details div:nth-child(odd of :not(.export-details-fullwidth)) {" << std::endl
      << "        text-align: right;" << std::endl
      << "        font-style: italic;" << std::endl
      << "      }" << std::endl
    << std::endl;

  outputfile
    << "      .conversation-list-header {" << std::endl
    << "        text-align: center;" << std::endl
    << "        font-size: xx-large;" << std::endl
    << "        color: var(--conversationlistheader-c);" << std::endl
    << "        padding: 10px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .header {" << std::endl
    << "        margin-top: 5px;" << std::endl
    << "        margin-bottom: 5px;" << std::endl
    << "        margin-left: 10px;" << std::endl
    << "        font-weight: bold;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .conversation-list {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        width: fit-content;" << std::endl
    << "        margin-top: 10px;" << std::endl
    << "        margin-bottom: 100px;" << std::endl
    << "        margin-right: auto;" << std::endl
    << "        margin-left: auto;" << std::endl
    << "        padding: 30px;" << std::endl
    << "        background-color: var(--conversationlist-bc);" << std::endl
    << "        color: var(--conversationlist-c);" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        border-radius: 10px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .conversation-list-item {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "        padding: 10px;" << std::endl
    << "        margin: auto;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-items: center;" << std::endl
    << "        align-content: center;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .avatar {" << std::endl
    << "        position: relative;" << std::endl
    << "        display: flex;" << std::endl
    << "        border-radius: 50%;" << std::endl
    << "        width: 60px;" << std::endl
    << "        height: 60px;" << std::endl
    << "        line-height: 60px;" << std::endl
    << "        text-align: center;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        font-size: 38px;" << std::endl
    << "        color: var(--avatar-c);" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .avatar-emoji-initial {" << std::endl
    << "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .note-to-self-icon {" << std::endl
    << "        background: #315FF4;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"80\" height=\"80\" viewBox=\"0 0 80 80\" fill=\"white\"><path d=\"M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z\"></path></svg>');" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: 75%;" << std::endl
    << "      }" << std::endl
    << "" << std::endl
    << "      .group-avatar-icon {" << std::endl
    << "        background: #315FF4;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"></path></svg>');" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: 80%;" << std::endl
    << "      }" << std::endl
    << "" << std::endl;

  for (uint i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, d_thread_recipient_id);
    if (rec_id == -1) [[unlikely]]
    {
      Logger::warning("Failed to get thread recipient id. Skipping.");
      continue;
    }

    if (getRecipientInfoFromMap(recipient_info, rec_id).hasavatar)
    {
      std::string avatar_path = (results.getValueAs<long long int>(i, "_id") == note_to_self_tid ?
                                 "Note to self (_id" + results(i, "_id") + ")" :
                                 sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name + " (_id" + results(i, "_id") + ")"));
      bepaald::replaceAll(&avatar_path, '\"', R"(\")");

      outputfile
        << "      .avatar-" << rec_id << " {" << std::endl
        << "        background-image: url(\"" << avatar_path << "/media/Avatar_" << rec_id << ".bin\");" << std::endl
        << "        background-position: center;" << std::endl
        << "        background-repeat: no-repeat;" << std::endl
        << "        background-size: cover;" << std::endl
        << "      }" << std::endl
        << "" << std::endl;
    }
    else if (results.isNull(i, "group_id")) // no avatar, no group
    {
      outputfile
        << "      .avatar-" << rec_id << " {" << std::endl
        << "        background: #" << getRecipientInfoFromMap(recipient_info, rec_id).color << ";" << std::endl
        << "      }" << std::endl
        << "" << std::endl;
    }
  }

  outputfile
    << "      .name-and-snippet {" << std::endl
    << "        position: relative;" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        padding-left: 30px;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-content: center;" << std::endl
    << "        width: 350px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .name-and-status {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .blocked {" << std::endl
    << "         background-image: url('data:image/svg+xml;utf-8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\" stroke-width=\"1\" ><path d=\"M12 1a11 11 0 1 0 11 11A11 11 0 0 0 12 1zm0 1.5a9.448 9.448 0 0 1 6.159 2.281L4.781 18.159A9.488 9.488 0 0 1 12 2.5zm0 19a9.448 9.448 0 0 1-6.159-2.281L19.219 5.841A9.488 9.488 0 0 1 12 21.5z\"></path></svg>');" << std::endl
    << "        filter: var(--icon-f);" << std::endl
    << "        display: inline-block;" << std::endl
    << "        height: 18px;" << std::endl
    << "        aspect-ratio: 1 / 1;" << std::endl
    << "        margin-right: 8px;" << std::endl
    << "        margin-top: 3px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .name {" << std::endl
    << "        font-weight: bold;" << std::endl
    << "        font-size: 18px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        margin: 0px;" << std::endl
    << "        padding: 0px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .groupsender {" << std::endl
    << "        font-weight: 500;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .snippet {" << std::endl
    << "        display: -webkit-box;" << std::endl
    << "        -webkit-line-clamp: 2;" << std::endl
    << "/*        line-clamp: 2; This is still in working draft, though the vendor extension version is well supported */" << std::endl
    << "        -webkit-box-orient: vertical;" << std::endl
    << "/*        box-orient: vertical; */" << std::endl
    << "        overflow: hidden;" << std::endl
    << "        text-overflow: ellipsis;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .index-date {" << std::endl
    << "        position: relative;" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        padding-left: 20px;" << std::endl
    << "        font-size: small;" << std::endl
    << "      /*font-style: italic;*/" << std::endl
    << "        text-align: right;" << std::endl
    << "        max-width: 100px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .main-link::before {" << std::endl
    << "        content: \" \";" << std::endl
    << "        position: absolute;" << std::endl
    << "        top: 0;" << std::endl
    << "        left: 0;" << std::endl
    << "        width: 100%;" << std::endl
    << "        height: 100%;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .menu-item > div {" << std::endl
    << "        margin-right: 5px;" << std::endl
    << "      }" << std::endl
    << std::endl;
  if (calllog || stickerpacks || blocked || settings)
  {
    outputfile
      << "      #menu {" << std::endl
      << "        display: flex;" << std::endl
      << "        flex-direction: column;" << std::endl
      << "        position: fixed;" << std::endl
      << "        top: 20px;" << std::endl
      << "        left: 20px;" << std::endl
      << "      }" << std::endl
      << std::endl
      << "      #menu a:link," << std::endl
      << "      #menu a:visited," << std::endl
      << "      #menu a:hover," << std::endl
      << "      #menu a:active {" << std::endl
      << "        color: #FFFFFF;" << std::endl
      << "        text-decoration: none;" << std::endl
      << "      }" << std::endl
      << std::endl;
  }
  outputfile
    << "      .menu-icon {" << std::endl
    << "        margin-right: 0px;" << std::endl
    << "        width: 30px;" << std::endl
    << "        aspect-ratio: 1 / 1;" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: cover;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .menu-item {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "        color: var(--menuitem-c);" << std::endl
    << "        align-items: center;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        padding: 5px;" << std::endl
    << "      }" << std::endl
    << std::endl;

  if (collapsiblemenu > 1) // we need an expandable menu
  {
    outputfile
      << "       #searchmenu {" << std::endl
      << "         display: none;" << std::endl
      << "       }" << std::endl
      << std::endl
      << "       .expandedsearchmenu .menu-item {" << std::endl
      << "         padding-left: 0px;" << std::endl
      << "         padding-bottom: 0px;" << std::endl
      << "       }" << std::endl
      << std::endl
      << "       .expandable-menu-item {" << std::endl
      << "         display: flex;" << std::endl
      << "         flex-direction: column;" << std::endl
      << "         margin-right: 0px;" << std::endl
      << "         cursor: pointer;" << std::endl
      << "       }  " << std::endl
      << std::endl
      << "      .expandedsearchmenu {" << std::endl
      << "        display: flex;" << std::endl
      << "        flex-direction: column;" << std::endl
      << "        align-items: flex-start;" << std::endl
      << "        max-height: 0px;" << std::endl
      << "        overflow: hidden;" << std::endl
      << "        padding: 0px;" << std::endl
      << "        opacity: 0%;" << std::endl
      << "        border: none; " << std::endl
      << "        background: var(--conversationbox-bc);" << std::endl
      << "        transition: max-height .3s ease-out, padding .3s ease-out, opacity .3s ease-out;" << std::endl
      << "      }" << std::endl
      << std::endl
      << "     .searchmenu:checked + .searchmenulabel + .expandedsearchmenu {" << std::endl
      << "       max-height: 170px;" << std::endl
      << "       padding-top: 10px;" << std::endl
      << "       opacity: 100%;" << std::endl
      << "       transition: max-height .3s ease-out, padding .3s ease-out, opacity .15s ease-out;" << std::endl
      << "     }" << std::endl
      << std::endl
      << "     .searchmenulabel {" << std::endl
      << "       cursor: pointer;" << std::endl
      << "     }" << std::endl
      << std::endl
      << "     .searchmenulabel .hamburger-icon {" << std::endl
      << "       display: inline-block;" << std::endl
      << "       width:30px;" << std::endl
      << "       height: 30px;" << std::endl
      << "       margin-right: 5px;" << std::endl
      << "       vertical-align: middle;" << std::endl
      << "       background: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M13.5 5.5A1.5 1.5 0 1 1 12 4a1.5 1.5 0 0 1 1.5 1.5zm-1.5 5a1.5 1.5 0 1 0 1.5 1.5 1.5 1.5 0 0 0-1.5-1.5zm0 6.5a1.5 1.5 0 1 0 1.5 1.5A1.5 1.5 0 0 0 12 17z\"></path></svg>');" << std::endl
      << "       filter: var(--icon-f);" << std::endl
      << "       transition: background 0.3s ease-out, transform 0.3s ease-out;" << std::endl
      << "      }" << std::endl
      << std::endl
      << "       .searchmenu:checked + .searchmenulabel .hamburger-icon {" << std::endl
      << "         background: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"30\" height=\"30\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M21 11.25h-8.25V3h-1.5v8.25H3v1.5h8.25V21h1.5v-8.25H21v-1.5z\"></path></g></svg>');" << std::endl
      << "         transform: rotate(45deg);" << std::endl
      << "         transition: background 0.3s ease-out, transform 0.3s ease-out;" << std::endl
      << "       }" << std::endl
      << std::endl
      << "       .searchmenulabel .label-text {" << std::endl
      << "         display: inline-block;" << std::endl
      << "         height: 100%;" << std::endl
      << "         vertical-align: middle;" << std::endl
      << "       }" << std::endl
      << std::endl;
  }

  outputfile
    << "      #theme {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "        position: fixed;" << std::endl
    << "        top: 20px;" << std::endl
    << "        right: 20px;" << std::endl
    << "      }" << std::endl
    << std::endl;
  if (themeswitching)
  {
    outputfile
      << "      .themebutton {" << std::endl
      << "        display: block;" << std::endl
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M11.5 7.75c0-0.4 0.34-0.77 0.78-0.74C14.9 7.15 17 9.33 17 12c0 2.67-2.09 4.85-4.72 5-0.44 0.02-0.78-0.34-0.78-0.75v-8.5Z\"/><path d=\"M12.97 0.73c-0.53-0.53-1.4-0.53-1.94 0L8.39 3.38H4.75c-0.76 0-1.37 0.61-1.37 1.37v3.64l-2.65 2.64c-0.53 0.53-0.53 1.4 0 1.94l2.65 2.64v3.64c0 0.76 0.61 1.38 1.37 1.38h3.64l2.64 2.64c0.53 0.53 1.4 0.53 1.94 0l2.64-2.63 3.64-0.01c0.76 0 1.38-0.62 1.38-1.38v-3.64l2.63-2.64c0.54-0.53 0.54-1.4 0-1.94l-2.62-2.61-0.01-3.67c0-0.76-0.62-1.38-1.38-1.38h-3.64l-2.64-2.64Zm-3.45 4L12 2.22l2.48 2.5c0.26 0.25 0.61 0.4 0.98 0.4h3.42v3.45c0.01 0.36 0.16 0.71 0.41 0.97L21.76 12l-2.48 2.48c-0.26 0.26-0.4 0.61-0.4 0.98v3.42h-3.43c-0.36 0.01-0.7 0.15-0.96 0.4L12 21.77l-2.48-2.48c-0.26-0.26-0.61-0.4-0.98-0.4H5.13v-3.42c0-0.37-0.15-0.72-0.4-0.98L2.22 12l2.5-2.48c0.25-0.26 0.4-0.61 0.4-0.98V5.13h3.41c0.37 0 0.72-0.15 0.98-0.4Z\"></path></g></svg>');" << std::endl
      << "        filter: var(--icon-f);" << std::endl
      << "      }" << std::endl
      << std::endl;
  }

  if (searchpage)
  {
    outputfile
      << "    .searchbutton {" << std::endl
      << "      display: block;" << std::endl
      << "      background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M10 2.125a7.875 7.875 0 1 0 4.716 14.182l4.989 4.989a1.125 1.125 0 0 0 1.59-1.591l-4.988-4.989A7.875 7.875 0 0 0 10 2.125zM3.875 10a6.125 6.125 0 1 1 12.25 0 6.125 6.125 0 0 1-12.25 0z\"></path></g></svg>');" << std::endl
      << "      filter: var(--icon-f);" << std::endl
      << "    }" << std::endl
      << std::endl;
  }

  if (calllog)
  {
    outputfile
      << "      .calllog-icon {" << std::endl
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M17.21 22a8.08 8.08 0 0 1-2.66-.51 20.79 20.79 0 0 1-7.3-4.73 21 21 0 0 1-4.74-7.3c-.78-2.22-.67-4 .35-5.45h0a5 5 0 0 1 2-1.67 2.72 2.72 0 0 1 3.51.81l2.11 3a2.69 2.69 0 0 1-.35 3.49l-.93.85c-.09.08-.15.22-.08.31A20 20 0 0 0 11 13a20 20 0 0 0 2.21 1.91.24.24 0 0 0 .3-.08l.85-.93a2.68 2.68 0 0 1 3.49-.35l3 2.11a2.68 2.68 0 0 1 .85 3.43 5.22 5.22 0 0 1-1.71 2 4.69 4.69 0 0 1-2.78.91zM4.09 4.87c-.46.64-1 1.77-.16 4.08a19.28 19.28 0 0 0 4.38 6.74A19.49 19.49 0 0 0 15 20.07c2.31.81 3.44.3 4.09-.16a3.55 3.55 0 0 0 1.2-1.42A1.21 1.21 0 0 0 20 16.9l-3-2.12a1.18 1.18 0 0 0-1.53.15l-.82.9a1.72 1.72 0 0 1-2.33.29 21.9 21.9 0 0 1-2.37-2.05 22.2 22.2 0 0 1-2-2.37 1.71 1.71 0 0 1 .3-2.32l.89-.82A1.19 1.19 0 0 0 9.21 7L7.1 4a1.19 1.19 0 0 0-1.51-.38 3.72 3.72 0 0 0-1.5 1.25z\"></path></svg>');" << std::endl
      << "        filter: var(--icon-f);" << std::endl
      << "    }" << std::endl
      << std::endl;
  }

  if (blocked)
  {
    outputfile
      << "      .blocked-icon {" << std::endl
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"-2 -1 29 29\" fill=\"white\" stroke=\"white\"><path d=\"M12 1a11 11 0 1 0 11 11A11 11 0 0 0 12 1zm0 1.5a9.448 9.448 0 0 1 6.159 2.281L4.781 18.159A9.488 9.488 0 0 1 12 2.5zm0 19a9.448 9.448 0 0 1-6.159-2.281L19.219 5.841A9.488 9.488 0 0 1 12 21.5z\"></path></svg>');" << std::endl
      << "        filter: var(--icon-f);" << std::endl
      << "    }" << std::endl
      << std::endl;
  }

  if (stickerpacks)
  {
    outputfile
      << "      .stickerpacks-icon {" << std::endl
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M21.2 5.072A5.55 5.55 0 0 0 18.928 2.8c-.977-.522-1.947-.8-4.62-.8H9.692c-2.673 0-3.643.278-4.62.8A5.55 5.55 0 0 0 2.8 5.072c-.522.977-.8 1.947-.8 4.62v4.616c0 2.673.278 3.643.8 4.62A5.55 5.55 0 0 0 5.072 21.2c1.118.567 2.363.837 3.616.785h.1a3 3 0 0 0 1.7-.53L20.734 14.4A3 3 0 0 0 22 11.949V9.692c0-2.673-.278-3.643-.8-4.62zM8.739 20.485a5.82 5.82 0 0 1-2.96-.608 4.02 4.02 0 0 1-1.656-1.656c-.365-.683-.623-1.363-.623-3.913V9.692c0-2.55.258-3.231.623-3.913a4.02 4.02 0 0 1 1.656-1.656c.683-.365 1.363-.623 3.913-.623h4.616c2.55 0 3.231.258 3.913.623a4.02 4.02 0 0 1 1.656 1.656c.365.683.623 1.363.623 3.913v2.257c-.002.101-.014.201-.036.3h-3.273c-2.8 0-3.872.3-4.975.89a6.17 6.17 0 0 0-2.575 2.575c-.575 1.074-.872 2.132-.888 4.769h-.014zm1.525-.7a6.63 6.63 0 0 1 .7-3.362 4.7 4.7 0 0 1 1.96-1.961c.755-.4 1.549-.712 4.268-.712h1.837z\"></path></svg>');" << std::endl
      << "        filter: var(--icon-f);" << std::endl
      << "    }" << std::endl
      << std::endl;
  }

  if (settings)
  {
    outputfile
      << "      .settings-icon {" << std::endl
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"none\"><path d=\"M12 8.5A3.5 3.5 0 1 1 8.5 12 3.5 3.5 0 0 1 12 8.5M12 7a5 5 0 1 0 5 5 5 5 0 0 0-5-5zm0-4.49a9.83 9.83 0 0 1 1.21.08l.21 2.49.91.33a5.72 5.72 0 0 1 .68.28l.88.42 1.91-1.62a9.23 9.23 0 0 1 1.71 1.71l-1.62 1.91.42.88a5.72 5.72 0 0 1 .28.68l.33.91 2.49.21a8.91 8.91 0 0 1 0 2.42l-2.49.21-.33.91a5.72 5.72 0 0 1-.28.68l-.42.88 1.62 1.91a9.23 9.23 0 0 1-1.71 1.71l-1.91-1.62-.88.42a5.72 5.72 0 0 1-.68.28l-.91.33-.21 2.49a9.19 9.19 0 0 1-2.42 0l-.21-2.49-.91-.33a5.72 5.72 0 0 1-.67-.28l-.88-.42-1.92 1.62a9.23 9.23 0 0 1-1.71-1.71l1.62-1.91-.42-.89a5.72 5.72 0 0 1-.28-.68l-.33-.91-2.49-.21a8.91 8.91 0 0 1 0-2.42l2.49-.21.33-.91A5.72 5.72 0 0 1 5.69 9l.42-.88L4.49 6.2A9.23 9.23 0 0 1 6.2 4.49l1.91 1.62.89-.42a5.72 5.72 0 0 1 .68-.28l.91-.33.21-2.49a9.83 9.83 0 0 1 1.2-.08h0M12 1a10.93 10.93 0 0 0-1.88.16 1 1 0 0 0-.79.9L9.17 4a7.64 7.64 0 0 0-.83.35L6.87 3.09a1 1 0 0 0-.66-.24 1 1 0 0 0-.54.15 11 11 0 0 0-2.62 2.62 1 1 0 0 0 0 1.25l1.29 1.47a7.64 7.64 0 0 0-.34.83l-1.92.16a1 1 0 0 0-.9.79 11 11 0 0 0 0 3.76 1 1 0 0 0 .9.79l1.92.16a7.64 7.64 0 0 0 .35.83l-1.26 1.47a1 1 0 0 0-.09 1.2A11 11 0 0 0 5.62 21a1 1 0 0 0 .61.19 1 1 0 0 0 .64-.23l1.47-1.25a7.64 7.64 0 0 0 .83.35l.16 1.92a1 1 0 0 0 .79.9A11.83 11.83 0 0 0 12 23a10.93 10.93 0 0 0 1.88-.16 1 1 0 0 0 .79-.9l.16-1.94a7.64 7.64 0 0 0 .83-.35l1.47 1.25a1 1 0 0 0 .66.24 1 1 0 0 0 .54-.16 11 11 0 0 0 2.67-2.6 1 1 0 0 0 0-1.25l-1.25-1.47a7.64 7.64 0 0 0 .35-.83l1.92-.16a1 1 0 0 0 .9-.79 11 11 0 0 0 0-3.76 1 1 0 0 0-.9-.79L20 9.17a7.64 7.64 0 0 0-.35-.83l1.25-1.47a1 1 0 0 0 .1-1.2 11 11 0 0 0-2.61-2.62 1 1 0 0 0-.61-.19 1 1 0 0 0-.64.23l-1.48 1.25a7.64 7.64 0 0 0-.83-.34l-.16-1.92a1 1 0 0 0-.79-.9A11.83 11.83 0 0 0 12 1z\"></path></svg>');" << std::endl
      << "        filter: var(--icon-f);" << std::endl
      << "    }" << std::endl
      << std::endl;
  }

  outputfile
    << "      @media print {" << std::endl
    << "        .conversation-list-header {" << std::endl
    << "          padding: 0;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "        .conversation-list-item {" << std::endl
    << "          break-inside: avoid;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "        .conversation-list {" << std::endl
    << "          margin: 0 auto;" << std::endl
    << "          display: block;" << std::endl
    << "          border-radius: 0;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "        .avatar {" << std::endl
    << "          -webkit-print-color-adjust: exact;" << std::endl
    << "          color-adjust: exact;" << std::endl
    << "          print-color-adjust: exact;" << std::endl
    << "          flex-shrink: 0;" << std::endl
    << "        }" << std::endl
    << std::endl;

  if (!exportdetails.empty())
    outputfile
      << "        .export-details {" << std::endl
      << "          display: grid;" << std::endl
      << "        }" << std::endl
      << std::endl;

  outputfile
    << "        #menu {" << std::endl
    << "          display: none;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "        #theme {" << std::endl
    << "          display: none;" << std::endl
    << "        }" << std::endl
    << "      }" << std::endl
    << std::endl
    << "    </style>" << std::endl
    << "  </head>" << std::endl
    << "  <body>" << std::endl;
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
    << std::endl
    << "  <input type=\"checkbox\" id=\"theme-switch\">" << std::endl
    << "  <div id=\"page\">" << std::endl
    << std::endl

    << "    <div class=\"conversation-list-header\">" << std::endl
    << "      Signal conversation list" << std::endl
    << "    </div>" << std::endl
    << "" << std::endl
    << "    <div class=\"conversation-list\">" << std::endl
    << "" << std::endl;

  // for item in threads
  bool pinnedheader = false;
  bool archivedheader = false;
  bool chatsheader = false;
  for (uint i = 0; i < results.rows(); ++i)
  {
    bool archived = false;
    if (d_database.tableContainsColumn("thread", "archived"))
      archived = (results.getValueAs<long long int>(i, "archived") != 0);
    if (archived && !archivedheader)
    {
      outputfile << "      <div class=\"header\">Archived conversations</div>" << std::endl;
      archivedheader = true;
    }

    bool pinned = false;
    if (d_database.tableContainsColumn("thread", "pinned"))
      pinned = (results.getValueAs<long long int>(i, "pinned") != 0);
    if (pinned && !pinnedheader)
    {
      outputfile << "      <div class=\"header\">Pinned</div>" << std::endl;
      pinnedheader = true;
    }

    if (pinnedheader && !pinned && !chatsheader && !archived) // this message is not pinned, but pinnedheader was previously shown
    {
      outputfile << "      <div class=\"header\">Chats</div>" << std::endl;
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

    bool isblocked = (results.getValueAs<long long int>(i, "blocked") == 1);
    bool isgroup = !results.isNull(i, "group_id");
    bool isnotetoself = (t_id == note_to_self_tid);
    bool emoji_initial = getRecipientInfoFromMap(recipient_info, rec_id).initial_is_emoji;
    bool hasavatar = getRecipientInfoFromMap(recipient_info, rec_id).hasavatar;

    long long int groupsender = -1;
    if (results.valueHasType<std::string>(i, "group_sender_id"))
      groupsender = bepaald::toNumber<long long int>(results.valueAsString(i, "group_sender_id"));

    std::string snippet = results.valueAsString(i, "snippet");
    HTMLescapeString(&snippet);

    if (Types::isStatusMessage(snippet_type))
      snippet = "(status message)";

    long long int datetime = results.getValueAs<long long int>(i, "date");
    std::string date_date = bepaald::toDateString(datetime / 1000, "%b %d, %Y");
    //std::string date_time = bepaald::toDateString(datetime / 1000, "%R"); // does not work with mingw
    std::string date_time = bepaald::toDateString(datetime / 1000, "%H:%M");

    std::string convo_url_path = (isnotetoself ? "Note to self (_id"s + bepaald::toString(t_id) + ")" : sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name + " (_id" + bepaald::toString(t_id) + ")"));
    HTMLescapeUrl(&convo_url_path);
    std::string convo_url_location = (isnotetoself ? "Note to self.html" : sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name + ".html"));
    HTMLescapeUrl(&convo_url_location);

    if (convo_url_location == ".html") [[unlikely]]
    {
      Logger::error("Sanitized+url encoded name was empty. This should never happen. Original display_name: '",
                    getRecipientInfoFromMap(recipient_info, rec_id).display_name, "'");
      return;
    }


    // if (t_id == 11)
    // {
    //   std::cout << "Snippet: " << snippet << std::endl;
    //   if (isgroup && groupsender > 0)
    //     std::cout << "GROUPSEND: " + getRecipientInfoFromMap(recipient_info, groupsender).display_name << std::endl;
    //   else
    //     std::cout << "isgroup: " << isgroup << std::endl << "groupsender: " << groupsender << std::endl;
    // }

    outputfile
      << "      <div class=\"conversation-list-item\">" << std::endl
      << "        <div class=\"avatar"
      << (((hasavatar || !isgroup) && !isnotetoself) ? " avatar-" + bepaald::toString(rec_id) : "")
      << ((isgroup && !hasavatar) ? " group-avatar-icon" : "")
      << ((emoji_initial && !hasavatar) ? " avatar-emoji-initial" : "")
      << (isnotetoself ? " note-to-self-icon" : "") << "\">" << std::endl

      << "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>" << std::endl
      << ((!hasavatar && !isgroup && !isnotetoself) ? "          <span>" + getRecipientInfoFromMap(recipient_info, rec_id).initial + "</span>\n" : "")
      << "        </div>" << std::endl
      << "        <div class=\"name-and-snippet\">" << std::endl
      << "          <div class=\"name-and-status\">" << std::endl;
    if (isblocked)
      outputfile << "            <div class=\"blocked\"></div>" << std::endl;
    outputfile
      << "            <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>" << std::endl
      << "            <pre class=\"name\">" << (isnotetoself ? "Note to self" : HTMLescapeString(getRecipientInfoFromMap(recipient_info, rec_id).display_name)) << "</pre>" << std::endl
      << "          </div>" << std::endl
      << "          <span class=\"snippet\">"
      << ((isgroup && groupsender > 0) ? "<span class=\"groupsender\">" + HTMLescapeString(getRecipientInfoFromMap(recipient_info, groupsender).display_name) + "</span>: " : "")
      << snippet << "</span>" << std::endl
      << "        </div>" << std::endl
      << "        <div class=\"index-date\">" << std::endl
      << "          <a href=\"" << convo_url_path << "/" << convo_url_location << "\" class=\"main-link\"></a>" << std::endl
      << "          <span>" << date_date << "</span>" << std::endl
      << "          <span>" << date_time << "</span>" << std::endl
      << "        </div>" << std::endl
      << "      </div>" << std::endl
      << std::endl;
  }

  if (calllog || stickerpacks || blocked || settings)
    outputfile
      << "    <div id=\"menu\">" << std::endl;

  if (collapsiblemenu > 1)
    outputfile
      << "         <div class=\"menu-item\">" << std::endl
      << "           <div class=\"expandable-menu-item\">" << std::endl
      << "             <input id=\"searchmenu\" class=\"searchmenu\" type=\"checkbox\">" << std::endl
      << "             <label for=\"searchmenu\" class=\"searchmenulabel\">" << std::endl
      << "               <span class=\"hamburger-icon\"></span><span class=\"label-text\">menu</span>" << std::endl
      << "             </label>" << std::endl
      << "             <div class=\"expandedsearchmenu\">" << std::endl
      << std::endl;

  if (calllog)
  {
    outputfile
      << "      <a href=\"calllog.html\">" << std::endl
      << "        <div class=\"menu-item\">" << std::endl
      << "          <div class=\"menu-icon calllog-icon\">" << std::endl
      << "          </div>" << std::endl
      << "          <div>" << std::endl
      << "            call log" << std::endl
      << "          </div>" << std::endl
      << "        </div>" << std::endl
      << "      </a>" << std::endl
      << std::endl;
  }
  if (stickerpacks)
  {
    outputfile
      << "      <a href=\"stickerpacks.html\">" << std::endl
      << "        <div class=\"menu-item\">" << std::endl
      << "          <div class=\"menu-icon stickerpacks-icon\">" << std::endl
      << "          </div>" << std::endl
      << "          <div>" << std::endl
      << "            stickerpacks" << std::endl
      << "          </div>" << std::endl
      << "        </div>" << std::endl
      << "      </a>" << std::endl
      << std::endl;
  }

  if (blocked)
  {
    outputfile
      << "      <a href=\"blockedlist.html\">" << std::endl
      << "        <div class=\"menu-item\">" << std::endl
      << "          <div class=\"menu-icon blocked-icon\">" << std::endl
      << "          </div>" << std::endl
      << "          <div>" << std::endl
      << "            blocked contacts" << std::endl
      << "          </div>" << std::endl
      << "        </div>" << std::endl
      << "      </a>" << std::endl
      << std::endl;
  }

  if (settings)
  {
    outputfile
      << "      <a href=\"settings.html\">" << std::endl
      << "        <div class=\"menu-item\">" << std::endl
      << "          <div class=\"menu-icon settings-icon\">" << std::endl
      << "          </div>" << std::endl
      << "          <div>" << std::endl
      << "            settings" << std::endl
      << "          </div>" << std::endl
      << "        </div>" << std::endl
      << "      </a>" << std::endl
      << std::endl;
  }

  if (collapsiblemenu > 1)
    outputfile
      << "             </div>" << std::endl
      << "           </div>" << std::endl
      << "         </div>" << std::endl;

  if (calllog || stickerpacks || blocked || settings)
    outputfile
      << "    </div>" << std::endl;

  if (themeswitching || searchpage)
  {
    outputfile << "    <div id=\"theme\">" << std::endl;
    if (searchpage)
    {
      outputfile
        << "      <div class=\"menu-item\">" << std::endl
        << "        <a href=\"searchpage.html\">" << std::endl
        << "          <span class=\"menu-icon searchbutton\">" << std::endl
        << "          </span>" << std::endl
        << "        </a>" << std::endl
        << "      </div>" << std::endl;
    }
    if (themeswitching)
    {
      outputfile
        << "      <div class=\"menu-item\">" << std::endl
        << "        <label for=\"theme-switch\">" << std::endl
        << "          <span class=\"menu-icon themebutton\">" << std::endl
        << "          </span>" << std::endl
        << "        </label>" << std::endl
        << "      </div>" << std::endl;
    }
    outputfile << "    </div>" << std::endl;
  }
  outputfile
    << "    </div>" << std::endl
    << "  </div>" << std::endl;

  if (!exportdetails.empty())
    outputfile << std::endl << exportdetails << std::endl;

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

  outputfile
    << "  </body>" << std::endl
    << "</html>" << std::endl;
}
