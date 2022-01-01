/*
    Copyright (C) 2019-2022  Selwin van Dijk

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
#include <fstream>

#include "../common_be.h"


#ifndef USE_CRYPTOPP
class evp_md_st;
#else
namespace CryptoPP
{
  class PasswordBasedKeyDerivationFunction;
  class HMAC_Base;
}
#endif

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
#ifndef USE_CRYPTOPP
  evp_md_st const *d_digest;
#else
  CryptoPP::PasswordBasedKeyDerivationFunction *d_pbkdf;
  CryptoPP::HMAC_Base *d_hmac;
#endif
  unsigned int d_digestsize;
  unsigned int d_pagesize;

  unsigned char *d_decrypteddata;
  uint64_t d_decrypteddatasize;

  static unsigned char constexpr s_saltmask = 0x3a;
  static int constexpr s_sqlliteheader_size = 16;
  static char constexpr s_sqlliteheader[s_sqlliteheader_size] = {'S', 'Q', 'L', 'i', 't', 'e', ' ', 'f', 'o', 'r', 'm', 'a', 't', ' ', '3', '\0'};

  struct DecodedData
  {
    unsigned char *d_data;
    uint64_t d_datasize;
  };

 public:
  explicit SqlCipherDecryptor(std::string const &path, int version = 4);
  SqlCipherDecryptor(SqlCipherDecryptor const &other) = delete;
  SqlCipherDecryptor &operator=(SqlCipherDecryptor const &other) = delete;
  ~SqlCipherDecryptor();
  inline bool ok() const;
  inline DecodedData data() const;
  inline bool writeToFile(std::string const &filename, bool overwrite) const;
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

inline bool SqlCipherDecryptor::writeToFile(std::string const &filename, bool overwrite) const
{
  if (!overwrite && bepaald::fileOrDirExists(filename))
  {
    std::cout << "File " << filename << " exists, use --overwrite to overwrite" << std::endl;
    return false;
  }

  std::ofstream out(filename);
  if (!out.is_open())
  {
    std::cout << "Failed to open " << filename << " for writing" << std::endl;
    return false;
  }

  if (!out.write(reinterpret_cast<char *>(d_decrypteddata), d_decrypteddatasize))
  {
    std::cout << "Error writing data to file" << std::endl;
    return false;
  }

  return true;
}

#endif
