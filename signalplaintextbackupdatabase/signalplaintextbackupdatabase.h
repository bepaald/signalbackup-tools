/*
  Copyright (C) 2024  Selwin van Dijk

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

class SignalPlaintextBackupDatabase
{
  MemSqliteDB d_database;
  bool d_ok;
  bool d_truncate;
  bool d_verbose;
 public:
  SignalPlaintextBackupDatabase(std::string const &sptbxml, bool truncate, bool verbose);
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase(SignalPlaintextBackupDatabase &&other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase const &other) = delete;
  SignalPlaintextBackupDatabase &operator=(SignalPlaintextBackupDatabase &&other) = delete;
  inline bool ok() const;
  inline bool listContacts() const;

  friend class SignalBackup;
  friend class DummyBackup;
};

inline bool SignalPlaintextBackupDatabase::ok() const
{
  return d_ok;
}

inline bool SignalPlaintextBackupDatabase::listContacts() const
{
  return d_database.prettyPrint(d_truncate, "SELECT address AS phone, MAX(contact_name) AS contact_name FROM smses GROUP BY phone ORDER BY phone");
}

#endif
