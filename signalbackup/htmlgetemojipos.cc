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

std::vector<std::pair<unsigned int, unsigned int>> SignalBackup::HTMLgetEmojiPos(std::string_view str) const
{
  std::vector<std::pair<unsigned int, unsigned int>> results;
  bool hit = true;

  for (unsigned int c = 0; c < str.size(); ++c)
  {
    // check first byte, emoji begin with one of a few possible bytes,
    // that are otherwise fairly rare in messages, find the first
    // possible starting position.
    if (static_cast<unsigned char>(str[c]) > 193U ||
        (static_cast<unsigned char>(str[c]) < 58U && static_cast<unsigned char>(str[c]) > 47U) ||
        static_cast<unsigned char>(str[c]) == 42U ||
        static_cast<unsigned char>(str[c]) == 35U)
    {
      // now that a (possible) emoji byte was found, lets see _which_ emoji it is.
      // many emoji start with the same byte sequence, so we check back to front
      for (std::string_view const &emoji_string : s_emoji_unicode_list)
      {
        if (str[c] != emoji_string[0])
          continue;
        hit = true;
        for (unsigned int i = emoji_string.size(); i-- ;)
        {
          if ((c + i) >= str.size() || str[c + i] != emoji_string[i])
          {
            hit = false;
            break;
          }
        }
        if (hit)
        {
          results.emplace_back(c, emoji_string.size());
          c += emoji_string.size() - 1;
          break;
        }
      }
    }
  }
  return results;
}

/*
std::vector<std::pair<unsigned int, unsigned int>> SignalBackup::HTMLgetEmojiPos(std::string_view str) const
{
  std::vector<std::pair<unsigned int, unsigned int>> results;
  std::string::size_type pos = 0;

  // check first byte, emoji begin with one of a few possible bytes,
  // that are otherwise fairly rare in messages, find the first
  // possible starting position.
  while ((pos = str.find_first_of(s_emoji_first_bytes, pos)) != std::string::npos)
  {
    for (std::string_view const &emoji_string : s_emoji_unicode_list)
    {
      bool hit = true;
      // now that a (likely) emoji byte was found, lets see _which_ emoji it is.
      // many emoji start with the same byte sequence, so we check back to front
      for (unsigned int i = emoji_string.size(); i-- ;)
        if ((pos + i) >= str.size() || str[pos + i] != emoji_string[i])
        {
          hit = false;
          break;
        }
      if (hit)
      {
        results.emplace_back(pos, emoji_string.size());
        pos += emoji_string.size() - 1;
      }
    }
    ++pos;
  }
  return results;
}
*/

/*
std::vector<std::pair<unsigned int, unsigned int>> SignalBackup::HTMLgetEmojiPos(std::string const &str) const
{
  std::vector<std::pair<unsigned int, unsigned int>> results;
  std::string::size_type pos = 0;

  while ((pos = str.find_first_of(s_emoji_first_bytes, pos)) != std::string::npos)
  {
    for (std::string const &emoji_string : s_emoji_unicode_list)
    {
      if (pos <= (str.size() - emoji_string.size()) &&
          std::memcmp(str.data() + pos, emoji_string.data(), emoji_string.size()) == 0)
      {
        results.emplace_back(pos, emoji_string.size());
        pos += emoji_string.size() - 1; // minus one because ++pos in while...
      }
    }
    ++pos;
  }
  return results;
}
*/
