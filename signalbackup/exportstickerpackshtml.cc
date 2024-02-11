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

bool SignalBackup::exportStickerPacksHTML(std::string const &directory, bool overwrite, bool append,
                                          bool light, bool themeswitching) const
{
  Logger::message("Writing stickerpacks.html...");

  if (!d_database.containsTable("sticker"))
  {
    Logger::error("No stickers in database");
    return false;
  }

  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT _id, sticker_id, pack_id, pack_title, pack_author, emoji "
                       "FROM sticker WHERE installed IS 1 AND cover IS 0 ORDER BY pack_id, sticker_id", &res))
    return false;
  if (res.rows() == 0)
  {
    Logger::error("No installed stickerpacks found in database");
    return false;
  }

  // set up dir
  if (!prepareOutputDirectory(directory, overwrite, true, append))
    return false;

  // write start of html
  std::ofstream stickerhtml(directory + "/stickerpacks.html", std::ios_base::binary);
  if (!stickerhtml.is_open())
  {
    Logger::error("Failed to open '", directory, "/stickerpacks.html' for writing");
    return false;
  }

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  stickerhtml << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
              << " by signalbackup-tools (" << VERSIONDATE << "). "
              << "Input database version: " << d_databaseversion << ". -->" << std::endl
              << "<!DOCTYPE html>" << std::endl
              << "<html>" << std::endl
              << "  <head>" << std::endl
              << "    <meta charset=\"utf-8\">" << std::endl
              << "    <title>Signal stickerpacks</title>" << std::endl;

  // STYLE
  stickerhtml << "    <style>" << std::endl
              << "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl
              << "        /* " << (light ? "light" : "dark") << "*/" << std::endl
              << "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << std::endl
              << "        --stickerlistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
              << "        --stickerlist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << std::endl
              << "        --stickeritem-bc: " << (light ? "#E7EBF3;" : "#303133;") << std::endl
              << "        --stickerlist-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
              << "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << std::endl
              << "        --icon-f: " << (light ? "brightness(0);" : "none;") << std::endl
              << "      }" << std::endl
              << std::endl;

  if (themeswitching)
  {
    stickerhtml << "    :root" << (themeswitching ? "[data-theme=\"" + (!light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl
                << "        /* " << (!light ? "light" : "dark") << "*/" << std::endl
                << "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << std::endl
                << "        --stickerlistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
                << "        --stickerlist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << std::endl
                << "        --stickeritem-bc: " << (!light ? "#E7EBF3;" : "#303133;") << std::endl
                << "        --stickerlist-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
                << "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << std::endl
                << "        --icon-f: " << (!light ? "brightness(0);" : "none;") << std::endl
                << "      }" << std::endl
                << std::endl;
  }
  stickerhtml
    << "      body {" << std::endl
    << "        margin: 0px;" << std::endl
    << "        padding: 0px;" << std::endl
    << "        width: 100%;" << std::endl
    << "      }" << std::endl
    << std::endl
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
    << "        min-height: 100vh;" << std::endl
    << "      }" << std::endl
    << std::endl
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
    << "      .sticker-list-header {" << std::endl
    << "        text-align: center;" << std::endl
    << "        font-size: xx-large;" << std::endl
    << "        color: var(--stickerlistheader-c);" << std::endl
    << "        padding: 10px;" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .sticker-list {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: row;" << std::endl
    << "        flex-wrap: wrap;" << std::endl
    << "        width: fit-content;" << std::endl
    << "        max-width: 1000px;" << std::endl
    << "        margin-top: 10px;" << std::endl
    << "        margin-bottom: 100px;" << std::endl
    << "        margin-right: auto;" << std::endl
    << "        margin-left: auto;" << std::endl
    << "        padding: 5px 30px 30px 30px;" << std::endl
    << "        background-color: var(--stickerlist-bc);" << std::endl
    << "        color: var(--stickerlist-c);" << std::endl
    << "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;" << std::endl
    << "        border-radius: 10px;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .sticker-pack-header {" << std::endl
    << "        flex-basis: 100%;" << std::endl
    << "        margin-left: 30px;" << std::endl
    << "        margin-bottom: 20px;" << std::endl
    << "        margin-top: 20px;" << std::endl
    << "        font-size: x-large;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .sticker-list-item {" << std::endl
    << "        display: flex;" << std::endl
    << "        flex-direction: column;" << std::endl
    << "        padding: 10px;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-items: center;" << std::endl
    << "        align-content: center;" << std::endl
    << std::endl
    << "        background-color: var(--stickeritem-bc);" << std::endl
    << "        margin: 5px;" << std::endl
    << "        border-radius: 0.6em;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .sticker {" << std::endl
    << "        display: flex;" << std::endl
    << "        width: 150px;" << std::endl
    << "        height: 150px;" << std::endl
    << "        justify-content: center;" << std::endl
    << "        align-items: center;" << std::endl
    << "      }" << std::endl
    << "      .sticker label {" << std::endl
    << "        display: flex;" << std::endl
    << "        max-width: 100%;" << std::endl
    << "        max-height: 100%;" << std::endl
    << "      }" << std::endl
    << "      .sticker input[type=checkbox] {" << std::endl
    << "        display: none;" << std::endl
    << "      }" << std::endl
    << "      .sticker img {" << std::endl
    << "        max-width: 100%;" << std::endl
    << "        max-height: 100%;" << std::endl
    << "        object-fit:contain;" << std::endl
    << "      }" << std::endl
    << "      .sticker img {" << std::endl
    << "        cursor: zoom-in;" << std::endl
    << "        z-index: 1;" << std::endl
    << "        position: relative;" << std::endl
    << "        background-color: #FF000000;" << std::endl
    << "        border-radius: 0px;" << std::endl
    << "        border: none;" << std::endl
    << "        transition: background-color .25s ease, border-radius .25s ease, border .25s ease, z-index .25s step-end, transform .25s ease;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .sticker input[type=checkbox]:checked ~ label > img {" << std::endl
    << "        border-radius: 0;" << std::endl
    << "        cursor: zoom-out;" << std::endl
    << "        z-index: 2;" << std::endl
    << "        position: relative;" << std::endl
    << "        transform: scale(2.5);" << std::endl
    << "        background-color: var(--stickeritem-bc);" << std::endl
    << "        border-radius: 0.6em;" << std::endl
    << "        border: 1px solid black;" << std::endl
    << "        transition: background-color .25s ease, border-radius .25s ease, border .25s ease, z-index .25s step-start, transform .25s ease;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      .footer .emoji {" << std::endl
    << "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;" << std::endl
    << "      }" << std::endl
    << std::endl
    << "      @media print {" << std::endl
    << "        .sticker-list-header {" << std::endl
    << "          padding: 0;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "       .sticker-list-item {" << std::endl
    << "          break-inside: avoid;" << std::endl
    << "        }" << std::endl
    << std::endl
    << "        .sticker-list {" << std::endl
    << "          margin: 0 auto;" << std::endl
    << "          display: block;" << std::endl
    << "          border-radius: 0;" << std::endl
    << "        }" << std::endl
    << std::endl
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
    << "  </head>" << std::endl;

  /* BODY */

  stickerhtml
    << "  <body>" << std::endl;
  if (themeswitching)
  {
    stickerhtml << R"(    <script>
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

  stickerhtml
    << "    <input type=\"checkbox\" id=\"theme-switch\">" << std::endl
    << std::endl
    << "    <div id=\"page\">" << std::endl
    << std::endl
    << "      <div class=\"sticker-list-header\">" << std::endl
    << "        Installed stickerpacks" << std::endl
    << "      </div>" << std::endl
    << std::endl
    << "      <div class=\"sticker-list\">" << std::endl
    << std::endl;

  // iterate stickers
  std::string prevpackid;
  for (uint i = 0; i < res.rows(); ++i)
  {
    std::string packid = res(i, "pack_id");
    if (packid != prevpackid)
    {
      // output header!
      std::string packtitle = res(i, "pack_title");
      std::string packauthor = res(i, "pack_author");

      //Logger::message("\"", packtitle, "\" by ", packauthor);
      stickerhtml
        << "        <div class=\"sticker-pack-header\">" << std::endl
        << "          " << packtitle << " by " << packauthor << std::endl
        << "        </div>" << std::endl
        << std::endl;

      prevpackid = packid;
    }

    long long int id = res.valueAsInt(i, "_id");
    if (id < 0)
    {
      Logger::warning("Unexpected id value");
      continue;
    }
    long long int stickerid = res.valueAsInt(i, "sticker_id");
    std::string emoji = res(i, "emoji");

    // output some data for this?
    //Logger::message("Sticker ", stickerid, ": ", emoji);
    stickerhtml
      << "        <div class=\"sticker-list-item\">" << std::endl
      << "          <div class=\"sticker\">" << std::endl
      << "            <input type=\"checkbox\" id=\"zoomCheck-" << packid << "-" << stickerid << "\">" << std::endl
      << "            <label for=\"zoomCheck-" << packid << "-" << stickerid << "\">" << std::endl
      << "              <img src=\"stickers/" << packid << "/Sticker_" << stickerid << ".bin\" alt=\"Sticker_" << stickerid << ".bin\">" << std::endl
      << "            </label>" << std::endl
      << "          </div>" << std::endl
      << "          <div class=\"footer\">" << stickerid << ". <span class=\"emoji\">" << emoji << "</span></div>" << std::endl
      << "        </div>" << std::endl
      << std::endl;

    // write actual file to disk
    auto it = std::find_if(d_stickers.begin(), d_stickers.end(), [id](auto const &s) { return s.first == static_cast<uint64_t>(id); });
    if (it == d_stickers.end())
    {
      Logger::warning("Failed to find sticker (id: ", id, ")");
      continue;
    }
    for (auto p : {"/stickers"s, "/stickers/"s + packid})
    {
      if (!bepaald::fileOrDirExists(directory + p))
      {
        if (!bepaald::createDir(directory + p))
        {
          Logger::error("Failed to create directory `", directory, p, "'");
          continue;
        }
      }
      else if (!bepaald::isDir(directory + p))
      {
        Logger::error("Failed to create directory `", directory, p, "'");
        continue;
      }
    }
    std::string stickerdatapath = directory + "/stickers/" + packid + "/Sticker_" + bepaald::toString(stickerid) + ".bin";
    // check actual sticker file
    if (bepaald::fileOrDirExists(stickerdatapath) && !overwrite)
    {
      if (!append)
        Logger::error("Avatar file exists. Not overwriting");
      continue;
    }
    std::ofstream stickerstream(stickerdatapath);
    if (!stickerstream.is_open())
    {
      Logger::error("Failed to open '", stickerdatapath, "' for writing");
      continue;
    }
    StickerFrame *s = it->second.get();
    stickerstream.write(reinterpret_cast<char *>(s->attachmentData()), s->attachmentSize());
    s->clearData();

  }

  // write end of html
  stickerhtml
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
    stickerhtml
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

  stickerhtml
    << "    </div>" << std::endl
    << "  </div>" << std::endl;


  if (themeswitching)
  {
    stickerhtml << R"(  <script>
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

  stickerhtml
    << "  </body>" << std::endl
    << "</html>" << std::endl;

  return true;
}
