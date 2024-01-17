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

bool SqliteDB::copyDb(SqliteDB const &source, SqliteDB const &target) // static
{
  sqlite3_backup *backup = sqlite3_backup_init(target.d_db, "main", source.d_db, "main");
  if (!backup)
  {
    Logger::error("SQL: ", sqlite3_errmsg(target.d_db));
    return false;
  }
  int rc = 0;
  if ((rc = sqlite3_backup_step(backup, -1)) != SQLITE_DONE)
  {
    Logger::error("SQL: ", sqlite3_errstr(rc));
    return false;
  }
  if (sqlite3_backup_finish(backup) != SQLITE_OK)
  {
    Logger::error("SQL: Error finishing backup");
    return false;
  }
  return true;
}
