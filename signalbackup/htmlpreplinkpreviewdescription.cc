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

#include "../common_be.h"

std::string SignalBackup::HTMLprepLinkPreviewDescription(std::string const &in) const
{
  // link preview can contain html, this is problematic for the export +
  // in the app the tags are stripped, and underscores are replaced with spaces
  // for some reason

  std::string cleaned(in);

  while (cleaned.find('<') != std::string::npos)
  {
    auto startpos = cleaned.find('<');
    auto endpos = cleaned.find('>') + 1;

    if (endpos != std::string::npos)
      cleaned.erase(startpos, endpos - startpos);
  }

  bepaald::replaceAll(&cleaned, "_", " ");
  return cleaned;
}
