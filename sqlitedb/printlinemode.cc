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

#include "sqlitedb.ih"

void SqliteDB::QueryResults::printLineMode() const
{
  if (rows() == 0 && columns() == 0)
  {
    std::cout << "(no results)" << std::endl;
    return;
  }

  // get longest header
  unsigned int maxheader = 0;
  for (auto const &h : d_headers)
    if (h.size() > maxheader)
      maxheader = h.size();

  // print
  for (unsigned int i = 0; i < rows(); ++i)
  {
    std::cout << " === Row " << i + 1 << "/" << rows() << " ===" << std::endl;
    for (uint j = 0; j < columns(); ++j)
    {

      std::cout << std::setfill(' ') << std::setw(maxheader) << std::right << d_headers[j] << " : ";

      if (valueHasType<std::string>(i, j))
        std::cout << getValueAs<std::string>(i, j) << std::endl;
      else if (valueHasType<int>(i, j))
        std::cout << bepaald::toString(getValueAs<int>(i, j)) << std::endl;
      else if (valueHasType<unsigned int>(i, j))
        std::cout << bepaald::toString(getValueAs<unsigned int>(i, j)) << std::endl;
      else if (valueHasType<long long int>(i, j))
        std::cout << bepaald::toString(getValueAs<long long int>(i, j)) << std::endl;
      else if (valueHasType<unsigned long long int>(i, j))
        std::cout << bepaald::toString(getValueAs<unsigned long long int>(i, j)) << std::endl;
      else if (valueHasType<unsigned long>(i, j))
        std::cout << bepaald::toString(getValueAs<unsigned long>(i, j)) << std::endl;
      else if (valueHasType<double>(i, j))
        std::cout << bepaald::toString(getValueAs<double>(i, j)) << std::endl;
      else if (valueHasType<std::nullptr_t>(i, j))
        std::cout << "(NULL)" << std::endl;
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
        std::cout << bepaald::bytesToHexString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                               getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second)
                  << std::endl;
      else
        std::cout << "(unhandled type)" << std::endl;

    }
  }
}
