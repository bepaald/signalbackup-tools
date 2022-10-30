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

#include "sqlcipherdecryptor.ih"

/*
  SQLCipher 1,2,3    ->  4
  - KDF Algorithm: PBKDF2-HMAC-SHA1 -> PBKDF2-HMAC-SHA512
  - KDF Iterations: 4000,4000,64000,256000 (") // unused, no password->key derivation is done, raw key is in config
  - HMAC: HMAC-SHA1 -> HMAC-SHA512
  - Pagesize: 1024 -> 4096
*/

SqlCipherDecryptor::SqlCipherDecryptor(std::string const &path, int version)
  :
  d_ok(false),
  d_path(path),
  d_key(nullptr),
  d_keysize(0),
  d_hmackey(nullptr),
  d_hmackeysize(0),
  d_salt(nullptr),
  d_saltsize(0),
#ifndef USE_CRYPTOPP
  d_digest(version == 4 ? EVP_sha512() : EVP_sha1()),
  d_digestsize(EVP_MD_size(d_digest)),
#else
  d_pbkdf(version == 4 ?
          static_cast<CryptoPP::PasswordBasedKeyDerivationFunction *>(new CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512>) :
          static_cast<CryptoPP::PasswordBasedKeyDerivationFunction *>(new CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1>)),
  d_hmac(version == 4 ?
         static_cast<CryptoPP::HMAC_Base *>(new CryptoPP::HMAC<CryptoPP::SHA512>) :
         static_cast<CryptoPP::HMAC_Base *>(new CryptoPP::HMAC<CryptoPP::SHA1>)),
  d_digestsize(d_hmac->DigestSize()),
#endif
  d_pagesize(version == 4 ? 4096 : 1024),
  d_decrypteddata(nullptr),
  d_decrypteddatasize(0)
{
  if (!getKey())
    return;

  // open database file
  std::ifstream dbfile(d_path + "/sql/db.sqlite", std::ios_base::in | std::ios_base::binary);
  if (!dbfile.is_open())
  {
    std::cout << "Failed to open database file '" << d_path + "/sql/db.sqlite" << "'" << std::endl;
    return;
  }

  // get file size (this will also be the output file size
  dbfile.seekg(0, std::ios_base::end);
  d_decrypteddatasize = dbfile.tellg();
  dbfile.seekg(0, std::ios_base::beg);

  // read salt
  d_saltsize = 16;
  d_salt = new unsigned char[d_saltsize];

  if (!dbfile.read(reinterpret_cast<char *>(d_salt), d_saltsize))
  {
    std::cout << "Failed to read salt from database file" << std::endl;
    return;
  }

  if (!getHmacKey())
    return;

#ifdef USE_CRYPTOPP
  d_hmac->SetKey(d_hmackey, d_hmackeysize);
#endif

  if (!decryptData(&dbfile))
    return;

  // std::cout << "CIPHER KEY: " << bepaald::bytesToHexString(d_key, d_keysize) << std::endl;
  // std::cout << "  HMAC KEY: " << bepaald::bytesToHexString(d_hmackey, d_hmackeysize) << std::endl;

  d_ok = true;
}
