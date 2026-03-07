/*
  Copyright (C) 2024-2026  Selwin van Dijk

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

#include "../common_be.h"

std::string SignalBackup::HTMLprepLinkPreviewDescription(std::string const &in) const
{
  // link preview can contain html, this is problematic for the export +
  // in the app the tags are stripped, and underscores are replaced with spaces
  // for some reason

  std::string cleaned(in);

  //std::cout << "IN : " << in << std::endl;

  std::string::size_type startpos = 0;
  while ((startpos = cleaned.find('<', startpos)) != std::string::npos)
  {
    auto endpos = cleaned.find('>', startpos);
    if (endpos == std::string::npos)
      break;

    // Check if what we have is a tag... THIS IS NOT PERFECT BUT SHOULD CATCH MOST CASES
    std::string_view tag(cleaned.data() + startpos, (endpos + 1) - startpos);
    //std::cout << "GOT TAG: " << tag << " (at " << startpos << ")" <<  std::endl;

    // after the opening '<', there can only be [a-zA-Z0-9]-_: and possibly a leading '/'. once a space is encountered there
    // could be an attribute of the form name[=value]. The name can be anything except '\t', '\n', '\f', ' ', '/', '>', '=', '"', '''.
    // the (optional) value can basically be anything I think (optionally quoted or doublequoted).
    // we will only check that the tag is valid, and then assume anything else is attribute(s)
    bool istag = false;
    for (unsigned int i = tag[1] == '/' ? 2 : 1; i < tag.size() - 1; ++i) // start after '<' or '</', end before '>'
    {
      if (tag[i] == ' ')
        break;
      if (!((tag[i] >= 'a' && tag[i] <= 'z') ||
            (tag[i] >= 'A' && tag[i] <= 'Z') ||
            (tag[i] >= '0' && tag[i] <= '9')))
      {
        istag = false;
        break;
      }
      else
        istag = true; // is _possible_ tag (we have some valid characters after '<(/)'
    }
    //std::cout << "IS TAG: " << istag << std::endl;

    if (istag)
      cleaned.erase(startpos, (endpos + 1) - startpos);
    else
      ++startpos;
  }

  bepaald::replaceAll(&cleaned, "_", " ");

  HTMLescapeString(&cleaned);

  return cleaned;
}
