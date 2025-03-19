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

void SignalBackup::HTMLwriteCallLinkDiv(std::ofstream &htmloutput, int indent, std::string const &url, std::string const &title,
                                        std::string const &description) const
{
  // get avatar color
  /*
    the first byte of the root key is used to get a random color, the root key
    as found in the url is the binary key in base16 encoding.

    See: https://github.com/signalapp/ringrtc/blob/9eec8cf6795899e0e0f6a17d4e902c160cc96f00/src/rust/src/lite/call_links/root_key.rs#L147-L155
    And: https://github.com/signalapp/ringrtc/blob/9eec8cf6795899e0e0f6a17d4e902c160cc96f00/src/rust/src/lite/call_links/base16.rs#L87-L145

    It looks complicated, but basically each pair of letters from the alphabet, represents one 8-bit number
    const ALPHABET: &[u8; 16] = b"bcdfghkmnpqrstxz";

    1st letter >  b   c   d   f   g   h   k   m   n   p   q   r   s   t   x   z
    2nd letter v
                  b  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
                  c  16  17  18  ....
                  d
                  [...]
                  t
                  x                                          ... 235 236 237 238 239
                  z  240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
  */
  std::string color = "E3E3FE"; // random default
  if (url.size() > STRLEN("https://signal.link/call/#key=") + 2)
  {
    auto consonant_base16_decode = [](char first, char second) STATICLAMBDA -> int32_t
    {
      int high = -1;
      int low  = -1;
      switch (first)
      {
        case 'b':
          high = 0;
          break;
        case 'c':
          high = 16;
          break;
        case 'd':
          high = 32;
          break;
        case 'f':
          high = 48;
          break;
        case 'g':
          high = 64;
          break;
        case 'h':
          high = 80;
          break;
        case 'k':
          high = 96;
          break;
        case 'm':
          high = 112;
          break;
        case 'n':
          high = 128;
          break;
        case 'p':
          high = 144;
          break;
        case 'q':
          high = 160;
          break;
        case 'r':
          high = 176;
          break;
        case 's':
          high = 192;
          break;
        case 't':
          high = 208;
          break;
        case 'x':
          high = 224;
          break;
        case 'z':
          high = 240;
          break;
      }
      switch (second)
      {
        case 'b':
          low = 0;
          break;
        case 'c':
          low = 1;
          break;
        case 'd':
          low = 2;
          break;
        case 'f':
          low = 3;
          break;
        case 'g':
          low = 4;
          break;
        case 'h':
          low = 5;
          break;
        case 'k':
          low = 6;
          break;
        case 'm':
          low = 7;
          break;
        case 'n':
          low = 8;
          break;
        case 'p':
          low = 9;
          break;
        case 'q':
          low = 10;
          break;
        case 'r':
          low = 11;
          break;
        case 's':
          low = 12;
          break;
        case 't':
          low = 13;
          break;
        case 'x':
          low = 14;
          break;
        case 'z':
          low = 15;
          break;
      }

      if (high < 0 || low < 0) [[unlikely]]
        return -1;

      return high + low;
    };
    /*
      // old version, the switch lookup is faster...
      auto consonant_base16_decode = [](char first, char second) -> int32_t
      {
        char constexpr alphabet[16] = {'b', 'c', 'd', 'f', 'g', 'h', 'k', 'm', 'n', 'p', 'q', 'r', 's', 't', 'x', 'z'};
        int high = -1;
        int low = -1;
        for (unsigned int i = 0; i < 16; ++i)
        {
          if (high < 0 && alphabet[i] == first)
          {
            high = i;
            if (low >= 0)
              break;
          }
          if (low < 0 && alphabet[i] == second)
          {
            low = i;
            if (high >= 0)
              break;
          }
        }
        if (high < 0 || low < 0) [[unlikely]]
          return -1;
        return (high << 4) + low;
      };
    */
    int32_t index = consonant_base16_decode(url[STRLEN("https://signal.link/call/#key=")],
                                            url[STRLEN("https://signal.link/call/#key=") + 1]);
    //std::cout << (index % 12) << std::endl;
    if (index > 0) [[likely]]
      color = s_html_random_colors[index % s_html_random_colors.size()].second;
  }

  htmloutput << std::string(indent, ' ') << "<div class=\"call-link\">\n";
  htmloutput << std::string(indent, ' ') << "  <div class=\"call-link-avatar-container\" style=\"background-color: #" << color << ";\">\n";
  htmloutput << std::string(indent, ' ') << "    <div class=\"call-link-avatar\"></div>\n";
  htmloutput << std::string(indent, ' ') << "  </div>\n";
  htmloutput << std::string(indent, ' ') << "  <div class=\"call-link-info\">\n";
  htmloutput << std::string(indent, ' ') << "    <span class=\"call-link-title\">"
             << (!title.empty() ? HTMLescapeString(title) : "Signal call") << "</span>\n";
  htmloutput << std::string(indent, ' ') << "    <pre>"
             << (!description.empty() ? HTMLescapeString(description) : "Use this link to join a Signal Call") << "</pre>\n";
  htmloutput << std::string(indent, ' ') << "  </div>\n";
  htmloutput << std::string(indent, ' ') << "</div>\n";
}
