/*
    Copyright (C) 2019  Selwin van Dijk

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

long long int SignalBackup::dateToMSecsSinceEpoch(std::string const &date) const
{
  long long int ret = -1;

  // check
  std::regex datestring("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}");
  if (std::regex_match(date, datestring))
  {
    std::tm t = {};
    std::istringstream ss(date);
    if (ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S"))
      ret = std::mktime(&t) * 1000;
  }
  else
    ret = bepaald::toNumber<long long int>(date);

  return ret;
}
