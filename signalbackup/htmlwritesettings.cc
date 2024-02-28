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
  std::ofstream outputfile(dir + "/settings.html", std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open '", dir, "/settings.html' for writing.");
    return false;
  }

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  outputfile
    << "<!-- Generated on " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") // %F an d%T do not work on minGW
    << " by signalbackup-tools (" << VERSIONDATE << "). "
    << "Input database version: " << d_databaseversion << ". -->" << std::endl
    << "<!DOCTYPE html>" << std::endl
    << "<html>" << std::endl
    << "  <head>" << std::endl
    << "    <meta charset=\"utf-8\">" << std::endl
    << "    <title>Signal settings</title>" << std::endl
    << "    <style>" << std::endl
    << "    </style>" << std::endl
    << "  </head>" << std::endl;

  // BODY
  outputfile
    << "  <body>" << std::endl;

  outputfile
    << std::endl
    << "  </body>" << std::endl
    << "</html>" << std::endl;

  return true;
}
