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

void SignalBackup::setMinimumId(std::string const &table, long long int offset) const
{
  if (offset == 0) // no changes requested
    return;

  if (offset < 0)
  {
    d_database.exec("UPDATE " + table + " SET _id = _id + (SELECT MAX(_id) from " + table + ") - (SELECT MIN(_id) from " + table + ") + ?", 1ll);
    d_database.exec("UPDATE " + table + " SET _id = _id - (SELECT MAX(_id) from " + table + ") + (SELECT MIN(_id) from " + table + ") + ?", (offset - 1));
  }
  else
  {
    d_database.exec("UPDATE " + table + " SET _id = _id + (SELECT MAX(_id) from " + table + ") - (SELECT MIN(_id) from " + table + ") + ?", offset);
    d_database.exec("UPDATE " + table + " SET _id = _id - (SELECT MAX(_id) from " + table + ") + (SELECT MIN(_id) from " + table + ")");
  }
}
