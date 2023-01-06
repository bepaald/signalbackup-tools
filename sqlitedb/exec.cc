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

// #include "sqlitedb.ih"

// bool SqliteDB::exec(std::string const &q, std::vector<std::any> const &params, QueryResults *results) const
// {
//   sqlite3_stmt *stmt;
//   if (sqlite3_prepare_v2(d_db, q.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
//   {
//     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//       "During sqlite3_prepare_v2(): " << sqlite3_errmsg(d_db) << std::endl <<
//       "  Query: \"" << q << "\"" << std::endl;
//     return false;
//   }

//   for (int i = 0; auto const &p : params)
//   {
//     if (isType<std::nullptr_t>(p))
//     {
//       if (execParamFiller(stmt, i + 1, nullptr) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<double>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<double>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<int>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<int>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<unsigned int>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<unsigned int>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<long long int>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<long long int>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<unsigned long>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<unsigned long>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<unsigned long long int>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<unsigned long long int>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<std::string>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<std::string>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else if (isType<std::pair<unsigned char *, size_t>>(p))
//     {
//       if (execParamFiller(stmt, i + 1, std::any_cast<std::pair<unsigned char *, size_t>>(p)) != SQLITE_OK)
//       {
//         std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//           "During sqlite3_bind_*(): " << sqlite3_errmsg(d_db) << std::endl <<
//           "  Query: \"" << q << "\"" << std::endl;
//         return false;
//       }
//     }
//     else
//     {
//       std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//         "Unhandled parameter type " << p.type().name() << std::endl <<
//         "  Query: \"" << q << "\"" << std::endl;
//       return false;
//     }
//   }

//   if (results)
//     results->clear();
//   int rc;
//   int row = 0;
//   while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
//   {
//     if (!results)
//       continue;

//     // if headers aren't set, set them
//     if (results->columns() == 0)
//       for (int i = 0; i < sqlite3_column_count(stmt); ++i)
//         results->emplaceHeader(sqlite3_column_name(stmt, i));

//     // set values
//     for (int i = 0; i < sqlite3_column_count(stmt); ++i)
//     {
//       if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER)
//         results->emplaceValue(row, sqlite3_column_int64(stmt, i));
//       else if (sqlite3_column_type(stmt, i) == SQLITE_FLOAT)
//         results->emplaceValue(row, sqlite3_column_double(stmt, i));
//       else if (sqlite3_column_type(stmt, i) == SQLITE_TEXT)
//         results->emplaceValue(row, std::string(reinterpret_cast<char const *>(sqlite3_column_text(stmt, i))));
//       else if (sqlite3_column_type(stmt, i) == SQLITE_NULL)
//         results->emplaceValue(row, nullptr);
//       else if (sqlite3_column_type(stmt, i) == SQLITE_BLOB)
//       {
//         size_t blobsize = sqlite3_column_bytes(stmt, i);
//         std::shared_ptr<unsigned char []> blob(new unsigned char[blobsize]);
//         std::memcpy(blob.get(), reinterpret_cast<unsigned char const *>(sqlite3_column_blob(stmt, i)), blobsize);
//         results->emplaceValue(row, std::make_pair(blob, blobsize));
//       }
//     }
//     ++row;
//   }
//   if (rc != SQLITE_DONE)
//   {
//     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//       "After sqlite3_step(): " << sqlite3_errmsg(d_db) << std::endl <<
//       "  Query: \"" << q << "\"" << std::endl;
//     return false;
//   }

//   if (sqlite3_finalize(stmt) != SQLITE_OK)
//   {
//     std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << " : " <<
//       "During sqlite3_finalize(): " << sqlite3_errmsg(d_db) << std::endl <<
//       "  Query: \"" << q << "\"" << std::endl;
//     return false;
//   }

//   return true;
// }
