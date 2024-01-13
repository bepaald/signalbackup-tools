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

#include "signalbackup.ih"

void SignalBackup::initFromDir(std::string const &inputdir, bool replaceattachments)
{

  std::cout << "Opening from dir!" << std::endl;

  std::cout << "Reading database..." << std::endl;
  FileSqliteDB database(inputdir + "/database.sqlite");
  if (!SqliteDB::copyDb(database, d_database))
    return;

  std::cout << "Reading HeaderFrame" << std::endl;
  if (!setFrameFromFile(&d_headerframe, inputdir + "/Header.sbf"))
    return;

  //d_headerframe->printInfo();

  std::cout << "Reading DatabaseVersionFrame" << std::endl;
  if (!setFrameFromFile(&d_databaseversionframe, inputdir + "/DatabaseVersion.sbf"))
    return;

  d_databaseversion = d_databaseversionframe->version();
  //d_databaseversionframe->printInfo();

  std::cout << "Reading SharedPreferenceFrame(s)" << std::endl;
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

  std::cout << "Reading KeyValueFrame(s)" << std::endl;
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

  std::cout << "Reading EndFrame" << std::endl;
  if (!setFrameFromFile(&d_endframe, inputdir + "/End.sbf"))
  {
    std::cout << bepaald::bold_on << "WARNING " << bepaald::bold_off
              << "EndFrame was not read: backup is probably incomplete" << std::endl;
    addEndFrame();
  }

  //d_endframe->printInfo();

  // avatars // NOTE, avatars are read in two passes to force correct order
  if (!d_showprogress)
    std::cout << "Reading AvatarFrames" << std::flush;
  std::error_code ec;
  std::filesystem::directory_iterator dirit(inputdir, ec);
  std::vector<std::string> avatarfiles;
  if (ec)
  {
    std::cout << "Error iterating directory `" << inputdir << "' : " << ec.message() << std::endl;
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
      std::cout << "\33[2K\rReading AvatarFrames: " << ++i << "/" << avatarfiles.size() << std::flush;
      if (i == avatarfiles.size())
        std::cout << std::endl;
    }

    std::filesystem::path avatarframe(file);
    std::filesystem::path avatarbin(file);
    avatarbin.replace_extension(".bin");

    DeepCopyingUniquePtr<AvatarFrame> temp;
    if (!setFrameFromFile(&temp, avatarframe.string()))
      return;
    if (!temp->setAttachmentData(avatarbin.string()))
      return;

    //temp->printInfo();

    std::string name = (d_databaseversion < 33) ? temp->name() : temp->recipient();

    d_avatars.emplace_back(name, temp.release());
  }

  std::cout << "Reading AttachmentFrames" << std::endl;
  //attachments
  dirit = std::filesystem::directory_iterator(inputdir, ec);
  if (ec)
  {
    std::cout << "Error iterating directory `" << inputdir << "' : " << ec.message() << std::endl;
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
    temp->setLazyDataRAW(temp->length(), attbin.string());

    if (replaced_attachement)
    {
      AttachmentMetadata amd = getAttachmentMetaData(attbin.string());

      if (!amd) // undo the replacement
      {
        std::cout << "Failed to get metadata on new attachment: " << attbin << std::endl;
        attbin.replace_extension(".bin");
        //if (!temp->setAttachmentData(attbin.string()))
        //  return;
        temp->setLazyDataRAW(temp->length(), attbin.string());
      }
      else
      {
        // update database
        if (!updatePartTableForReplace(amd, temp->rowId()))
        {
          std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
                    << ": Failed to insert new attachment into database" << std::endl;
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
    std::cout << " - Replaced " << replaced_count << " attachments" << std::endl;

  std::cout << "Reading StickerFrames" << std::endl;
  //stickers
  dirit = std::filesystem::directory_iterator(inputdir, ec);
  if (ec)
  {
    std::cout << "Error iterating directory `" << inputdir << "' : " << ec.message() << std::endl;
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
    if (!temp->setAttachmentData(stickerbin.string()))
      return;

    uint64_t rowid = temp->rowId();
    d_stickers.emplace(std::make_pair(rowid, temp.release()));
  }

#ifdef BUILT_FOR_TESTING
  // check for file 'BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE'
  // set d_found_sqlite_sequence_in_backup
  if (bepaald::fileOrDirExists(inputdir + "/BUILT_FOR_TESTING_FOUND_SQLITE_SEQUENCE"))
    d_found_sqlite_sequence_in_backup = true;
#endif


  std::cout << "Done!" << std::endl;
  d_ok = true;
}
