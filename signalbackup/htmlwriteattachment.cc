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

bool SignalBackup::HTMLwriteAttachment(std::string const &directory, std::string const &threaddir,
                                       long long int rowid, long long int uniqueid,  bool overwrite,
                                       bool append) const
{
  if (!d_attachments.contains({rowid, uniqueid}))
    return false;

  // directory + threaddir is guaranteed to exist at this point, check/create 'media'
  if (!bepaald::fileOrDirExists(directory + "/" + threaddir + "/media"))
  {
    if (!bepaald::createDir(directory + "/" + threaddir + "/media"))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": failed to create directory `" << directory << "/" << threaddir << "/media" << std::endl;
      return false;
    }
  }
  else if (!bepaald::isDir(directory + "/" + threaddir + "/media"))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": failed to create directory `" << directory << "/" << threaddir << "/media" << std::endl;
    return false;
  }

  // check actual attachmentfile file
  std::string attachment_filename = directory + "/" + threaddir +
    "/media/Attachment_" + bepaald::toString(rowid) + "_" + bepaald::toString(uniqueid) + ".bin";
  if (bepaald::fileOrDirExists(attachment_filename))
  {
    if (append) // file already exists, but we were asked to just use the existing file, so we're done
      return true;

    if (!overwrite) // file already exists, but we were no asked to overwrite -> error!
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Attachment file exists. Not overwriting" << std::endl;
      return false;
    }
  }

  // write actual attachment:
  AttachmentFrame *a = d_attachments.at({rowid, uniqueid}).get();
  std::ofstream attachmentstream(attachment_filename, std::ios_base::binary);
  if (!attachmentstream.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to open file for writing: '" << attachment_filename << "'" << std::endl;
    return false;
  }
  else
  {
    if (!attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize()))
      return false;
    // write was succesfull. drop attachment data
    a->clearData();
  }
  return true;
}
