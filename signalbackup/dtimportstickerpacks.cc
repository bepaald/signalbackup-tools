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

bool SignalBackup::dtImportStickerPacks(SqliteDB const &ddb, std::string const &databasedir)
{
  if (!d_database.containsTable("sticker") ||
      !ddb.containsTable("sticker_packs") ||
      !ddb.containsTable("stickers"))
  {
    Logger::error("Database does not contain expected sticker tables");
    return false;
  }

  // get all stickerpacks
  SqliteDB::QueryResults dtstickerpacks;
  ddb.exec("SELECT id, key, author, coverStickerId, title, status FROM sticker_packs", &dtstickerpacks);

  for (uint i = 0; i < dtstickerpacks.rows(); ++i)
  {

    // check if this pack is already installed in the backup...
    std::string dtpackid = dtstickerpacks(i, "id");
    if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM sticker WHERE installed = 1 AND pack_id IS ?", dtpackid, -1) > 0)
    {
      if (d_verbose) [[unlikely]]
        Logger::message("Skipping stickerpack '", dtpackid, "': Already installed");
      continue;
    }

    // if it is not installed (but 'known'), check if it is known to Android (and skip if so)
    long long int dt_installed = dtstickerpacks(i, "status") == "installed";
    if (dt_installed == 0 &&
        d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM sticker WHERE pack_id IS ?", dtpackid, -1) > 0)
    {
      if (d_verbose) [[unlikely]]
        Logger::message("Skipping stickerpack '", dtpackid, "': Already installed");
      continue;
    }

    // the pack may not be installed, but may be known, in which case, the cover already exists...
    // when this is the case, the cover can be skipped on import (it's already there) and the 'installed'
    // status must be updated instead.
    long long int known_backup_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM sticker WHERE installed = 0 AND pack_id IS ?", dtpackid, -1);

    std::string dtkey = dtstickerpacks(i, "key");
    std::pair<unsigned char *, size_t> dtkey_data = Base64::base64StringToBytes(dtkey);
    dtkey = bepaald::bytesToHexString(dtkey_data, true);
    bepaald::destroyPtr(&dtkey_data.first, &dtkey_data.second);
    std::string dtauthor = dtstickerpacks(i, "author");
    std::string dttitle = dtstickerpacks(i, "title");
    long long int dtcoversticker = dtstickerpacks.valueAsInt(i, "coverStickerId");

    SqliteDB::QueryResults dtstickers;
    ddb.exec("SELECT id, emoji, isCoverOnly, path FROM stickers WHERE packId IS ?" +
             (dt_installed == 0 ? " AND id = " + bepaald::toString(dtcoversticker) : "") , dtpackid, &dtstickers);
    if (dtstickers.rows() == 0)
      continue;

    if (d_verbose) [[unlikely]]
      Logger::message("Importing ", dtstickers.rows(), " stickers from stickerpack ", dtpackid, " (key: ", dtkey, ")");

    for (uint j = 0; j < dtstickers.rows(); ++j)
    {
      std::string dtpath = dtstickers(j, "path");
      long long int dtcoveronly = dtstickers.valueAsInt(j, "isCoverOnly");
      long long int dtstickerid = dtstickers.valueAsInt(j, "id");
      std::string dtemoji = dtstickers(j, "emoji");

      // get filelength
      long long int filelength = -1;
      {
        std::ifstream dtstickerfile(databasedir + "/stickers.noindex/" + dtpath, std::ios_base::binary | std::ios_base::in);
        if (!dtstickerfile.is_open())
        {
          Logger::error("Error opening Desktop sticker at path '", databasedir + "/stickers.noindex/" + dtpath, "'. Skipping...");
          continue;
        }
        dtstickerfile.seekg(0, std::ios_base::end);
        filelength = dtstickerfile.tellg();
      }
      if (filelength == -1)
      {
        Logger::error("Failed to get Sticker filesize. Skipping...");
        return false;
      }

      // install the sticker (if not already present)
      if (dtstickerid != dtcoversticker || // if this is not the cover,
          known_backup_id == -1)           // or its the cover of an unknown pack
      {                                    // -> add it!
        std::any retval;
        if (!insertRow("sticker", {{"pack_id", dtpackid},
                                   {"pack_key", dtkey},
                                   {"pack_title", dttitle},
                                   {"pack_author", dtauthor},
                                   {"sticker_id", dtstickerid},
                                   {"cover", dtstickerid == dtcoversticker ? 1 : 0},
                                   {"emoji", dtstickerid == dtcoversticker ? "" : dtemoji},
                                   {"installed", dt_installed},
                                   {"file_path", "[has_non_null_constraint_but_is_recreated_on_backup_restore]"},
                                   {"file_length", filelength}}, "_id", &retval))
        {
          Logger::error("Error inserting sticker");
          return false;
        }
        long long int new_sticker_id = std::any_cast<long long int>(retval);
        // add file to d_stickers...
        DeepCopyingUniquePtr<StickerFrame> new_sticker_frame;
        if (setFrameFromStrings(&new_sticker_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_sticker_id),
                                                                             "LENGTH:uint32:" + bepaald::toString(filelength)}))
        {
          new_sticker_frame->setLazyDataRAW(filelength, databasedir + "/stickers.noindex/" + dtpath);
          d_stickers.emplace(new_sticker_id, new_sticker_frame.release());
        }
        else
        {
          Logger::error("Failed to add new sticker to backup");
          d_database.exec("DELETE FROM sticker WHERE _id = ?", new_sticker_id);
          continue;
        }
      }
      if (dtstickerid == dtcoversticker && // if this is the cover sticker (this condition is only here to do this only once)
          known_backup_id != -1)           // and the pack is already known, just not _installed_
        d_database.exec("UPDATE sticker SET installed = 1, pack_key = ? WHERE _id = ?", {dtkey, known_backup_id});


      if (dtstickerid == dtcoversticker && // this was the cover sticker
          dtcoveronly == 0 &&              // but also a valid sticker itself
          dt_installed == 1)               // we're fully installing, not just making known
      {
        std::any retval;
        if (!insertRow("sticker", {{"pack_id", dtpackid},
                                   {"pack_key", dtkey},
                                   {"pack_title", dttitle},
                                   {"pack_author", dtauthor},
                                   {"sticker_id", dtstickerid},
                                   {"cover", 0},
                                   {"emoji", dtemoji},
                                   {"installed", 1},
                                   {"file_path", "[has_non_null_constraint_but_is_recreated_on_backup_restore]"},
                                   {"file_length", filelength}}, "_id", &retval))
        {
          Logger::error("Error inserting sticker");
          return false;
        }
        long long int new_sticker_id = std::any_cast<long long int>(retval);
        DeepCopyingUniquePtr<StickerFrame> new_sticker_frame2;
        if (setFrameFromStrings(&new_sticker_frame2, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_sticker_id),
                                                                              "LENGTH:uint32:" + bepaald::toString(filelength)}))
        {
          new_sticker_frame2->setLazyDataRAW(filelength, databasedir + "/stickers.noindex/" + dtpath);
          d_stickers.emplace(new_sticker_id, new_sticker_frame2.release());
        }
        else
        {
          Logger::error("Failed to add new sticker to backup");
          d_database.exec("DELETE FROM sticker WHERE _id = ?", new_sticker_id);
          continue;
        }
      }
    }
  }

  return true;
}
/*
                  id = 9acc9e8aba563d26a4994e69263e3b25
                 key = Wm3/OUjCjvubeq+T7MN1xp/DFueAd+0mhnoU0QoPahI=
              author = Agnes Lee
      coverStickerId = 24
           createdAt = 1668618591135
    downloadAttempts = 1
         installedAt = 1668688271632
            lastUsed =
              status = installed
        stickerCount = 24
               title = Bandit the Cat
     attemptedStatus = downloaded
            position = 0
           storageID = 7ApKBVHNF+ZaKsOIUYOvjw==
      storageVersion = 170
storageUnknownFields =
    storageNeedsSync = 0

         id = 24
     packId = 9acc9e8aba563d26a4994e69263e3b25
      emoji =
     height = 512
isCoverOnly = 1
   lastUsed =
       path = 03/0399b0c0b87f750ddd65c58617b1d18c3951f894c688154f965a76194f79e74b
      width = 512

*/
