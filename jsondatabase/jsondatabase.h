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

#ifndef JSONDATABASE_H_
#define JSONDATABASE_H_

#include "../memsqlitedb/memsqlitedb.h"

class JsonDatabase
{
  MemSqliteDB d_database;
  bool d_ok;
  bool d_verbose;
 public:
  JsonDatabase(std::string const &jsonfile, bool verbose);
  JsonDatabase(JsonDatabase const &other) = default;
  JsonDatabase(JsonDatabase &&other) = default;
  JsonDatabase &operator=(JsonDatabase const &other) = default;
  JsonDatabase &operator=(JsonDatabase &&other) = default;
  inline bool ok() const;
  inline void listChats() const;

  friend class SignalBackup;
};

inline bool JsonDatabase::ok() const
{
  return d_ok;
}

inline void JsonDatabase::listChats() const
{
  d_database.prettyPrint("SELECT * FROM chats");
}

#endif
