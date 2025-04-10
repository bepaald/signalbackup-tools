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

std::string SignalBackup::HTMLescapeString(std::string const &body) const
{
  std::string result(body);
  HTMLescapeString(&result);
  return result;
}

void SignalBackup::HTMLescapeString(std::string *body, std::set<int> const *const positions_excluded_from_escape) const
{
  // escape special html chars second, so the span's added by emojifinder (next) aren't escaped
  int positions_added = 0;
  std::string::size_type strlen = body->length();
  for (unsigned int i = 0; i < strlen; ++i)
  {
    if ((*body)[i] > '>' ||
        (*body)[i] < '"') [[likely]]
      continue;

    //std::cout << "I, POSITIONS_ADDED: " << i << "," << positions_added << std::endl;
    switch ((*body)[i])
    {
      case '\'':
        if (!positions_excluded_from_escape ||
            !bepaald::contains(positions_excluded_from_escape, (i - positions_added)))
        {
          body->replace(i, 1, "&apos;");
          strlen += STRLEN("&apos;") - 1;
          i += STRLEN("&apos;") - 1;
          positions_added += STRLEN("&apos;") - 1; // this adds the '&', so it isnt esacped again
        }
        break;
      case '&':
        if (!positions_excluded_from_escape ||
            !bepaald::contains(positions_excluded_from_escape, (i - positions_added)))
        {
          body->replace(i, 1, "&amp;");
          strlen += STRLEN("&amp;") - 1;
          i += STRLEN("&amp;") - 1;
          positions_added += STRLEN("&amp;") - 1;
        }
        break;
      case '"':
        if (!positions_excluded_from_escape ||
            !bepaald::contains(positions_excluded_from_escape, (i - positions_added)))
        {
          body->replace(i, 1, "&quot;");
          strlen += STRLEN("&quot;") - 1;
          i += STRLEN("&quot;") - 1;
          positions_added += STRLEN("&quot;") - 1;
        }
        break;
      case '>':
        if (!positions_excluded_from_escape ||
            !bepaald::contains(positions_excluded_from_escape, (i - positions_added)))
        {
          body->replace(i, 1, "&gt;");
          strlen += STRLEN("&gt;") - 1;
          i += STRLEN("&gt;") - 1;
          positions_added += STRLEN("&gt;") - 1;
        }
        break;
      case '<':
        if (!positions_excluded_from_escape ||
            !bepaald::contains(positions_excluded_from_escape, (i - positions_added)))
        {
          body->replace(i, 1, "&lt;");
          strlen += STRLEN("&lt;") - 1;
          i += STRLEN("&lt;") - 1;
          positions_added += STRLEN("&lt;") - 1;
        }
        break;
    }
  }
}
