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

#include "desktopdatabase.ih"

std::string DesktopDatabase::readEncryptedKey() const
{
  std::string encrypted_key;

  std::fstream config(d_configdir + "/config.json", std::ios_base::in | std::ios_base::binary);
  if (!config.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open input: ", d_configdir, "/config.json");
    return encrypted_key;
  }

  std::string line;
  std::regex keyregex("^\\s*\"encryptedKey\":\\s*\"([a-zA-Z0-9]+)\",?$");
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
    Logger::warning("Failed to read encrypted key from config.json, trying plaintext key...");
    return encrypted_key;
  }
  encrypted_key = m[1].str();
  return encrypted_key;
}
