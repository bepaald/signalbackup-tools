/*
    Copyright (C) 2019-2020  Selwin van Dijk

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


void SignalBackup::initFromDir(std::string const &inputdir)
{

  std::cout << "Opening from dir!" << std::endl;

  std::cout << "Reading database..." << std::endl;
  SqliteDB database(inputdir + "/database.sqlite");
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
  int idx = 0;
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

  std::cout << "Reading EndFrame" << std::endl;
  if (!setFrameFromFile(&d_endframe, inputdir + "/End.sbf"))
    return;

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
    if (avatar.path().extension() == ".sbf" && avatar.path().filename().string().starts_with("Avatar_"))
      avatarfiles.push_back(avatar.path());

  std::sort(avatarfiles.begin(), avatarfiles.end());

  for (unsigned int i = 0; auto const &file : avatarfiles)
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

    std::unique_ptr<AvatarFrame> temp;
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
  for (auto const &att : dirit)
  {
    if (att.path().extension() != ".sbf" || !att.path().filename().string().starts_with("Attachment_"))
      continue;

    std::filesystem::path attframe = att.path();
    std::filesystem::path attbin = att.path();
    attbin.replace_extension(".bin");

    std::unique_ptr<AttachmentFrame> temp;
    if (!setFrameFromFile(&temp, attframe.string()))
      return;
    if (!temp->setAttachmentData(attbin.string()))
      return;

    uint64_t rowid = temp->rowId();
    uint64_t attachmentid = temp->attachmentId();
    d_attachments.emplace(std::make_pair(rowid, attachmentid), temp.release());
  }

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

    std::unique_ptr<StickerFrame> temp;
    if (!setFrameFromFile(&temp, stickerframe.string()))
      return;
    if (!temp->setAttachmentData(stickerbin.string()))
      return;

    uint64_t rowid = temp->rowId();
    d_stickers.emplace(std::make_pair(rowid, temp.release()));
  }


  d_ok = true;
}
