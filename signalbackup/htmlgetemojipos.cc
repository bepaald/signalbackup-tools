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
        results.emplace_back(std::make_pair(pos, emoji_string.size()));
        pos += emoji_string.size() - 1; // minus one because ++pos in while...
      }
    }
    ++pos;
  }
  return results;
}
