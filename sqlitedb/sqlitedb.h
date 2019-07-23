/*
    Copyright (C) 2019  Selwin van Dijk

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

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include <sqlite3.h>
#include <memory>
#include <cstring>
#include <vector>
#include <iostream>
#include <any>

class SqliteDB
{
  sqlite3 *d_db;
  bool d_ok;
 public:
  inline explicit SqliteDB(std::string const &name);
  inline SqliteDB(SqliteDB const &other) = delete;
  inline SqliteDB &operator=(SqliteDB const &other) = delete;
  inline ~SqliteDB();
  inline bool ok() const;
  //inline void exec(std::string const &q);
  inline void exec(std::string const &q, std::vector<std::vector<std::pair<std::string, std::any>>> *results = nullptr);
  void exec(std::string const &q, std::vector<std::any> const &params, std::vector<std::vector<std::pair<std::string, std::any>>> *results = nullptr);
 private:
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::string const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, long long int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, double param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::pair<std::shared_ptr<unsigned char []>, size_t> const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::nullptr_t param) const;
  template <typename T>
  inline bool isType(std::any const &a) const;
};

inline SqliteDB::SqliteDB(std::string const &name)
  :
  d_db(nullptr),
  d_ok(false)
{
  d_ok = (sqlite3_open(name.c_str(), &d_db) == 0);
}

inline SqliteDB::~SqliteDB()
{
  if (d_ok)
    sqlite3_close(d_db);
}

inline bool SqliteDB::ok() const
{
  return d_ok;
}

// inline void SqliteDB::exec(std::string const &q)
// {
//   char *errmsg;
//   if (sqlite3_exec(d_db, q.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK)
//   {
//     std::cout << "SQL Error: " << errmsg << std::endl;
//     sqlite3_free(errmsg);
//   }
// }

inline void SqliteDB::exec(std::string const &q, std::vector<std::vector<std::pair<std::string, std::any>>> *results)
{
  exec(q, std::vector<std::any>(), results);
}

template <typename T>
inline bool SqliteDB::isType(std::any const &a) const
{
  return (a.type() == typeid(T));
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, std::string const &param) const
{
  //std::cout << "Binding STRING at " << count << ": " << param.c_str() << std::endl;
  return sqlite3_bind_text(stmt, count, param.c_str(), -1, SQLITE_TRANSIENT);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, std::pair<std::shared_ptr<unsigned char []>, size_t> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first.get()), param.second, SQLITE_TRANSIENT);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, long long int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, std::nullptr_t) const
{
  //std::cout << "Binding NULL at " << count << std::endl;
  return sqlite3_bind_null(stmt, count);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, double param) const
{
  //std::cout << "Binding DOUBLE at " << count << ": " << param << std::endl;
  return sqlite3_bind_double(stmt, count, param);
}

#endif
