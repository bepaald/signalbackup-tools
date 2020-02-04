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

#include "signalbackup.ih"

void SignalBackup::escapeXmlString(std::string *str) const
{
  size_t pos = 0;
  while (pos != str->size())
  {
    if (str->at(pos) == '&')
    {
      str->replace(pos, 1, "&amp;");
      pos += STRLEN("&amp;");
      continue;
    }

    if (str->at(pos) == '<')
    {
      str->replace(pos, 1, "&lt;");
      pos += STRLEN("&lt;");
      continue;
    }

    if (str->at(pos) == '>')
    {
      str->replace(pos, 1, "&gt;");
      pos += STRLEN("&gt;");
      continue;
    }

    if (str->at(pos) == '"')
    {
      str->replace(pos, 1, "&quot;");
      pos += STRLEN("&quot;");
      continue;
    }

    if (str->at(pos) == '\'')
    {
      str->replace(pos, 1, "&apos;");
      pos += STRLEN("&apos;");
      continue;
    }

    // [^\u0020-\uD7FF] <-- range that's escaped (note the ^)

    /*
      under \u0020 = control chars (escape, linebreak, etc...)
    */
    if ((static_cast<unsigned int>(str->at(pos)) & 0xFF) < 0x20)
    {
      std::string rep = "&#" + bepaald::toString(static_cast<unsigned int>(str->at(pos)) & 0xFF) + ";";
      str->replace(pos, 1, rep);
      pos += rep.length();
      continue;
    }

    /*
      If you know that the data is UTF-8, then you just have to check the high bit:

      0xxxxxxx = single-byte ASCII character
      1xxxxxxx = part of multi-byte character

      Or, if you need to distinguish lead/trail bytes:

      10xxxxxx = 2nd, 3rd, or 4th byte of multi-byte character
      110xxxxx = 1st byte of 2-byte character
      1110xxxx = 1st byte of 3-byte character
      11110xxx = 1st byte of 4-byte character
    */

    /*
      Over 0xd7ff.
    */

    //0x800 - 0xffff is only 3 byte utf chars, and are represented by utf16 points directly?
    if ((str->at(pos) & 0b11110000) == 0b11100000)
    {
      if (pos + 2 >= str->size())
      {
        ++pos;
        continue;
      }
      uint32_t unicode = 0;
      unicode += (static_cast<uint32_t>(str->at(pos) & 0b0001111) << 12);
      unicode += (static_cast<uint32_t>(str->at(pos + 1) & 0b0111111) << 6);
      unicode += (static_cast<uint32_t>(str->at(pos + 2) & 0b0111111));

      std::string rep = "&#" + bepaald::toString(unicode) + ";";

      str->replace(pos, 3, rep);
      pos += rep.length();
      continue;
    }

    // Beyond 0xffff is only 4 byte utf chars
    if ((str->at(pos) & 0b11111000) == 0b11110000) // or 0b11110000
    {

      if (pos + 3 >= str->size())
      {
        ++pos;
        continue;
      }

      /*
        bytes of unicode char:
        UTF8:    11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
        UNICODE: ........ ...XXX_XX XXXX_XXXX XX_XXXXXX {21}
        U' (UNICODE - 0x10000): yyyyyyyyyzzzzzzzzzz
        UTF16 LOW: 110110yyyyyyyyyy
        UTF16 HI : 110111zzzzzzzzzz
      */

      uint32_t unicode = 0;
      unicode += (static_cast<uint32_t>(str->at(pos) & 0b0000111) << 18);
      unicode += (static_cast<uint32_t>(str->at(pos + 1) & 0b0111111) << 12);
      unicode += (static_cast<uint32_t>(str->at(pos + 2) & 0b0111111) << 6);
      unicode += (static_cast<uint32_t>(str->at(pos + 3) & 0b0111111));
      unicode -= 0x10000;

      std::string rep = "&#" + bepaald::toString(0xd800 + (unicode >> 10)) + ";&#" + bepaald::toString(0xdc00 + (unicode & 0x3FF)) + ";";

      str->replace(pos, 4, rep);
      pos += rep.length();
      continue;
    }

    ++pos;

  }
}
