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

bool SignalBackup::HTMLwriteSettings(std::string const &dir, bool overwrite, bool append, bool light [[maybe_unused]],
                                     bool themeswitching [[maybe_unused]], std::string const &exportdetails [[maybe_unused]]) const
{
  Logger::message("Writing settings.html...");

  if (bepaald::fileOrDirExists(dir + "/settings.html"))
  {
    if (!overwrite && !append)
    {
      Logger::error("'", dir, "/settings.html' exists. Use --overwrite to overwrite.");
      return false;
    }
  }
  std::ofstream outputfile(WIN_LONGPATH(dir + "/settings.html"), std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", dir, "/settings.html' for writing.");
    return false;
  }

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  outputfile <<
    "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << // %F an d%T do not work on minGW
    " by signalbackup-tools (" << VERSIONDATE << "). "
    "Input database version: " << d_databaseversion << ". -->\n";

  outputfile <<
    "<!DOCTYPE html>\n"
    "<html>\n"
    "  <head>\n"
    "    <meta charset=\"utf-8\">\n"
    "    <title>Signal settings</title>\n"
    "    <style>\n"
    "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {\n"
    "        /* " << (light ? "light" : "dark") << " */\n"
    "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << "\n"
    "        --body-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --settingslistheader-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --settingslist-bc: " << (light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
    "        --settingslist-c: " << (light ? "#000000;" : "#FFFFFF;") << "\n"
    "        --keyvaluecontainer-b: " << (light ? "#E7EBF3;" : "#303133;") << "\n"
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
      "        --settingslistheader-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --settingslist-bc: " << (!light ? "#FBFCFF;" : "#1B1C1F;") << "\n"
      "        --settingslist-c: " << (!light ? "#000000;" : "#FFFFFF;") << "\n"
      "        --keyvaluecontainer-b: " << (!light ? "#E7EBF3;" : "#303133;") << "\n"
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
    "      .settings-list-header {\n"
    "        text-align: center;\n"
    "        font-size: xx-large;\n"
    "        color: var(--settingslistheader-c);\n"
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
    "      .settings-list {\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        width: fit-content;\n"
    "        margin-top: 10px;\n"
    "        margin-bottom: 100px;\n"
    "        margin-right: auto;\n"
    "        margin-left: auto;\n"
    "        padding: 30px;\n"
    "        background-color: var(--settingslist-bc);\n"
    "        color: var(--settingslist-c);\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        border-radius: 10px;\n"
    "      }\n"
    "\n"
    "      .keyvaluepair-container {\n"
    "        border-radius: .6em;\n"
    "        margin: 7px 0;\n"
    "        padding: 10px 30px;\n"
    "        position: relative;\n"
    "        background: var(--keyvaluecontainer-b);\n"
    "      }\n"
    "\n"
    "      .keyvalue-item {\n"
    "        padding: 10px;\n"
    "        margin: auto;\n"
    "      }\n"
    "\n"
    "      .keyvalue {\n"
    "        display: flex;\n"
    "        flex-direction: column;\n"
    "        margin-left: 5px;\n"
    "        margin-right: 5px;\n"
    "      }\n"
    "\n"
    "      .key {\n"
    "        font-weight: bold;\n"
    "        font-size: 18px;\n"
    "        font-family: Roboto, \"Noto Sans\", \"Liberation Sans\", OpenSans, sans-serif;\n"
    "        margin: 0px;\n"
    "        padding: 0px;\n"
    "      }\n"
    "\n"
    "      .value {\n"
    "        font-style: italic;\n"
    "      }\n"
    "\n"
    "      .menu-item > div {\n"
    "        margin-right: 5px;\n"
    "      }\n"
    "\n"
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
    "      #theme {\n"
    "        display: flex;\n"
    "        flex-direction: row;\n"
    "        position: fixed;\n"
    "        top: 20px;\n"
    "        right: 20px;\n"
    "      }\n"
    "\n";

  if (themeswitching)
    outputfile <<
      "      .themebutton {\n"
      "        display: block;\n"
      "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 25 25\" fill=\"white\" stroke=\"white\"><defs><path id=\"s\" d=\"m12.5,2.5 2.5,2.5c0.27,0.27 0.63,0.43 1,0.43L19.62,5.42h1.79l-3.8e-5,-0.4C21.41,4.24 20.77,3.61 20,3.6l-3.8,0 -2.64,-2.65C13,0.4 12,0.4 11.5,0.96l-0.28,0.28z\"/></defs><path d=\"m12,8.13c0,-0.4 0.35,-0.8 0.8,-0.76 2.7,0.14 4.86,2.4 4.86,5.13 0,2.75 -2.15,5 -4.86,5.14 -0.45,0 -0.8,-0.35 -0.8,-0.77z\"/><use href=\"%23s\"/><use href=\"%23s\" transform=\"rotate(45 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(90 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(135 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(180 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(225 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(270 12.5 12.5)\"/><use href=\"%23s\" transform=\"rotate(315 12.5 12.5)\"/></svg>');\n"
      "        filter: var(--icon-f);\n"
      "      }\n"
      "\n";

  outputfile <<
    "      .nav-up {\n"
    "        background-image: url('data:image/svg+xml;utf-8,<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" viewBox=\"0 0 20 20\" fill=\"white\" stroke=\"white\"><path d=\"M9.5,17.5l1.1,-1.1l-4.9,-4.9l-1.1,-0.8H17V9.2H4.6l1.1,-0.8l4.9,-5L9.5,2.5L2,10L9.5,17.5z\"/></svg>');\n"
    "        filter: var(--icon-f);\n"
    "      }\n"
    "\n"
    "      @media print {\n"
    "        .settings-list-header {\n"
    "          padding: 0;\n"
    "        }\n"
    "\n"
    "        .settings-list-item {\n"
    "          break-inside: avoid;\n"
    "        }\n"
    "\n"
    "        .settings-list {\n"
    "          margin: 0 auto;\n"
    "          display: block;\n"
    "          border-radius: 0;\n"
    "        }\n"
    "\n"
    "        .export-details {\n"
    "          display: grid;\n"
    "        }\n"
    "\n"
    "        #menu {\n"
    "          display: none;\n"
    "        }\n"
    "\n"
    "        #theme {\n"
    "          display: none;\n"
    "        }\n"
    "\n";

  if (!exportdetails.empty())
    outputfile <<
      "        .export-details {\n"
      "          display: grid;\n"
      "        }\n"
      "\n";

  outputfile <<
    "      }\n"
    "    </style>\n"
    "  </head>\n";

  // BODY
  outputfile <<
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
    "    <input type=\"checkbox\" id=\"theme-switch\">\n"
    "\n"
    "    <div id=\"page\">\n"
    "\n"
    "    <div class=\"settings-list-header\">\n"
    "      Signal settings\n"
    "    </div>\n"
    "\n"
    "    <div class=\"settings-list\">\n"
    "\n";

  bool hasoutput = false;

  // output keyvalueframes if "settings.*" (others may include private keys and such)
  if (std::any_of(d_keyvalueframes.begin(), d_keyvalueframes.end(), [](auto const &kv) STATICLAMBDA { return STRING_STARTS_WITH(kv->key(), "settings."); } ))
  {
    hasoutput = true;

    outputfile <<
      "      <div class=\"header\">From KeyValue frames</div>\n"
      "      <div class=\"keyvaluepair-container\">\n"
      "\n";

    for (auto const &kv : d_keyvalueframes)
    {
      if (STRING_STARTS_WITH(kv->key(), "settings."))
      {
        std::string key = kv->key();
        std::string value = kv->value();
        std::string valuetype = kv->valueType();

        if (valuetype == "STRING")
        {
          value = "\"" + value + "\"";
          HTMLescapeString(&value);
        }
        else if (valuetype == "BLOB")
          value = "(base64: )" + value;
        else if (valuetype == "FLOAT")
          value = "(base64 float: )" + value;

        outputfile <<
          "        <div class=\"keyvalue-item\">\n"
          "          <div class=\"keyvalue\">\n"
          "            <div class=\"key\">" << key << "</div>\n"
          "            <span class=\"value\">" << value << "</span>\n"
          "          </div>\n"
          "        </div>\n"
          "\n";
      }
    }
    outputfile <<
      "      </div>\n";
  }

  // output sharedpreferenceframes (if not private keys (these were in these frames only in (very) old databases))
  if (std::any_of(d_sharedpreferenceframes.begin(), d_sharedpreferenceframes.end(),
                  [](auto const &sp) STATICLAMBDA { return !STRING_STARTS_WITH(sp->key(), "pref_identity_") && sp->key().find("private") == std::string::npos; } ))
  {
    hasoutput = true;

    outputfile <<
      "      <div class=\"header\">From SharedPreference frames</div>\n"
      "      <div class=\"keyvaluepair-container\">\n"
      "\n";

    for (auto const &sp : d_sharedpreferenceframes)
    {
      std::string key = sp->key();
      if (STRING_STARTS_WITH(key, "pref_identity_") || (key.find("private") != std::string::npos))
        continue;
      std::vector<std::string> values = sp->value();
      std::string valuetype = sp->valueType();

      if (valuetype == "STRING" || valuetype == "STRINGSET")
      {
        for (unsigned int i = 0; i < values.size(); ++i)
        {
          values[i] = "\"" + values[i] + "\"";
          HTMLescapeString(&values[i]);
        }
      }

      if (values.empty())
      {
        if (valuetype.empty())
          values.emplace_back("???");
        else
          values.emplace_back("(empty)");
      }

      outputfile <<
        "        <div class=\"keyvalue-item\">\n"
        "          <div class=\"keyvalue\">\n"
        "            <div class=\"key\">" << key << "</div>\n";
      for (unsigned int i = 0; i < values.size(); ++i)
        outputfile <<
          "            <span class=\"value\">" << values[i] << "</span>\n";
      outputfile <<
        "          </div>\n"
        "        </div>\n"
        "\n";
    }
    outputfile <<
      "      </div>\n";
  }

  if (!hasoutput)
    outputfile <<
      "        <div class=\"conversation-list-item\">\n"
      "            <span style=\"font-weight: bold; font-size: 18px\">(none)</span>\n"
      "        </div>\n";

  outputfile <<
    "    </div>\n"
    "\n";
  // write end of html

  outputfile <<
    "\n"
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
    outputfile <<
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

  outputfile <<
    "    </div>\n";

  if (!exportdetails.empty())
    outputfile << "\n" << exportdetails << "\n";

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
    "\n"
    "  </body>\n"
    "</html>\n";

  return true;
}
