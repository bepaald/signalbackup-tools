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

CSVReader::CSVState CSVReader::readRow(std::string const &row, CSVReader::CSVState laststate)
{
  CSVState state = laststate;
  size_t i = 0;

  if (laststate == CSVState::UNQUOTEDFIELD)
  {
    d_results.resize(d_results.size() + 1); // got next row, make room
    d_results.back().emplace_back("");
  }
  else
    i = d_results.back().size() - 1;

  for (unsigned int c = 0; c < row.size(); ++c)
  {
    switch (state)
    {
      case CSVState::UNQUOTEDFIELD:
        switch (row[c])
        {
          case ',': // end of field
            d_results.back().push_back("");
            ++i;
            break;
          case '"':
            state = CSVState::QUOTEDFIELD;
            break;
          default:
            d_results.back()[i].push_back(row[c]);
            break;
        }
        break;
      case CSVState::QUOTEDFIELD:
        switch (row[c])
        {
          case '"':
            state = (c == row.size() - 1) ? CSVState::UNQUOTEDFIELD : CSVState::QUOTEDQUOTE;
            break;
          default:
            d_results.back()[i].push_back(row[c]);
            break;
        }
        break;
      case CSVState::QUOTEDQUOTE:
        switch (row[c])
        {
          case ',': // , after closing quote
            d_results.back().push_back("");
            ++i;
            state = CSVState::UNQUOTEDFIELD;
            break;
          case '"': // "" -> "
            d_results.back()[i].push_back('"');
            state = CSVState::QUOTEDFIELD;
            break;
          default:  // end of quote
            state = CSVState::UNQUOTEDFIELD;
            break;
        }
        break;
    }
  }

  if (state != CSVState::UNQUOTEDFIELD)
    d_results.back()[i].push_back('\n');

  return state;
}
