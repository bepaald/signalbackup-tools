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
 public:
  class QueryResults
  {
    std::vector<std::string> d_headers;
    std::vector<std::vector<std::any>> d_values;
   public:
    inline void emplaceHeader(std::string &&h);
    inline std::vector<std::string> const &headers() const;
    inline std::string const &header(size_t idx) const;

    inline void emplaceValue(size_t row, std::any &&a);
    inline std::any value(size_t row, std::string const &header) const;
    inline std::any const &value(size_t row, size_t idx) const;
    inline std::vector<std::any> const &row(size_t row) const;
    template <typename T>
    inline bool valueHasType(size_t row, size_t idx) const;
    template <typename T>
    inline T getValueAs(size_t row, size_t idx) const;
    inline size_t rows() const;
    inline size_t columns() const;
    inline void clear();
    void prettyPrint() const;
    template <typename T>
    inline bool contains(T const &value) const;
   private:
    std::wstring wideString(std::string const &narrow) const;
    inline int idxOfHeader(std::string const &header) const;
  };

 private:
  sqlite3 *d_db;
  bool d_ok;
 public:
  inline explicit SqliteDB(std::string const &name, bool readonly = true);
  inline SqliteDB(SqliteDB const &other) = delete;
  inline SqliteDB &operator=(SqliteDB const &other) = delete;
  inline ~SqliteDB();
  inline bool ok() const;
  inline void exec(std::string const &q, QueryResults *results = nullptr) const;
  inline void exec(std::string const &q, std::any const &param, QueryResults *results = nullptr) const;
  void exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results = nullptr) const;
  static bool copyDb(SqliteDB const &source, SqliteDB const &target);
  inline int changed() const;
 private:
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::string const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, long long int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, double param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::pair<std::shared_ptr<unsigned char []>, size_t> const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::nullptr_t param) const;
  template <typename T>
  inline bool isType(std::any const &a) const;
};

inline SqliteDB::SqliteDB(std::string const &name, bool readonly)
  :
  d_db(nullptr),
  d_ok(false)
{
  if (name != ":memory:" && readonly)
    d_ok = (sqlite3_open_v2(name.c_str(), &d_db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK);
  else
    d_ok = (sqlite3_open(name.c_str(), &d_db) == SQLITE_OK);
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

inline void SqliteDB::exec(std::string const &q, QueryResults *results) const
{
  exec(q, std::vector<std::any>(), results);
}

inline void SqliteDB::exec(std::string const &q, std::any const &param, QueryResults *results) const
{
  exec(q, std::vector<std::any>{param}, results);
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

inline int SqliteDB::changed() const
{
  return sqlite3_changes(d_db);
}

inline void SqliteDB::QueryResults::emplaceHeader(std::string &&h)
{
  d_headers.emplace_back(h);
}

inline std::string const &SqliteDB::QueryResults::header(size_t idx) const
{
  return d_headers[idx];
}

inline std::vector<std::string> const &SqliteDB::QueryResults::headers() const
{
  return d_headers;
}

inline void SqliteDB::QueryResults::emplaceValue(size_t row, std::any &&a)
{
  if (d_values.size() < row + 1)
    d_values.resize(row + 1);

  d_values[row].emplace_back(a);
}

inline std::any const &SqliteDB::QueryResults::value(size_t row, size_t idx) const
{
  return d_values[row][idx];
}

inline int SqliteDB::QueryResults::idxOfHeader(std::string const &header) const
{
  for (uint i = 0; i < d_headers.size(); ++i)
    if (d_headers[i] == header)
      return i;
  return -1;
}

inline std::any SqliteDB::QueryResults::value(size_t row, std::string const &header) const
{
  int i = idxOfHeader(header);
  if (i > -1)
    return d_values[row][i];
  return std::any{nullptr};
}

template <typename T>
inline bool SqliteDB::QueryResults::valueHasType(size_t row, size_t idx) const
{
  return (d_values[row][idx].type() == typeid(T));
}

template <typename T>
inline T SqliteDB::QueryResults::getValueAs(size_t row, size_t idx) const
{
  return std::any_cast<T>(d_values[row][idx]);
}

inline size_t SqliteDB::QueryResults::rows() const
{
  return d_values.size();
}

inline size_t SqliteDB::QueryResults::columns() const
{
  return d_headers.size();
}

inline void SqliteDB::QueryResults::clear()
{
  d_headers.clear();
  d_values.clear();
}

template <typename T>
inline bool SqliteDB::QueryResults::contains(T const &value) const
{
  for (uint i = 0; i < d_values.size(); ++i)
    for (uint j = 0; j < d_values[i].size(); ++j)
      if (d_values[i][j].type() == typeid(T))
        if (std::any_cast<T>(d_values[i][j]) == value)
          return true;
  return false;
}

inline std::vector<std::any> const &SqliteDB::QueryResults::row(size_t row) const
{
  return d_values[row];
}

#endif
