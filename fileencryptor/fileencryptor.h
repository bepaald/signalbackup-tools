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

#ifndef FILEENCRYPTOR_H_
#define FILEENCRYPTOR_H_

#include <memory>
#include <utility>

#include "../cryptbase/cryptbase.h"

class FileEncryptor : public CryptBase
{
  std::string d_passphrase;
  uint32_t d_backupfileversion;
 public:
  FileEncryptor(std::string const &passphrase, unsigned char const *salt, uint64_t salt_size, unsigned char const *iv, uint64_t iv_size, uint32_t backupfileversion, bool verbose);
  explicit FileEncryptor(std::string const &passphrase, uint32_t backupfileversion, bool verbose);
  FileEncryptor();
  inline FileEncryptor(FileEncryptor const &other);
  inline FileEncryptor &operator=(FileEncryptor const &other);
  inline FileEncryptor(FileEncryptor &&other);
  inline FileEncryptor &operator=(FileEncryptor &&other);
  inline bool init(std::string const &passphrase, unsigned char const *salt, uint64_t salt_size, unsigned char const *iv, uint64_t iv_size, uint32_t backupfileversion, bool verbose);
  bool init(unsigned char const *salt, uint64_t salt_size, unsigned char const *iv, uint64_t iv_size);
  inline std::pair<unsigned char *, uint64_t> encryptFrame(std::pair<std::shared_ptr<unsigned char[]>, uint64_t> const &data);
  inline std::pair<unsigned char *, uint64_t> encryptFrame(std::pair<unsigned char *, uint64_t> const &data);
  std::pair<unsigned char *, uint64_t> encryptFrame(unsigned char *data, uint64_t length);
  std::pair<unsigned char *, uint64_t> encryptAttachment(unsigned char *data, uint64_t length);
};

inline FileEncryptor::FileEncryptor(FileEncryptor const &other)
  :
  CryptBase(other),
  d_passphrase(other.d_passphrase),
  d_backupfileversion(other.d_backupfileversion)
{}

inline FileEncryptor &FileEncryptor::operator=(FileEncryptor const &other)
{
  if (this != &other)
  {
    CryptBase::operator=(other);
    d_passphrase = other.d_passphrase;
    d_backupfileversion = other.d_backupfileversion;
  }
  return *this;
}

inline FileEncryptor::FileEncryptor(FileEncryptor &&other)
  :
  CryptBase(std::move(other)),
  d_passphrase(std::move(other.d_passphrase)),
  d_backupfileversion(std::move(other.d_backupfileversion))
{}

inline FileEncryptor &FileEncryptor::operator=(FileEncryptor &&other)
{
  if (this != &other)
  {
    CryptBase::operator=(other);
    d_passphrase = std::move(other.d_passphrase);
    d_backupfileversion = std::move(other.d_backupfileversion);
  }
  return *this;
}

inline bool FileEncryptor::init(std::string const &passphrase, unsigned char const *salt, uint64_t salt_size, unsigned char const *iv, uint64_t iv_size, uint32_t backupfileversion, bool verbose)
{
  d_passphrase = passphrase;
  d_backupfileversion = backupfileversion;
  d_verbose = verbose;
  return init(salt, salt_size, iv, iv_size);
}

inline std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(std::pair<unsigned char *, uint64_t> const &data)
{
  return encryptFrame(data.first, data.second);
}

inline std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(std::pair<std::shared_ptr<unsigned char[]>, uint64_t> const &data)
{
  return encryptFrame(data.first.get(), data.second);
}

#endif
