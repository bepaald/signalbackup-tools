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

#include "../scopeguard/scopeguard.h"
#include "../mimetypes/mimetypes.h"

std::string SignalBackup::getAvatarExtension(long long int recipient_id) const
{
  std::string extension;
  auto it = std::find_if(d_avatars.begin(), d_avatars.end(),
                         [recipient_id](auto const &p) { return p.first == bepaald::toString(recipient_id); });
  if (it == d_avatars.end())
    return extension;

  std::optional<std::string> mimetype = it->second->mimetype();
  if (!mimetype) // ensure that mimetype is set
  {
    it->second->attachmentData(); // counting on the side effect of setting mimetype
    ScopeGuard clear_avatar_data([&](){it->second->clearData();});
    mimetype = it->second->mimetype();
    if (!mimetype)
      mimetype = std::string();
  }
  extension = MimeTypes::getExtension(*mimetype, "bin");

  return extension;
}
