/*
  Copyright (C) 2023  Selwin van Dijk

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
                                    std::map<long long int, RecipientInfo> *recipientinfo [[maybe_unused]], long long int notetoself_tid [[maybe_unused]],
                                    bool overwrite, bool append, bool light, bool themeswitching) const
{
  std::cout << "Writing calllog.html..." << std::endl;

  if (bepaald::fileOrDirExists(directory + "/calllog.html"))
  {
    if (!overwrite && ! append)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": '" << directory << "/calllog.html' exists. Use --overwrite to overwrite." << std::endl;
      return;
    }
  }
  std::ofstream outputfile(directory + "/calllog.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to open '" << directory << "/calllog.html' for writing." << std::endl;
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
  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT "
                       "_id,call_id,message_id,peer,type,direction,event,timestamp,ringer,deletion_timestamp "
                       "FROM call WHERE "
                       "message_id IN (SELECT DISTINCT _id FROM " + d_mms_table + " WHERE thread_id IN (" + threadlist + "))",
                       &results))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to query database for thread snippets." << std::endl;
    return;
  }

  results.prettyPrint();
  return;

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%F %T") // %F an d%T do not work on minGW
  outputfile << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
             << " by signalbackup-tools (" << VERSIONDATE << "). "
             << "Input database version: " << d_databaseversion << ". -->" << std::endl
             << "<!DOCTYPE html>" << std::endl
             << "<html>" << std::endl
             << "  <head>" << std::endl
             << "    <meta charset=\"utf-8\">" << std::endl
             << "    <title>Signal conversation list</title>" << std::endl;

  // STYLE
  outputfile << "    <style>" << std::endl
             << "    :root" << (themeswitching ? "[data-theme=\"" + (light ? "light"s : "dark") + "\"]" : "") << " {" << std::endl
             << "        /* " << (light ? "light" : "dark") << "*/" << std::endl
             << "        --body-bgc: " << (light ? "#EDF0F6;" : "#000000;") << std::endl
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
      << "        /* " << (!light ? "light" : "dark") << "*/" << std::endl
      << "        --body-bgc: " << (!light ? "#EDF0F6;" : "#000000;") << std::endl
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
    << "      }" << std::endl
    << "" << std::endl
    << "      #theme-switch {" << std::endl
    << "        display: none;" << std::endl
    << "      }" << std::endl
    << "" << std::endl

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
        document.cookie = name + "=" + (value || "")  + expires + "; path=/";
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
    << "";

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

  // END
  outputfile
    << "  </body>" << std::endl
    << "</html>" << std::endl;
}
