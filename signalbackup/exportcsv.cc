/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

#include "../common_filesystem.h"

void SignalBackup::duplicateQuotes(std::string *s) const
{
  size_t pos = 0;
  while ((pos = s->find('"', pos)) != std::string::npos)
    s->insert((++pos)++, 1, '"'); // this is beautiful ;P
}

bool SignalBackup::exportCsv(std::string const &filename, std::string const &table, bool overwrite) const
{
  if (!overwrite && (bepaald::fileOrDirExists(filename) && !bepaald::isDir(filename)))
  {
    Logger::message("File ", filename, " exists, use --overwrite to overwrite");
    return false;
  }

  // output header
  std::ofstream outputfile(filename, std::ios_base::binary);
  if (!outputfile.is_open())
  {
    Logger::error("Failed to open output file '", filename, "' for writing");
    return false;
  }

  SqliteDB::QueryResults results;
  if (!d_database.exec("SELECT * FROM " + table, &results))
  {
    Logger::error("Gathering data from database");
    return false;
  }

  // output header
  for (unsigned int i = 0; i < results.columns(); ++i)
    outputfile << results.header(i) << ((i == results.columns() - 1) ? '\n' : ',');

  // output data
  for (unsigned int j = 0; j < results.rows(); ++j)
    for (unsigned int i = 0; i < results.columns(); ++i)
    {
      std::string vas = results.valueAsString(j, i);
      duplicateQuotes(&vas);
      bool escape = (vas.find_first_of(",\"\n") != std::string::npos) || // contains newline, quote or comma
        (!vas.empty() && (std::find_if(vas.begin(), vas.end(), [](char c) STATICLAMBDA { return !std::isspace(c); }) == vas.end())); // is all whitespace (and non empty)
      outputfile << (escape ? "\"" : "") << vas << (escape ? "\"" : "") << ((i == results.columns() - 1) ? '\n' : ',');
    }

  return true;
}
