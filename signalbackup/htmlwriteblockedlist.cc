/*
  Copyright (C) 2024  Selwin van Dijk

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

bool SignalBackup::HTMLwriteBlockedlist(std::string const &dir, std::map<long long int, RecipientInfo> *recipient_info,
                                        bool overwrite, bool append, bool light, bool themeswitching,
                                        std::string const &exportdetails) const
{
  Logger::message("Writing blockedlist.html...");

  if (bepaald::fileOrDirExists(dir + "/blockedlist.html"))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", dir, "/blockedlist.html' exists. Use --overwrite to overwrite.");
      return false;
    }
  }
  std::ofstream outputfile(dir + "/blockedlist.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", dir, "/blockedlist.html' for writing.");
    return false;
  }


  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT _id FROM recipient WHERE blocked = 1", &results))
  {
    Logger::error("Failed to query database for blocked contacts.");
    return false;
  }

  // write start of html

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  outputfile
    << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") // %F an d%T do not work on minGW
    << " by signalbackup-tools (" << VERSIONDATE << "). "
    << "Input database version: " << d_databaseversion << ". -->" << std::endl
    << "<!DOCTYPE html>" << std::endl
    << "<html>" << std::endl
    << "  <head>" << std::endl
    << "    <meta charset=\"utf-8\">" << std::endl
    << "    <title>Signal blocked contacts list</title>" << std::endl
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
    << std::endl
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
    << std::endl
    << "      .menu-item > div {" << std::endl
    << "        margin-right: 5px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      #theme {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        position: fixed;" << std::endl
    << "        top: 20px;" << std::endl
    << "        right: 20px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .themebutton {" << std::endl
    << "        display: block;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M11.5 7.75c0-0.4 0.34-0.77 0.78-0.74C14.9 7.15 17 9.33 17 12c0 2.67-2.09 4.85-4.72 5-0.44 0.02-0.78-0.34-0.78-0.75v-8.5Z\"/><path d=\"M12.97 0.73c-0.53-0.53-1.4-0.53-1.94 0L8.39 3.38H4.75c-0.76 0-1.37 0.61-1.37 1.37v3.64l-2.65 2.64c-0.53 0.53-0.53 1.4 0 1.94l2.65 2.64v3.64c0 0.76 0.61 1.38 1.37 1.38h3.64l2.64 2.64c0.53 0.53 1.4 0.53 1.94 0l2.64-2.63 3.64-0.01c0.76 0 1.38-0.62 1.38-1.38v-3.64l2.63-2.64c0.54-0.53 0.54-1.4 0-1.94l-2.62-2.61-0.01-3.67c0-0.76-0.62-1.38-1.38-1.38h-3.64l-2.64-2.64Zm-3.45 4L12 2.22l2.48 2.5c0.26 0.25 0.61 0.4 0.98 0.4h3.42v3.45c0.01 0.36 0.16 0.71 0.41 0.97L21.76 12l-2.48 2.48c-0.26 0.26-0.4 0.61-0.4 0.98v3.42h-3.43c-0.36 0.01-0.7 0.15-0.96 0.4L12 21.77l-2.48-2.48c-0.26-0.26-0.61-0.4-0.98-0.4H5.13v-3.42c0-0.37-0.15-0.72-0.4-0.98L2.22 12l2.5-2.48c0.25-0.26 0.4-0.61 0.4-0.98V5.13h3.41c0.37 0 0.72-0.15 0.98-0.4Z\"></path></g></svg>');" << std::endl
    << "        filter: var(--icon-f);" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .nav-up {" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"></path></svg>');" << std::endl
    << "        filter: var(--icon-f);" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .conversation-list-header {" << std::endl
    << "        text-align: center;" << std::endl
    << "        font-size: xx-large;" << std::endl
    << "        color: var(--conversationlistheader-c);" << std::endl
    << "        padding: 10px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
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
    << std::endl
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
    << std::endl
    << "      .avatar-emoji-initial {" << std::endl
    << "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .note-to-self-icon {" << std::endl
    << "        background: #315FF4;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"80\" height=\"80\" viewBox=\"0 0 80 80\" fill=\"white\"><path d=\"M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z\"></path></svg>');" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: 80%;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .group-avatar-icon {" << std::endl
    << "        background: #315FF4;" << std::endl
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" width=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"></path></svg>');" << std::endl
    << "        background-position: center;" << std::endl
    << "        background-repeat: no-repeat;" << std::endl
    << "        background-size: 80%;" << std::endl
    << "      }" << std::endl
    << std::endl;

  for (uint i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, "_id");
    if (getRecipientInfoFromMap(recipient_info, rec_id).hasavatar) // avatar not working yet... need path...
    {
      std::string avatarpath;
      long long int thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", rec_id, -1);
      if (thread_id != -1)
      {
        std::string thread_dir = sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name + " (_id" + bepaald::toString(thread_id) + ")");
        if (bepaald::fileOrDirExists(dir + "/" + thread_dir + "/media/Avatar_" + bepaald::toString(rec_id) + ".bin"))
          avatarpath = std::move(thread_dir);
      }

      if (avatarpath.empty()) // avatar not already present in thread, write out own...
      {
        if (HTMLwriteAvatar(rec_id, dir, std::string(), overwrite, append).empty())
        {
          Logger::warning("Failed to set path or write avatar. Skipping");
          continue;
        }
      }
      else
        avatarpath += "/";

      bepaald::replaceAll(&avatarpath, '\"', R"(\")");
      outputfile
        << "      .avatar-" << rec_id << " {" << std::endl
        << "        background-image: url(\"" << avatarpath << "media/Avatar_" << rec_id << ".bin\");" << std::endl
        << "        background-position: center;" << std::endl
        << "        background-repeat: no-repeat;" << std::endl
        << "        background-size: cover;" << std::endl
        << "      }" << std::endl
        << std::endl;
    }
    else
    {
      if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", rec_id, -1) == 0)   // no avatar, no group
      {
        outputfile
          << "      .avatar-" << rec_id << " {" << std::endl
          << "        background: #" << getRecipientInfoFromMap(recipient_info, rec_id).color << ";" << std::endl
          << "      }" << std::endl
          << std::endl;
      }
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
    << "        font-weight: bold;" << std::endl
    << "        font-size: 18px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        margin: 0px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .name-and-snippet pre {" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "      }" << std::endl
    << std::endl;

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
    << std::endl;
  outputfile
    << "    </style>" << std::endl
    << "  </head>" << std::endl;

  // BODY
  outputfile
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
    << "    <input type=\"checkbox\" id=\"theme-switch\">" << std::endl
    << std::endl
    << "    <div id=\"page\">" << std::endl
    << std::endl
    << "      <div class=\"conversation-list-header\">" << std::endl
    << "        Signal blocked contacts" << std::endl
    << "      </div>" << std::endl
    << std::endl
    << "      <div class=\"conversation-list\">" << std::endl
    << std::endl;

  // write blocked list
  for (uint i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, "_id");
    bool hasavatar = getRecipientInfoFromMap(recipient_info, rec_id).hasavatar;
    bool isgroup = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", rec_id, -1) == 1;
    bool emoji_initial = getRecipientInfoFromMap(recipient_info, rec_id).initial_is_emoji;

    //Logger::message(rec_id, " : ", sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name));

    outputfile
      << "        <div class=\"conversation-list-item\">" << std::endl
      << "          <div class=\"avatar"
      << ((hasavatar || !isgroup) ? " avatar-" + bepaald::toString(rec_id) : "")
      << ((isgroup && !hasavatar) ? " group-avatar-icon" : "")
      << ((emoji_initial && !hasavatar) ? " avatar-emoji-initial" : "") << "\">" << std::endl
      << ((!hasavatar && !isgroup) ? "            <span>" + getRecipientInfoFromMap(recipient_info, rec_id).initial + "</span>\n" : "")
      << "          </div>" << std::endl
      << "          <div class=\"name-and-snippet\">" << std::endl
      << "            <pre class=\"name\">" << HTMLescapeString(getRecipientInfoFromMap(recipient_info, rec_id).display_name) << "</pre>" << std::endl
      << "          </div>" << std::endl
      << "        </div>" << std::endl;
  }

  // write end of html
  outputfile
    << std::endl
    << "        <div id=\"menu\">" << std::endl
    << "          <a href=\"index.html\">" << std::endl
    << "            <div class=\"menu-item\">" << std::endl
    << "              <div class=\"menu-icon nav-up\">" << std::endl
    << "              </div>" << std::endl
    << "              <div>" << std::endl
    << "                index" << std::endl
    << "              </div>" << std::endl
    << "            </div>" << std::endl
    << "          </a>" << std::endl
    << "        </div>" << std::endl
    << std::endl;

  if (themeswitching)
  {
    outputfile
      << "      <div id=\"theme\">" << std::endl
      << "        <div class=\"menu-item\">" << std::endl
      << "          <label for=\"theme-switch\">" << std::endl
      << "            <span class=\"menu-icon themebutton\">" << std::endl
      << "           </span>" << std::endl
      << "         </label>" << std::endl
      << "        </div>" << std::endl
      << "      </div>" << std::endl
      << std::endl;
  }

  outputfile
    << "      </div>" << std::endl
    << "    </div>" << std::endl;

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
    << std::endl
    << "  </body>" << std::endl
    << "</html>" << std::endl;

  return true;
}
