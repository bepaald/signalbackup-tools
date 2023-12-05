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

#include "logger.h"

void Logger::outputHead(std::string const &file, std::string const &standardout, bool overwrite,
                                std::pair<std::string, std::string> const &control) // static
{
  if (s_instance->d_file)
  {
    // print any control codes if supported
    if (s_instance->d_controlcodessupported)
      *(s_instance->d_file) << control.first;

    *(s_instance->d_file) << file;

    if (s_instance->d_usetimestamps)
      dispTime(*(s_instance->d_file));

    // print any control codes if supported
    if (s_instance->d_controlcodessupported)
      *(s_instance->d_file) << control.second;

    if (!file.empty())
      *(s_instance->d_file) << ": ";
  }

  if (overwrite)
    std::cout << (s_instance->d_controlcodessupported ? "\33[2K\r" : "\r");

  // print any control codes if supported
  if (s_instance->d_controlcodessupported)
    std::cout << control.first;

  std::cout << standardout;

  // print any control codes if supported
  if (s_instance->d_controlcodessupported)
    std::cout << control.second;

  if (s_instance->d_usetimestamps)
    dispTime(std::cout);
  if (!standardout.empty())
    std::cout << ": ";
}

void Logger::outputHead(std::string const &head, bool overwrite, std::pair<std::string, std::string> const &control) // static
{
  outputHead(head, head, overwrite, control);
}
