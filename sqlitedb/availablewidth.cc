/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#include "sqlitedb.ih"

int SqliteDB::QueryResults::availableWidth() const
{
#if defined(_WIN32) || defined(__MINGW64__) // this is untested, I don't have windows
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  if (ret)
    return (csbi.dwSize.X - 1 < 40) ? 40 : csbi.dwSize.X - 1;
  return 80;
#else // !windows
  struct winsize ts;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ts) != -1)
    return (ts.ws_col < 40) ? 40 : ts.ws_col;
  return 80; // some random default;
#endif
}
