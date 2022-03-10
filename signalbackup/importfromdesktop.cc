/*
    Copyright (C) 2022  Selwin van Dijk

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

#include "../sqlcipherdecryptor/sqlcipherdecryptor.h"

bool SignalBackup::importFromDesktop(std::string const &dir)
{
  if (dir.empty())
  {
    // try to set dir automatically
  }

  SqlCipherDecryptor sqlcipherdecryptor(dir);
  if (!sqlcipherdecryptor.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }

  auto [data, size] = sqlcipherdecryptor.data(); // unsigned char *, uint64_t
  std::pair<unsigned char *, uint64_t> desktopdata = {data, size};
  SqliteDB ddb(&desktopdata);
  if (!ddb.ok())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : Failed to open database" << std::endl;
    return false;
  }


  // actual functionality comes here :)
  // ...
  sqlcipherdecryptor.writeToFile("desktop.sqlite", true);
  ddb.print("SELECT id,type,body,hasAttachments,source,sourceDevice,conversationId,sent_at FROM messages");

  return false;
}
