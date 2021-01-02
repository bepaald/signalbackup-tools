/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

void SignalBackup::duplicateQuotes(std::string *s) const
{
  size_t pos = 0;
  while ((pos = s->find('"', pos)) != std::string::npos)
    s->insert((++pos)++, 1, '"'); // this is beautiful ;P
}

void SignalBackup::exportCsv(std::string const &filename, std::string const &table) const
{
  /*
  if (checkFileExists(filename))
  {
    std::cout << "File " << filename << " exists. Refusing to overwrite" << std::endl;
    return;
  }
  */

  // output header
  std::ofstream outputfile(filename, std::ios_base::binary);

  SqliteDB::QueryResults results;
  d_database.exec("SELECT * FROM " + table, &results);

  // output header
  for (uint i = 0; i < results.columns(); ++i)
    outputfile << results.header(i) << ((i == results.columns() - 1) ? '\n' : ',');

  // output data
  for (uint j = 0; j < results.rows(); ++j)
    for (uint i = 0; i < results.columns(); ++i)
    {
      std::string vas = results.valueAsString(j, i);
      duplicateQuotes(&vas);
      bool escape = (vas.find_first_of(",\"\n") != std::string::npos) || // contains newline, quote or comma
        (!vas.empty() && (std::find_if(vas.begin(), vas.end(), [](char c){ return !std::isspace(c); }) == vas.end())); // is all whitespace (and non empty)
      outputfile << (escape ? "\"" : "") << vas << (escape ? "\"" : "") << ((i == results.columns() - 1) ? '\n' : ',');
    }
}
