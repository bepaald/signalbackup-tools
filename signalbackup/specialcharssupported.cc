/*
  Copyright (C) 2025  Selwin van Dijk

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

#if !defined(_WIN32) && !defined(__MINGW64__)

#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>

bool SignalBackup::specialCharsSupported(std::string const &path) const
{
  std::ifstream mounts("/proc/self/mounts", std::ios_base::in | std::ios_base::binary);
  if (!mounts.is_open())
  {
    if (d_verbose) [[unlikely]]
      Logger::warning("Could not open '/proc/self/mounts' for reading");
    return true; // we'll assume it will work when not on windows
  }

  // put mount paths and types in mount_info
  std::vector<std::pair<std::string, std::string>> mount_info;
  std::string dummy, mpath, mtype;
  while (!mounts.eof())
  {
    mounts >> dummy >> mpath >> mtype >> dummy >> dummy >> dummy;
    if (mtype != "autofs")
      mount_info.emplace_back(mpath, mtype);
  }

  // sort mount_info by path length...
  std::sort(mount_info.begin(), mount_info.end(), [](auto const &a, auto const &b){ return a.first.length() > b.first.length(); });

  // std::cout << "Got mount info" << std::endl;
  // for (auto const &[p, t] : mount_info)
  //   std::cout << p << " " << t << std::endl;

  std::error_code ec;
  std::string canonicalpath = std::filesystem::canonical(path, ec);
  if (ec)
    return false;

  // std::cout << "Got canonical path: " << canonicalpath << std::endl;

  // find output path in mount_info, check type
  for (auto const &[p, t] : mount_info)
    if (canonicalpath.starts_with(p))
    {
      if (d_verbose) [[unlikely]]
        Logger::message("Detected target filesystem type '", t, "' (path ", path, ")");

      if (t == "ext4" ||
          t == "ext3" ||
          t == "ext2" ||
          t == "btrfs" ||
          t == "zfs")
        return true;
      else
        return false;
    }
  return false;
}

#else

bool SignalBackup::specialCharsSupported(std::string const &) const
{
  return false;
}

#endif
