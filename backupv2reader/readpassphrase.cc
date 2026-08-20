/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "backupv2reader.h"

#include <string>
#include "../logger/logger.h"
#include "../common_filesystem.h"

std::string BackupV2Reader::readPassphrase(std::string_view data) const
{
  std::string ipp;
  if (bepaald::isRegularFile(data))
  {
    // read from file
    auto [ppdata, ppsize] = bepaald::readFileFully(data);
    if (!ppdata || ppsize == 0)
    {
      Logger::error("Failed to read passphrase from file");
      return std::string();
    }
    ipp = std::string(reinterpret_cast<char *>(ppdata.get()), ppsize);
  }
  else
    ipp = data;

  // replace '=' with '0' and '#' with 'O',
  // lowercase and remove any char outside a-z0-9
  std::string opp;
  opp.reserve(64);
  for (char c : ipp)
  {
    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z'))
      opp += c;
    if (c <= 'Z' && c >= 'A')
      opp += c + ('a' - 'A');

    // these are display characters used to differentiate visually ambiguous
    // characters (0 and O).
    // See https://github.com/signalapp/Signal-Android/blob/main/core/models-jvm/src/main/java/org/signal/core/models/AccountEntropyPool.kt
    else if (c == '#') [[unlikely]]
      opp += 'o';
    else if (c == '=') [[unlikely]]
      opp += '0';
  }

  return opp;
}

#endif
