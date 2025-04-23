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

#include "../common_regex.h"

bool SignalBackup::unescapeXmlString(std::string *s) const
{
  //std::cout << " IN: " << *s << std::endl;

  if (s->find('&') == std::string::npos)
    return true;

  bepaald::replaceAll(s, "&amp;", "&");
  bepaald::replaceAll(s, "&apos;", "'");
  bepaald::replaceAll(s, "&quot;", "\"");
  bepaald::replaceAll(s, "&lt;", "<");
  bepaald::replaceAll(s, "&gt;", ">");

  REGEX entity_regex("&#(x?[0-9a-fA-F]+);", REGEX_FLAGS);
  REGEX_SMATCH_RESULTS m;
  std::string searchstr(*s);
  int codepointcomplete = 0;
  uint32_t unicode_codepoint = 0;
  int match_position = 0;
  int match_length = 0;
  while (REGEX_SEARCH(searchstr, m, entity_regex))
  {
    // digits
    std::string utf16str(m[1]); // value-part of the match
    uint32_t utf16 = 0;
    if (codepointcomplete == 1) // we're here because we need more data
      match_length += m.length(0);
    else
    {
      match_position = m.position(); // pos and length of entire
      match_length = m.length(0);     // match (including &#;)
    }
    // check leading x, interpret as hex...
    if (utf16str[0] == 'x') [[unlikely]] // I dont think this exists in Signal plaintext
    {                                    // backups but it does in SMS Backup & Restore
      utf16str = utf16str.substr(1);
      utf16 = bepaald::toNumberFromHex<uint32_t>(utf16str);
    }
    else
      utf16 = bepaald::toNumber<uint32_t>(utf16str);

    //std::cout << "found utf16: " << utf16 << " (" << utf16str << ")" << std::endl;
    if (utf16 > 0xFFFF) [[unlikely]] // SMS Backup & Restore stores as unicode32
    {
      codepointcomplete = 0;
      unicode_codepoint = utf16;
    }
    else
      codepointcomplete = utf16ToUnicodeCodepoint(utf16, &unicode_codepoint);
    if (codepointcomplete == 1)
    {
      //std::cout << "requested more" << std::endl;
      searchstr = m.suffix();
      continue;
    }
    if (codepointcomplete == -1) [[unlikely]]
    {
      Logger::warning("Failed to fully un-escape XML string: '", *s, "'");
      return false;
    }

    //std::cout << "Codepoint: " << unicode_codepoint << " UTF8: ";
    //std::string utf8(unicodeToUtf8(unicode_codepoint));
    //std::cout << bepaald::bytesToHexString(reinterpret_cast<unsigned char *>(utf8.data()), utf8.size()) << std::endl;

    // codepointcomplete == 0 : do the replace... start search at top...
    s->replace(match_position, match_length, unicodeToUtf8(unicode_codepoint));
    unicode_codepoint = 0;
    searchstr = *s;
  }
  if (codepointcomplete != 0)
  {
    Logger::warning("Failed fully to un-escape XML string: '", *s, "'");
    return false;
  }

  //std::cout << "OUT: " << *s << std::endl;

  return true;
}
