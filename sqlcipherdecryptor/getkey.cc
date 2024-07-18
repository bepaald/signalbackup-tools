/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#include "sqlcipherdecryptor.ih"

bool SqlCipherDecryptor::getKey()
{

#if defined(_WIN32) || defined(__MINGW64__) // only windows for now...
  if (getEncryptedKey())
  {
    if (d_verbose) [[unlikely]]
      Logger::message("Initialized from encryptedkey");
    return true;
  }
#endif

  // read key from config.json
  std::fstream config(d_configpath + "/config.json", std::ios_base::in | std::ios_base::binary);
  if (!config.is_open())
  {
    Logger::error("Failed to open input: ", d_configpath, "/config.json");
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

  d_keysize = 32;
  d_key = new unsigned char[d_keysize];

  std::string keystr = m[1].str();

  for (uint i = 0; i < keystr.size(); i += 2)
  {
    auto [p, ec] = std::from_chars(keystr.c_str() + i, keystr.c_str() + i + 2,
                                   *reinterpret_cast<uint8_t *>(d_key + i / 2), 16);
    if (ec != std::errc())
    {
      Logger::error("Failed to parse key from hex data");
      return false;
    }
  }

  //std::cout << bepaald::bytesToHexString(d_key, d_keysize) << std::endl;

  return true;
}
