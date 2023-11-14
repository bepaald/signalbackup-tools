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

bool SignalBackup::prepareOutputDirectory(std::string const &directory, bool overwrite, bool allowappend, bool append) const
{
  // check if dir exists, create if not
  if (!bepaald::fileOrDirExists(directory))
  {
    // try to create
    if (!bepaald::createDir(directory))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed to create directory `" << directory << "'"
                << " (errno: " << std::strerror(errno) << ")" << std::endl; // note: errno is not required to be set by std
      // temporary !!
      {
        std::error_code ec;
        std::filesystem::space_info const si = std::filesystem::space(directory, ec);
        if (!ec)
        {
          std::cout << "Available  : " << static_cast<std::intmax_t>(si.available) << std::endl;
          std::cout << "Backup size: " << d_fd->total() << std::endl;
        }
      }
      return false;
    }
  }

  // directory exists, but is it a dir?
  if (!bepaald::isDir(directory))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": `" << directory << "' is not a directory." << std::endl;
    return false;
  }

  // and is it empty?
  if (!bepaald::isEmpty(directory) &&                 // NOT EMPTY, but we cant write into it,
      ((allowappend && !append) || !allowappend))     // because append is not allowed, or allowed but not requested
  {
    if (!overwrite)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Directory '" << directory << "' is not empty. Use --overwrite to clear directory contents before export";
      if (allowappend)
        std::cout << "," << std::endl << "       or --append to only write new files." << std::endl;
      else
        std::cout << "." << std::endl;
      return false;
    }
    std::cout << "Clearing contents of directory '" << directory << "'..." << std::endl;
    if (!bepaald::clearDirectory(directory))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed to empty directory '" << directory << "'" << std::endl;
      return false;
    }
  }

  return true;
}
