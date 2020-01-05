/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

// This function was taken from https://github.com/agauniyal/rang/
// Used here to (poorly!) detect support for ansi escape codes
bool SqliteDB::QueryResults::supportsAnsi() const
{
#if defined(_WIN32) || defined(__MINGW64__)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hConsole, &mode);
  return mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
#endif
  static const bool result = []
                             {
                               const char *Terms[]
                                 = { "ansi",    "color",  "console", "cygwin", "gnome",
                                     "konsole", "kterm",  "linux",   "msys",   "putty",
                                     "rxvt",    "screen", "vt100",   "xterm" };
                               const char *env_p = std::getenv("TERM");
                               if (env_p == nullptr)
                                 return false;
                               return std::any_of(std::begin(Terms), std::end(Terms),
                                                  [&](const char *term)
                                                  {
                                                    return std::strstr(env_p, term) != nullptr;
                                                  });
                             }();
  return result;
}
