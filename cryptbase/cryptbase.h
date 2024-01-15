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

#ifndef CRYPTBASE_H_
#define CRYPTBASE_H_

#include <cstring>
#include <cctype>

#include "../common_be.h"

class CryptBase
{
 protected:
  uint static constexpr MACSIZE = 10;
  bool d_ok;
  unsigned char *d_backupkey;
  uint64_t d_backupkey_size;
  unsigned char *d_cipherkey;
  uint64_t d_cipherkey_size;
  unsigned char *d_mackey;
  uint64_t d_mackey_size;
  unsigned char *d_iv;
  uint64_t d_iv_size;
  unsigned char *d_salt;
  uint64_t d_salt_size;
  uint64_t d_counter;
 public:
  inline CryptBase();
  inline CryptBase(CryptBase const &other);
  inline CryptBase &operator=(CryptBase const &other);
  inline ~CryptBase();
  inline bool ok() const;
 protected:
  bool getCipherAndMac(uint hashoutputsize, size_t outputsize);
  bool getBackupKey(std::string const &passphrase);
  inline void uintToFourBytes(unsigned char *bytes, uint32_t val) const;
  inline uint32_t fourBytesToUint(unsigned char *b) const;
};

inline CryptBase::CryptBase()
  :
  d_ok(false),
  d_backupkey(nullptr),
  d_backupkey_size(0),
  d_cipherkey(nullptr),
  d_cipherkey_size(0),
  d_mackey(nullptr),
  d_mackey_size(0),
  d_iv(nullptr),
  d_iv_size(0),
  d_salt(nullptr),
  d_salt_size(0),
  d_counter(0)
{}

inline CryptBase::CryptBase(CryptBase const &other)
  :
  d_ok(false),
  d_backupkey(nullptr),
  d_backupkey_size(other.d_backupkey_size),
  d_cipherkey(nullptr),
  d_cipherkey_size(other.d_cipherkey_size),
  d_mackey(nullptr),
  d_mackey_size(other.d_mackey_size),
  d_iv(nullptr),
  d_iv_size(other.d_iv_size),
  d_salt(nullptr),
  d_salt_size(other.d_salt_size),
  d_counter(other.d_counter)
{
  if (other.d_backupkey)
  {
    d_backupkey = new unsigned char[d_backupkey_size];
    std::memcpy(d_backupkey, other.d_backupkey, d_backupkey_size);
  }

  if (other.d_cipherkey)
  {
    d_cipherkey = new unsigned char[d_cipherkey_size];
    std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
  }

  if (other.d_mackey)
  {
    d_mackey = new unsigned char[d_mackey_size];
    std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
  }

  if (other.d_iv)
  {
    d_iv = new unsigned char[d_iv_size];
    std::memcpy(d_iv, other.d_iv, d_iv_size);
  }
  if (other.d_salt)
  {
    d_salt = new unsigned char[d_salt_size];
    std::memcpy(d_salt, other.d_salt, d_salt_size);
  }

  d_ok = true;
}

inline CryptBase &CryptBase::operator=(CryptBase const &other)
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_iv, &d_iv_size);
    bepaald::destroyPtr(&d_salt, &d_salt_size);
    bepaald::destroyPtr(&d_backupkey, &d_backupkey_size);
    bepaald::destroyPtr(&d_mackey, &d_mackey_size);
    bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);

    d_backupkey_size = other.d_backupkey_size;
    d_cipherkey_size = other.d_cipherkey_size;
    d_mackey_size = other.d_mackey_size;
    d_iv_size = other.d_iv_size;
    d_salt_size = other.d_salt_size;

    if (other.d_backupkey)
    {
      d_backupkey = new unsigned char[d_backupkey_size];
      std::memcpy(d_backupkey, other.d_backupkey, d_backupkey_size);
    }

    if (other.d_cipherkey)
    {
      d_cipherkey = new unsigned char[d_cipherkey_size];
      std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
    }

    if (other.d_mackey)
    {
      d_mackey = new unsigned char[d_mackey_size];
      std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
    }

    if (other.d_iv)
    {
      d_iv = new unsigned char[d_iv_size];
      std::memcpy(d_iv, other.d_iv, d_iv_size);
    }
    if (other.d_salt)
    {
      d_salt = new unsigned char[d_salt_size];
      std::memcpy(d_salt, other.d_salt, d_salt_size);
    }
  }
  return *this;
}

inline CryptBase::~CryptBase()
{
  bepaald::destroyPtr(&d_iv, &d_iv_size);
  bepaald::destroyPtr(&d_salt, &d_salt_size);
  bepaald::destroyPtr(&d_backupkey, &d_backupkey_size);
  bepaald::destroyPtr(&d_mackey, &d_mackey_size);
  bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);
}

inline bool CryptBase::ok() const
{
  return d_ok;
}

inline void CryptBase::uintToFourBytes(unsigned char *bytes, uint32_t val) const
{
  val = bepaald::swap_endian(val);
  std::memcpy(bytes, reinterpret_cast<unsigned char *>(&val), 4);
}

inline uint32_t CryptBase::fourBytesToUint(unsigned char *b) const
{
  uint32_t res = 0;
  res |= static_cast<uint32_t>(b[3] & 0xFF);
  res |= static_cast<uint32_t>(b[2] & 0xFF) << 8;
  res |= static_cast<uint32_t>(b[1] & 0xFF) << 16;
  res |= static_cast<uint32_t>(b[0] & 0xFF) << 24;
  return res;
}

#endif
