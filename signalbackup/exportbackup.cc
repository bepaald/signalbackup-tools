/*
  Copyright (C) 2024  Selwin van Dijk

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

bool SignalBackup::exportBackup(std::string const &filename, std::string const &passphrase, bool overwrite,
                                bool keepattachmentdatainmemory, bool onlydb)
{
  // if output is existing directory, or doesn't exist but ends in directory delim. -> output to dir
  if ((bepaald::fileOrDirExists(filename) && bepaald::isDir(filename)) ||
      (!bepaald::fileOrDirExists(filename) &&
      (filename.back() == '/' || filename.back() == std::filesystem::path::preferred_separator)))
    return exportBackupToDir(filename, overwrite, keepattachmentdatainmemory, onlydb);

  // export to file
  return exportBackupToFile(filename, passphrase, overwrite, keepattachmentdatainmemory);
}
