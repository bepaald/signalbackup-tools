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

#include "signalbackup.ih"

std::string SignalBackup::tgBuildBody(std::string const &bodyjson) const
{
  std::string body;
  long long int fragments = d_database.getSingleResultAs<long long int>("SELECT json_array_length(?, '$')", bodyjson, -1);
  if (fragments == -1)
  {
    Logger::error("Failed to get number of text fragments from message body. Body data: '" + bodyjson + "'");
    return body;
  }

  for (uint i = 0; i < fragments; ++i)
    body += d_database.getSingleResultAs<std::string>("SELECT json_extract(?, '$[" + bepaald::toString(i) + "].text')", bodyjson, std::string());

  return body;
}
