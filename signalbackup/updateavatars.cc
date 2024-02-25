/*
  Copyright (C) 2024  Selwin van Dijk

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

void SignalBackup::updateAvatars(long long int id1, long long int id2) // if id2 == -1, id1 is an offset
{                                                                      // else, change id1 into id2

  int changedcount = 0;
  std::string id1str = bepaald::toString(id1);

  for (uint i = 0; i < d_avatars.size(); ++i)
  {
    int oldrid = bepaald::toNumber<int>(d_avatars[i].first);

    if (oldrid == id1 || id2 == -1)
    {
      d_avatars[i].first = (id2 == -1 ? bepaald::toString(oldrid + id1) : bepaald::toString(id2));
      d_avatars[i].second->setRecipient(bepaald::toString(id2 == -1 ? oldrid + id1 : id2));
      ++changedcount;
    }
  }

  if (d_verbose) [[unlikely]]
    Logger::message("     Updated ", changedcount, " avatar recipients.");

}
