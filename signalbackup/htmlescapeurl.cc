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

std::string SignalBackup::HTMLescapeUrl(std::string const &in) const
{
  std::string result(in);
  HTMLescapeUrl(&result);
  return result;
}

void SignalBackup::HTMLescapeUrl(std::string *in) const
{
  for (unsigned int i = 0; i < in->size(); ++i)
  {
    if (!((*in)[i] >= 'A' && (*in)[i] <= 'Z') &&  // A-Z
        !((*in)[i] >= 'a' && (*in)[i] <= 'z') &&  // a-z
        !((*in)[i] >= '0' && (*in)[i] <= '9') &&  // 0-9
        !((*in)[i] >= '\'' && (*in)[i] <= '*') && // ' ( ) *
        (*in)[i] != '!' &&
        (*in)[i] != '-' &&
        (*in)[i] != '.' &&
        (*in)[i] != '_' &&
        (*in)[i] != '~')
    {
      // it is not an allowed character, escape it
      std::string escape = "%" + bepaald::toHexString(static_cast<int>((*in)[i]) & 0xff);
      in->replace(i, 1, escape);
      i += escape.size() - 1;
    }
  }
}
