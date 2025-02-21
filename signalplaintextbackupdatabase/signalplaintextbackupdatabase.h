/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef SIGNALPLAINTEXTBACKUPDATABASE_H_
#define SIGNALPLAINTEXTBACKUPDATABASE_H_

#include "../memsqlitedb/memsqlitedb.h"
#include "../logger/logger.h"
#include "../common_be.h"

#if __cpp_lib_span >= 202002L
#include <span>
#endif

class SignalPlaintextBackupDatabase
{
  MemSqliteDB d_database;
  bool d_ok;
  bool d_truncate;
  bool d_verbose;
  //std::set<std::string> d_warningsgiven;
  std::string d_countrycode;
 public:
#if __cpp_lib_span >= 202002L
  SignalPlaintextBackupDatabase(std::span<std::string const> const &sptbxmls, bool truncate, bool verbose,
                                std::vector<std::pair<std::string, std::string>> namemap,
                                std::string const &namemap_file, std::string const &countrycode,
                                bool autogroupnames);
#else
  SignalPlaintextBackupDatabase(std::vector<std::string> const &sptbxmls, bool truncate, bool verbose,
                                std::vector<std::pair<std::string, std::string>> namemap,
                                std::string const &namemap_file, std::string const &countrycode,
                                bool autogroupnames);
#endif
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase &&other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase &&other) = delete;
  inline bool ok() const;
  inline bool listContacts() const;

  friend class SignalBackup;
  friend class DummyBackup;

 private:
  inline std::string normalizePhoneNumber(std::string const &in, bool show = true);// const;
  std::set<std::string> norm_shown;
};

inline bool SignalPlaintextBackupDatabase::ok() const
{
  return d_ok;
}

inline bool SignalPlaintextBackupDatabase::listContacts() const
{
  SqliteDB::QueryResults addresses;
  d_database.exec("WITH adrs AS "
                  "("
                  "  SELECT DISTINCT address FROM smses UNION ALL SELECT DISTINCT sourceaddress AS address FROM smses"
                  ") "
                  "SELECT DISTINCT address FROM adrs WHERE address IS NOT NULL ORDER BY address", &addresses);
  //addresses.prettyPrint(d_truncate);

  if (addresses.rows() == 0) [[unlikely]]
    Logger::message("(no contacts found in XML file)");
  else
    Logger::message(" is_chat   ", std::setw(20), std::left, " address", std::setw(0), " :  name");

  for (unsigned int i = 0; i < addresses.rows(); ++i)
  {
    std::string cn = d_database.getSingleResultAs<std::string>("SELECT MAX(contact_name) FROM smses "
                                                               "WHERE contact_name IS NOT '(Unknown)' "
                                                               "  AND contact_name IS NOT NULL "
                                                               "  AND contact_name IS NOT '' "
                                                               "  AND address = ?", addresses.value(i, 0), std::string());
    long long int is_chat = d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM smses WHERE address = ? AND skip = 0", addresses.value(i, 0), 0);
    Logger::message((is_chat > 0 ? "   (*)     " : "           "), std::setw(20), std::left, addresses(i, "address"), std::setw(0), " : \"", cn, "\"");
  }
  /*
  std::vector<std::string> numbers
    {
      {"00-1-202-688-5500"},
      {"(202)688-5500"},
      {"+12026885500"},
      {"011-1-202-688-5500"},
      {"011381688-5500"},
      {"00381688-5500"},
      {"+381688-5500"}
    };
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  for (auto const &n : numbers)
    std::cout << std::setw(19) << std::left << n << " : " << std::setw(0) << normalizePhoneNumber(n) << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  */
  return true;
}

inline std::string SignalPlaintextBackupDatabase::normalizePhoneNumber(std::string const &in, bool show)// const
{
  if (show && norm_shown.find(in) == norm_shown.end())
    Logger::message("normalizePhoneNumber in:  ", in);

  std::string result;

  // if in is group (phone1~phone2~phone3~etc), split and recurse...
  if (std::string::size_type pos = in.find('~');
      pos != std::string::npos &&
      pos != 0 &&
      pos != in.size() - 1)
  {
    std::string::size_type start = 0;
    std::string::size_type end;
    while ((end = in.find('~', start)) != std::string::npos)
    {
      result += normalizePhoneNumber(in.substr(start, end - start), false) + '~';
      start = end + 1;
    }
    // get last bit
    result += normalizePhoneNumber(in.substr(start), false);
  }
  else
  {
    result = in;

#if __cpp_lib_erase_if >= 202002L
    unsigned int removed = std::erase_if(result, [](char c) { return (c < '0' || c > '9') && c != '+'; });
#else
    unsigned int removed = 0;
    result.erase(std::remove_if(result.begin(), result.end(),
                                [&](char c)
                                {
                                  if ((c < '0' || c > '9') && c != '+')
                                  {
                                    ++removed;
                                    return true;
                                  }
                                  return false;
                                }), result.end());
#endif

    if (removed > result.size())
    {
      if (show && norm_shown.find(in) == norm_shown.end())
      {
        norm_shown.insert(in);
        Logger::message("normalizePhoneNumber out: ", in);
      }
      return in;
    }

    if (STRING_STARTS_WITH(result, "00"))
      result = "+" + result.substr(STRLEN("00"));
    else if (STRING_STARTS_WITH(result, "011"))
      result = "+" + result.substr(STRLEN("011"));

    // Special case to deal with numbers that start with _two_ international call prefixes _ countrycodes:
    // eg (with countrycode '1'): 01110019999999999
    if (result.size() >= 15 && // we'll assume max number size of 15 (sources differ), the plus stands for (at least) 2 digits.
        !d_countrycode.empty() &&
        d_countrycode[0] == '+' &&
        STRING_STARTS_WITH(result, d_countrycode) &&
        (result.substr(d_countrycode.size(), (d_countrycode.size() - 1) + 2) == ("00" + d_countrycode.substr(1)) ||
         result.substr(d_countrycode.size(), (d_countrycode.size() - 1) + 3) == ("011" + d_countrycode.substr(1)))) [[unlikely]]
    {
      Logger::warning("Detected doubled prefix and countrycode in phone number (", in, ")");
      result = normalizePhoneNumber(result.substr(d_countrycode.size()), false);
    }

    if (result[0] != '+' && !d_countrycode.empty())
      result = d_countrycode + (result[0] == '0' ? result.substr(1) : result);
  }
  if (result.size() >= 9)
  {
    if (show && norm_shown.find(in) == norm_shown.end())
    {
      norm_shown.insert(in);
      Logger::message("normalizePhoneNumber out: ", result);
    }
    return result;
  }

  if (show && norm_shown.find(in) == norm_shown.end())
  {
    norm_shown.insert(in);
    Logger::message("normalizePhoneNumber out: ", in);
  }
  return in;
}

#endif
