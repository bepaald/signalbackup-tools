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

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include <sqlite3.h>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <any>
#include <iterator>
#include <limits>
#include <list>
#if __cpp_lib_ranges >= 201911L
#include <ranges>
#endif

#include "../common_be.h"
#include "../memfiledb/memfiledb.h"
#include "../logger/logger.h"

class SqliteDB
{
 public:
  class QueryResults
  {
    std::vector<std::string> d_headers;
    std::vector<std::vector<std::any>> d_values;
    int d_columncount{0};

   public:
    inline void reserveColumnCount(int cnt);
    inline void emplaceHeader(std::string &&h);
    inline std::vector<std::string> const &headers() const;
    inline std::string const &header(size_t idx) const;
    inline bool hasColumn(std::string const &h) const;
    inline void emplaceValue(size_t row, std::any &&a);
    inline std::any value(size_t row, std::string_view header) const;
    template <typename T>
    inline T getValueAs(size_t row, std::string_view header) const;
    template <typename T>
    inline T getValueAs(size_t row, size_t idx) const;
    inline std::any const &value(size_t row, size_t idx) const;
    inline std::vector<std::any> const &row(size_t row) const;
    template <typename T>
    inline bool valueHasType(size_t row, size_t idx) const;
    template <typename T>
    inline bool valueHasType(size_t row, std::string_view header) const;
    inline bool isNull(size_t row, size_t idx) const;
    inline bool isNull(size_t row, std::string_view header) const;
    inline bool empty() const;
    inline size_t rows() const;
    inline size_t columns() const;
    inline void clear();
    void printSingleLine(long long int row = -1) const;
    void printLineMode(long long int row = -1) const;
    void prettyPrint(bool truncate, long long int row = -1) const;
    void print(long long int row = -1, bool printheader = true) const;
    std::string valueAsString(size_t row, size_t column) const;
    std::string valueAsString(size_t row, std::string_view header) const;
    long long int valueAsInt(size_t row, size_t column, long long int def = -1) const;
    long long int valueAsInt(size_t row, std::string_view header, long long int def = -1) const;
    inline std::string operator()(size_t row, std::string_view header) const;
    inline std::string operator()(std::string_view header) const;
    template <typename T>
    inline bool contains(T const &value) const;
    bool removeColumn(unsigned int idx);
    bool renameColumn(unsigned int idx, std::string const &name);
    inline bool removeRow(unsigned int idx);
    inline QueryResults getRow(unsigned int idx);

   private:
    inline int idxOfHeader(std::string_view header) const;
    int availableWidth() const;
    inline uint64_t charCount(std::string const &utf8) const;
  };

 private:
  mutable std::map<std::string, bool> d_tables; // cache results of containsTable/tableContainsColumn
  mutable std::map<std::string, std::map<std::string, bool>> d_columns;
  std::string d_name;
  sqlite3 *d_db;
  sqlite3_vfs *d_vfs;
  mutable std::list<sqlite3_stmt *> d_stmt_cache; // cache (prepared) statements for reuse in subsequent transactions
  unsigned int d_cache_size;
  sqlite3_stmt *d_stmt_pragma_schema_version;
  mutable char const *d_error_tail;
  std::pair<unsigned char *, uint64_t> *d_data;  // non-owning pointer!
  uint32_t d_databasewriteversion;
  bool d_readonly;
  bool d_ok;
  mutable int32_t d_schema_version;

 protected:
  inline explicit SqliteDB();
  inline explicit SqliteDB(std::string const &name, bool readonly = true);
  inline explicit SqliteDB(std::pair<unsigned char *, uint64_t> *data);
  inline SqliteDB(SqliteDB const &other);
  inline SqliteDB &operator=(SqliteDB const &other);
  inline ~SqliteDB();
 public:
  inline bool ok() const;
  inline bool saveToFile(std::string const &filename) const;
  inline bool exec(std::string_view q, QueryResults *results = nullptr, bool verbose = false) const;
  inline bool exec(std::string_view q, std::any const &param, QueryResults *results = nullptr, bool verbose = false) const;
#if __cpp_lib_ranges >= 201911L
  template <typename R> requires std::ranges::input_range<R> && std::is_same<std::any, std::ranges::range_value_t<R>>::value
  inline bool exec(std::string_view q, R &&params, QueryResults *results = nullptr, bool verbose = false) const;
#endif
  inline bool exec(std::string_view q, std::vector<std::any> const &params, QueryResults *results = nullptr, bool verbose = false) const;
  template <typename T>
  inline T getSingleResultAs(std::string_view q, T const &defaultval) const;
  template <typename T>
  inline T getSingleResultAs(std::string_view q, std::any const &param, T const &defaultval) const;
  template <typename T>
  inline T getSingleResultAs(std::string_view q, std::vector<std::any> const &params, T const &defaultval) const;
  inline bool print(std::string_view q) const;
  inline bool print(std::string_view q, std::any const &param) const;
  inline bool print(std::string_view q, std::vector<std::any> const &params) const;
  inline bool prettyPrint(bool truncate, std::string_view q) const;
  inline bool prettyPrint(bool truncate, std::string_view q, std::any const &param) const;
  inline bool prettyPrint(bool truncate, std::string_view q, std::vector<std::any> const &params) const;
  inline bool printLineMode(std::string_view q) const;
  inline bool printLineMode(std::string_view q, std::any const &param) const;
  inline bool printLineMode(std::string_view q, std::vector<std::any> const &params) const;
  inline bool printSingleLine(std::string_view q) const;
  inline bool printSingleLine(std::string_view q, std::any const &param) const;
  inline bool printSingleLine(std::string_view q, std::vector<std::any> const &params) const;
  static bool copyDb(SqliteDB const &source, SqliteDB const &target);
  inline int changed() const;
  inline long long int lastId() const;
  inline bool containsTable(std::string const &tablename) const;
  inline bool tableContainsColumn(std::string const &tablename, std::string const &columnname) const;
  template <typename... columnnames>
  inline bool tableContainsColumn(std::string const &tablename, std::string const &columnname, columnnames const &... list) const;
  inline void clearTableCache() const;
  inline void freeMemory();
  void checkDatabaseWriteVersion() const;
  inline bool getStatement(std::string_view q, sqlite3_stmt **statement) const;
  inline void setCacheSize(unsigned int size = 1);

 private:
  inline bool initFromFile();
  inline bool initFromMemory();
  inline void destroy();
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::string const &param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::string_view param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, char const *param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, unsigned char const *param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, int param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, unsigned int param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, long param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, unsigned long param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, long long int param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, unsigned long long int param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, double param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::pair<std::shared_ptr<unsigned char []>, unsigned long> const &param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::pair<std::shared_ptr<unsigned char []>, unsigned long long> const &param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::pair<unsigned char *, unsigned long> const &param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::pair<unsigned char *, unsigned long long> const &param) const;
  inline int execParamFiller(int count, sqlite3_stmt *stmt, std::nullptr_t param) const;
  template <typename T>
  inline bool isType(std::any const &a) const;
  inline bool prepareSchemaVersionStatement();
  inline bool schemaVersionChanged() const;
  void setDatabaseWriteVersion();
  //static inline int authorizer(void *userdata, int actioncode, char const *, char const *, char const *, char const *);

  inline bool registerCustoms() const;
  static inline void tokencount(sqlite3_context *context, int argc, sqlite3_value **argv);
  static inline void token(sqlite3_context *context, int argc, sqlite3_value **argv);
  static inline void jsonlong(sqlite3_context *context, int argc, sqlite3_value **argv);
};

inline SqliteDB::SqliteDB()
  :
  SqliteDB(":memory:")
{}

inline SqliteDB::SqliteDB(std::string const &name, bool readonly)
  :
  d_name(name),
  d_db(nullptr),
  d_vfs(nullptr),
  d_cache_size(1),
  d_stmt_pragma_schema_version(nullptr),
  d_error_tail(nullptr),
  d_data(nullptr),
  d_databasewriteversion(0),
  d_readonly(readonly),
  d_ok(false),
  d_schema_version(std::numeric_limits<int32_t>::min())
{
  d_ok = initFromFile();
}

inline SqliteDB::SqliteDB(std::pair<unsigned char *, uint64_t> *data)
  :
  d_db(nullptr),
  d_vfs(MemFileDB::sqlite3_memfilevfs(data)),
  d_cache_size(1),
  d_stmt_pragma_schema_version(nullptr),
  d_error_tail(nullptr),
  d_data(data),
  d_databasewriteversion(0),
  d_readonly(true),
  d_ok(false),
  d_schema_version(std::numeric_limits<int32_t>::min())
{
  d_ok = initFromMemory();
}

inline SqliteDB::SqliteDB(SqliteDB const &other)
  :
  SqliteDB(":memory:")
{
  if (d_ok)
    d_ok = copyDb(other, *this);
  d_ok &= prepareSchemaVersionStatement();
}

inline SqliteDB &SqliteDB::operator=(SqliteDB const &other)
{
  if (this != &other)
  {
    // destroy this if its already an existing thing
    destroy();

    // create
    d_db = nullptr;
    d_vfs = nullptr;
    d_stmt_cache.clear();
    d_cache_size = other.d_cache_size;
    d_stmt_pragma_schema_version = nullptr;
    d_error_tail = nullptr;
    d_name = ":memory:";
    d_data = nullptr;
    d_databasewriteversion = other.d_databasewriteversion;
    d_readonly = other.d_readonly;
    d_ok = initFromFile();
    d_schema_version = other.d_schema_version;
    if (d_ok)
      d_ok = copyDb(other, *this);
    d_ok &= prepareSchemaVersionStatement();
  }
  return *this;
}

inline SqliteDB::~SqliteDB()
{
  destroy();
}

inline bool SqliteDB::initFromFile()
{
  bool initok = false;
  if (d_name != ":memory:" && d_readonly)
    initok = (sqlite3_open_v2(d_name.c_str(), &d_db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK);
  else
    initok = (sqlite3_open(d_name.c_str(), &d_db) == SQLITE_OK);

  if (!initok)
    return false;

  setDatabaseWriteVersion();

  if (!prepareSchemaVersionStatement())
    return false;

  //sqlite3_set_authorizer(d_db, authorizer, &d_schema_changed);

  return registerCustoms();
}

inline bool SqliteDB::initFromMemory()
{
  bool initok = false;
  if (sqlite3_vfs_register(d_vfs, 0) == SQLITE_OK)
    initok = (sqlite3_open_v2(MemFileDB::vfsName(), &d_db, SQLITE_OPEN_READONLY, MemFileDB::vfsName()) == SQLITE_OK);

  if (!initok)
    return false;

  setDatabaseWriteVersion();

  if (!prepareSchemaVersionStatement())
    return false;

  //sqlite3_set_authorizer(d_db, authorizer, &d_schema_changed);

  return registerCustoms();
}

inline void SqliteDB::destroy()
{
  //if (d_stmt)
  //  sqlite3_finalize(d_stmt);
  for (auto &stmt : d_stmt_cache)
    if (stmt)
      sqlite3_finalize(stmt);


  if (d_stmt_pragma_schema_version)
    sqlite3_finalize(d_stmt_pragma_schema_version);

  if (d_vfs)
    sqlite3_vfs_unregister(d_vfs);

  if (d_db)
    sqlite3_close(d_db);
}

inline bool SqliteDB::ok() const
{
  return d_ok;
}

inline bool SqliteDB::saveToFile(std::string const &filename) const
{
  SqliteDB database(filename, false /*readonly*/);
  if (!SqliteDB::copyDb(*this, database))
  {
    Logger::error("Failed to export sqlite database");
    return false;
  }
  Logger::message("Saved database to file '", filename, "'");
  return true;
}

inline bool SqliteDB::exec(std::string_view q, QueryResults *results, bool verbose) const
{
  return exec(q, std::vector<std::any>(), results, verbose);
}

inline bool SqliteDB::exec(std::string_view q, std::any const &param, QueryResults *results, bool verbose) const
{
  return exec(q, std::vector<std::any>{param}, results, verbose);
}

#if __cpp_lib_ranges >= 201911L
template <typename R> requires std::ranges::input_range<R> && std::is_same<std::any, std::ranges::range_value_t<R>>::value
inline bool SqliteDB::exec(std::string_view q, R &&params, QueryResults *results, bool verbose) const
#else
inline bool SqliteDB::exec(std::string_view q, std::vector<std::any> const &params, QueryResults *results, bool verbose) const
#endif
{
  if (verbose) [[unlikely]]
    Logger::message("Running query: \"", q, "\"");

  sqlite3_stmt *stmt;
  if (!getStatement(q, &stmt)) // get prepared statment from cache, or produce a new one (and cache it)
    return false;

  if (static_cast<int>(params.size()) != sqlite3_bind_parameter_count(stmt)) [[unlikely]]
  {
    if (sqlite3_bind_parameter_count(stmt) < static_cast<int>(params.size()))
      Logger::warning("Too few placeholders in query!");
    else if (sqlite3_bind_parameter_count(stmt) > static_cast<int>(params.size()))
      Logger::warning("Too many placeholders in query!");
    Logger::warning_indent("-> Query: \"", q, "\" (parameters: ", params.size(),
                           ", placeholders: ", sqlite3_bind_parameter_count(stmt), ")");
  }

#if __cplusplus > 201703L
  for (int i = 0; auto const &p : params)
#else
  int i = 0;
  for (auto const &p : params)
#endif
  {
    // order empirically determined
    if (isType<long long int>(p))
    {
      //std::cout << std::endl << "PARAM LONGLONGINT" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<long long int>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::nullptr_t>(p))
    {
      //std::cout << std::endl << "PARAM NULL" << std::endl;
      if (execParamFiller(i + 1, stmt, nullptr) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::string_view>(p))
    {
      //std::cout << std::endl << "PARAM SV" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::string_view>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::pair<unsigned char *, unsigned long>>(p))
    {
      //std::cout << std::endl << "PARAM PAIR UCHAR*,ULONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::pair<unsigned char *, unsigned long>>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<int>(p))
    {
      //std::cout << std::endl << "PARAM INT" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<int>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::string>(p))
    {
      //std::cout << std::endl << "PARAM S" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::string>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<unsigned long>(p))
    {
      //std::cout << std::endl << "PARAM ULONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<unsigned long>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::pair<std::shared_ptr<unsigned char []>, unsigned long>>(p))
    {
      //std::cout << std::endl << "PARAM PAIR SHAREDPTR,ULONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::pair<std::shared_ptr<unsigned char []>, unsigned long>>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::pair<std::shared_ptr<unsigned char []>, unsigned long long>>(p))
    {
      //std::cout << std::endl << "PARAM PAIR SHAREDPTR,ULONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::pair<std::shared_ptr<unsigned char []>, unsigned long long>>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<std::pair<unsigned char *, unsigned long long>>(p))
    {
      //std::cout << std::endl << "PARAM PAIR UCHAR*,ULONGLONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<std::pair<unsigned char *, unsigned long long>>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<char const *>(p))
    {
      //std::cout << std::endl << "PARAM CHAR *" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<char const *>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<double>(p))
    {
      //std::cout << std::endl << "PARAM DOUBLE" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<double>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<unsigned int>(p))
    {
      //std::cout << std::endl << "PARAM UINT" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<unsigned int>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<unsigned char const *>(p))
    {
      //std::cout << std::endl << "PARAM UCHAR *" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<unsigned char const *>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<long>(p))
    {
      //std::cout << std::endl << "PARAM LONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<long>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    else if (isType<unsigned long long int>(p))
    {
      //std::cout << std::endl << "PARAM ULONGLONG" << std::endl;
      if (execParamFiller(i + 1, stmt, std::any_cast<unsigned long long int>(p)) != SQLITE_OK) [[unlikely]]
      {
        Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
        Logger::error_indent("-> Query: \"", q, "\"");
        return false;
      }
    }
    // else if (isType<StaticTextParam>(p))
    // {
    //   //std::cout << std::endl << "PARAM STATIC TEXT" << std::endl;
    //   if (execParamFiller(i + 1, std::any_cast<StaticTextParam>(p)) != SQLITE_OK) [[unlikely]]
    //   {
    //     Logger::error("During sqlite3_bind_*(): ", sqlite3_errmsg(d_db));
    //     Logger::error_indent("-> Query: \"", q, "\"");
    //     return false;
    //   }
    // }
    else
    {
      Logger::error("Unhandled parameter type ", p.type().name());
      Logger::error_indent("-> Query: \"", q, "\"");
      return false;
    }
    ++i;
  }

  if (results)
  {
    results->clear();
    results->reserveColumnCount(sqlite3_column_count(stmt));
  }

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
      auto coltype = sqlite3_column_type(stmt, c);
      // order empirically determined
      if (coltype == SQLITE_INTEGER)
        results->emplaceValue(row, sqlite3_column_int64(stmt, c));
      else if (coltype == SQLITE_NULL)
        results->emplaceValue(row, nullptr);
      else if (coltype == SQLITE_TEXT)
        results->emplaceValue(row, std::string(reinterpret_cast<char const *>(sqlite3_column_text(stmt, c))));
      else if (coltype == SQLITE_BLOB)
      {
        size_t blobsize = sqlite3_column_bytes(stmt, c);
        std::shared_ptr<unsigned char []> blob;
        if (blobsize) [[likely]]
        {
          blob.reset(new unsigned char[blobsize]);
          std::memcpy(blob.get(), reinterpret_cast<unsigned char const *>(sqlite3_column_blob(stmt, c)), blobsize);
        }
        results->emplaceValue(row, std::make_pair(blob, blobsize));
      }
      else if (coltype == SQLITE_FLOAT)
        results->emplaceValue(row, sqlite3_column_double(stmt, c));
    }
    ++row;
  }

  if (rc != SQLITE_DONE) [[unlikely]]
  {
    Logger::error("After sqlite3_step(): ", sqlite3_errmsg(d_db));
    char *expanded_query = sqlite3_expanded_sql(stmt);
    if (expanded_query)
    {
      Logger::error_indent("-> Query: \"", expanded_query, "\"");
      sqlite3_free(expanded_query);
    }

    return false;
  }

  if (schemaVersionChanged()) [[unlikely]]
  {
    //Logger::message("QUERY CHANGED SCHEMA VERSION: ", q);
    //Logger::message("VERSION NOW AT: ", d_schema_version);
    clearTableCache();
  }

  return true;
}

#if __cpp_lib_ranges >= 201911L
inline bool SqliteDB::exec(std::string_view q, std::vector<std::any> const &params, QueryResults *results, bool verbose) const
{
  return exec(q, std::views::all(params), results, verbose);
}
#endif

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string_view q, T const &defaultval) const
{
  return getSingleResultAs<T>(q, std::vector<std::any>(), defaultval);
}

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string_view q, std::any const &param, T const &defaultval) const
{
  return getSingleResultAs<T>(q, std::vector<std::any>{param}, defaultval);
}

template <typename T>
inline T SqliteDB::getSingleResultAs(std::string_view q, std::vector<std::any> const &params, T const &defaultval) const
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

inline bool SqliteDB::print(std::string_view q) const
{
  return print(q, std::vector<std::any>());
}

inline bool SqliteDB::print(std::string_view q, std::any const &param) const
{
  return print(q, std::vector<std::any>{param});
}

inline bool SqliteDB::print(std::string_view q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.print();
  return ret;
}

inline bool SqliteDB::prettyPrint(bool truncate, std::string_view q) const
{
  return prettyPrint(truncate, q, std::vector<std::any>());
}

inline bool SqliteDB::prettyPrint(bool truncate, std::string_view q, std::any const &param) const
{
  return prettyPrint(truncate, q, std::vector<std::any>{param});
}

inline bool SqliteDB::prettyPrint(bool truncate, std::string_view q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.prettyPrint(truncate);
  return ret;
}

inline bool SqliteDB::printLineMode(std::string_view q) const
{
  return printLineMode(q, std::vector<std::any>());
}

inline bool SqliteDB::printLineMode(std::string_view q, std::any const &param) const
{
  return printLineMode(q, std::vector<std::any>{param});
}

inline bool SqliteDB::printLineMode(std::string_view q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.printLineMode();
  return ret;
}

inline bool SqliteDB::printSingleLine(std::string_view q) const
{
  return printSingleLine(q, std::vector<std::any>());
}

inline bool SqliteDB::printSingleLine(std::string_view q, std::any const &param) const
{
  return printSingleLine(q, std::vector<std::any>{param});
}

inline bool SqliteDB::printSingleLine(std::string_view q, std::vector<std::any> const &params) const
{
  QueryResults results;
  bool ret = exec(q, params, &results);
  results.printSingleLine();
  return ret;
}

template <typename T>
inline bool SqliteDB::isType(std::any const &a) const
{
  return (a.type() == typeid(T));
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::string const &param) const
{
  //std::cout << "Binding STRING at " << count << ": " << param.c_str() << std::endl;
  return sqlite3_bind_text(stmt, count, param.c_str(), -1, SQLITE_TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::string_view param) const
{
  //std::cout << "Binding STRING_VIEW at " << count << ": " << param << std::endl;
  return sqlite3_bind_text(stmt, count, param.data(), param.size(), SQLITE_TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, char const *param) const
{
  //std::cout << "Binding CHAR CONST * at " << count << ": " << param << std::endl;
  return sqlite3_bind_text(stmt, count, param, -1, SQLITE_STATIC);//TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, unsigned char const *param) const
{
  //std::cout << "Binding UNSIGNED CHAR CONST * at " << count << std::endl;
  return sqlite3_bind_text(stmt, count, reinterpret_cast<char const *>(param), -1, SQLITE_STATIC);//TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::pair<std::shared_ptr<unsigned char []>, unsigned long> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first.get()), static_cast<int>(param.second), SQLITE_STATIC);//TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::pair<std::shared_ptr<unsigned char []>, unsigned long long> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first.get()), static_cast<int>(param.second), SQLITE_STATIC);//TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::pair<unsigned char *, unsigned long> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first), static_cast<int>(param.second), SQLITE_STATIC);//TRANSIENT);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::pair<unsigned char *, unsigned long long> const &param) const
{
  //std::cout << "Binding BLOB at " << count << std::endl;
  return sqlite3_bind_blob(stmt, count, reinterpret_cast<void *>(param.first), static_cast<int>(param.second), SQLITE_STATIC);//TRANSIENT);
}

// inline int SqliteDB::execParamFiller(int count, StaticTextParam const &param) const
// {
//   //std::cout << "Binding STATIC TEXT at " << count << std::endl;
//   return sqlite3_bind_text(stmt, count, param.ptr, static_cast<int>(param.size), SQLITE_STATIC);
// }

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, unsigned int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, long param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, unsigned long param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, static_cast<sqlite_int64>(param));
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, long long int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, param);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, unsigned long long int param) const
{
  //std::cout << "Binding long long int at " << count << ": " << param << std::endl;
  return sqlite3_bind_int64(stmt, count, static_cast<sqlite_int64>(param));
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, std::nullptr_t) const
{
  //std::cout << "Binding NULL at " << count << std::endl;
  return sqlite3_bind_null(stmt, count);
}

inline int SqliteDB::execParamFiller(int count, sqlite3_stmt *stmt, double param) const
{
  //std::cout << "Binding DOUBLE at " << count << ": " << param << std::endl;
  return sqlite3_bind_double(stmt, count, param);
}

inline int SqliteDB::changed() const
{
  return sqlite3_changes(d_db);
}

inline long long int SqliteDB::lastId() const
{
  return sqlite3_last_insert_rowid(d_db);
}

inline bool SqliteDB::containsTable(std::string const &tablename) const
{
  if (auto it = d_tables.find(tablename); it != d_tables.end())
    return it->second;

  QueryResults tmp;
  if (exec("SELECT DISTINCT tbl_name FROM sqlite_master WHERE type = 'table' AND tbl_name = ?", tablename, &tmp) &&
      tmp.rows() > 0)
  {
    d_tables.emplace(tablename, true);
    return true;
  }

  d_tables.emplace(tablename, false);
  return false;
}

inline bool SqliteDB::tableContainsColumn(std::string const &tablename, std::string const &columnname) const
{
  auto it1 = d_columns.find(tablename);
  if (it1 != d_columns.end())
    if (auto it2 = it1->second.find(columnname); it2 != it1->second.end())
      return it2->second;

  QueryResults tmp;
  if (exec("SELECT 1 FROM PRAGMA_TABLE_XINFO(?) WHERE name = ?", {tablename, columnname}, &tmp) &&
      tmp.rows() > 0)
  {
    if (it1 != d_columns.end())
      it1->second.emplace(columnname, true);
    else
      d_columns[tablename].emplace(columnname, true);
    return true;
  }
  if (it1 != d_columns.end())
    it1->second.emplace(columnname, false);
  else
    d_columns[tablename].emplace(columnname, false);
  return false;
}

template <typename... columnnames>
inline bool SqliteDB::tableContainsColumn(std::string const &tablename, std::string const &columnname, columnnames const &... list) const
{
  return tableContainsColumn(tablename, columnname) && tableContainsColumn(tablename, list...);
}

inline void SqliteDB::clearTableCache() const
{
  d_tables.clear();
  d_columns.clear();
}

inline void SqliteDB::freeMemory()
{
  sqlite3_db_release_memory(d_db);
}

inline void SqliteDB::QueryResults::reserveColumnCount(int cnt)
{
  d_columncount = cnt;
  d_headers.reserve(d_columncount);
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

inline bool SqliteDB::QueryResults::hasColumn(std::string const &h) const
{
  return bepaald::contains(d_headers, h);
}

inline void SqliteDB::QueryResults::emplaceValue(size_t row, std::any &&a)
{
  if (d_values.size() < row + 1)
  {
    d_values.resize(row + 1);
    d_values[row].reserve(d_columncount);
  }

  d_values[row].emplace_back(a);
}

inline std::any const &SqliteDB::QueryResults::value(size_t row, size_t idx) const
{
  return d_values[row][idx];
}

inline int SqliteDB::QueryResults::idxOfHeader(std::string_view header) const
{
  for (int i = 0; i < static_cast<int>(d_headers.size()); ++i)
    if (d_headers[i] == header)
      return i;
  [[unlikely]] return -1;
}

inline std::any SqliteDB::QueryResults::value(size_t row, std::string_view header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    Logger::warning("Column `", header, "' not found in query results");
    return std::any{nullptr};
  }
  return d_values[row][i];
}

template <typename T>
inline T SqliteDB::QueryResults::getValueAs(size_t row, std::string_view header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    Logger::warning("Column `", header, "' not found in query results");
    return T{};
  }
  return getValueAs<T>(row, i);
}

template <typename T>
inline T SqliteDB::QueryResults::getValueAs(size_t row, size_t idx) const
{
  if (d_values[row][idx].type() != typeid(T)) [[unlikely]]
  {
    Logger::message("Getting value of field '", d_headers[idx], "' (idx ", idx, "). Value as string: ", valueAsString(row, idx));
    Logger::message("Type: ", d_values[row][idx].type().name(), " Requested type: ", typeid(T).name());
    //return T{};
  }
  return std::any_cast<T>(d_values[row][idx]);
}

template <typename T>
inline bool SqliteDB::QueryResults::valueHasType(size_t row, std::string_view header) const
{
  int i = idxOfHeader(header);
  if (i == -1) [[unlikely]]
  {
    Logger::warning("Column `", header, "' not found in query results");
    return false;
  }
  return valueHasType<T>(row, i);
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

inline bool SqliteDB::QueryResults::isNull(size_t row, std::string_view header) const
{
  return valueHasType<std::nullptr_t>(row, header);
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

inline std::string SqliteDB::QueryResults::operator()(size_t row, std::string_view header) const
{
  return valueAsString(row, header);
}

inline std::string SqliteDB::QueryResults::operator()(std::string_view header) const
{
  return valueAsString(0, header);
}

template <typename T>
inline bool SqliteDB::QueryResults::contains(T const &value) const
{
  for (unsigned int i = 0; i < d_values.size(); ++i)
    for (unsigned int j = 0; j < d_values[i].size(); ++j)
      if (d_values[i][j].type() == typeid(T))
        if (std::any_cast<T>(d_values[i][j]) == value)
          return true;
  return false;
}

inline std::vector<std::any> const &SqliteDB::QueryResults::row(size_t row) const
{
  return d_values[row];
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
  for (unsigned int i = 0; i < utf8.size(); ++i)
    if ((utf8[i] & 0b11111000) == 0b11110000) [[unlikely]]
      ret -= 3;
    else if ((utf8[i] & 0b11100000) == 0b11000000) [[unlikely]]
      --ret;
    else if ((utf8[i] & 0b11110000) == 0b11100000) [[unlikely]]
      ret -= 2;
  return ret;
}

inline bool SqliteDB::QueryResults::removeRow(unsigned int idx)
{
  if (idx >= d_values.size())
    return false;

  d_values.erase(d_values.begin() + idx);
  return true;
}

inline SqliteDB::QueryResults SqliteDB::QueryResults::getRow(unsigned int idx)
{
  QueryResults tmp;
  tmp.d_headers = d_headers;
  tmp.d_values.push_back(d_values[idx]);
  return tmp;
}

inline bool SqliteDB::prepareSchemaVersionStatement()
{
  if (!d_stmt_pragma_schema_version) [[likely]]
    return (sqlite3_prepare_v2(d_db,
                               "PRAGMA schema_version",
                               STRLEN("PRAGMA schema_version"),
                               &d_stmt_pragma_schema_version,
                               nullptr) == SQLITE_OK);
  else
    return (sqlite3_reset(d_stmt_pragma_schema_version) == SQLITE_OK);
}

inline bool SqliteDB::schemaVersionChanged() const
{
  // step, verify column count
  if (sqlite3_step(d_stmt_pragma_schema_version) != SQLITE_ROW ||
      sqlite3_column_count(d_stmt_pragma_schema_version) != 1) [[unlikely]]
  {
    Logger::error("Failed to step schema_version query or unexpected column count");
    return true;
  }

  // get current version
  int32_t schema_version = sqlite3_column_int(d_stmt_pragma_schema_version, 0);

  // reset for next call (and to unlock database (since we dont continue stepping until SQLITE_DONE))
  if (sqlite3_reset(d_stmt_pragma_schema_version) != SQLITE_OK) [[unlikely]]
  {
    Logger::error("Failed to reset schema_version statement");
    return true;
  }

  if (d_schema_version != schema_version)
  {
    d_schema_version = schema_version;
    return true;
  }
  return false;
}

inline bool SqliteDB::registerCustoms() const
{
  return sqlite3_create_function(d_db, "TOKENCOUNT", -1, SQLITE_UTF8, nullptr, &tokencount, nullptr, nullptr) == SQLITE_OK &&
    sqlite3_create_function(d_db, "TOKEN", -1, SQLITE_UTF8, nullptr, &token, nullptr, nullptr) == SQLITE_OK &&
    sqlite3_create_function(d_db, "JSONLONG", -1, SQLITE_UTF8, nullptr, &jsonlong, nullptr, nullptr) == SQLITE_OK;
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

  unsigned char delim = ' ';
  if (argc > 1 && argv[1] && sqlite3_value_text(argv[1])[0])
    delim = (sqlite3_value_text(argv[1])[0]);

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
    unsigned char delim = ' ';
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
      unsigned int len = endpos - startpos;
      char *result = new char[len];
      std::memcpy(result, text + startpos, len);
      sqlite3_result_text(context, result, static_cast<int>(len), SQLITE_TRANSIENT);
      bepaald::destroyPtr(&result, &len);
      return;
    }

  }
  sqlite3_result_null(context);
}

inline void SqliteDB::jsonlong(sqlite3_context *context, int argc, sqlite3_value **argv) // static
{
  if (argc == 1) [[likely]]
  {
    // value is already a number probably
    if (sqlite3_value_type(argv[0]) != SQLITE_TEXT) [[likely]]
    {
      sqlite3_result_value(context, argv[0]);
      return;
    }

    // we have a string, check if it's '{low: N, high: M, unsigned}'
    unsigned char const *text = sqlite3_value_text(argv[0]);
    if (!text)
      return; // not sure what we're doing here...

    SqliteDB::QueryResults res;
    SqliteDB sqldb(":memory:");
    if (sqldb.exec("SELECT "
                   "IIF(json_valid(?1), json_extract(?1, '$.low'), NULL) AS low, "
                   "IIF(json_valid(?1), json_extract(?1, '$.high'), NULL) AS high, "
                   "IIF(json_valid(?1), json_extract(?1, '$.unsigned'), NULL) AS unsigned",
                   text, &res) &&
        res.rows() == 1 &&
        (!res.isNull(0, "low") || !res.isNull(0, "high")))
    {
      long long int jl = 0;
      if (!res.isNull(0, "high"))
        jl |= (res.getValueAs<long long int>(0, "high") << 32);
      if (!res.isNull(0, "low"))
        jl |= (res.getValueAs<long long int>(0, "low") & 0xFFFFFFFF);
      sqlite3_result_int64(context, jl);
      return;
    }

    //return copy of found string, it is not a json long object...
    sqlite3_result_value(context, argv[0]);
    return;
  }
  //std::cout << "No results, returning null (" << argc << ")" << std::endl;
  sqlite3_result_null(context);
}

inline bool SqliteDB::getStatement(std::string_view q, sqlite3_stmt **statement) const
{
  auto it = std::find_if(d_stmt_cache.begin(), d_stmt_cache.end(),
                         [&](sqlite3_stmt *stmt) { return sqlite3_sql(stmt) == q; });
  if (it == d_stmt_cache.end()) // query not cached
  {
    //std::cout << std::endl << "NEW STATEMENT :'" << q << "'" << std::endl;

    // cache is full, make room for new statement
    if (d_stmt_cache.size() >= d_cache_size)
    {
      sqlite3_finalize(d_stmt_cache.back());
      d_stmt_cache.pop_back();
    }

    // create new statement and prepare it.
    sqlite3_stmt *result;
    // prepare the statement, and put it in front of list
    if (sqlite3_prepare_v2(d_db, q.data(), q.size(), &result, &d_error_tail) != SQLITE_OK) [[unlikely]]
    {
      Logger::error("During sqlite3_prepare_v2(): ", sqlite3_errmsg(d_db));
      // attempt to mark the token that sqlite choked on
      long long int error_pos = std::distance(q.data(), d_error_tail);
      long long int error_start = error_pos; // find the token where the error starts...
      while (error_start > 0 &&
             ((q[error_start - 1] >= 'a' && q[error_start - 1] <= 'z') ||
              (q[error_start - 1] >= 'A' && q[error_start - 1] <= 'Z') ||
              (q[error_start - 1] >= '0' && q[error_start - 1] <= '9')))
        --error_start;
      Logger::error_indent("-> Query: \"",
                           q.substr(0, error_start),
                           Logger::Control::BOLD,
                           q.substr(error_start, error_pos - error_start),
                           Logger::Control::NORMAL,
                           q.substr(error_pos),
                           "\"");

      return false;
    }
    // cache it
    d_stmt_cache.push_front(result);
    // set return, done!
    *statement = result;
    return true;
  }
  // else statement found in cache!
  d_stmt_cache.splice(d_stmt_cache.begin(), d_stmt_cache, it); // move it to front
  if (sqlite3_reset(*it) != SQLITE_OK) [[unlikely]] // reuse existing prepared statement
  {
    Logger::error("During sqlite3_reset(): ", sqlite3_errmsg(d_db));
    Logger::error_indent("-> Query: \"", q, "\"");
    return false;
  }
  *statement = *it;
  return true;
}

inline void SqliteDB::setCacheSize(unsigned int size)
{
  //Logger::message("Setting statement cache size to ", size);
  d_cache_size = size;
  if (d_stmt_cache.size() > d_cache_size)
  {
    unsigned int i = 0;
    for (auto it = d_stmt_cache.begin(); it != d_stmt_cache.end(); ++it, ++i)
      if (i >= d_cache_size && *it)
        sqlite3_finalize(*it);
    d_stmt_cache.resize(d_cache_size);
  }
}

// inline int SqliteDB::authorizer(void *userdata, int actioncode, char const *, char const *, char const *, char const *)
// {
//   // 9  SQLITE_DELETE
//   // 18 SQLITE_INSERT
//   // 19 SQLITE_PRAGMA
//   // 20 SQLITE_READ
//   // 21 SQLITE_SELECT
//   // 22 SQLITE_TRANSACTION
//   // 23 SQLITE_UPDATE
//   // 24 SQLITE_ATTACH
//   // 25 SQLITE_DETACH
//   // 31 SQLITE_FUNCTION
//   // 33 SQLITE_RECURSIVE
//   if (actioncode <= 8 ||
//       (actioncode >= 10 && actioncode <= 17) ||
//       (actioncode >= 26 && actioncode <= 30) ||
//       actioncode == 32 ||
//       actioncode >= 34)
//   {
//     //Logger::message("CHANGE! ", actioncode);
//     *(reinterpret_cast<bool *>(userdata)) = true;
//   }

//   return SQLITE_OK;
// }

#endif
