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

#include "sqlitedb.ih"

void SqliteDB::exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results) const
{
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(d_db, q.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    return;
  }

  for (uint i = 0; i < params.size(); ++i)
  {
    if (isType<std::nullptr_t>(params[i]))
    {
      if (execParamFiller(stmt, i + 1, nullptr) != SQLITE_OK)
        std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    }
    else if (isType<double>(params[i]))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<double>(params[i])) != SQLITE_OK)
        std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    }
    else if (isType<long long int>(params[i]))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<long long int>(params[i])) != SQLITE_OK)
        std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    }
    else if (isType<std::string>(params[i]))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<std::string>(params[i])) != SQLITE_OK)
        std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    }
    else if (isType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(params[i]))
    {
      if (execParamFiller(stmt, i + 1, std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(params[i])) != SQLITE_OK)
        std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
    }
    else
      std::cout << "Error : Unhandled parameter type " << params[i].type().name() << std::endl;
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
      for (int i = 0; i < sqlite3_column_count(stmt); ++i)
        results->emplaceHeader(sqlite3_column_name(stmt, i));

    // set values
    for (int i = 0; i < sqlite3_column_count(stmt); ++i)
    {
      if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER)
        results->emplaceValue(row, sqlite3_column_int64(stmt, i));
      else if (sqlite3_column_type(stmt, i) == SQLITE_FLOAT)
        results->emplaceValue(row, sqlite3_column_double(stmt, i));
      else if (sqlite3_column_type(stmt, i) == SQLITE_TEXT)
        results->emplaceValue(row, std::string(reinterpret_cast<char const *>(sqlite3_column_text(stmt, i))));
      else if (sqlite3_column_type(stmt, i) == SQLITE_NULL)
        results->emplaceValue(row, nullptr);
      else if (sqlite3_column_type(stmt, i) == SQLITE_BLOB)
      {
        size_t blobsize = sqlite3_column_bytes(stmt, i);
        std::shared_ptr<unsigned char []> blob(new unsigned char[blobsize]);
        std::memcpy(blob.get(), reinterpret_cast<unsigned char const *>(sqlite3_column_blob(stmt, i)), blobsize);
        results->emplaceValue(row, std::make_pair(blob, blobsize));
      }
    }
    ++row;
  }
  if (rc != SQLITE_DONE)
    std::cout << "SQL Error: " << sqlite3_errmsg(d_db) << std::endl;
  sqlite3_finalize(stmt);
}

std::wstring SqliteDB::QueryResults::wideString(std::string const &narrow) const
{
  std::setlocale(LC_ALL, "en_US.utf8");
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(narrow);
}

void SqliteDB::QueryResults::prettyPrint() const
{
  std::setlocale(LC_ALL, "en_US.utf8");
  std::freopen(nullptr, "a", stdout);

  std::vector<std::vector<std::wstring>> contents;

  // add headers
  contents.resize(contents.size() + 1);
  for (unsigned int i = 0; i < d_headers.size(); ++i)
    contents.back().emplace_back(wideString(d_headers[i]));

  for (unsigned int i = 0; i < rows(); ++i)
  {
    contents.resize(contents.size() + 1);
    for (uint j = 0; j < columns(); ++j)
    {
      if (valueHasType<std::string>(i, j))
      {
        contents.back().emplace_back(wideString(getValueAs<std::string>(i, j)));
      }
      else if (valueHasType<long long int>(i, j))
      {
        contents.back().emplace_back(bepaald::toWString(getValueAs<long long int>(i, j)));
      }
      else if (valueHasType<double>(i, j))
      {
        contents.back().emplace_back(bepaald::toWString(getValueAs<double>(i, j)));
      }
      else if (valueHasType<std::nullptr_t>(i, j))
      {
        contents.back().emplace_back(L"(NULL)");
      }
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
      {
        contents.back().emplace_back(bepaald::bytesToHexWString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                                                getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second));
      }
      else
      {
        contents.back().emplace_back(L"(unhandled type)");
      }
    }
  }

  std::vector<uint> widths(contents[0].size(), 0);
  for (uint col = 0; col < contents[0].size(); ++col)
    for (uint row = 0; row < contents.size(); ++row)
    {
      if (contents[row][col].length() > 30)
      {
        contents[row][col].resize(25);
        contents[row][col] += L"[...]";
      }
      if (widths[col] < contents[row][col].length())
        widths[col] = contents[row][col].length();
    }

  std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;

  for (uint i = 0; i < contents.size(); ++i)
  {
    std::wcout.setf(std::ios_base::left);
    for (uint j = 0; j < contents[i].size(); ++j)
      std::wcout << L"| " << std::setw(widths[j]) << std::setfill(L' ') << contents[i][j] << std::setw(0) << L" ";
    std::wcout << L"|" << std::endl;

    // another bar under top row
    if (i == 0)
      std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;
  }
  std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;

  std::freopen(nullptr, "a", stdout);

  return;
}
