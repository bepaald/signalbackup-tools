/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

std::string SignalBackup::HTMLwriteAvatar(long long int recipient_id, std::string const &directory,
                                          std::string const &threaddir, bool overwrite, bool append) const
{
  std::string avatar;
  auto pos = d_avatars.end();
  if ((pos =
       std::find_if(d_avatars.begin(), d_avatars.end(),
                    [recipient_id](auto const &p) { return p.first == bepaald::toString(recipient_id); })) != d_avatars.end())
  {
    avatar = "media/Avatar_" + pos->first + ".bin";

    // directory + threaddir is guaranteed to exist at this point, check/create 'media'
    if (!bepaald::fileOrDirExists(directory + "/" + threaddir + "/media"))
    {
      if (!bepaald::createDir(directory + "/" + threaddir + "/media"))
      {
        Logger::error("Failed to create directory `", directory, "/", threaddir, "/media");
        return std::string();
      }
    }
    else if (!bepaald::isDir(directory + "/" + threaddir + "/media"))
    {
      Logger::error("Failed to create directory `", directory, "/", threaddir, "/media");
      return std::string();
    }

    // check actual avatar file
    if (bepaald::fileOrDirExists(directory + "/" + threaddir + "/" + avatar))
    {
      if (append) // file already exists, but we were asked to just use the existing file, so we're done
        return avatar;
      if (!overwrite) // file already exists, but we were no asked to overwrite -> error!
      {
        Logger::error("Avatar file exists. Not overwriting");
        return std::string();
      }
    }

    // directory exists, now write avatar
    AvatarFrame *a = pos->second.get();

    std::ofstream avatarstream(directory + "/" + threaddir + "/" + avatar, std::ios_base::binary);
    if (!avatarstream.is_open())
    {
      Logger::error("Failed to open file for writing: '", directory, "/", threaddir, "/", avatar, "'");
      return std::string();
    }
    else
      if (!avatarstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize()))
        return std::string();
  }
  return avatar;
}
