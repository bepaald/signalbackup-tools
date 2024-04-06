/*
  Copyright (C) 2024  Selwin van Dijk

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

#ifndef MEMSQLITEDB_H_
#define MEMSQLITEDB_H_

#include "../sqlitedb/sqlitedb.h"

class MemSqliteDB : public SqliteDB
{
 public:
  inline MemSqliteDB();
  inline explicit MemSqliteDB(std::pair<unsigned char *, uint64_t> *data);
  ~MemSqliteDB() = default;
};

inline MemSqliteDB::MemSqliteDB()
  :
  SqliteDB(":memory:")
{
  exec("PRAGMA synchronous = OFF");
}

inline MemSqliteDB::MemSqliteDB(std::pair<unsigned char *, uint64_t> *data)
  :
  SqliteDB(data)
{
  exec("PRAGMA synchronous = OFF");
}


#endif
