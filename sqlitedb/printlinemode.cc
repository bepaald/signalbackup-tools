/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

#include "sqlitedb.ih"

void SqliteDB::QueryResults::printLineMode(long long int row) const
{
  if (rows() == 0 && columns() == 0)
  {
    Logger::message("(no results)");
    return;
  }

  // get longest header
  unsigned int maxheader = 0;
  for (auto const &h : d_headers)
    if (h.size() > maxheader)
      maxheader = h.size();

  // print
  long long int startrow = row == -1 ? 0 : row;
  long long int endrow = row == -1 ? rows() : row + 1;
  for (unsigned int i = startrow; i < endrow; ++i)
  {
    Logger::message(" === Row ", i + 1, "/", rows(), " ===");
    for (unsigned int j = 0; j < columns(); ++j)
    {
      if (valueHasType<long long int>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<long long int>(i, j)));
      else if (valueHasType<std::nullptr_t>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", "(NULL)");
      else if (valueHasType<std::string>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", getValueAs<std::string>(i, j));
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ",
                        bepaald::bytesToHexString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                                  getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second));
      else if (valueHasType<int>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<int>(i, j)));
      else if (valueHasType<unsigned int>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<unsigned int>(i, j)));
      else if (valueHasType<unsigned long long int>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<unsigned long long int>(i, j)));
      else if (valueHasType<unsigned long>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<unsigned long>(i, j)));
      else if (valueHasType<double>(i, j))
        Logger::message(std::setfill(' '), std::setw(maxheader), std::right, d_headers[j], " : ", bepaald::toString(getValueAs<double>(i, j)));
      else
        Logger::message("(unhandled type)");
    }
  }
}
