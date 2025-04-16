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

#ifndef SQLCIPHERDECRYPTOR_H_
#define SQLCIPHERDECRYPTOR_H_

#include <string>
#include <fstream>

#include "../common_filesystem.h"
#include "../logger/logger.h"

struct evp_md_st;

class SqlCipherDecryptor
{
  std::string d_databasepath;
  unsigned char *d_key;
  unsigned char *d_hmackey;
  unsigned char *d_salt;
  evp_md_st const *d_digest;
  unsigned char *d_decrypteddata;
  uint64_t d_decrypteddatasize;
  size_t d_digestname_size;
  char *d_digestname;
  unsigned int d_keysize;
  unsigned int d_hmackeysize;
  unsigned int d_saltsize;
  unsigned int d_digestsize;
  unsigned int d_pagesize;
  bool d_verbose;
  bool d_ok;

  static unsigned char constexpr s_saltmask = 0x3a;
  static int constexpr s_sqlliteheader_size = 16;
  static char constexpr s_sqlliteheader[s_sqlliteheader_size] = {'S', 'Q', 'L', 'i', 't', 'e', ' ', 'f', 'o', 'r', 'm', 'a', 't', ' ', '3', '\0'};

  struct DecodedData
  {
    unsigned char *d_data;
    uint64_t d_datasize;
  };

 public:
  explicit SqlCipherDecryptor(std::string const &databasepath, std::string const &hexkey,
                              int version, bool verbose);
  SqlCipherDecryptor(SqlCipherDecryptor const &other) = delete;
  SqlCipherDecryptor &operator=(SqlCipherDecryptor const &other) = delete;
  ~SqlCipherDecryptor();
  inline bool ok() const;
  inline DecodedData data() const;
  inline bool writeToFile(std::string const &filename, bool overwrite) const;
 private:
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
    Logger::error("File ", filename, " exists, use --overwrite to overwrite");
    return false;
  }

  std::ofstream out(filename, std::ios_base::binary);
  if (!out.is_open())
  {
    Logger::error("Failed to open ", filename, " for writing");
    return false;
  }

  if (!out.write(reinterpret_cast<char *>(d_decrypteddata), static_cast<std::streamsize>(d_decrypteddatasize)))
  {
    Logger::error("Error writing data to file");
    return false;
  }

  return true;
}

#endif
