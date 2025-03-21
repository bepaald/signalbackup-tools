/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

#include "../desktopattachmentreader/desktopattachmentreader.h"
#include "../attachmentmetadata/attachmentmetadata.h"

bool SignalBackup::dtSetAvatar(std::string const &avatarpath, std::string const &key, int64_t size, int version,
                               long long int rid, std::string const &databasedir)
{
  // set avatar
  //std::string avatarpath = res("avatar");
  if (avatarpath.empty())
    return true;

  if (version >= 2 && (key.empty() || size <= 0))
  {
    Logger::error("Decryption info for avatar not valid. (version: ", version, ", key: ", key, ", size: ", size, ")");
    return false;
  }


  // get attachment metadata !! NOTE RAW POINTER
  AttachmentMetadata amd;
  std::string fullpath(databasedir + "/attachments.noindex/" + avatarpath);
  if (version >= 2)
  {
    DesktopAttachmentReader dar(version, fullpath, key, size);
#if __cpp_lib_out_ptr >= 202106L
    std::unique_ptr<unsigned char[]> att_data;
    if (dar.getAttachmentData(std::out_ptr(att_data), d_verbose) != 0)
#else
    unsigned char *att_data = nullptr;
    if (dar.getAttachmentData(&att_data, d_verbose) != 0)
#endif
    {
      Logger::error("Failed to get avatar data");
      return false;
    }
#if __cpp_lib_out_ptr >= 202106L
    amd = AttachmentMetadata::getAttachmentMetaData(fullpath, att_data.get(), size); // get metadata from heap
#else
    amd = AttachmentMetadata::getAttachmentMetaData(fullpath, att_data, size);       // get metadata from heap
    if (att_data)
      delete[] att_data;
#endif
  }
  else
    amd = AttachmentMetadata::getAttachmentMetaData(fullpath);                        // get from file


  if (!amd)
    return false;

  DeepCopyingUniquePtr<AvatarFrame> new_avatar_frame;
  if (setFrameFromStrings(&new_avatar_frame, std::vector<std::string>{"RECIPIENT:string:" + bepaald::toString(rid),
                                                                      "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
  {
    new_avatar_frame->setReader(new DesktopAttachmentReader(version, fullpath, key, size));
    d_avatars.emplace_back(std::make_pair(bepaald::toString(rid), std::move(new_avatar_frame)));
    return true;
  }
  return false;
}
