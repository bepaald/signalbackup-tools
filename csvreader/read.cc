/*
  Copyright (C) 2021-2024  Selwin van Dijk

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

#include "csvreader.ih"

bool CSVReader::read()
{
  CSVState initialstate = CSVState::UNQUOTEDFIELD;
  std::string row;
  while (!d_csvfile.eof())
  {
    std::getline(d_csvfile, row);
    if (row.empty()) // skip empty rows
      continue;
    if (d_csvfile.eof())
      return true;
    if (d_csvfile.bad() || d_csvfile.fail())
    {
      Logger::error("Reading CSV file");
      return false;
    }
    Logger::message("Got row: ", row);
    if ((initialstate = readRow(row, initialstate)) == CSVState::UNQUOTEDFIELD)
    {
      Logger::message("READ    :", d_results.back());

      // extra check: all rows must have same number of fields
      if (d_results.size() == 1) [[unlikely]]
        d_fields = d_results.back().size();
      else //
        if (d_results.back().size() != d_fields) [[unlikely]]
        {
          Logger::error("invalid csv");
          return false;
        }
    }
  }
  return initialstate == CSVState::UNQUOTEDFIELD;
}
