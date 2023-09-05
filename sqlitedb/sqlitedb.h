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

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include <sqlite3.h>
#include <memory>
#include <vector>
#include <iostream>
#include <any>
#if __cpp_lib_ranges >= 201911L && !defined(__clang__) // ranges does not currently seem to work with clang
#include <ranges>
#endif

#include "../common_be.h"
#include "../memfiledb/memfiledb.h"

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
    template <typename T>
    inline T getValueAs(size_t row, std::string const &header) const;
    inline std::any const &value(size_t row, size_t idx) const;
    inline std::vector<std::any> const &row(size_t row) const;
    template <typename T>
    inline bool valueHasType(size_t row, size_t idx) const;
    template <typename T>
    inline bool valueHasType(size_t row, std::string const &header) const;
    inline bool isNull(size_t row, size_t idx) const;
    inline bool isNull(size_t row, std::string const &header) const;
    template <typename T>
    inline T getValueAs(size_t row, size_t idx) const;
    inline bool empty() const;
    inline size_t rows() const;
    inline size_t columns() const;
    inline void clear();
    void printLineMode() const;
    void prettyPrint() const;
    void print(bool printheader = true) const;
    std::string valueAsString(size_t row, size_t column) const;
    std::string valueAsString(size_t row, std::string const &header) const;
    long long int valueAsInt(size_t row, size_t column) const;
    long long int valueAsInt(size_t row, std::string const &header) const;
    inline std::string operator()(size_t row, std::string const &header) const;
    inline std::string operator()(std::string const &header) const;
    template <typename T>
    inline bool contains(T const &value) const;
    bool removeColumn(uint idx);
    inline bool removeRow(uint idx);
    inline QueryResults getRow(uint idx);

   private:
    //std::wstring wideString(std::string const &narrow) const;
    inline int idxOfHeader(std::string const &header) const;
    //bool supportsAnsi() const;
    //bool isTerminal() const;
    inline bool useEscapeCodes() const;
    int availableWidth() const;
    inline uint64_t charCount(std::string const &utf8) const;
  };

 private:
  sqlite3 *d_db;
  sqlite3_vfs *d_vfs;
  bool d_ok;

 public:
  inline explicit SqliteDB(std::string const &name, bool readonly = true);
  inline explicit SqliteDB(std::pair<unsigned char *, uint64_t> *data);
  inline SqliteDB(SqliteDB const &other) = delete;
  inline SqliteDB &operator=(SqliteDB const &other) = delete;
  inline ~SqliteDB();
  inline bool ok() const;
  inline bool exec(std::string const &q, QueryResults *results = nullptr) const;
  inline bool exec(std::string const &q, std::any const &param, QueryResults *results = nullptr) const;
#if __cpp_lib_ranges >= 201911L && !defined(__clang__)
  template <typename R> requires std::ranges::input_range<R> && std::is_same<std::any, std::ranges::range_value_t<R>>::value
  inline bool exec(std::string const &q, R &&params, QueryResults *results = nullptr) const;
#endif
  inline bool exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results = nullptr) const;
  template <typename T>
  inline T getSingleResultAs(std::string const &q, T defaultval) const;
  template <typename T>
  inline T getSingleResultAs(std::string const &q, std::any const &param, T defaultval) const;
  template <typename T>
  inline T getSingleResultAs(std::string const &q, std::vector<std::any> const &params, T defaultval) const;
  inline bool print(std::string const &q) const;
  inline bool print(std::string const &q, std::any const &param) const;
  inline bool print(std::string const &q, std::vector<std::any> const &params) const;
  inline bool prettyPrint(std::string const &q) const;
  inline bool prettyPrint(std::string const &q, std::any const &param) const;
  inline bool prettyPrint(std::string const &q, std::vector<std::any> const &params) const;
  inline bool printLineMode(std::string const &q) const;
  inline bool printLineMode(std::string const &q, std::any const &param) const;
  inline bool printLineMode(std::string const &q, std::vector<std::any> const &params) const;
  static bool copyDb(SqliteDB const &source, SqliteDB const &target);
  inline int changed() const;
  inline long long int lastInsertRowid() const;
  inline bool containsTable(std::string const &tablename) const;
  inline bool tableContainsColumn(std::string const &tablename, std::string const &columnname) const;
  template <typename... columnnames>
  inline bool tableContainsColumn(std::string const &tablename, std::string const &columnname, columnnames... list) const;
  inline void freeMemory();

 private:
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::string const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, unsigned int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, long long int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, unsigned long param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, unsigned long long int param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, double param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::pair<std::shared_ptr<unsigned char []>, size_t> const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::pair<unsigned char *, size_t> const &param) const;
  inline int execParamFiller(sqlite3_stmt *stmt, int count, std::nullptr_t param) const;
  template <typename T>
  inline bool isType(std::any const &a) const;

  inline bool registerCustoms() const;
  static inline void tokencount(sqlite3_context *context, int argc, sqlite3_value **argv);
  static inline void token(sqlite3_context *context, int argc, sqlite3_value **argv);
};

inline SqliteDB::SqliteDB(std::string const &name, bool readonly)
  :
  d_db(nullptr),
  d_vfs(nullptr),
  d_ok(false)
{
  if (name != ":memory:" && readonly)
    d_ok = (sqlite3_open_v2(name.c_str(), &d_db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK);
  else
    d_ok = (sqlite3_open(name.c_str(), &d_db) == SQLITE_OK);

  if (d_ok)
    d_ok = registerCustoms();
}

inline SqliteDB::SqliteDB(std::pair<unsigned char *, uint64_t> *data)
  :
  d_db(nullptr),
  d_vfs(MemFileDB::sqlite3_memfilevfs(data)),
  d_ok(false)
{
  if (sqlite3_vfs_register(d_vfs, 0) == SQLITE_OK)
    d_ok = (sqlite3_open_v2(MemFileDB::vfsName(), &d_db, SQLITE_OPEN_READONLY, MemFileDB::vfsName()) == SQLITE_OK);

  if (d_ok)
    d_ok = registerCustoms();
}

inline SqliteDB::~SqliteDB()
{
  if (d_db)
    sqlite3_close(d_db);

  if (d_vfs)
    sqlite3_vfs_unregister(d_vfs);
}

inline bool SqliteDB::ok() const
{
  return d_ok;
}

inline bool SqliteDB::exec(std::string const &q, QueryResults *results) const
{
  return exec(q, std::vector<std::any>(), results);
}

inline bool SqliteDB::exec(std::string const &q, std::any const &param, QueryResults *results) const
{
  return exec(q, std::vector<std::any>{param}, results);
}

#if __cpp_lib_ranges >= 201911L && !defined(__clang__)
template <typename R> requires std::ranges::input_range<R> && std::is_same<std::any, std::ranges::range_value_t<R>>::value
inline bool SqliteDB::exec(std::string const &q, R &&params, QueryResults *results) const
#else
inline bool SqliteDB::exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results) const
#endif
{
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(d_db, q.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
      "During sqlite3_prepare_v2(): " << sqlite3_errmsg(d_db) << std::endl <<
      "  Query: \"" << q << "\"" << std::endl;
    return false;
  }

  if (static_cast<int>(params.size()) != sqlite3_bind_parameter_count(stmt)) [[unlikely]]
  {
    if (sqlite3_bind_parameter_count(stmt) < static_cast<int>(params.size()))
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": " << "Too few placeholders in query!";
    else if (sqlite3_bind_parameter_count(stmt) > static_cast<int>(params.size()))
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": " << "Too many placeholders in query!";
    std::cout << " \"" << q << "\" (parameters: " << params.size()
              << ", placeholders: " << sqlite3_bind_parameter_count(stmt) << ")" << std::endl;
  }

#if __cplusplus > 201703L
  for (int i = 0; auto const &p : params)
#else
  int i = 0;
  for (auto const &p : params)
#endif
  {
    if (isType<std::nullptr_t>(p))
    {
      if (execParamFiller(stmt, i + 1, nullptr) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<double>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<double>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<int>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<int>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<unsigned int>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<unsigned int>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<long long int>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<long long int>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<unsigned long>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<unsigned long>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<unsigned long long int>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<unsigned long long int>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<std::string>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<std::string>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else if (isType<std::pair<unsigned char *, size_t>>(p))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<std::pair<unsigned char *, size_t>>(p)) != SQLITE_OK)
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
          "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
          "  Query: \"" << q << "\"" << std::endl;
        return false;
      }
    }
    else
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
        "Unhandled parameter type " << p.type().name() << std::endl <<
        "  Query: \"" << q << "\"" << std::endl;
      return false;
    }
    ++i;
  }

  if (results)
    results->clear();
  int rc;
  int row = 0;
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    if (!results)
      continue;

    // if headers aren't set, set them
    if (results->columns() == 0)
      for (int c = 0; c < sqlite3_column_count(stmt); ++c)
        results->emplaceHeader(sqlite3_column_name(stmt, c));

    // set values
    for (int c = 0; c < sqlite3_column_count(stmt); ++c)
    {
      if (sqlite3_column_type(stmt, c) == SQLITE_INTEGER)
        results->emplaceValue(row, sqlite3_column_int64(stmt, c));
      else if (sqlite3_column_type(stmt, c) == SQLITE_FLOAT)
        results->emplaceValue(row, sqlite3_column_double(stmt, c));
      else if (sqlite3_column_type(stmt, c) == SQLITE_TEXT)
        results->emplaceValue(row, std::string(reinterpret_cast<char const *>(sqlite3_column_text(stmt, c))));
      else if (sqlite3_column_type(stmt, c) == SQLITE_NULL)
        results->emplaceValue(row, nullptr);
      else if (sqlite3_column_type(stmt, c) == SQLITE_BLOB)
      {
        size_t blobsize = sqlite3_column_bytes(stmt, c);
        std::shared_ptr<unsigned char []> blob(new unsigned char[blobsize]);
        std::memcpy(blob.get(), reinterpret_cast<unsigned char const *>(sqlite3_column_blob(stmt, c)), blobsize);
        results->emplaceValue(row, std::make_pair(blob, blobsize));
      }
    }
    ++row;
  }
  if (rc != SQLITE_DONE)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
      "After sqlite3_step(): " << sqlite3_errmsg(d_db) << std::endl <<
      "  Query: \"" << q << "\"" << std::endl;
    return false;
  }

  if (sqlite3_finalize(stmt) != SQLITE_OK)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
      "During sqlite3_finalize(): " << sqlite3_errmsg(d_db) << std::endl <<
      "  Query: \"" << q << "\"" << std::endl;
    return false;
  }

  return true;
}

#if __cpp_lib_ranges >= 201911L && !defined(__clang__)
inline bool SqliteDB::exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results) const
{
  return exec(q, std::views::all(params), results);
}
#endif

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string const &q, T defaultval) const
{
  return getSingleResultAs<T>(q, std::vector<std::any>(), defaultval);
}

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string const &q, std::any const &param, T defaultval) const
{
  return getSingleResultAs<T>(q, std::vector<std::any>{param}, defaultval);
}

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string const &q, std::vector<std::any> const &params, T defaultval) const
{
  QueryResults tmp;
  if (!exec(q, params, &tmp))
    return defaultval;

  if (tmp.rows() != 1 ||
      tmp.columns() != 1 ||
      !tmp.valueHasType<T>(0, 0))
  {
    //if (tmp.rows() && tmp.columns())
    //  std::cout << "Type: " << tmp.value(0, 0).type().name() << " Requested type: " << typeid(T).name() << std::endl;
    return defaultval;
  }
  return tmp.getValueAs<T>(0, 0);
}

inline bool SqliteDB::print(std::string const &q) const
{
  return print(q, std::vector<std::any>());
}

inline bool SqliteDB::print(std::string const &q, std::any const &param) const
{
  return print(q, std::vector<std::any>{param});
}

inline bool SqliteDB::print(std::string const &q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.print();
  return ret;
}

inline bool SqliteDB::prettyPrint(std::string const &q) const
{
  return prettyPrint(q, std::vector<std::any>());
}

inline bool SqliteDB::prettyPrint(std::string const &q, std::any const &param) const
{
  return prettyPrint(q, std::vector<std::any>{param});
}

inline bool SqliteDB::prettyPrint(std::string const &q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.prettyPrint();
  return ret;
}

inline bool SqliteDB::printLineMode(std::string const &q) const
{
  return printLineMode(q, std::vector<std::any>());
}

inline bool SqliteDB::printLineMode(std::string const &q, std::any const &param) const
{
  return printLineMode(q, std::vector<std::any>{param});
}

inline bool SqliteDB::printLineMode(std::string const &q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.printLineMode();
  return ret;
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

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, std::pair<unsigned char *, size_t> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first), param.second, SQLITE_TRANSIENT);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, unsigned int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, long long int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, unsigned long param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(sqlite3_stmt *stmt, int count, unsigned long long int param) const
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

inline long long int SqliteDB::lastInsertRowid() const
{
  return sqlite3_last_insert_rowid(d_db);
}

inline bool SqliteDB::containsTable(std::string const &tablename) const
{
  QueryResults tmp;
  if (exec("SELECT DISTINCT tbl_name FROM sqlite_master WHERE type = 'table' AND tbl_name = '" + tablename + "'", &tmp))
    return (tmp.rows() > 0);
  return false;
}

inline bool SqliteDB::tableContainsColumn(std::string const &tablename, std::string const &columnname) const
{
  QueryResults tmp;
  if (exec("SELECT 1 FROM PRAGMA_TABLE_XINFO('" + tablename + "') WHERE name == '" + columnname + "'", &tmp))
    return (tmp.rows() > 0);
  return false;
}

template <typename... columnnames>
inline bool SqliteDB::tableContainsColumn(std::string const &tablename, std::string const &columnname, columnnames... list) const
{
  return tableContainsColumn(tablename, columnname) && tableContainsColumn(tablename, list...);
}

inline void SqliteDB::freeMemory()
{
  sqlite3_db_release_memory(d_db);
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
  [[unlikely]] return -1;
}

inline std::any SqliteDB::QueryResults::value(size_t row, std::string const &header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
              << ": Column `" << header << "' not found in query results" << std::endl;
    return std::any{nullptr};
  }
  return d_values[row][i];
}

template <typename T>
inline T SqliteDB::QueryResults::getValueAs(size_t row, std::string const &header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
              << ": Column `" << header << "' not found in query results" << std::endl;
    return T{};
  }

  if (d_values[row][i].type() != typeid(T)) [[unlikely]]
  {
    std::cout << "Getting value of field '" << header << "' (idx " << i << "). Value as string: " << valueAsString(row, i) << std::endl;
    std::cout << "Type: " << d_values[row][i].type().name() << " Requested type: " << typeid(T).name() << std::endl;
    //return T{};
  }
  return std::any_cast<T>(d_values[row][i]);
}

template <typename T>
inline bool SqliteDB::QueryResults::valueHasType(size_t row, std::string const &header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
              << ": Column `" << header << "' not found in query results" << std::endl;
    return false;
  }
  return (d_values[row][i].type() == typeid(T));
}

template <typename T>
inline bool SqliteDB::QueryResults::valueHasType(size_t row, size_t idx) const
{
  return (d_values[row][idx].type() == typeid(T));
}

inline bool SqliteDB::QueryResults::isNull(size_t row, size_t idx) const
{
  return valueHasType<std::nullptr_t>(row, idx);
}

inline bool SqliteDB::QueryResults::isNull(size_t row, std::string const &header) const
{
  return valueHasType<std::nullptr_t>(row, header);
}

template <typename T>
inline T SqliteDB::QueryResults::getValueAs(size_t row, size_t idx) const
{
  return std::any_cast<T>(d_values[row][idx]);
}

inline bool SqliteDB::QueryResults::empty() const
{
  return d_values.empty();
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

inline std::string SqliteDB::QueryResults::operator()(size_t row, std::string const &header) const
{
  return valueAsString(row, header);
}

inline std::string SqliteDB::QueryResults::operator()(std::string const &header) const
{
  return valueAsString(0, header);
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

bool SqliteDB::QueryResults::useEscapeCodes() const
{
  return bepaald::supportsAnsi() && bepaald::isTerminal();
}

/*
  If you know that the data is UTF-8, then you just have to check the high bit:

  0xxxxxxx = single-byte ASCII character
  1xxxxxxx = part of multi-byte character

  Or, if you need to distinguish lead/trail bytes:

  10xxxxxx = 2nd, 3rd, or 4th byte of multi-byte character
  110xxxxx = 1st byte of 2-byte character
  1110xxxx = 1st byte of 3-byte character
  11110xxx = 1st byte of 4-byte character
*/
inline uint64_t SqliteDB::QueryResults::charCount(std::string const &utf8) const
{
  uint64_t ret = utf8.length();
  for (uint i = 0; i < utf8.size(); ++i)
    if ((utf8[i] & 0b11111000) == 0b11110000)
      ret -= 3;
    else if ((utf8[i] & 0b11110000) == 0b11100000)
      ret -= 2;
    else if ((utf8[i] & 0b11100000) == 0b11000000)
      --ret;
  return ret;
}

inline bool SqliteDB::QueryResults::removeRow(uint idx)
{
  if (idx >= d_values.size())
    return false;

  d_values.erase(d_values.begin() + idx);
  return true;
}

inline SqliteDB::QueryResults SqliteDB::QueryResults::getRow(uint idx)
{
  QueryResults tmp;
  tmp.d_headers = d_headers;
  tmp.d_values.push_back(d_values[idx]);
  return tmp;
}

inline bool SqliteDB::registerCustoms() const
{
  return sqlite3_create_function(d_db, "TOKENCOUNT", -1, SQLITE_UTF8, nullptr, &tokencount, nullptr, nullptr) == SQLITE_OK &&
    sqlite3_create_function(d_db, "TOKEN", -1, SQLITE_UTF8, nullptr, &token, nullptr, nullptr) == SQLITE_OK;
}

inline void SqliteDB::tokencount(sqlite3_context *context, int argc, sqlite3_value **argv) //static
{
  if (argc == 0)
  {
    sqlite3_result_int(context, 0);
    return;
  }

  if (sqlite3_value_type(argv[0]) != SQLITE_TEXT)
  {
    sqlite3_result_int(context, 0);
    return;
  }

  unsigned char const *text = sqlite3_value_text(argv[0]);
  if (!text || !text[0])
  {
    sqlite3_result_int(context, 0);
    return;
  }

  char delim = ' ';
  if (argc > 1 && argv[1] && sqlite3_value_text(argv[1])[0])
    delim = (sqlite3_value_text(argv[1]))[0];

  int startpos = 0;
  int count = 1;

  while (text[startpos])
  {
    if (text[startpos] == delim)
    {
      ++count;
      while (text[startpos] == delim)
        ++startpos;
    }
    else
      ++startpos;
  }
  sqlite3_result_int(context, count);
}

inline void SqliteDB::token(sqlite3_context *context, int argc, sqlite3_value **argv) // static
{
  if (argc > 1)
  {
    if (sqlite3_value_type(argv[0]) != SQLITE_TEXT)
    {
      sqlite3_result_null(context);
      return;
    }

    // get params
    unsigned char const *text = sqlite3_value_text(argv[0]);
    if (!text)
      return;
    unsigned int idx = sqlite3_value_int(argv[1]);
    char delim = ' ';
    if (argc > 2 && argv[2] && sqlite3_value_text(argv[2])[0])
      delim = (sqlite3_value_text(argv[2]))[0];

    //std::cout << "DELIM: '" << delim << "' (" << (static_cast<int>(delim) & 0xff) << ")" << std::endl;

    // find the requested token
    unsigned int startpos = 0;
    unsigned int endpos = 0;
    unsigned int count = 0;
    while (text[startpos])
    {
      if (count == idx) // look for end delim
      {
        endpos = startpos;
        break;
      }
      if (text[startpos] == delim)
      {
        ++count;
        while (text[startpos] == delim)
          ++startpos;
      }
      else
        ++startpos;
    }

    while (text[endpos] && text[endpos] != delim)
      ++endpos;

    //std::cout << " TEXT: " << text << std::endl;
    //std::cout << "RANGE: " << std::string(startpos, ' ') << "^" << std::string(endpos < startpos ? 0 : endpos - startpos, ' ') << "^" <<std::endl;

    if (endpos > startpos)
    {
      int len = endpos - startpos;
      char *result = new char[len];
      std::memcpy(result, text + startpos, len);
      sqlite3_result_text(context, result, len, SQLITE_TRANSIENT);
      bepaald::destroyPtr(&result, &len);
      return;
    }

  }
  sqlite3_result_null(context);
}
#endif
