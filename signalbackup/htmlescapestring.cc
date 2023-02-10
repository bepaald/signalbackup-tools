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

void SignalBackup::HTMLescapeString(std::string *body, std::set<int> const *const positions_excluded_from_escape) const
{
  // escape special html chars second, so the span's added by emojifinder (next) aren't escaped
  int positions_added = 0;
  for (uint i = 0; i < body->length(); ++i)
  {
    //std::cout << "I, POSITIONS_ADDED: " << i << "," << positions_added << std::endl;
    switch (body->at(i))
    {
      case '&':
        if (!positions_excluded_from_escape ||
            !positions_excluded_from_escape->contains(i - positions_added))
        {
          body->replace(i, 1, "&amp;");
          positions_added += STRLEN("&amp;") - 1;
          i += STRLEN("&amp;") - 1;
          //changed = true;
        }
        break;
      case '<':
        if (!positions_excluded_from_escape ||
            !positions_excluded_from_escape->contains(i - positions_added))
        {
          body->replace(i, 1, "&lt;");
          positions_added += STRLEN("&lt;") - 1;
          i += STRLEN("&lt;") - 1;
          //changed = true;
        }
        break;
      case '>':
        if (!positions_excluded_from_escape ||
            !positions_excluded_from_escape->contains(i - positions_added))
        {
          body->replace(i, 1, "&gt;");
          i += STRLEN("&gt;") - 1;
          positions_added += STRLEN("&gt;") - 1;
          //changed = true;
        }
        break;
      case '"':
        if (!positions_excluded_from_escape ||
            !positions_excluded_from_escape->contains(i - positions_added))
        {
          body->replace(i, 1, "&quot;");
          i += STRLEN("&quot;") - 1;
          positions_added += STRLEN("&quot;") - 1;
          //changed = true;
        }
        break;
      case '\'':
        if (!positions_excluded_from_escape ||
            !positions_excluded_from_escape->contains(i - positions_added))
        {
          body->replace(i, 1, "&apos;");
          i += STRLEN("&apos;") - 1;
          positions_added += STRLEN("&apos;") - 1;
          //changed = true;
        }
        break;
    }
  }
}
