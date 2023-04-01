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

SignalBackup::RecipientInfo const &SignalBackup::getRecipientInfoFromMap(std::map<long long int, RecipientInfo> *recipient_info,
                                                                         long long int rid) const
{
  if (bepaald::contains(recipient_info, rid))
    return recipient_info->at(rid);

  setRecipientInfo({rid}, recipient_info);
  return recipient_info->at(rid);
}
