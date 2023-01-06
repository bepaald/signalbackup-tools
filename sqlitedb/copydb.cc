/*
  Copyright (C) 2019-2023  Selwin van Dijk

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
    std::cout << "SQL Error: " << sqlite3_errmsg(target.d_db) << std::endl;
    return false;
  }
  int rc = 0;
  if ((rc = sqlite3_backup_step(backup, -1)) != SQLITE_DONE)
    std::cout << "SQL Error: " << sqlite3_errstr(rc) << std::endl;
  if (sqlite3_backup_finish(backup) != SQLITE_OK)
  {
    std::cout << "SQL Error: Error finishing backup" << std::endl;
    return false;
  }
  return true;
}
