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

void SqliteDB::QueryResults::prettyPrint(long long int row) const
{
  if (rows() == 0 && columns() == 0)
  {
    Logger::message("(no results)");
    return;
  }

  std::vector<std::vector<std::string>> contents;

  // add headers
  contents.resize(contents.size() + 1);
  for (unsigned int i = 0; i < d_headers.size(); ++i)
    contents.back().emplace_back(d_headers[i]);

  // set data
  long long int startrow = row == -1 ? 0 : row;
  long long int endrow = row == -1 ? rows() : row + 1;
  for (unsigned int i = startrow; i < endrow; ++i)
  {
    contents.resize(contents.size() + 1);
    for (uint j = 0; j < columns(); ++j)
    {
      if (valueHasType<std::string>(i, j))
      {
        contents.back().emplace_back(getValueAs<std::string>(i, j));
        std::string::size_type newline = std::string::npos;
        if ((newline = contents.back().back().find('\n')) != std::string::npos)
        {
          contents.back().back().resize(newline);
          contents.back().back() += "[\\n...]";
        }
      }
      else if (valueHasType<int>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<int>(i, j)));
      else if (valueHasType<unsigned int>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<unsigned int>(i, j)));
      else if (valueHasType<long long int>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<long long int>(i, j)));
      else if (valueHasType<unsigned long long int>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<unsigned long long int>(i, j)));
      else if (valueHasType<unsigned long>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<unsigned long>(i, j)));
      else if (valueHasType<double>(i, j))
        contents.back().emplace_back(bepaald::toString(getValueAs<double>(i, j)));
      else if (valueHasType<std::nullptr_t>(i, j))
        contents.back().emplace_back("(NULL)");
      else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
        contents.back().emplace_back(bepaald::bytesToHexString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
                                                                getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second));
      else
        contents.back().emplace_back("(unhandled type)");
    }
  }

  // calculate widths
  std::vector<uint> widths(contents[0].size(), 0);
  for (uint col = 0; col < contents[0].size(); ++col)
    for (uint row = 0; row < contents.size(); ++row)
      if (widths[col] < charCount(contents[row][col]))
        widths[col] = charCount(contents[row][col]);

  int totalw = std::accumulate(widths.begin(), widths.end(), 0) + 3 * columns() + 1;
  //std::cout << " total width: " << totalw << std::endl;
  //std::cout << " available width: " << availableWidth() << std::endl;

  if (totalw > availableWidth())
  {
    uint fairwidthpercol = (availableWidth() - 1) / contents[0].size() - 3;
    //std::cout << "cols: " << contents[0].size() << " size per col (adjusted for table space): " << fairwidthpercol << " LEFT: " << availableWidth() - (contents[0].size() * fairwidthpercol + 3 * contents[0].size() + 1) << std::endl;
    int spaceleftbyshortcols = availableWidth() - (contents[0].size() * fairwidthpercol + 3 * contents[0].size() + 1);
    std::vector<int> oversizedcols;
    uint widestcol = 0;
    uint maxwidth = 0;
    // each column has availableWidth() / nCols (- tableedges) available by fairness.
    // add to this all the space not needed by the columns that are
    // less wide than availableWidth() / nCols anyway.
    for (uint i = 0; i < widths.size(); ++i)
    {
      if (widths[i] > maxwidth)
      {
        maxwidth = widths[i];
        widestcol = i;
      }
      if (widths[i] <= fairwidthpercol)
      {
        spaceleftbyshortcols += fairwidthpercol - widths[i];
        //std::cout << "Column " << i << " has room to spare: " << " " << widths[i] << " (" << fairwidthpercol - widths[i] << ")" << std::endl;
      }
      else
      {
        oversizedcols.push_back(i);
        //std::cout << "Column " << i << " is oversized: " << " " << widths[i] << std::endl;
      }
    }

    uint maxwidthpercol = std::max(static_cast<int>(fairwidthpercol + spaceleftbyshortcols / oversizedcols.size()), 5);
    int leftforlongcols = spaceleftbyshortcols % oversizedcols.size();
    //std::cout << L"Real max width per col: " << maxwidthpercol << L" (left " << leftforlongcols << L")" << std::endl;

    // maybe some oversized column are not oversized anymore...
    bool changed = true;
    while (changed)
    {
      changed = false;
      for (auto it = oversizedcols.begin(); it != oversizedcols.end(); ++it)
      {
        if (widths[*it] < maxwidthpercol)
        {
          //std::cout << "Column has room, needs " << widths[*it] << " available: " << maxwidthpercol << " adds " << maxwidthpercol - widths[*it] << " to pool of leftspace for " << oversizedcols.size() - 1 << " remaining oversized cols" << std::endl;
          changed = true;
          maxwidthpercol += (maxwidthpercol - widths[*it]) / (oversizedcols.size() - 1);
          oversizedcols.erase(it);
          break;
        }
        //else
        //{
        //  std::cout << "Column needs that extra space " << widths[*it] << " available: " << maxwidthpercol << std::endl;
        //}
      }
      //std::cout << "" << std::endl;
    }

    //std::cout << "REAL max width per col: " << maxwidthpercol << " (left " << leftforlongcols << ")" << std::endl;

    // update widths
    widths.clear();
    widths.resize(contents[0].size());
    for (uint col = 0; col < contents[0].size(); ++col)
      for (uint row = 0; row < contents.size(); ++row)
      {
        if (charCount(contents[row][col]) > maxwidthpercol + ((col == widestcol) ? leftforlongcols : 0))
        {
          contents[row][col].resize(maxwidthpercol + ((col == widestcol) ? leftforlongcols : 0) - 5); // this might overcrop, because the max is set to charcount
          contents[row][col] += "[...]";                                                              // while the resize is done at bytecount. We could crop at
        }                                                                                             // charcount, but some char take two columns of terminal, so
        if (widths[col] < charCount(contents[row][col]))                                              // we might then undercrop, which is worse.
          widths[col] = charCount(contents[row][col]);
      }
  }

  //std::cout << std::string(availableWidth(), '*') << std::endl;
  //bool ansi = useEscapeCodes();
  Logger::message(std::string(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, '-'));
  for (uint row = 0; row < contents.size(); ++row)
  {
    unsigned int pos = 1; // for seeking horizontal position with ANSI escape codes, this starts counting at 1
    for (uint col = 0; col < contents[row].size(); ++col)
    {
      Logger::message_start(std::left, "| ", std::setw(widths[col]), std::setfill(' '), contents[row][col], std::setw(0), " ");
      //if (ansi) // if we support control codes, make 'sure' the cursor is at the right position
      //{
        pos += 2 + widths[col] + 1; // "| " + content + " "
        Logger::message_start(Logger::ControlChar("\033[" + bepaald::toString(pos - 1) + "G ")); // prints a space right before where the next '|' will come
      //}
    }
    Logger::message("|");

    // another bar under top row
    if (row == 0)
      Logger::message(std::string(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, '-'));
  }
  Logger::message(std::string(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, '-'));

  return;
}



// void SqliteDB::QueryResults::prettyPrint() const
// {
//   if (rows() == 0 && columns() == 0)
//   {
//     std::cout << "(no results)" << std::endl;
//     return;
//   }

//   std::setlocale(LC_ALL, "en_US.utf8");
//   std::freopen(nullptr, "a", stdout);

//   std::vector<std::vector<std::wstring>> contents;

//   // add headers
//   contents.resize(contents.size() + 1);
//   for (unsigned int i = 0; i < d_headers.size(); ++i)
//     contents.back().emplace_back(wideString(d_headers[i]));

//   // set data
//   for (unsigned int i = 0; i < rows(); ++i)
//   {
//     contents.resize(contents.size() + 1);
//     for (uint j = 0; j < columns(); ++j)
//     {
//       if (valueHasType<std::string>(i, j))
//       {

//         /*
//         std::wstring tmp = wideString(getValueAs<std::string>(i, j));
//         if (tmp.length() != charCount(getValueAs<std::string>(i, j)))
//         {
//           std::wcout << L"SIZES DIFFER:" << std::endl;
//           std::wcout << tmp << std::endl;
//         }
//         */

//         contents.back().emplace_back(wideString(getValueAs<std::string>(i, j)));
//         std::string::size_type newline = std::string::npos;
//         if ((newline = contents.back().back().find('\n')) != std::string::npos)
//           contents.back().back().resize(newline);
//       }
//       else if (valueHasType<long long int>(i, j))
//       {
//         contents.back().emplace_back(bepaald::toWString(getValueAs<long long int>(i, j)));
//       }
//       else if (valueHasType<double>(i, j))
//       {
//         contents.back().emplace_back(bepaald::toWString(getValueAs<double>(i, j)));
//       }
//       else if (valueHasType<std::nullptr_t>(i, j))
//       {
//         contents.back().emplace_back(L"(NULL)");
//       }
//       else if (valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
//       {
//         contents.back().emplace_back(bepaald::bytesToHexWString(getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).first.get(),
//                                                                 getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j).second));
//       }
//       else
//       {
//         contents.back().emplace_back(L"(unhandled type)");
//       }
//     }
//   }

//   // calculate widths
//   std::vector<uint> widths(contents[0].size(), 0);
//   for (uint col = 0; col < contents[0].size(); ++col)
//     for (uint row = 0; row < contents.size(); ++row)
//       if (widths[col] < contents[row][col].length())
//         widths[col] = contents[row][col].length();

//   int totalw = std::accumulate(widths.begin(), widths.end(), 0) + 3 * columns() + 1;
//   //std::wcout << L" total width: " << totalw << std::endl;
//   //std::wcout << L" available width: " << availableWidth() << std::endl;

//   if (totalw > availableWidth())
//   {
//     uint fairwidthpercol = (availableWidth() - 1) / contents[0].size() - 3;
//     //std::wcout << L"cols: " << contents[0].size() << L" size per col (adjusted for table space): " << fairwidthpercol << L" LEFT: " << availableWidth() - (contents[0].size() * fairwidthpercol + 3 * contents[0].size() + 1) << std::endl;
//     int spaceleftbyshortcols = availableWidth() - (contents[0].size() * fairwidthpercol + 3 * contents[0].size() + 1);
//     std::vector<int> oversizedcols;
//     uint widestcol = 0;
//     uint maxwidth = 0;
//     // each column has availableWidth() / nCols (- tableedges) available by fairness.
//     // add to this all the space not needed by the columns that are
//     // less wide than availableWidth() / nCols anyway.
//     for (uint i = 0; i < widths.size(); ++i)
//     {
//       if (widths[i] > maxwidth)
//       {
//         maxwidth = widths[i];
//         widestcol = i;
//       }
//       if (widths[i] <= fairwidthpercol)
//       {
//         spaceleftbyshortcols += fairwidthpercol - widths[i];
//         //std::wcout << L"Column " << i << L" has room to spare: " << L" " << widths[i] << L" (" << fairwidthpercol - widths[i] << L")" << std::endl;
//       }
//       else
//       {
//         oversizedcols.push_back(i);
//         //std::wcout << L"Column " << i << L" is oversized: " << L" " << widths[i] << std::endl;
//       }
//     }

//     uint maxwidthpercol = std::max(static_cast<int>(fairwidthpercol + spaceleftbyshortcols / oversizedcols.size()), 5);
//     int leftforlongcols = spaceleftbyshortcols % oversizedcols.size();
//     //std::wcout << L"Real max width per col: " << maxwidthpercol << L" (left " << leftforlongcols << L")" << std::endl;

//     // maybe some oversized column are not oversized anymore...
//     bool changed = true;
//     while (changed)
//     {
//       changed = false;
//       for (auto it = oversizedcols.begin(); it != oversizedcols.end(); ++it)
//       {
//         if (widths[*it] < maxwidthpercol)
//         {
//           //std::wcout << L"Column has room, needs " << widths[*it] << L" available: " << maxwidthpercol << L" adds " << maxwidthpercol - widths[*it] << L" to pool of leftspace for " << oversizedcols.size() - 1 << L" remaining oversized cols" << std::endl;
//           changed = true;
//           maxwidthpercol += (maxwidthpercol - widths[*it]) / (oversizedcols.size() - 1);
//           oversizedcols.erase(it);
//           break;
//         }
//         //else
//         //{
//         //  std::wcout << L"Column needs that extra space " << widths[*it] << L" available: " << maxwidthpercol << std::endl;
//         //}
//       }
//       //std::wcout << L"" << std::endl;
//     }

//     //std::wcout << L"REAL max width per col: " << maxwidthpercol << L" (left " << leftforlongcols << L")" << std::endl;

//     // update widths
//     widths.clear();
//     widths.resize(contents[0].size());
//     for (uint col = 0; col < contents[0].size(); ++col)
//       for (uint row = 0; row < contents.size(); ++row)
//       {
//         if (contents[row][col].length() > maxwidthpercol + ((col == widestcol) ? leftforlongcols : 0))
//         {
//           contents[row][col].resize(maxwidthpercol + ((col == widestcol) ? leftforlongcols : 0) - 5);
//           contents[row][col] += L"[...]";
//         }
//         if (widths[col] < contents[row][col].length())
//         {
//           widths[col] = contents[row][col].length();
//         }
//       }
//   }

//   //std::wcout << std::wstring(availableWidth(), L'*') << std::endl;
//   bool ansi = useEscapeCodes();
//   std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;
//   for (uint row = 0; row < contents.size(); ++row)
//   {
//     std::wcout.setf(std::ios_base::left);
//     unsigned int pos = 1; // for seeking horizontal position with ANSI escape codes, this starts counting at 1
//     for (uint col = 0; col < contents[row].size(); ++col)
//     {
//       std::wcout << L"| " << std::setw(widths[col]) << std::setfill(L' ') << contents[row][col] << std::setw(0) << L" ";
//       if (ansi)
//       {
//         pos += 2 + widths[col] + 1; // "| " + content + " "
//         std::wcout << L"\033[" << pos - 1 << L"G "; // prints a space right before where the next '|' will come
//       }
//     }
//     std::wcout << L"|" << std::endl;

//     // another bar under top row
//     if (row == 0)
//       std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;
//   }
//   std::wcout << std::wstring(std::accumulate(widths.begin(), widths.end(), 0) + 2 * columns() + columns() + 1, L'-') << std::endl;

//   std::freopen(nullptr, "a", stdout);

//   return;
// }
