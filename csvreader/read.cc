/*
    Copyright (C) 2021  Selwin van Dijk

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
      std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off << "reading CSV file" << std::endl;
      return false;
    }
    std::cout << "Got row: " << row << std::endl;
    if ((initialstate = readRow(row, initialstate)) == CSVState::UNQUOTEDFIELD)
    {

      std::cout << "READ    :";
      for (uint i = 0; i < d_results.back().size(); ++i)
        std::cout << d_results.back()[i] << ",";
      std::cout << std::endl;

      // extra check: all rows must have same number of fields
      [[unlikely]] if (d_results.size() == 1)
        d_fields = d_results.back().size();
      else //
        [[unlikely]] if (d_results.back().size() != d_fields)
        {
          std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
                    << ": invalid csv" << std::endl;
          return false;
        }
    }
  }
  return initialstate == CSVState::UNQUOTEDFIELD;
}
