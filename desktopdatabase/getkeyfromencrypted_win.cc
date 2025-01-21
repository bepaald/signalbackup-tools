/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#if defined(_WIN32) || defined(__MINGW64__)

#include "desktopdatabase.ih"

#include "../base64/base64.h"
#include <dpapi.h>
#include <openssl/core_names.h>

bool DesktopDatabase::getKeyFromEncrypted_win()
{



  // 1. get the encrypted key from config.json
  std::string keystr = readEncryptedKey();
  if (keystr.empty())
    return false;

  unsigned long encryptedkey_data_length = keystr.size() / 2;
  std::unique_ptr<unsigned char[]> encryptedkey_data(new unsigned char[encryptedkey_data_length]);
  bepaald::hexStringToBytes(keystr, encryptedkey_data.get(), encryptedkey_data_length);






  // 2. get the key to decrypt the encrypted key
  //*****  2a. get the base64 encoded encrypted key to decrypt the encrypted key ******//
  std::fstream localstate(d_configdir + "/Local State", std::ios_base::in | std::ios_base::binary);
  if (!localstate.is_open())
  {
    Logger::error("Failed to open input: ", d_configdir, "/Local State");
    return false;
  }
  std::string line;
  std::regex keyregex(".*\"encrypted_key\":\\s*\"([^\"]*)\".*");
  std::smatch m;
  bool found = false;
  while (std::getline(localstate, line))
  {
    //std::cout << "Line: " << line << std::endl;
    if (std::regex_match(line, m, keyregex))
      if (m.size() == 2) // m[0] is full match, m[1] is first submatch (which we want)
      {
        found = true;
        break;
      }
  }

  if (!found)
  {
    Logger::error("Failed to read key from Local State");
    return false;
  }

  std::string encrypted_encryptedkey_keyb64 = m[1].str();
  //std::cout << encrypted_encryptedkey_keyb64 << std::endl;

  //***** 2b. decrypt it ******//
  std::pair<unsigned char *, size_t> encrypted_encryptedkey_key = Base64::base64StringToBytes(encrypted_encryptedkey_keyb64);
  //std::cout << "enc Key size: " << encrypted_encryptedkey_key.second << std::endl;
  //std::cout << "enc Key:      " << bepaald::bytesToHexString(encrypted_encryptedkey_key.first, encrypted_encryptedkey_key.second) << std::endl;

  // the encrypted key starts with 'D' 'P' 'A' 'P' 'I' {0x44, 0x50, 0x41, 0x50, 0x41}, skip this...
  DATA_BLOB encrypted_encryptedkey_key_blob{static_cast<unsigned long>(encrypted_encryptedkey_key.second - STRLEN("DPAPI")), encrypted_encryptedkey_key.first + STRLEN("DPAPI")};
  DATA_BLOB encryptedkey_key_blob;
  if (!CryptUnprotectData(&encrypted_encryptedkey_key_blob, nullptr, nullptr, nullptr, nullptr, 0, &encryptedkey_key_blob)) [[unlikely]]
  {
    Logger::error("Failed to decrypt key (1)");
    bepaald::destroyPtr(&encrypted_encryptedkey_key.first, &encrypted_encryptedkey_key.second);
    return false;
  }
  bepaald::destroyPtr(&encrypted_encryptedkey_key.first, &encrypted_encryptedkey_key.second);
  uint64_t encryptedkey_key_length = encryptedkey_key_blob.cbData;
  std::unique_ptr<unsigned char[]> encryptedkey_key(new unsigned char[encryptedkey_key_length]);
  std::memcpy(encryptedkey_key.get(), encryptedkey_key_blob.pbData, encryptedkey_key_length);
  LocalFree(encryptedkey_key_blob.pbData);
  //std::cout << "Decrypted key to decrypt encrypted key: " << bepaald::bytesToHexString(encryptedkey_key.get(), encryptedkey_key_length) << std::endl << std::endl;




  // 3. Now decrypt the encrypted_key using the decrypted key from local state
  // the encrypted key (from step 1) is made up of
  // - a 3 byte header ('v', '1', '0')
  // - a 12 byte nonce
  // - 64 bytes of encrypted data
  // - 16 bytes mac
  uint64_t header_length = 3;
  unsigned char *header = encryptedkey_data.get();
  uint64_t nonce_length = 12;
  unsigned char *nonce = encryptedkey_data.get() + header_length;
  uint64_t mac_length = 16;
  unsigned char *mac = encryptedkey_data.get() + (encryptedkey_data_length - mac_length);
  uint64_t encdata_length = encryptedkey_data_length - mac_length - header_length - nonce_length;
  unsigned char *encdata = nonce + nonce_length;
  //std::cout << bepaald::bytesToHexString(header, header_length) << std::endl;
  //std::cout << bepaald::bytesToHexString(nonce, nonce_length) << std::endl;
  //std::cout << bepaald::bytesToHexString(encdata, encdata_length) << std::endl;
  //std::cout << bepaald::bytesToHexString(mac, mac_length) << std::endl;

  unsigned char v10header[3] = {'v', '1', '0'};
  if (std::memcmp(header, v10header, header_length) != 0) [[unlikely]]
    Logger::warning("Unexpected header value: ", bepaald::bytesToHexString(header, header_length));

  // Create and initialize the decryption context & cipher
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  std::unique_ptr<EVP_CIPHER, decltype(&::EVP_CIPHER_free)> cipher(EVP_CIPHER_fetch(NULL, "AES-256-GCM", NULL), &::EVP_CIPHER_free);
  if (!ctx || !cipher)
  {
    Logger::error("Failed to create decryption context or cipher");
    return false;
  }

  // set parameters (to set and check MAC)
  OSSL_PARAM params[2];
  params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG, mac, mac_length);
  params[1] = OSSL_PARAM_construct_end();

  if (!EVP_DecryptInit_ex2(ctx.get(), cipher.get(), encryptedkey_key.get(), nonce, params)) [[unlikely]]
  {
    Logger::error("Failed to initialize decryption operation");
    return false;
  }

  int len = 0;
  uint64_t key_hexstr_length = 64;
  std::unique_ptr<unsigned char[]> key_hexstr(new unsigned char[key_hexstr_length]);
  if (!EVP_DecryptUpdate(ctx.get(), key_hexstr.get(), &len, encdata, encdata_length)) [[unlikely]]
  {
    Logger::error("Failed to decrypt key (2)");
    return false;
  }

  if (EVP_DecryptFinal_ex(ctx.get(), key_hexstr.get() + len, &len) > 0) [[likely]]
  {
    // the decrypted data is not the actual key, but the key as hex in ascii for some reason...
    d_hexkey = std::string(reinterpret_cast<char const *>(key_hexstr.get()), key_hexstr_length);
    //std::cout << "KEY !! : " << d_hexkey << std::endl;
    return true;
  }
  else [[unlikely]]
  {
    Logger::error("Failed to finalize decryption (possibly MAC failed)");
    return false;
  }
}

#endif
