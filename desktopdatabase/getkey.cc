/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

#include "desktopdatabase.ih"

#include <regex>
#include <fstream>

bool DesktopDatabase::getKey()
{
  if (getKeyFromEncrypted())
  {
    if (d_verbose) [[unlikely]]
      Logger::message("Initialized from encryptedkey");
    return true;
  }

  // read key from config.json
  std::fstream config(d_configdir + "/config.json", std::ios_base::in | std::ios_base::binary);
  if (!config.is_open())
  {
    Logger::error("Failed to open input: ", d_configdir, "/config.json");
    return false;
  }
  std::string line;
  //std::regex keyregex("PRAGMA KEY = \"x\\\\'([a-zA-Z0-9]{64})\\\\'\";");

  /*
    $ cat ~/.config/Signal/config.json | pcregrep -o1 "^\s*\"key\":\s*\"([a-z0-9]{64})\"$"
    aac2f422c149db6180b1a76df1ee462101c11d2d2347044ef055a956dfcbfa98
  */
  std::regex keyregex("^\\s*\"key\":\\s*\"([a-zA-Z0-9]{64})\",?$");
  std::smatch m;
  bool found = false;
  while (std::getline(config, line))
  {
    //std::cout << "Line: " << line << std::endl;
    if (std::regex_match(line, m, keyregex))
      if (m.size() == 2) // m[0] is full match, m[1] is first submatch (which we want)
      {
        found = true;
        break;
      }
  }

  if (!found)
  {
    Logger::error("Failed to read key from config.json");
    return false;
  }

  d_hexkey = m[1].str();

  //std::cout << bepaald::bytesToHexString(d_key, d_keysize) << std::endl;

  return true;
}
