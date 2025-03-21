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

#include "logger.h"

#include <chrono>

// prints out a header containing current date and time
void Logger::firstUse() // static
{
  if (!s_instance->d_used) [[unlikely]]
  {
    s_instance->d_used = true;

    std::time_t cur = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (s_instance->d_currentoutput)
      *(s_instance->d_currentoutput) << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " ***" << "\n";
    std::cout << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " ***" << std::endl;
  }
}
