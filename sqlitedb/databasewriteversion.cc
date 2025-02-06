/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

void SqliteDB::setDatabaseWriteVersion()
{
  if (d_data && d_data->second >= 99)
  {
    d_databasewriteversion =
      (d_data->first[96] << 24) |
      (d_data->first[97] << 16) |
      (d_data->first[98] << 8) |
      (d_data->first[99]);
    return;
  }

  if (!d_name.empty() && d_name != ":memory:")
  {
    std::ifstream databasefile(d_name, std::ios_base::in | std::ios_base::binary);
    if (databasefile.is_open() &&
        databasefile.seekg(96))
    {
      databasefile.read(reinterpret_cast<char *>(&d_databasewriteversion), 4);
      d_databasewriteversion = bepaald::swap_endian(d_databasewriteversion);
      return;
    }
  }
}

void SqliteDB::checkDatabaseWriteVersion() const
{
  if (d_databasewriteversion > SQLITE_VERSION_NUMBER)
  {
    Logger::warning("Database was created with a newer version of SQLite3 than this program is using. If you");
    Logger::warning_indent("see a 'malformed database schema' error, please update your SQLite3 version.");
    Logger::warning_indent("Database was written by version: ", d_databasewriteversion);
    Logger::warning_indent("This program is using version:   ", SQLITE_VERSION_NUMBER);
  }
}
