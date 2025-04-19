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

#include "signalbackup.ih"

bool SignalBackup::exportBackupToDir(std::string const &directory, bool overwrite, bool keepattachmentdatainmemory, bool onlydb)
{
  Logger::message("\nExporting backup into '", directory, "/'");

  if (!prepareOutputDirectory(directory, overwrite))
    return false;

  bool exportok = true;

  // export headerframe:
  if (!onlydb)
  {
    Logger::message("Writing HeaderFrame...");
    if (!writeRawFrameDataToFile(directory + "/Header.sbf", d_headerframe)) [[unlikely]]
    {
      Logger::error("Failed to write headerframe");
      exportok = false;
    }

    // export databaseversionframe
    Logger::message("Writing DatabaseVersionFrame...");
    if (!writeRawFrameDataToFile(directory + "/DatabaseVersion.sbf", d_databaseversionframe)) [[unlikely]]
    {
      Logger::error("Failed to write databaseversionframe");
      exportok = false;
    }

    // export attachments
    Logger::message("Writing Attachments...");
    for (auto const &aframe : d_attachments)
    {
      AttachmentFrame *a = aframe.second.get();
      uint64_t rowid = a->rowId();
      int64_t uniqueid = a->attachmentId();
      if (uniqueid == 0)
        uniqueid = -1;
      std::string attachment_basefilename = directory + "/Attachment_" + bepaald::toString(rowid) + "_" + bepaald::toString(uniqueid);

      // write frame
      if (!writeRawFrameDataToFile(attachment_basefilename + ".sbf", a)) [[unlikely]]
      {
        Logger::error("Failed to write attachmentframe");
        exportok = false;
        continue;
      }

      // write actual attachment:
      std::ofstream attachmentstream(attachment_basefilename + ".bin", std::ios_base::binary);
      if (!attachmentstream.is_open()) [[unlikely]]
      {
        Logger::error("Failed to open file for writing: ", directory, attachment_basefilename, ".bin");
        exportok = false;
        continue;
      }
      else
      {
        unsigned char const *data = a->attachmentData(d_verbose);
        if (!data) [[unlikely]]
        {
          Logger::error("Failed to retrieve attachment data for attachment (rowid: ", rowid, " uniqueid: ", uniqueid, ")");
          exportok = false;
          continue;
        }
        if (!attachmentstream.write(reinterpret_cast<char const *>(data), a->attachmentSize())) [[unlikely]]
        {
          Logger::error("Failed write attachmentdata");
          exportok = false;
          continue;
        }
      }

      if (!keepattachmentdatainmemory && a)
      {
        MEMINFO("BEFORE DROPPING ATTACHMENT DATA");
        a->clearData();
        MEMINFO("AFTER DROPPING ATTACHMENT DATA");
      }
    }

    // export avatars
    Logger::message("Writing Avatars...");
#if __cplusplus > 201703L
    for (int count = 1; auto const &aframe : d_avatars)
#else
    int count = 1;
    for (auto const &aframe : d_avatars)
#endif
    {
      AvatarFrame *a = aframe.second.get();
      std::string avatar_basefilename = directory + "/Avatar_" +
        std::string(bepaald::numDigits(d_avatars.size()) - bepaald::numDigits(count), '0') + bepaald::toString(count)
        + "_" + ((d_databaseversion < 33) ? a->name() : a->recipient());
      ++count;

      // write frame
      if (!writeRawFrameDataToFile(avatar_basefilename + ".sbf", a)) [[unlikely]]
      {
        Logger::error("Failed to write avatarframe");
        exportok = false;
        continue;
      }

      // write actual avatar:
      std::ofstream avatarstream(avatar_basefilename + ".bin", std::ios_base::binary);
      if (!avatarstream.is_open()) [[unlikely]]
      {
        Logger::error("Failed to open file for writing: ", directory, avatar_basefilename, ".bin");
        exportok = false;
        continue;
      }
      else
        if (!avatarstream.write(reinterpret_cast<char *>(a->attachmentData(d_verbose)), a->attachmentSize())) [[unlikely]]
        {
          Logger::error("Failed to write avatar data");
          exportok = false;
          continue;
        }
    }

    // export sharedpreferences
    Logger::message("Writing SharedPrefFrame(s)...");
#if __cplusplus > 201703L
    for (int count = 1; auto const &spframe : d_sharedpreferenceframes)
#else
    count = 1;
    for (auto const &spframe : d_sharedpreferenceframes)
#endif
      if (!writeRawFrameDataToFile(directory + "/SharedPreference_" + bepaald::toString(count++) + ".sbf", spframe)) [[unlikely]]
      {
        Logger::error("Failed to write sharedpreferenceframe");
        exportok = false;
        continue;
      }

    // export keyvalues
    Logger::message("Writing KeyValueFrame(s)...");
#if __cplusplus > 201703L
    for (int count = 1; auto const &kvframe : d_keyvalueframes)
#else
    count = 1;
    for (auto const &kvframe : d_keyvalueframes)
#endif
      if (!writeRawFrameDataToFile(directory + "/KeyValue_" + bepaald::toString(count++) + ".sbf", kvframe)) [[unlikely]]
      {
        Logger::error("Failed to write keyvalueframe");
        exportok = false;
        continue;
      }

    // export stickers
    Logger::message("Writing StickerFrames...");
#if __cplusplus > 201703L
    for (int count = 1; auto const &sframe : d_stickers)
#else
    count = 1;
    for (auto const &sframe : d_stickers)
#endif
    {
      StickerFrame *s = sframe.second.get();
      std::string sticker_basefilename = directory + "/Sticker_" + bepaald::toString(count++) + "_" + bepaald::toString(s->rowId());

      // write frame
      if (!writeRawFrameDataToFile(sticker_basefilename + ".sbf", s)) [[unlikely]]
      {
        Logger::error("Failed to write stickerframe");
        exportok = false;
        continue;
      }

      // write actual sticker data
      std::ofstream stickerstream(sticker_basefilename + ".bin", std::ios_base::binary);
      if (!stickerstream.is_open()) [[unlikely]]
      {
        Logger::error("Failed to open file for writing: ", directory, sticker_basefilename, ".bin");
        exportok = false;
        continue;
      }
      else
        if (!stickerstream.write(reinterpret_cast<char *>(s->attachmentData(d_verbose)), s->attachmentSize())) [[unlikely]]
        {
          Logger::error("Failed to write sticker data");
          exportok = false;
          continue;
        }
    }

    // export endframe
    Logger::message("Writing EndFrame...");
    if (!writeRawFrameDataToFile(directory + "/End.sbf", d_endframe)) [[unlikely]]
    {
      Logger::error("Failed to write endframe");
      exportok = false;
    }
  }

  // export database
  Logger::message("Writing database...");
  if (!d_database.saveToFile(directory + "/database.sqlite")) [[unlikely]]
  {
    Logger::error("Failed to write SQLite database");
    exportok = false;
  }

#ifdef BUILT_FOR_TESTING
  // if d_found_sqlite_sequence_in_database
  //   write('BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE');
  if (d_found_sqlite_sequence_in_backup)
  {
    std::ofstream bft(directory + "/BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE", std::ios_base::binary);
    bft.close();
  }
#endif

  Logger::message("Done!");
  return exportok;
}
