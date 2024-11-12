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

  bool listempty = false;
  if (results.rows() == 0)
    listempty = true;

  // write start of html
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  outputfile
    << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") // %F an d%T do not work on minGW
    << " by signalbackup-tools (" << VERSIONDATE << "). "
    << "Input database version: " << d_databaseversion << ". -->" << '\n'
    << "<!DOCTYPE html>" << '\n'
    << "<html>" << '\n'
    << "  <head>" << '\n'
    << "    <meta charset=\"utf-8\">" << '\n'
    << "    <title>Signal blocked contacts list</title>" << '\n'
    << "    <style>" << '\n'
    << "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << '\n'
    << "        /* " << (light ? "light" : "dark") << " */" << '\n'
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
      << "        /* " << (!light ? "light" : "dark") << " */" << '\n'
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
    << "" << '\n';

  outputfile
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
    << '\n';
  if (themeswitching)
  {
    outputfile
      << "      .themebutton {" << '\n'
      << "        display: block;" << '\n'
      << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 24 24\" fill=\"white\" stroke=\"white\"><g><path d=\"M11.5 7.75c0-0.4 0.34-0.77 0.78-0.74C14.9 7.15 17 9.33 17 12c0 2.67-2.09 4.85-4.72 5-0.44 0.02-0.78-0.34-0.78-0.75v-8.5Z\"/><path d=\"M12.97 0.73c-0.53-0.53-1.4-0.53-1.94 0L8.39 3.38H4.75c-0.76 0-1.37 0.61-1.37 1.37v3.64l-2.65 2.64c-0.53 0.53-0.53 1.4 0 1.94l2.65 2.64v3.64c0 0.76 0.61 1.38 1.37 1.38h3.64l2.64 2.64c0.53 0.53 1.4 0.53 1.94 0l2.64-2.63 3.64-0.01c0.76 0 1.38-0.62 1.38-1.38v-3.64l2.63-2.64c0.54-0.53 0.54-1.4 0-1.94l-2.62-2.61-0.01-3.67c0-0.76-0.62-1.38-1.38-1.38h-3.64l-2.64-2.64Zm-3.45 4L12 2.22l2.48 2.5c0.26 0.25 0.61 0.4 0.98 0.4h3.42v3.45c0.01 0.36 0.16 0.71 0.41 0.97L21.76 12l-2.48 2.48c-0.26 0.26-0.4 0.61-0.4 0.98v3.42h-3.43c-0.36 0.01-0.7 0.15-0.96 0.4L12 21.77l-2.48-2.48c-0.26-0.26-0.61-0.4-0.98-0.4H5.13v-3.42c0-0.37-0.15-0.72-0.4-0.98L2.22 12l2.5-2.48c0.25-0.26 0.4-0.61 0.4-0.98V5.13h3.41c0.37 0 0.72-0.15 0.98-0.4Z\"></path></g></svg>');" << '\n'
      << "        filter: var(--icon-f);" << '\n'
      << "      }" << '\n'
      << '\n';
  }
  outputfile
    << "      .nav-up {" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"></path></svg>');" << '\n'
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
    << "        min-width: 460px;" << '\n'
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
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"80\" height=\"80\" viewBox=\"0 0 80 80\" fill=\"white\"><path d=\"M58,7.5A6.51,6.51 0,0 1,64.5 14L64.5,66A6.51,6.51 0,0 1,58 72.5L22,72.5A6.51,6.51 0,0 1,15.5 66L15.5,14A6.51,6.51 0,0 1,22 7.5L58,7.5M58,6L22,6a8,8 0,0 0,-8 8L14,66a8,8 0,0 0,8 8L58,74a8,8 0,0 0,8 -8L66,14a8,8 0,0 0,-8 -8ZM60,24L20,24v1.5L60,25.5ZM60,34L20,34v1.5L60,35.5ZM60,44L20,44v1.5L60,45.5ZM50,54L20,54v1.5L50,55.5Z\"></path></svg>');" << '\n'
    << "        background-position: center;" << '\n'
    << "        background-repeat: no-repeat;" << '\n'
    << "        background-size: 80%;" << '\n'
    << "      }" << '\n'
    << '\n'
    << "      .group-avatar-icon {" << '\n'
    << "        background: #315FF4;" << '\n'
    << "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"40\" height=\"40\" viewBox=\"0 0 40 40\" fill=\"white\"><path d=\"M29,16.75a6.508,6.508 0,0 1,6.5 6.5L35.5,24L37,24v-0.75a8,8 0,0 0,-6.7 -7.885,6.5 6.5,0 1,0 -8.6,0 7.941,7.941 0,0 0,-2.711 0.971A6.5,6.5 0,1 0,9.7 25.365,8 8,0 0,0 3,33.25L3,34L4.5,34v-0.75a6.508,6.508 0,0 1,6.5 -6.5h6a6.508,6.508 0,0 1,6.5 6.5L23.5,34L25,34v-0.75a8,8 0,0 0,-6.7 -7.885,6.468 6.468,0 0,0 1.508,-7.771A6.453,6.453 0,0 1,23 16.75ZM14,25.5a5,5 0,1 1,5 -5A5,5 0,0 1,14 25.5ZM21,10.5a5,5 0,1 1,5 5A5,5 0,0 1,21 10.5Z\"></path></svg>');" << '\n'
    << "        background-position: center;" << '\n'
    << "        background-repeat: no-repeat;" << '\n'
    << "        background-size: 80%;" << '\n'
    << "      }" << '\n'
    << '\n';

  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, "_id");
    if (getRecipientInfoFromMap(recipient_info, rec_id).hasavatar)
    {
      std::string avatarpath;
      std::string avatar_extension = getAvatarExtension(rec_id);
      long long int thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", rec_id, -1);
      if (thread_id != -1)
      {
        std::string thread_dir = sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name + " (_id" + bepaald::toString(thread_id) + ")");
        if (bepaald::fileOrDirExists(dir + "/" + thread_dir + "/media/Avatar_" + bepaald::toString(rec_id) + "." + avatar_extension))
          avatarpath = std::move(thread_dir);
      }

      if (avatarpath.empty()) // avatar not already present anywhere, write out own...
      {
        if (!bepaald::fileOrDirExists(dir + "/" + "/media/Avatar_" + bepaald::toString(rec_id) + "." + avatar_extension)) // maybe htmlroot/media/avatar was already
          if (HTMLwriteAvatar(rec_id, dir, std::string(), overwrite, append).empty())                                     // written by writefullcontacts...
          {
            Logger::warning("Failed to set path or write avatar. Skipping");
            continue;
          }
      }
      else
        avatarpath += "/";

      bepaald::replaceAll(&avatarpath, '\"', R"(\")");
      HTMLescapeUrl(&avatarpath);

      outputfile
        << "      .avatar-" << rec_id << " {\n"
        << "        background-image: url(\"" << avatarpath << "media/Avatar_" << rec_id << "." << avatar_extension << "\");\n"
        << "        background-position: center;\n"
        << "        background-repeat: no-repeat;\n"
        << "        background-size: cover;\n"
        << "      }\n"
        << '\n';
    }
    else
    {
      if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", rec_id, -1) == 0)   // no avatar, no group
      {
        outputfile
          << "      .avatar-" << rec_id << " {\n"
          << "        background: #" << getRecipientInfoFromMap(recipient_info, rec_id).color << ";\n"
          << "      }\n"
          << '\n';
      }
    }
  }

  outputfile
    << "      .name-and-snippet {\n"
    << "        position: relative;\n"
    << "        display: flex;\n"
    << "        flex-direction: column;\n"
    << "        padding-left: 30px;\n"
    << "        justify-content: center;\n"
    << "        align-content: center;\n"
    << "        width: 350px;\n"
    << "        font-weight: bold;\n"
    << "        font-size: 18px;\n"
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    << "        margin: 0px;\n"
    << "      }\n"
    << '\n'
    << "      .name-and-snippet pre {\n"
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    << "      }\n"
    << '\n';

  outputfile
    << "      @media print {\n"
    << "        .conversation-list-header {\n"
    << "          padding: 0;\n"
    << "        }\n"
    << '\n'
    << "        .conversation-list-item {\n"
    << "          break-inside: avoid;\n"
    << "        }\n"
    << '\n'
    << "        .conversation-list {\n"
    << "          margin: 0 auto;\n"
    << "          display: block;\n"
    << "          border-radius: 0;\n"
    << "        }\n"
    << '\n'
    << "        .avatar {\n"
    << "          -webkit-print-color-adjust: exact;\n"
    << "          color-adjust: exact;\n"
    << "          print-color-adjust: exact;\n"
    << "          flex-shrink: 0;\n"
    << "        }\n"
    << '\n';

  if (!exportdetails.empty())
    outputfile
      << "        .export-details {\n"
      << "          display: grid;\n"
      << "        }\n"
      << '\n';

  outputfile
    << "        #menu {\n"
    << "          display: none;\n"
    << "        }\n"
    << '\n'
    << "        #theme {\n"
    << "          display: none;\n"
    << "        }\n"
    << "      }\n"
    << '\n';
  outputfile
    << "    </style>\n"
    << "  </head>\n";

  // BODY
  outputfile
    << "  <body>\n";
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
    << "    <input type=\"checkbox\" id=\"theme-switch\">\n"
    << '\n'
    << "    <div id=\"page\">\n"
    << '\n'
    << "      <div class=\"conversation-list-header\">\n"
    << "        Signal blocked contacts\n"
    << "      </div>\n"
    << '\n'
    << "      <div class=\"conversation-list\">\n"
    << '\n';

  // write blocked list
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    long long int rec_id = results.valueAsInt(i, "_id");
    bool hasavatar = getRecipientInfoFromMap(recipient_info, rec_id).hasavatar;
    bool isgroup = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM groups WHERE group_id = (SELECT IFNULL(group_id, 0) FROM recipient WHERE _id = ?)", rec_id, -1) == 1;
    bool emoji_initial = getRecipientInfoFromMap(recipient_info, rec_id).initial_is_emoji;

    //Logger::message(rec_id, " : ", sanitizeFilename(getRecipientInfoFromMap(recipient_info, rec_id).display_name));

    outputfile
      << "        <div class=\"conversation-list-item\">\n"
      << "          <div class=\"avatar"
      << ((hasavatar || !isgroup) ? " avatar-" + bepaald::toString(rec_id) : "")
      << ((isgroup && !hasavatar) ? " group-avatar-icon" : "")
      << ((emoji_initial && !hasavatar) ? " avatar-emoji-initial" : "") << "\">\n"
      << ((!hasavatar && !isgroup) ? "            <span>" + getRecipientInfoFromMap(recipient_info, rec_id).initial + "</span>\n" : "")
      << "          </div>\n"
      << "          <div class=\"name-and-snippet\">\n"
      << "            <pre class=\"name\">" << HTMLescapeString(getRecipientInfoFromMap(recipient_info, rec_id).display_name) << "</pre>\n"
      << "          </div>\n"
      << "        </div>\n";
  }

  if (listempty)
  {
    outputfile
      << "        <div class=\"conversation-list-item\">\n"
      << "            <span style=\"font-weight: bold; font-size: 18px\">(none)</span>\n"
      << "        </div>\n";
  }

  // write end of html
  outputfile
    << '\n'
    << "        <div id=\"menu\">\n"
    << "          <a href=\"index.html\">\n"
    << "            <div class=\"menu-item\">\n"
    << "              <div class=\"menu-icon nav-up\">\n"
    << "              </div>\n"
    << "              <div>\n"
    << "                index\n"
    << "              </div>\n"
    << "            </div>\n"
    << "          </a>\n"
    << "        </div>\n"
    << '\n';

  if (themeswitching)
  {
    outputfile
      << "      <div id=\"theme\">\n"
      << "        <div class=\"menu-item\">\n"
      << "          <label for=\"theme-switch\">\n"
      << "            <span class=\"menu-icon themebutton\">\n"
      << "           </span>\n"
      << "         </label>\n"
      << "        </div>\n"
      << "      </div>\n"
      << '\n';
  }

  outputfile
    << "      </div>\n"
    << "    </div>\n";

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

  outputfile
    << '\n'
    << "  </body>\n"
    << "</html>\n";

  return true;
}
