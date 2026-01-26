/*
  Copyright (C) 2024-2026  Selwin van Dijk

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

#include "../common_filesystem.h"
#include "../desktopattachmentreader/desktopattachmentreader.h"

bool SignalBackup::dtImportStickerPacks(SqliteDB const &ddb, std::string const &databasedir)
{
  if (!d_database.containsTable("sticker") ||
      !ddb.containsTable("sticker_packs") ||
      !ddb.containsTable("stickers"))
  {
    Logger::warnOnce("Database does not contain expected sticker tables.");
    return true; // this is not an error per se, if the tables arent there, we have imported all (0) stickerpacks...
  }

  // get all stickerpacks
  SqliteDB::QueryResults dtstickerpacks;
  ddb.exec("SELECT id, key, author, coverStickerId, title, status FROM sticker_packs", &dtstickerpacks);

  for (unsigned int i = 0; i < dtstickerpacks.rows(); ++i)
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
    std::string stickerquery = "SELECT id, emoji, isCoverOnly, path";
    if (ddb.tableContainsColumn("stickers", "version", "localKey", "size")) [[likely]]
      stickerquery += ", version, localKey, size";
    else
      stickerquery += ", 0 AS version, '' AS localKey, 0 AS size";
    stickerquery += " FROM stickers WHERE packId = ?";
    ddb.exec(stickerquery +
             (dt_installed == 0 ? " AND id = " + bepaald::toString(dtcoversticker) : "") , dtpackid, &dtstickers);
    if (dtstickers.rows() == 0)
      continue;

    if (d_verbose) [[unlikely]]
      Logger::message("Importing ", dtstickers.rows(), " stickers from stickerpack ", dtpackid, " (key: ", dtkey, ")");

    for (unsigned int j = 0; j < dtstickers.rows(); ++j)
    {
      long long int dtcoveronly = dtstickers.valueAsInt(j, "isCoverOnly");
      long long int dtstickerid = dtstickers.valueAsInt(j, "id");
      std::string dtemoji = dtstickers(j, "emoji");
      uint64_t filelength = dtstickers.valueAsInt(j, "size", 0);
      long long int version = dtstickers.valueAsInt(j, "version");
      std::string localkey = dtstickers(j, "localKey");
      std::string fullpath(databasedir + "/stickers.noindex/" + dtstickers(j, "path"));

      // get filelength to make sure it is not larger than whats in the database.
      // incorrect (too big) sizes have been observed in the database, leading to buffer
      // overflows when writing the decrypted data.
      uint64_t filesize_on_disk = bepaald::fileSize(fullpath);

      // if we got the file size
      if (filesize_on_disk != 0 && filesize_on_disk != static_cast<std::uintmax_t>(-1))
      {
        // IF file size without MAC and IV (= attachment_data + 0-padding by Signal + padding to first multiple of 16 by AES
        // <
        // size in database
        // THEN size in database is wrong: even filesize_on_disk - 32 - 16 is already more than we have available...
        if (filesize_on_disk - 32 /*MAC LENGTH*/ - 16 /*IV LENGTH*/ < filelength ||
            filelength == 0)
        {
          Logger::error("Sticker size in database is incorrect. Skipping...");
          return false;

          // TODO: decrypt the sticker and get its "attachment_data + 0-padding by Signal"-size
          //       (out_len in DesktopAttachmentReader)
          //       this is also not the _real_ size, but at least theses bytes are all going
          //       to be available to write.

          // Logger::message("Adjusting sticker size as it appears to large in database.");
          // Logger::message("Size in DB: ", filelength, " Size on disk: ", filesize_on_disk,
          //                 " (with padding and 48 bytes IV+MAC)");
          // filelength = filesize_on_disk - 32 - 16;
        }
      }

      if (filelength == 0)
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
          //new_sticker_frame->setLazyDataRAW(filelength, databasedir + "/stickers.noindex/" + dtpath);
          //new_sticker_frame->setReader(new RawFileAttachmentReader(databasedir + "/stickers.noindex/" + dtpath));
          new_sticker_frame->setReader(new DesktopAttachmentReader(version, fullpath, localkey, filelength));
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
          //new_sticker_frame2->setLazyDataRAW(filelength, databasedir + "/stickers.noindex/" + dtpath);
          //new_sticker_frame2->setReader(new RawFileAttachmentReader(databasedir + "/stickers.noindex/" + dtpath));
          new_sticker_frame2->setReader(new DesktopAttachmentReader(version, fullpath, localkey, filelength));
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
