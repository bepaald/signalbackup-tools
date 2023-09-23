/*
  Copyright (C) 2023  Selwin van Dijk

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

bool SignalBackup::dtSetAvatar(std::string const &avatarpath, long long int rid, std::string const &databasedir)
{
  // set avatar
  //std::string avatarpath = res("avatar");
  if (avatarpath.empty())
    return false;

  AttachmentMetadata amd = getAttachmentMetaData(databasedir + "/attachments.noindex/" + avatarpath);
  if (!amd)
    return false;

  std::unique_ptr<AvatarFrame> new_avatar_frame;
  if (setFrameFromStrings(&new_avatar_frame, std::vector<std::string>{"RECIPIENT:string:" + bepaald::toString(rid),
                                                                      "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
  {
    new_avatar_frame->setLazyDataRAW(amd.filesize, databasedir + "/attachments.noindex/" + avatarpath);
    d_avatars.emplace_back(std::make_pair(bepaald::toString(rid), std::move(new_avatar_frame)));
    return true;
  }
  return false;
}
