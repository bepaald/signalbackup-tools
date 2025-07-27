/*
  Copyright (C) 2025  Selwin van Dijk

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

#ifndef ADBBACKUPDATABASE_H_
#define ADBBACKUPDATABASE_H_

#include "../filesqlitedb/filesqlitedb.h"

#include <memory>

class AdbBackupDatabase
{
  FileSqliteDB d_db;
  std::unique_ptr<unsigned char []> d_combined_secret;
  int d_encryption_secret_length;
  unsigned char *d_encryption_secret; // non-owning pointer
  int d_mac_secret_length;
  unsigned char *d_mac_secret;        // non-owning pointer
  bool d_ok;
  bool d_verbose;

 public:
  AdbBackupDatabase(std::string const &backupdir, std::string const &passphrase, bool verbose);
  inline bool ok() const;
  inline int macSecretLength() const;
  inline unsigned char const *macSecret() const;
  inline int encryptionSecretLength() const;
  inline unsigned char const *encryptionSecret() const;
};

inline bool AdbBackupDatabase::ok() const
{
  return d_ok;
}

inline int AdbBackupDatabase::macSecretLength() const
{
  return d_mac_secret_length;
}

inline unsigned char const *AdbBackupDatabase::macSecret() const
{
  return d_mac_secret;
}

inline int AdbBackupDatabase::encryptionSecretLength() const
{
  return d_encryption_secret_length;
}

inline unsigned char const *AdbBackupDatabase::encryptionSecret() const
{
  return d_encryption_secret;
}


#endif
