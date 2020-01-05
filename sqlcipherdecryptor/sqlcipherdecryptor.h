/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

#ifndef SQLCIPHERDECRYPTOR_H_
#define SQLCIPHERDECRYPTOR_H_

#include <string>

#include "../common_be.h"

namespace CryptoPP
{
  class PasswordBasedKeyDerivationFunction;
  class HMAC_Base;
}

class SqlCipherDecryptor
{
  bool d_ok;
  std::string d_path;
  unsigned char *d_key;
  unsigned int d_keysize;
  unsigned char *d_hmackey;
  unsigned int d_hmackeysize;
  unsigned char *d_salt;
  unsigned int d_saltsize;
  CryptoPP::PasswordBasedKeyDerivationFunction *d_pbkdf;
  CryptoPP::HMAC_Base *d_hmac;
  unsigned int d_pagesize;
  unsigned int d_digestsize;

  unsigned char *d_decrypteddata;
  unsigned int d_decrypteddatasize;

  static unsigned char constexpr s_saltmask = 0x3a;
  static char constexpr s_sqlliteheader[16] = {'S', 'Q', 'L', 'i', 't', 'e', ' ', 'f', 'o', 'r', 'm', 'a', 't', ' ', '3', '\0'};

  struct DecodedData
  {
    unsigned char *d_data;
    unsigned int d_datasize;
  };

 public:
  explicit SqlCipherDecryptor(std::string const &path, int version = 4);
  SqlCipherDecryptor(SqlCipherDecryptor const &other) = delete;
  SqlCipherDecryptor &operator=(SqlCipherDecryptor const &other) = delete;
  ~SqlCipherDecryptor();
  inline bool ok() const;
  inline DecodedData data() const;
 private:
  bool getKey();
  bool getHmacKey();
  bool decryptData(std::ifstream *dbfile);
};

inline bool SqlCipherDecryptor::ok() const
{
  return d_ok;
}

inline SqlCipherDecryptor::DecodedData SqlCipherDecryptor::data() const
{
  return {d_decrypteddata, d_decrypteddatasize};
}

#endif
