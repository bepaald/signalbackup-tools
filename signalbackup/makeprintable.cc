/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

std::string SignalBackup::makePrintable(std::string const &in) const
{
  std::string printable_uuid(in);

  unsigned int offset = (STRING_STARTS_WITH(in, "__signal_group__v2__!") ? STRLEN("__signal_group__v2__!") + 4 :
                         (STRING_STARTS_WITH(in, "__textsecure_group__!") ? STRLEN("__textsecure_group__!") + 4 : 4));

  if (STRING_STARTS_WITH(in, "__signal_group__v2__!") ||
      STRING_STARTS_WITH(in, "__textsecure_group__!"))
  {
    if (offset < in.size()) [[likely]]
      std::replace_if(printable_uuid.begin() + offset, printable_uuid.end(), [](char c){ return c != '-'; }, 'x');
    else
      printable_uuid = "xxx";
  }
  else if (std::all_of(printable_uuid.begin(), printable_uuid.end(), [](char c){ return (c >= '0' && c <= '9') || c == ' ' || c == '+' || c == '~'; }) &&
           printable_uuid.size() >= 10)
    std::replace_if(printable_uuid.begin(), printable_uuid.end() - 4, [](char c){ return (c >= '0' && c <= '9'); }, 'x');
  else
    printable_uuid = "xxx";
  return printable_uuid;
}
