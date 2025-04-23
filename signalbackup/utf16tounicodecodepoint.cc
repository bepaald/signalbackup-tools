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

#if __cpp_lib_unreachable >= 202202L
#include <utility>
#endif

// return:
// 0 = ok, finished
// -1 = error, stop!
// 1 = need more data to complete codepoint
int SignalBackup::utf16ToUnicodeCodepoint(uint16_t utf16, uint32_t *codepoint) const
{
  /*
    U = some unicode codepoint > 0x100000
    U' = yyyyyyyyyyxxxxxxxxxx  // U - 0x10000
    W1 = 110110yyyyyyyyyy      // 0xD800 + yyyyyyyyyy
    W2 = 110111xxxxxxxxxx      // 0xDC00 + xxxxxxxxxx
  */
  // low surrogate
  if (utf16 >= 0xDC00 && utf16 < 0xE000)
  {
    *codepoint += (utf16 & 0b0000'0011'1111'1111);
    *codepoint += 0x10000;
    return 0;
  }

  if (*codepoint != 0) [[unlikely]]
    return -1;

  // check if it is a high surrogate
  if (utf16 >= 0xD800 && utf16 < 0xE000)
  {
    *codepoint = ((utf16 & 0b0000'0011'1111'1111) << 10);
    return 1;
  }

  if (utf16 < 0xD800 || utf16 >= 0xE000) // single char codepoint
  {
    *codepoint = utf16;
    return 0;
  }

#if __cpp_lib_unreachable >= 202202L
  std::unreachable();
#endif
  return -1;
}
