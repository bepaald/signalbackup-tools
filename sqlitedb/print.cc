/*
    Copyright (C) 2019-2020  Selwin van Dijk

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


void SqliteDB::QueryResults::print() const
{

  if (rows() == 0 && columns() == 0)
  {
    std::cout << "(no results)" << std::endl;
    return;
  }


  for (unsigned int i = 0; i < d_headers.size(); ++i)
    std::cout << d_headers[i] << ((i < d_headers.size() - 1) ? "|" : "");
  std::cout << std::endl;

  for (unsigned int i = 0; i < rows(); ++i)
  {
    for (uint j = 0; j < columns(); ++j)
    {
      if (valueHasType<std::string>(i, j))
        std::cout << getValueAs<std::string>(i, j);
      else if (valueHasType<long long int>(i, j))
        std::cout << bepaald::toString(getValueAs<long long int>(i, j));
      else if (valueHasType<double>(i, j))
        std::cout << bepaald::toString(getValueAs<double>(i, j));
      else if (valueHasType<std::nullptr_t>(i, j))
        std::cout << "(NULL)";
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
        std::cout << bepaald::bytesToHexString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                               getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second);
      else
        std::cout << "(unhandled type)";

      if (j < columns() - 1)
        std::cout << "|";
    }
    std::cout << std::endl;
  }
}
