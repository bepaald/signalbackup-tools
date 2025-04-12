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

#include "desktopdatabase.ih"

#include <tuple>
#include <fstream>

bool DesktopDatabase::init(std::string const &rawdb)
{
  // get directories
  if (d_configdir.empty() || d_databasedir.empty())
  {
    std::tie(d_configdir, d_databasedir) = getDesktopDir();
    if (d_configdir.empty() || d_databasedir.empty()) [[unlikely]]
    {
      Logger::warning("Failed to set default location of Signal Desktop data.");
      Logger::warning_indent("Consider using `--desktopdir <DIR>' to specify manually.");
      Logger::warning_indent("Attempting to continue, but this will likely cause errors.");
    }
  }

  // open pre-decrypted desktop database
  if (!rawdb.empty()) [[unlikely]]
  {
    std::ifstream database(rawdb, std::ios_base::in | std::ios_base::binary);
    if (!database.is_open())
    {
      Logger::error("failed to open database file '", rawdb, "'");
      return false;
    }

    uint64_t size = bepaald::fileSize(rawdb);
    d_rawdb.reset(new unsigned char[size]);

    if (!(database.read(reinterpret_cast<char *>(d_rawdb.get()), size)))
    {
      Logger::error("Failed to read database data from raw file");
      return false;
    }
    std::pair<unsigned char *, uint64_t> desktopdata = {d_rawdb.get(), size};
    d_database = MemSqliteDB(&desktopdata);

    if (!d_database.ok())
    {
      Logger::error("Failed to open database");
      return false;
    }

    d_database.checkDatabaseWriteVersion();

    return true;
  }

  // check if a wal (Write-Ahead Logging) file is present in path, and warn user to (cleanly) shut Signal Desktop down
  if (bepaald::fileOrDirExists(d_databasedir + "/sql/db.sqlite-wal"))
  {
    if (!d_ignorewal) // error
    {
      Logger::error("Found Sqlite-WAL file (write-ahead logging).");
      Logger::error_indent("Desktop data may not be fully up-to-date.");
      Logger::error_indent("Maybe Signal Desktop has not cleanly shut down?");
      Logger::error_indent("(pass `--ignorewal' to disable this warning)");
      return false;
    }
    Logger::warning("Found Sqlite-WAL file (write-ahead logging).");
    Logger::warning_indent("Desktop data may not be fully up-to-date.");
  }

  // get key
  if (d_hexkey.empty())
    if (!getKey())
    {
      Logger::error("Failed to get sqlcipher key to decrypt Signal Desktop database");
      return false;
    }

  if (d_showkey)
    Logger::message("Signal Desktop key (hex): ", d_hexkey);

  // decrypt the database
  d_cipherdb.reset(new SqlCipherDecryptor(d_databasedir + "/sql/db.sqlite", d_hexkey, d_cipherversion, d_verbose));
  if (!d_cipherdb->ok())
    return false;

  // get the decrypted data
  auto [data, size] = d_cipherdb->data(); // unsigned char *, uint64_t

  // disable WAL (Write-Ahead Logging) on database, reading from memory
  // otherwise will not work see https://www.sqlite.org/fileformat.html
  if (data[0x12] == 2)
    data[0x12] = 1;
  if (data[0x13] == 2)
    data[0x13] = 1;

  std::pair<unsigned char *, uint64_t> desktopdata = {data, size};
  d_database = MemSqliteDB(&desktopdata);
  if (!d_database.ok())
  {
    Logger::error("Failed to open database");
    return false;
  }

  d_database.checkDatabaseWriteVersion();

  return true;
}
