/*
  Copyright (C) 2024-2025  Selwin van Dijk

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
#include "../mimetypes/mimetypes.h"

bool SignalBackup::HTMLwriteStickerpacks(std::string const &directory, bool overwrite, bool append,
                                         bool light, bool themeswitching, std::string const &exportdetails) const
{
  Logger::message("Writing stickerpacks.html...");

  if (!d_database.containsTable("sticker"))
  {
    Logger::error("No stickers in database");
    return false;
  }

  SqliteDB::QueryResults res_installed;
  if (!d_database.exec("SELECT _id, sticker_id, pack_id, pack_title, pack_author, emoji "
                       "FROM sticker WHERE installed IS 1 AND cover IS 0 ORDER BY pack_title, pack_id, sticker_id", &res_installed))
    return false;
  SqliteDB::QueryResults res_known;
  if (!d_database.exec("SELECT _id, sticker_id, pack_id, pack_title, pack_author, emoji "
                       "FROM sticker WHERE installed IS 0 AND cover IS 1 ORDER BY pack_title, pack_id, sticker_id", &res_known))
    return false;

  if (res_installed.rows() == 0 && res_known.rows() == 0)
  {
    Logger::warning("No stickerpacks found in database");
    return false;
  }

  if (d_verbose) [[unlikely]]
    Logger::message("Exporting ", res_installed.rows(), " installed and ", res_known.rows(), " known stickerpacks");

  if (bepaald::fileOrDirExists(directory + "/stickerpacks.html"))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", directory, "/stickerpacks.html' exists. Use --overwrite to overwrite.");
      return false;
    }
  }

  // open file
  std::ofstream stickerhtml(WIN_LONGPATH(directory + "/stickerpacks.html"), std::ios_base::binary);
  if (!stickerhtml.is_open())
  {
    Logger::error("Failed to open '", directory, "/stickerpacks.html' for writing");
    return false;
  }

  // start html output
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  stickerhtml <<
    "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") <<
    " by signalbackup-tools (" << VERSIONDATE << "). "
    "Input database version: " << d_databaseversion << ". -->\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "  <head>\n"
    "    <meta charset=\"utf-8\">\n"
    "    <title>Signal stickerpacks</title>\n";

  // STYLE
  stickerhtml <<
    "    <style>\n"
    "      :root {\n"
    "        --imgsize: 150px;\n"
    "        --cellpadding: 10px;\n"
    "        --cellmargin: 5px;\n"
    "        --cellsize: calc(var(--imgsize) + 2 * var(--cellpadding) + 2 * var(--cellmargin));\n"
    "      }\n"
    "\n"
    "      :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {\n"
    "        /* " << (light ? "light" : "dark") << "*/\n"
    "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << "\n"
    "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --stickerlistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --stickerlist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
    "        --stickeritem-bc: " << (light ? "#E7EBF3;" : "#303133;") << "\n"
    "        --stickerlist-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --menuitem-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --icon-f: " << (light ? "brightness(0);" : "none;") << "\n"
    "        --msgreactioninfo-bc: " << (light ? "#D2D6DE;" : "#505050;") << "\n"
    "      }\n"
    "\n";

  if (themeswitching)
  {
    stickerhtml <<
      "      :root[data-theme=\"" + (!light ? "light"s : "dark") + "\"] {\n"
      "        /* " << (!light ? "light" : "dark") << "*/\n"
      "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << "\n"
      "        --body-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --stickerlistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --stickerlist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
      "        --stickeritem-bc: " << (!light ? "#E7EBF3;" : "#303133;") << "\n"
      "        --stickerlist-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --menuitem-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --icon-f: " << (!light ? "brightness(0);" : "none;") << "\n"
      "        --msgreactioninfo-bc: " << (!light ? "#D2D6DE;" : "#505050;") << "\n"
      "      }\n"
      "\n";
  }
  stickerhtml <<
    "      body {\n"
    "        margin: 0px;\n"
    "        padding: 0px;\n"
    "        width: 100%;\n"
    "        background-color: var(--body-bgc);\n"
    "      }\n"
    "\n"
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
    stickerhtml <<
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

  stickerhtml <<
    "      #menu {\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        position: fixed;\n"
    "        top: 20px;\n"
    "        left: 20px;\n"
    "      }\n"
    "\n"
    "      #menu a:link,\n"
    "      #menu a:visited,\n"
    "      #menu a:hover,\n"
    "      #menu a:active {\n"
    "        color: #FFFFFF;\n"
    "        text-decoration: none;\n"
    "      }\n"
    "\n"
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
    "\n"
    "      #bottom {\n"
    "        display: flex;\n"
    "        position: fixed;\n"
    "        bottom: 20px;\n"
    "        right: 20px;\n"
    "      }\n"
    "\n"
    "      .menu-item-bottom {\n"
    "        display: flex;\n"
    "        color: var(--menuitem-c);\n"
    "        border-radius: 50%;\n"
    "        background-color: var(--msgreactioninfo-bc);\n"
    "        width: 40px;\n"
    "        height: 40px;\n"
    "        align-items: center;\n"
    "        justify-content: center;\n"
    "        padding: 0px;\n"
    "      }\n"
    "\n"
    "      .nav-bottom {\n"
    "        transform: rotate(270deg);\n"
    "        width: 25px;\n"
    "        height: 25px;\n"
    "      }\n"
    "\n"
    "      .nav-one {\n"
    "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"none\" stroke=\"white\"><path style=\"stroke-width: 3;\" d=\"M 13.796428,2.9378689 6.7339026,10.000394 13.795641,17.062131\"/></svg>');\n"
    "        filter: var(--icon-f);\n"
    "      }\n"
    "\n"
    "      .menu-item > div {\n"
    "        margin-right: 5px;\n"
    "      }\n"
    "\n";

  if (themeswitching)
  {
    stickerhtml <<
      "      #theme {\n"
      "        display: flex;\n"
      "        flex-direction: column;\n"
      "        position: fixed;\n"
      "        top: 20px;\n"
      "        right: 20px;\n"
      "      }\n"
      "\n"
      "      .themebutton {\n"
      "        display: block;\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 25 25\" fill=\"white\" stroke=\"white\"><defs><path id=\"s\" d=\"m12.5,2.5 2.5,2.5c0.27,0.27 0.63,0.43 1,0.43L19.62,5.42h1.79l-3.8e-5,-0.4C21.41,4.24 20.77,3.61 20,3.6l-3.8,0 -2.64,-2.65C13,0.4 12,0.4 11.5,0.96l-0.28,0.28z\"/></defs><path d=\"m12,8.13c0,-0.4 0.35,-0.8 0.8,-0.76 2.7,0.14 4.86,2.4 4.86,5.13 0,2.75 -2.15,5 -4.86,5.14 -0.45,0 -0.8,-0.35 -0.8,-0.77z\"/><use href=\"%23s\"/><use href=\"%23s\" transform=\"rotate(45 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(90 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(135 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(180 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(225 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(270 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(315 12.5 12.5)\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "      }\n"
      "\n";
  }
  stickerhtml <<
    "      .nav-up {\n"
    "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"/></svg>');\n"
    "        filter: var(--icon-f);\n"
    "      }\n"
    "\n"
    "      .sticker-list-header {\n"
    "        text-align: center;\n"
    "        font-size: xx-large;\n"
    "        color: var(--stickerlistheader-c);\n"
    "        padding: 10px;\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "      }\n"
    "\n"
    "      .sticker-list {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "        flex-wrap: wrap;\n"
    "        width: fit-content;\n"
    "        max-width: calc(5 * var(--cellsize));\n"
    "        margin-top: 10px;\n"
    "        margin-bottom: 100px;\n"
    "        margin-right: auto;\n"
    "        margin-left: auto;\n"
    "        padding: 5px 30px 30px 30px;\n"
    "        background-color: var(--stickerlist-bc);\n"
    "        color: var(--stickerlist-c);\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        border-radius: 10px;\n"
    "      }\n"
    "\n"
    "      .pack-status {\n"
    "        font-weight: bold;\n"
    "        margin-top: 20px;\n"
    "        margin-left: 10px;\n"
    "        flex-basis: 100%;\n"
    "      }\n"
    "\n"
    "      .sticker-pack-header {\n"
    "        flex-basis: 100%;\n"
    "        margin-bottom: 20px;\n"
    "        margin-top: 20px;\n"
    "      }\n"
    "      .sticker-pack-header-container {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "      }\n"
    "      .sticker-pack-header-cover {\n"
    "        height: 60px;\n"
    "        margin-left: 5px;\n"
    "        margin-right: 5px;\n"
    "      }\n"
    "      .sticker-pack-header-cover img {\n"
    "        height: 100%;\n"
    "        width: 100%;\n"
    "        object-fit: contain;\n"
    "      }\n"
    "      .sticker-header-title {\n"
    "        font-size: x-large;\n"
    "      }\n"
    "      .sticker-header-author {\n"
    "        font-size: large;\n"
    "        margin-left: 2px;\n"
    "      }\n"
    "\n"
    "      .sticker-list-item {\n"
    "        display: block;\n"
    "        padding: var(--cellpadding);\n"
    "        background-color: var(--stickeritem-bc);\n"
    "        margin: var(--cellmargin);\n"
    "        border-radius: 0.6em;\n"
    "        width: min-content;\n"
    "      }\n"
    "\n"
    "      .sticker {\n"
    "        display: block;\n"
    "        width: var(--imgsize);\n"
    "        height: var(--imgsize);\n"
    "        margin-bottom: 5px;\n"
    "        margin-left: auto;\n"
    "        margin-right: auto;\n"
    "      }\n"
    "      .sticker form,\n"
    "      .sticker label {\n"
    "        display: block;\n"
    "        width: 100%;\n"
    "        height: 100%;\n"
    "      }\n"
    "      .sticker input[type=checkbox] {\n"
    "        display: none;\n"
    "      }\n"
    "      .sticker img {\n"
    "        width: 100%;\n"
    "        height: 100%;\n"
    "        object-fit: contain;\n"
    "        position: relative;\n"
    "      }\n"
    "      .sticker img {\n"
    "        cursor: zoom-in;\n"
    "        z-index: 1;\n"
    "        background-color: #00000000;\n"
    "        border-radius: 0px;\n"
    "        border: 0px solid var(--body-bgc);\n"
    "        padding: 0px;\n"
    "        transition: background-color .25s ease, border-radius .25s ease, border .25s ease, z-index .25s step-end, transform .25s ease, padding 0.25s ease;\n"
    "      }\n"
    "\n"
    "      .sticker input[type=checkbox]:checked ~ label > img {\n"
    "        cursor: zoom-out;\n"
    "        z-index: 2;\n"
    "        transform: scale(3);\n"
    "        background-color: var(--stickeritem-bc);\n"
    "        border-radius: 9px;\n"
    "        border: 1px solid var(--body-bgc);\n"
    "        padding: 3px;\n"
    "        transition: background-color .25s ease, border-radius .25s ease, border .25s ease, z-index .25s step-start, transform .25s ease, padding 0.25s ease;\n"
    "      }\n"
    "\n"
    "      .footer {\n"
    "        width: fit-content;\n"
    "        margin-left: auto;\n"
    "        margin-right: auto;\n"
    "      }\n"
    "\n"
    "      .footer .emoji {\n"
    "        font-family: \"Apple Color Emoji\", \"Noto Color Emoji\", sans-serif;\n"
    "      }\n"
    "\n"
    "      @media screen and (max-width: 975px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(4 * var(--cellsize));\n"
    "        }\n"
    "      }\n"
    "      @media screen and (max-width: 795px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(3 * var(--cellsize)); \n"
    "        }\n"
    "      }\n"
    "      @media screen and (max-width: 615px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(2 * var(--cellsize));\n"
    "        }\n"
    "      }\n"
    "      @media screen and (max-width: 435px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(1 * var(--cellsize));\n"
    "        }\n"
    "      }\n"
    "\n"
    "      @media print {\n"
    "        .sticker-list {\n"
    "          max-width: 880px\n"
    "        }\n"
    "      }\n"
    "      @media print and (max-width: 895px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(880px - (var(--imgsize) + 2 * var(--cellpadding) + 2 * 3px));\n"
    "        }\n"
    "      }\n"
    "      @media print and (max-width: 719px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(880px - 2 * (var(--imgsize) + 2 * var(--cellpadding) + 2 * 3px));\n"
    "        }\n"
    "      }\n"
    "      @media print and (max-width: 543px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(880px - 3 * (var(--imgsize) + 2 * var(--cellpadding) + 2 * 3px));\n"
    "        }\n"
    "      }\n"
    "      @media print and (max-width: 367px) {\n"
    "        .sticker-list {\n"
    "          max-width: calc(880px - 4 * (var(--imgsize) + 2 * var(--cellpadding) + 2 * 3px))\n"
    "        }\n"
    "      }\n"
    "\n"
    "      @media print {\n"
    "        .sticker-list-header {\n"
    "          padding: 0;\n"
    "        }\n"
    "\n"
    "       .sticker-list-item {\n"
    "          break-inside: avoid;\n"
    "          margin: 3px;\n"
    "        }\n"
    "\n"
    "        .sticker-list {\n"
    "          border-radius: 0;\n"
    "          padding-left: 0;\n"
    "          padding-right: 0;\n"
    "        }\n"
    "\n"
    "        .sticker img,\n"
    "        .sticker input[type=checkbox]:checked ~ label > img { \n"
    "          z-index: 1;\n"
    "          cursor: default;\n"
    "          background-color: #00000000;\n"
    "          border-radius: 0px;\n"
    "          border: none;\n"
    "          padding: 0px;\n"
    "          transition: none;\n"
    "          transform: none;\n"
    "        }\n"
    "\n";

  if (!exportdetails.empty())
    stickerhtml <<
      "        .export-details {\n"
      "          display: grid;\n"
      "        }\n"
      "\n";

  stickerhtml <<
    "        #bottom,\n"
    "        #theme,\n"
    "        #menu {\n"
    "          display: none;\n"
    "        }\n"
    "      }\n"
    "\n"
    "    </style>\n"
    "  </head>\n";

  /* BODY */

  stickerhtml
    << "  <body>\n";
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

  stickerhtml <<
    "    <input type=\"checkbox\" id=\"theme-switch\">\n"
    "\n"
    "    <div id=\"page\">\n"
    "\n"
    "      <div class=\"sticker-list-header\">\n"
    "        Stickerpacks\n"
    "      </div>\n"
    "\n"
    "      <div class=\"sticker-list\">\n"
    "\n";

  // iterate stickers
  std::string prevpackid;
  for (auto const *const res : {&res_installed, &res_known})
  {
    if (res == &res_installed && res->rows())
      stickerhtml << "        <div class=\"pack-status\">Installed</div>\n";
    if (res == &res_known && res->rows())
      stickerhtml << "        <div class=\"pack-status\">Available</div>\n";

    for (unsigned int i = 0; i < res->rows(); ++i)
    {
      std::string packid = res->valueAsString(i, "pack_id");
      if (packid != prevpackid)  // output header!
      {
        std::string packtitle = res->valueAsString(i, "pack_title");
        HTMLescapeString(&packtitle);

        std::string packauthor = res->valueAsString(i, "pack_author");
        HTMLescapeString(&packauthor);

        if (d_verbose) [[unlikely]]
          Logger::message("Exporting '", packtitle, "' by ", packauthor);

        // get cover:
        long long int cover_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM sticker WHERE pack_id = ? AND cover = 1", packid, -1);

        stickerhtml
          << "        <div class=\"sticker-pack-header\">\n"
          << "          <div class=\"sticker-pack-header-container\">\n";
        std::string ext("bin");
        if (cover_id == -1 || !writeStickerToDisk(cover_id, packid, directory, overwrite, append, &ext)) [[unlikely]]
          Logger::message("No cover found for stickerpack ", packid);
        else
          stickerhtml <<
            "            <div class=\"sticker-pack-header-cover\">\n"
            "              <img src=\"stickers/" << packid << "/Sticker_" << cover_id << "." << ext << "\" alt=\"cover\">\n"
            "            </div>\n";
        stickerhtml <<
          "            <div>\n"
          "              <div class=\"sticker-header-title\">" << packtitle << "</div>\n"
          "              <div class=\"sticker-header-author\">" << packauthor << "</div>\n"
          "            </div>\n"
          "          </div>\n"
          "        </div>\n"
          "\n";

        prevpackid = packid;

        if (res == &res_known) // for packs known, but not installed, stop here (only output header)
          continue;
      }

      long long int id = res->valueAsInt(i, "_id");
      if (id < 0)
      {
        Logger::warning("Unexpected id value");
        continue;
      }
      long long int stickerid = res->valueAsInt(i, "sticker_id");
      std::string emoji = res->valueAsString(i, "emoji");

      // write actual file to disk
      std::string ext("bin");
      if (!writeStickerToDisk(id, packid, directory, overwrite, append, &ext)) [[unlikely]]
      {
        Logger::warning("There was a problem writing the sitcker data to file");
        continue;
      }

      //Logger::message("Sticker ", stickerid, ": ", emoji);
      stickerhtml <<
        "        <div class=\"sticker-list-item\">\n"
        "          <div class=\"sticker\">\n"
        "            <form autocomplete=\"off\">\n"
        "              <input type=\"checkbox\" id=\"zoomCheck-" << packid << "-" << id << "\">\n"
        "              <label for=\"zoomCheck-" << packid << "-" << id << "\">\n"
        "                <img src=\"stickers/" << packid << "/Sticker_" << id << "." << ext << "\" alt=\"Sticker_" << id << "." << ext << "\">\n"
        "              </label>\n"
        "            </form>\n"
        "          </div>\n"
        "          <div class=\"footer\">" << stickerid << ". <span class=\"emoji\">" << emoji << "</span></div>\n"
        "        </div>\n"
        "\n";
    }
  }

  // write end of html
  stickerhtml <<
    "         <div id=\"bottom\">\n"
    "           <a href=\"#pagebottom\" title=\"Jump to bottom\">\n"
    "             <div class=\"menu-item-bottom\">\n"
    "               <span class=\"menu-icon nav-one nav-bottom\"></span>\n"
    "             </div>\n"
    "           </a>\n"
    "        </div>\n"
    "        <div id=\"menu\">\n"
    "          <a href=\"index.html\">\n"
    "            <div class=\"menu-item\">\n"
    "              <div class=\"menu-icon nav-up\">\n"
    "              </div>\n"
    "              <div>\n"
    "                index\n"
    "              </div>\n"
    "            </div>\n"
    "          </a>\n"
    "        </div>\n"
    "\n";

  if (themeswitching)
  {
    stickerhtml <<
      "      <div id=\"theme\">\n"
      "        <div class=\"menu-item\">\n"
      "          <label for=\"theme-switch\">\n"
      "            <span class=\"menu-icon themebutton\">\n"
      "           </span>\n"
      "         </label>\n"
      "        </div>\n"
      "      </div>\n"
      "\n";
  }

  stickerhtml <<
    "    </div>\n"
    "  </div>\n";

  if (!exportdetails.empty())
    stickerhtml << '\n' << exportdetails << '\n';

  stickerhtml <<
    "  <a id=\"pagebottom\"></a>\n";


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

  stickerhtml <<
    "  </body>\n"
    "</html>\n";

  return true;
}

#include "../scopeguard/scopeguard.h"

bool SignalBackup::writeStickerToDisk(long long int id, std::string const &packid, std::string const &directory,
                                      bool overwrite, bool append, std::string *extension) const
{
  // write actual file to disk

  // find the sticker with id
  auto it = d_stickers.find(id);

  if (it == d_stickers.end()) [[unlikely]]
  {
    Logger::warning("Failed to find sticker (id: ", id, ")");
    return false;
  }

  // make sure a 'stickers/' subdirectory exists, and 'stickers/stickerpack_id/' exists
  for (auto const &subdir : {"/stickers"s, "/stickers/"s + packid})
  {
    if (!bepaald::fileOrDirExists(directory + subdir))
    {
      if (!bepaald::createDir(directory + subdir))
      {
        Logger::error("Failed to create directory `", directory, "/", subdir, "'");
        return false;
      }
    }
    else if (!bepaald::isDir(directory + subdir))
    {
      Logger::error("Failed to create directory `", directory, "/", subdir, "'");
      return false;
    }
  }

  StickerFrame *s = it->second.get();

  // get the data, so the mimetype is determined
  unsigned char const *stickerdata = s->attachmentData();
  ScopeGuard clear_sticker_data([&](){s->clearData();});

  std::optional<std::string> mimetype = s->mimetype();
  if (mimetype)
    *extension = MimeTypes::getExtension(*mimetype, "bin");

  std::string stickerdatapath = directory + "/stickers/" + packid + "/Sticker_" + bepaald::toString(id) + "." + *extension;
  // check actual sticker file
  if (bepaald::fileOrDirExists(stickerdatapath) && !overwrite)
  {
    if (!append)
    {
      Logger::error("Avatar file exists. Not overwriting");
      return false;
    }
    // file exists, we are appending, we assume we're done
    return true;
  }
  std::ofstream stickerstream(WIN_LONGPATH(stickerdatapath), std::ios_base::binary);
  if (!stickerstream.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open '", stickerdatapath, "' for writing");
    return false;
  }
  if (!stickerstream.write(reinterpret_cast<char const *>(stickerdata), s->attachmentSize())) [[unlikely]]
  {
    Logger::error("Failed to write sticker data to file");
    return false;
  }
  return true;
}
