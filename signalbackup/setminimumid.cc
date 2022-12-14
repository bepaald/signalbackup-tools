/*
  Copyright (C) 2019-2022  Selwin van Dijk

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

void SignalBackup::setMinimumId(std::string const &table, long long int offset, std::string const &col) const
{
  std::cout << __FUNCTION__ << " " << table << std::endl;
  if (offset == 0) // no changes requested
    return;

  // change sign on all values:
  d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " * -1");

  // change sign back && apply offset
  d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " * -1 + ?", offset);

  /*
  // OLD VERSION
  // move everything to max + offset, the subtract max again. This works, but only if the id's are handled in order.
  if (offset < 0)
  {
    d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " + (SELECT MAX(" + col + ") from " + table + ") - (SELECT MIN(" + col + ") from " + table + ") + ?", 1ll);
    d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " - (SELECT MAX(" + col + ") from " + table + ") + (SELECT MIN(" + col + ") from " + table + ") + ?", (offset - 1));
  }
  else
  {
    d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " + (SELECT MAX(" + col + ") from " + table + ") - (SELECT MIN(" + col + ") from " + table + ") + ?", offset);
    d_database.exec("UPDATE " + table + " SET " + col + " = " + col + " - (SELECT MAX(" + col + ") from " + table + ") + (SELECT MIN(" + col + ") from " + table + ")");
  }
  */
}
