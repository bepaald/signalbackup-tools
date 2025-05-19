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

#include "logger.h"

void Logger::outputHead(std::string const &file, std::string const &standardout, bool overwrite,
                        std::pair<std::string_view, std::string_view> const &prepost,
                        std::pair<std::string_view, std::string_view> const &control)
{
  if (d_currentoutput)
  {
    if (!file.empty())
      *(d_currentoutput) << prepost.first;

    *(d_currentoutput) << file;

    if (d_usetimestamps)
      dispTime(*(d_currentoutput));

    if (!file.empty())
      *(d_currentoutput) << prepost.second;
  }

  if (overwrite)
    std::cout << (d_controlcodessupported ? "\33[2K\r" : "\r");

  if (!standardout.empty())
    std::cout << prepost.first;

  // print any control codes if supported
  if (d_controlcodessupported)
    std::cout << control.first;

  std::cout << standardout;

  // print any control codes if supported
  if (d_controlcodessupported)
    std::cout << control.second;

  if (d_usetimestamps)
    dispTime(std::cout);
  if (!standardout.empty())
    std::cout << prepost.second;
}

void Logger::outputHead(std::string const &head, bool overwrite,
                        std::pair<std::string_view, std::string_view> const &prepost,
                        std::pair<std::string_view, std::string_view> const &control)
{
  outputHead(head, head, overwrite, prepost, control);
}
