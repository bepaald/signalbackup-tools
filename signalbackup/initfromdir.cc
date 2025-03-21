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

#include "../filesqlitedb/filesqlitedb.h"
#include "../rawfileattachmentreader/rawfileattachmentreader.h"
#include "../attachmentmetadata/attachmentmetadata.h"

void SignalBackup::initFromDir(std::string const &inputdir, bool replaceattachments)
{

  Logger::message("Opening from dir!");

  Logger::message("Reading database...");
  FileSqliteDB database(inputdir + "/database.sqlite");
  if (!SqliteDB::copyDb(database, d_database))
    return;

  Logger::message("Reading HeaderFrame");
  if (!setFrameFromFile(&d_headerframe, inputdir + "/Header.sbf"))
    return;
  d_backupfileversion = d_headerframe->version();

  //d_headerframe->printInfo();

  Logger::message("Reading DatabaseVersionFrame");
  if (!setFrameFromFile(&d_databaseversionframe, inputdir + "/DatabaseVersion.sbf"))
    return;
  d_databaseversion = d_databaseversionframe->version();
  //d_databaseversionframe->printInfo();

  setColumnNames();

  Logger::message("Reading SharedPreferenceFrame(s)");
  int idx = 1;
  while (true)
  {
    d_sharedpreferenceframes.resize(d_sharedpreferenceframes.size() + 1);
    if (!setFrameFromFile(&d_sharedpreferenceframes.back(), inputdir + "/SharedPreference_" + bepaald::toString(idx) + ".sbf", true))
    {
      d_sharedpreferenceframes.pop_back();
      break;
    }
    //d_sharedpreferenceframes.back()->printInfo();
    ++idx;
  }

  Logger::message("Reading KeyValueFrame(s)");
  idx = 1;
  while (true)
  {
    d_keyvalueframes.resize(d_keyvalueframes.size() + 1);
    if (!setFrameFromFile(&d_keyvalueframes.back(), inputdir + "/KeyValue_" + bepaald::toString(idx) + ".sbf", true))
    {
      d_keyvalueframes.pop_back();
      break;
    }
    //d_keyvalueframes.back()->printInfo();
    ++idx;
  }

  Logger::message("Reading EndFrame");
  if (!setFrameFromFile(&d_endframe, inputdir + "/End.sbf"))
  {
    Logger::warning("EndFrame was not read: backup is probably incomplete");
    addEndFrame();
  }

  //d_endframe->printInfo();

  // avatars // NOTE, avatars are read in two passes to force correct order
  if (!d_showprogress)
    Logger::message_start("Reading AvatarFrames");
  std::error_code ec;
  std::filesystem::directory_iterator dirit(inputdir, ec);
  std::vector<std::string> avatarfiles;
  if (ec)
  {
    Logger::message_end();
    Logger::error("Error iterating directory `", inputdir, "' : ", ec.message());
    return;
  }
  for (auto const &avatar : dirit) // put all Avatar_[...].sbf files in vector:
    if (avatar.path().extension() == ".sbf" && STRING_STARTS_WITH(avatar.path().filename().string(), "Avatar_"))
      avatarfiles.push_back(avatar.path().string());

  std::sort(avatarfiles.begin(), avatarfiles.end());

#if __cplusplus > 201703L
  for (unsigned int i = 0; auto const &file : avatarfiles)
#else
  unsigned int i = 0;
  for (auto const &file : avatarfiles)
#endif
  {
    if (d_showprogress)
    {
      Logger::message_overwrite("Reading AvatarFrames: ", ++i, "/", avatarfiles.size());
      if (i == avatarfiles.size())
        Logger::message_overwrite("Reading AvatarFrames: ", avatarfiles.size(), "/", avatarfiles.size(), Logger::Control::ENDOVERWRITE);
    }

    std::filesystem::path avatarframe(file);
    std::filesystem::path avatarbin(file);
    avatarbin.replace_extension(".bin");

    DeepCopyingUniquePtr<AvatarFrame> temp;
    if (!setFrameFromFile(&temp, avatarframe.string()))
      return;
    //if (!temp->setAttachmentDataFromFile(avatarbin.string()))
    //  return;
    temp->setReader(new RawFileAttachmentReader(avatarbin.string()));

    //temp->printInfo();

    std::string name = (d_databaseversion < 33) ? temp->name() : temp->recipient();

    d_avatars.emplace_back(name, temp.release());
  }
  if (!d_showprogress)
    Logger::message_end();

  Logger::message("Reading AttachmentFrames");
  //attachments
  dirit = std::filesystem::directory_iterator(inputdir, ec);
  if (ec)
  {
    Logger::error("Error iterating directory `", inputdir, "' : ", ec.message());
    return;
  }
  int replaced_count = 0;
  for (auto const &att : dirit)
  {
    if (att.path().extension() != ".sbf" || !STRING_STARTS_WITH(att.path().filename().string(), "Attachment_"))
      continue;

    std::filesystem::path attframe = att.path();
    std::filesystem::path attbin = att.path();
    attbin.replace_extension(".bin");

    bool replaced_attachement = false;
    if (replaceattachments)
    {
      attbin.replace_extension(".new");
      if (bepaald::fileOrDirExists(attbin))
        replaced_attachement = true;
      else
        attbin.replace_extension(".bin");
    }

    DeepCopyingUniquePtr<AttachmentFrame> temp;
    if (!setFrameFromFile(&temp, attframe.string()))
      return;

    //if (!temp->setAttachmentData(attbin.string()))
    //  return;
    //temp->setLazyDataRAW(temp->length(), attbin.string());
    temp->setReader(new RawFileAttachmentReader(/*temp->length(), */attbin.string()));

    if (replaced_attachement)
    {
      AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(attbin.string());

      if (!amd) // undo the replacement
      {
        Logger::error("Failed to get metadata on new attachment: ", attbin);
        attbin.replace_extension(".bin");
        //if (!temp->setAttachmentData(attbin.string()))
        //  return;
        //temp->setLazyDataRAW(temp->length(), attbin.string());
        temp->setReader(new RawFileAttachmentReader(/*temp->length(), */attbin.string()));
      }
      else
      {
        // update database
        if (!updatePartTableForReplace(amd, temp->rowId()))
        {
          Logger::error("Failed to insert new attachment into database");
          return;
        }

        // set correct size on AttachmentFrame
        temp->setLength(amd.filesize);

        ++replaced_count;
      }
    }

    uint64_t rowid = temp->rowId();
    int64_t attachmentid = temp->attachmentId();
    d_attachments.emplace(std::make_pair(rowid, attachmentid ? attachmentid : -1), temp.release());

    MEMINFO("ADDED ATTACHMENT");
  }

  if (replaced_count)
    Logger::message(" - Replaced ", replaced_count, " attachments");

  Logger::message("Reading StickerFrames");
  //stickers
  dirit = std::filesystem::directory_iterator(inputdir, ec);
  if (ec)
  {
    Logger::error("Error iterating directory `", inputdir, "' : ", ec.message());
    return;
  }
  for (auto const &sticker : dirit)
  {
    if (sticker.path().extension() != ".sbf" || sticker.path().filename().string().substr(0, STRLEN("Sticker_")) != "Sticker_")
      continue;

    std::filesystem::path stickerframe = sticker.path();
    std::filesystem::path stickerbin = sticker.path();
    stickerbin.replace_extension(".bin");

    DeepCopyingUniquePtr<StickerFrame> temp;
    if (!setFrameFromFile(&temp, stickerframe.string()))
      return;
    //if (!temp->setAttachmentDataFromFile(stickerbin.string()))
    //  return;
    temp->setReader(new RawFileAttachmentReader(stickerbin.string()));

    uint64_t rowid = temp->rowId();
    d_stickers.emplace(std::make_pair(rowid, temp.release()));
  }

#ifdef BUILT_FOR_TESTING
  // check for file 'BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE'
  // set d_found_sqlite_sequence_in_backup
  if (bepaald::fileOrDirExists(inputdir + "/BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE"))
    d_found_sqlite_sequence_in_backup = true;
#endif


  Logger::message("Done!");
  d_ok = true;
}
