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

#include <regex>

#include "../common_filesystem.h"

bool SignalBackup::makeFilenameUnique(std::string const &path, std::string *file_or_dir) const
{
  while (bepaald::fileOrDirExists(bepaald::concat(path, "/", *file_or_dir)))
  {
    //std::cout << std::endl << "File exists: " << path << "/" << file_or_dir << " -> ";

    std::filesystem::path p(*file_or_dir);
    std::regex numberedfile(".*( \\(([0-9]*)\\))$");
    std::smatch sm;
    std::string filestem(p.stem().string());
    //std::string ext(p.extension().string());
    int counter = 2;
    if (regex_match(filestem, sm, numberedfile) && sm.size() >= 3 && sm[2].matched)
    {
      // increase the counter
      counter = bepaald::toNumber<int>(sm[2].str()) + 1;
      // remove " (xx)" part from stem
      filestem.erase(sm[1].first, sm[1].second);
    }
    *file_or_dir = filestem + " (" + bepaald::toString(counter) + ")" + p.extension().string();

    //std::cout << file_or_dir << std::endl;
  }

  return true;

}
