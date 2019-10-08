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

void SqliteDB::QueryResults::prettyPrint() const
{
  if (rows() == 0 && columns() == 0)
  {
    std::cout << "(no results)" << std::endl;
    return;
  }

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
