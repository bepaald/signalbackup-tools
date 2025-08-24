/*
  Copyright (C) 2019-2025  Selwin van Dijk

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
#include "../common_regex.h"

#include <sstream>

long long int SignalBackup::dateToMSecsSinceEpoch(std::string const &date, bool *fromdatestring) const
{
  long long int ret = -1;

  // check
  REGEX datestring("[[:space:]]*[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}[[:space:]]*");
  if (REGEX_MATCH(date, datestring))
  {
    std::tm t = {};  // sets all to 0: NO daylight savings...
    t.tm_isdst = -1; // set daylight savings time to unknown (handle automatically)
    std::istringstream ss(date);
    if (ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S"))
      ret = std::mktime(&t) * 1000;
    if (fromdatestring)
      *fromdatestring = true;
  }
  else
  {
    ret = bepaald::toNumber<long long int>(date, -1);
    if (fromdatestring)
      *fromdatestring = false;
  }

  return ret;
}
