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

#include "desktopdatabase.ih"

#if !defined(_WIN32) && !defined(__MINGW64__) && (!defined(__APPLE__) || !defined(__MACH__)) // NOT IMPLEMENTED YET -> SEE https://github.com/bepaald/get_signal_desktop_key

#include "desktopdatabase.ih"

void DesktopDatabase::getSecrets_linux(std::set<std::string> *secrets) const
{
  Logger::error       ("Found encrypted sqlcipher key in config file. Decrypting this key");
  Logger::error_indent("is not yet supported on Linux. To obtain the decrypted key, please");
  Logger::error_indent("see: https://github.com/bepaald/get_signal_desktop_key and use the");
  Logger::error_indent("`--desktopkey' option to pass it to this program");
  Logger::error_indent("This functionality is planned to be included here in the future.");

  return;
}

#endif
