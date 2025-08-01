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

#include "../base64/base64.h"
#include "../filesqlitedb/filesqlitedb.h"
#include "../scopeguard/scopeguard.h"

#include <optional>
#include <memory>

class AdbBackupDatabase
{
  FileSqliteDB d_db;
  std::string d_selfphone;
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
  inline std::optional<std::string> decryptMessageBody(std::string const &encbody) const;
  inline std::string const &selfphone() const;
 private:
  static std::optional<std::pair<std::unique_ptr<unsigned char[]>, int>> decrypt(unsigned char *encdata, int enclength,
                                                                                 unsigned char *mackey, int maclength,
                                                                                 unsigned char *key, int keylength);

  friend class AdbBackupAttachmentReader;
  friend class SignalBackup;
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

inline std::optional<std::string> AdbBackupDatabase::decryptMessageBody(std::string const &encbody_b64) const
{
  auto encbody = Base64::base64StringToBytes(encbody_b64);
  if (encbody.second == 0) [[unlikely]]
  {
    Logger::error("Failed to b64 decode encrypted message body");
    return std::string();
  }
  ScopeGuard encbody_guard([&](){ if (encbody.first) delete[] encbody.first; });

  auto decbody = decrypt(encbody.first, encbody.second,
                         d_mac_secret, d_mac_secret_length,
                         d_encryption_secret, d_encryption_secret_length);
  if (!decbody.has_value()) [[unlikely]]
    return std::optional<std::string>();

  return std::string(reinterpret_cast<char *>(decbody.value().first.get()), decbody.value().second);
}

inline std::string const &AdbBackupDatabase::selfphone() const
{
  return d_selfphone;
}

#endif
