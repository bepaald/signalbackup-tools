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
      Logger::error("Failed to create directory `", directory, "'",
                    " (errno: ", std::strerror(errno), ")"); // note: errno is not required to be set by std
      // temporary !!
      {
        std::error_code ec;
        std::filesystem::space_info const si = std::filesystem::space(directory, ec);
        if (!ec)
        {
          Logger::message("Available  : ", static_cast<std::intmax_t>(si.available));
          Logger::message("Backup size: ", d_fd->total());
        }
      }
      return false;
    }
  }

  // directory exists, but is it a dir?
  if (!bepaald::isDir(directory))
  {
    Logger::error("`", directory, "' is not a directory.");
    return false;
  }

  // and is it empty?
  if (!bepaald::isEmpty(directory) &&                 // NOT EMPTY, but we cant write into it,
      ((allowappend && !append) || !allowappend))     // because append is not allowed, or allowed but not requested
  {
    if (!overwrite)
    {
      if (allowappend)
      {
        Logger::error("Directory '", directory, "' is not empty. Use --overwrite to clear directory contents before");
        Logger::error_indent("export, or --append to only write new files.");
      }
      else
        Logger::error("Directory '", directory, "' is not empty. Use --overwrite to clear directory contents before export.");
      return false;
    }
    Logger::message("Clearing contents of directory '", directory, "'...");
    if (!bepaald::clearDirectory(directory))
    {
      Logger::error("Failed to empty directory '", directory, "'");
      return false;
    }
  }

  return true;
}
