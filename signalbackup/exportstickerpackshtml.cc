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

bool SignalBackup::exportStickerPacksHTML(std::string const &directory, bool overwrite, bool append) const
{
  if (!d_database.containsTable("sticker"))
  {
    Logger::error("No stickers in database");
    return false;
  }

  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT _id, sticker_id, pack_id, pack_title, pack_author, emoji "
                       "FROM sticker WHERE installed IS 1 ORDER BY pack_id, sticker_id", &res))
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
  // std::ofstream stickerhtml(directory + "/stickerpacks.html", std::ios_base::binary);
  // if (!stickerhtml.is_open())
  // {
  //   Logger::error();
  //   return false;
  // }

  // stickerhtml << ...

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

      Logger::message("\"", packtitle, "\" by ", packauthor);
      // stickerhtml << ...

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
    Logger::message("Sticker ", stickerid, ": ", emoji);
    // stickerhtml << ...

    // write actual file to disk
    auto it = std::find_if(d_stickers.begin(), d_stickers.end(), [id](auto const &s) { return s.first == static_cast<uint64_t>(id); });
    if (it == d_stickers.end())
    {
      Logger::warning("Failed to find sticker (id: ", id, ")");
      continue;
    }
  }

  // write end of html
  // stickerhtml << ...

  return true;
}
