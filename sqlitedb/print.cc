/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SqliteDB::QueryResults::print(long long int row, bool printheader) const
{

  if (rows() == 0 && columns() == 0)
  {
    Logger::message("(no results)");
    return;
  }

  if (printheader)
  {
    Logger::message_start();
    for (unsigned int i = 0; i < d_headers.size(); ++i)
      Logger::message_continue(d_headers[i], ((i < d_headers.size() - 1) ? "|" : ""));
    Logger::message_end();
  }

  long long int startrow = row == -1 ? 0 : row;
  long long int endrow = row == -1 ? rows() : row + 1;
  for (unsigned int i = startrow; i < endrow; ++i)
  {
    Logger::message_start();
    for (unsigned int j = 0; j < columns(); ++j)
    {
      if (valueHasType<long long int>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<long long int>(i, j)));
      else if (valueHasType<std::nullptr_t>(i, j))
        Logger::message_continue("(NULL)");
      else if (valueHasType<std::string>(i, j))
        Logger::message_continue(getValueAs<std::string>(i, j));
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
        Logger::message_continue(bepaald::bytesToHexString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                                           getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second));
      else if (valueHasType<double>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<double>(i, j)));
      else if (valueHasType<int>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<int>(i, j)));
      else if (valueHasType<unsigned int>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<unsigned int>(i, j)));
      else if (valueHasType<unsigned long long int>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<unsigned long long int>(i, j)));
      else if (valueHasType<unsigned long>(i, j))
        Logger::message_continue(bepaald::toString(getValueAs<unsigned long>(i, j)));
      else
        Logger::message_continue("(unhandled type)");

      if (j < columns() - 1)
        Logger::message_continue("|");
    }
    Logger::message_end();
  }
}
