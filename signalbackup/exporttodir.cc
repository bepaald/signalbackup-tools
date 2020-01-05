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


bool SignalBackup::exportBackupToDir(std::string const &directory, bool overwrite)
{
  std::cout << std::endl << "Exporting backup into '" << directory << "/'" << std::endl;

  // maybe check directory exists, and is empty or make it empty if overwrite was requested.
  if (!bepaald::isEmpty(directory))
  {
    if (!overwrite)
    {
      std::cout << "Directory '" << directory << "' is not empty. Use --overwrite to clear directory before export" << std::endl;
      return false;
    }
    std::cout << "Clearing contents of directory '" << directory << "'..." << std::endl;
    if (!bepaald::clearDirectory(directory))
    {
      std::cout << "Failed to empty directory '" << directory << "'" << std::endl;
      return false;
    }
  }

  // export headerframe:
  std::cout << "Writing HeaderFrame..." << std::endl;
  if (!writeRawFrameDataToFile(directory + "/Header.sbf", d_headerframe))
    return false;

  // export databaseversionframe
  std::cout << "Writing DatabaseVersionFrame..." << std::endl;
  if (!writeRawFrameDataToFile(directory + "/DatabaseVersion.sbf", d_databaseversionframe))
    return false;

  // export attachments
  std::cout << "Writing Attachments..." << std::endl;
  for (auto const &aframe : d_attachments)
  {
    AttachmentFrame *a = aframe.second.get();
    std::string attachment_basefilename = directory + "/Attachment_" + bepaald::toString(a->rowId()) + "_" + bepaald::toString(a->attachmentId());

    // write frame
    if (!writeRawFrameDataToFile(attachment_basefilename + ".sbf", a))
      return false;

    // write actual attachment:
    std::ofstream attachmentstream(attachment_basefilename + ".bin", std::ios_base::binary);
    if (!attachmentstream.is_open())
    {
      std::cout << "Failed to open file for writing: " << directory << attachment_basefilename << ".bin" << std::endl;
      return false;
    }
    else
      if (!attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize()))
        return false;
  }

  // export avatars
  std::cout << "Writing Avatars..." << std::endl;
  for (int count = 1 ; auto const &aframe : d_avatars)
  {
    AvatarFrame *a = aframe.second.get();
    std::string avatar_basefilename = directory + "/Avatar_" + bepaald::toString(count++) + "_" + ((d_databaseversion < 33) ? a->name() : a->recipient());

    // write frame
    if (!writeRawFrameDataToFile(avatar_basefilename + ".sbf", a))
      return false;

    // write actual avatar:
    std::ofstream avatarstream(avatar_basefilename + ".bin", std::ios_base::binary);
    if (!avatarstream.is_open())
    {
      std::cout << "Failed to open file for writing: " << directory << avatar_basefilename << ".bin" << std::endl;
      return false;
    }
    else
      if (!avatarstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize()))
        return false;
  }

  // export sharedpreferences
  std::cout << "Writing SharedPrefFrame(s)..." << std::endl;
  for (int count = 0; auto const &spframe : d_sharedpreferenceframes)
    if (!writeRawFrameDataToFile(directory + "/SharedPreference_" + bepaald::toString(count++) + ".sbf", spframe))
      return false;

  // export stickers
  std::cout << "Writing StickerFrames..." << std::endl;
  for (int count = 0; auto const &sframe : d_stickers)
  {
    StickerFrame *s = sframe.second.get();
    std::string sticker_basefilename = directory + "/Sticker_" + bepaald::toString(count++) + "_" + bepaald::toString(s->rowId());

    // write frame
    if (!writeRawFrameDataToFile(sticker_basefilename + ".sbf", s))
      return false;

    // write actual sticker data
    std::ofstream stickerstream(sticker_basefilename + ".bin", std::ios_base::binary);
    if (!stickerstream.is_open())
    {
      std::cout << "Failed to open file for writing: " << directory << sticker_basefilename << ".bin" << std::endl;
      return false;
    }
    else
      if (!stickerstream.write(reinterpret_cast<char *>(s->attachmentData()), s->attachmentSize()))
        return false;
  }

  // export endframe
  std::cout << "Writing EndFrame..." << std::endl;
  if (!writeRawFrameDataToFile(directory + "/End.sbf", d_endframe))
    return false;

  // export database
  std::cout << "Writing database..." << std::endl;
  SqliteDB database(directory + "/database.sqlite", false /*readonly*/);
  if (!SqliteDB::copyDb(d_database, database))
  {
    std::cout << "Error exporting sqlite database" << std::endl;
    return false;
  }

  std::cout << "Done!" << std::endl;
  return true;
}
