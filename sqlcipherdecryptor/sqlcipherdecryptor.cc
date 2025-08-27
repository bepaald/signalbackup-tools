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

#include "sqlcipherdecryptor.ih"

#include "../common_be.h"
#include "../common_bytes.h"

/*
  SQLCipher 1,2,3    ->  4
  - KDF Algorithm: PBKDF2-HMAC-SHA1 -> PBKDF2-HMAC-SHA512
  - KDF Iterations: 4000,4000,64000,256000 (") // unused, no password->key derivation is done,
                                               // since the raw key is in config. In newer versions
                                               // of Signal Desktop, the key is encrypted in the
                                               // config, but encrypted from the user env, so still
                                               // no KD is done
  - HMAC: HMAC-SHA1 -> HMAC-SHA512
  - Pagesize: 1024 -> 4096
*/

SqlCipherDecryptor::SqlCipherDecryptor(std::string const &databasepath, std::string const &hexkey,
                                       int version, bool verbose)
  :
  d_databasepath(databasepath),
  d_key(nullptr),
  d_hmackey(nullptr),
  d_salt(nullptr),
  d_digest(version >= 4 ? EVP_sha512() : EVP_sha1()),
  d_decrypteddata(nullptr),
  d_decrypteddatasize(0),
  d_digestname_size((version >= 4 ? STRLEN("SHA512") : STRLEN("SHA1")) + 1),
  d_digestname(version >= 4 ?
               new char[d_digestname_size] {'S', 'H', 'A', '5', '1', '2', '\0'} :
               new char[d_digestname_size] {'S', 'H', 'A', '1', '\0'}),
  d_keysize(0),
  d_hmackeysize(0),
  d_saltsize(0),
  d_digestsize(EVP_MD_size(d_digest)),
  d_pagesize(version >= 4 ? 4096 : 1024),
  d_verbose(verbose),
  d_ok(false)
{
  if (hexkey.empty())
    return;

  d_keysize = hexkey.size() / 2;
  d_key = new unsigned char[d_keysize];
  if (!bepaald::hexStringToBytes(hexkey, d_key, d_keysize))
  {
    Logger::error("Failed to set key from provided hex string");
    return;
  }

  // open database file
  std::ifstream dbfile(d_databasepath, std::ios_base::in | std::ios_base::binary);
  if (!dbfile.is_open())
  {
    Logger::error("Failed to open database file '", d_databasepath, "'");
    return;
  }

  // get file size (this will also be the output file size)
  //dbfile.seekg(0, std::ios_base::end);
  //d_decrypteddatasize = dbfile.tellg();
  //dbfile.seekg(0, std::ios_base::beg);
  d_decrypteddatasize = bepaald::fileSize(d_databasepath);

  if (d_verbose) [[unlikely]]
    Logger::message("Opening Desktop database `", d_databasepath, "' (", d_decrypteddatasize, " bytes)");

  // read salt
  d_saltsize = 16;
  d_salt = new unsigned char[d_saltsize];

  if (!dbfile.read(reinterpret_cast<char *>(d_salt), d_saltsize))
  {
    Logger::error("Failed to read salt from database file");
    return;
  }

  if (!getHmacKey())
    return;

  if (d_verbose) [[unlikely]]
    Logger::message("Starting decrypt...");
  if (!decryptData(&dbfile))
    return;
  if (d_verbose) [[unlikely]]
    Logger::message("Done!");

  // std::cout << "CIPHER KEY: " << bepaald::bytesToHexString(d_key, d_keysize) << std::endl;
  // std::cout << "  HMAC KEY: " << bepaald::bytesToHexString(d_hmackey, d_hmackeysize) << std::endl;

  d_ok = true;
}
