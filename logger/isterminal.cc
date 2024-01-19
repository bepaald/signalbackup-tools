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

#include "logger.h"

bool Logger::isTerminal()
{
#ifdef HAS_UNISTD_H_ // defined if unistd.h is available
  static const bool result = []
  {
    return isatty(STDOUT_FILENO);
  }();
  return result;
#else
#if defined(_WIN32) || defined(__MINGW64__)
  DWORD filetype = GetFileType(GetStdHandle(STD_OUTPUT_HANDLE));
  return filetype != FILE_TYPE_PIPE &&  filetype != FILE_TYPE_DISK; // this is not foolproof (eg output is printer)...
#endif
  return false;
#endif
}
