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

#include <set>

class SignalPlaintextBackupDatabase
{
  MemSqliteDB d_database;
  bool d_ok;
  bool d_truncate;
  bool d_verbose;
  std::set<std::string> d_warningsgiven;
 public:
  SignalPlaintextBackupDatabase(std::string const &sptbxml, bool truncate, bool verbose,
                                std::vector<std::pair<std::string, std::string>> const &namemap);
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase &&other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase &&other) = delete;
  inline bool ok() const;
  inline bool listContacts() const;

  friend class SignalBackup;
  friend class DummyBackup;

 private:
  inline void warnOnce(std::string const &warning, bool error = false);
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

  for (unsigned int i = 0; i < addresses.rows(); ++i)
  {
    std::string cn = d_database.getSingleResultAs<std::string>("SELECT MAX(contact_name) FROM smses WHERE contact_name IS NOT '(Unknown)' AND contact_name IS NOT NULL AND contact_name IS NOT '' AND address = ?", addresses.value(i, 0), std::string());
    Logger::message(std::setw(20), std::left, addresses(i, "address"), std::setw(0), " : \"", cn, "\"");
  }

  return true;
}

inline void SignalPlaintextBackupDatabase::warnOnce(std::string const &warning, bool error)
{
  if (!bepaald::contains(d_warningsgiven, warning))
  {
    if (error)
      Logger::error(warning);
    else
      Logger::warning(warning);
    d_warningsgiven.insert(warning);
  }
}

#endif
