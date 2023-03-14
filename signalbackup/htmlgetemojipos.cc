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

std::vector<std::pair<unsigned int, unsigned int>> SignalBackup::HTMLgetEmojiPos(std::string const &str) const
{
  // for (char c : str)
  // {
  //   if (std::isprint(c))
  //     std::cout << c;
  //   else
  //     std::cout << std::hex << static_cast<int>(c & 0xff);
  // }
  // std::cout << "" << std::endl;

  std::vector<std::pair<unsigned int, unsigned int>> results;

  for (uint i = 0; i < std::max(static_cast<unsigned int>(str.size()), s_emoji_min_size) - s_emoji_min_size; ++i)
  {
    //std::cout << "Checking byte " << std::dec << i << ": " << std::hex << static_cast<int>(str[i] & 0xff) << std::endl;
    if (bepaald::contains(s_emoji_first_bytes, str[i]))
    {
      for (char const *const emoji_string : s_emoji_unicode_list)
      {
        int emoji_size = std::strlen(emoji_string);
        if (i <= (str.size() - emoji_size) &&
            std::strncmp(str.data() + i, emoji_string, emoji_size) == 0)
        {

          // std::cout << "matched emoji: ";
          // for (uint c = 0; c < emoji_size; ++c)
          //   std::cout << std::hex << static_cast<int>(emoji_string[c] & 0xff);
          // std::cout << "" << std::endl;

          results.emplace_back(std::make_pair(i, emoji_size));
          i += emoji_size - 1; // minus one because ++i in for loop

          //str->insert(i, "<*>");
          //str->insert(i + 3 + emoji_size, "<*>");
          //i += 3 + emoji_size + 3;

          //std::cout << *str << std::endl;
        }
      }
    }
    //else if (str->at(i) != ' ') // spaces don't count
    //  all_emoji = false;
  }
  return results;
}
