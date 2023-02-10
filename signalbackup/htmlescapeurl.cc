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

#include "signalbackup.ih"

void SignalBackup::HTMLescapeUrl(std::string *in) const
{
  for (uint i = 0; i < in->size(); ++i)
  {
    if (!(in->at(i) >= 'A' && in->at(i) <= 'Z') &&  // A-Z
        !(in->at(i) >= 'a' && in->at(i) <= 'z') &&  // a-z
        !(in->at(i) >= '0' && in->at(i) <= '9') &&  // 0-9
        !(in->at(i) >= '\'' && in->at(i) <= '*') && // ' ( ) *
        in->at(i) != '!' &&
        in->at(i) != '-' &&
        in->at(i) != '.' &&
        in->at(i) != '_' &&
        in->at(i) != '~')
    {
      // it is not an allowed character, escape it
      std::string escape = "%" + bepaald::toString(static_cast<int>(in->at(i)) & 0xff, true);
      in->replace(i, 1, escape);
      i += escape.size() - 1;
    }
  }
}
