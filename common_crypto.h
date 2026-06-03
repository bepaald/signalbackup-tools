/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifndef COMMON_CRYPTO_H_
#define COMMON_CRYPTO_H_

#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

#include <memory>
#include <cstring>

#include "logger/logger.h"

namespace bepaald
{
  inline std::pair<std::unique_ptr<unsigned char []>, size_t> hkdf_sha256(void const *key, size_t key_size,
                                                                          void const *info, size_t info_size,
                                                                          size_t outputsize);

  inline bool checkHmac_sha256(void const *mackey, size_t mackey_size,
                               void const *data, size_t data_size,
                               void const *expected, size_t expected_size);

  inline std::pair<std::unique_ptr<unsigned char []>, size_t> decrypt(EVP_CIPHER const *ciphertype,
                                                                      void const *key, void const *iv,
                                                                      void const *ciphertext, size_t ciphertext_size);

  inline std::pair<std::unique_ptr<unsigned char []>, size_t> decrypt_aes_256_ctr(void const *key, void const *iv,
                                                                                  void const *ciphertext, size_t ciphertext_size);

  inline std::pair<std::unique_ptr<unsigned char []>, size_t> decrypt_aes_256_cbc(void const *key, void const *iv,
                                                                                  void const *ciphertext, size_t ciphertext_size);

}

inline std::pair<std::unique_ptr<unsigned char []>, size_t> bepaald::hkdf_sha256(void const *key, size_t key_size,
                                                                                 void const *info, size_t info_size,
                                                                                 size_t outputsize)
{
  std::unique_ptr<EVP_PKEY_CTX, decltype(&::EVP_PKEY_CTX_free)> ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr), &::EVP_PKEY_CTX_free);
  if (EVP_PKEY_derive_init(ctx.get()) != 1 ||
      EVP_PKEY_CTX_set_hkdf_md(ctx.get(), EVP_sha256()) != 1) [[unlikely]]
  {
    Logger::error("Failed to init HKDF");
    return {nullptr, 0};
  }

  if (EVP_PKEY_CTX_set1_hkdf_key(ctx.get(), reinterpret_cast<unsigned char const *>(key), key_size) != 1 ||
      EVP_PKEY_CTX_add1_hkdf_info(ctx.get(), reinterpret_cast<unsigned char const *>(info), info_size) != 1) [[unlikely]]
  {
    Logger::error("Failed to set data for HKDF");
    return {nullptr, 0};
  }

  std::pair<std::unique_ptr<unsigned char[]>, size_t> res{new unsigned char[outputsize], outputsize};
  if (EVP_PKEY_derive(ctx.get(), res.first.get(), &res.second) != 1) [[unlikely]]
  {
    Logger::error("Error deriving HKDF");
    return {nullptr, 0};
  }

  return res;
}

inline bool bepaald::checkHmac_sha256(void const *mackey, size_t mackey_size,
                                      void const *data, size_t data_size,
                                      void const *expected, size_t expected_size)
{
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  std::unique_ptr<unsigned char []> calculated_mac(new unsigned char[SHA256_DIGEST_LENGTH]);
  if (HMAC(EVP_sha256(), reinterpret_cast<unsigned char const *>(mackey), mackey_size,
           reinterpret_cast<unsigned char const *>(data), data_size,
           calculated_mac.get(), &digest_size) == nullptr) [[unlikely]]
  {
    Logger::error("Failed to calculate HMAC for master_secret");
    return false;
  }
  if (std::memcmp(expected, calculated_mac.get(), expected_size) != 0) [[unlikely]]
  {
    Logger::error("HMAC check failed. Mac read: ", bepaald::bytesToHexString(reinterpret_cast<unsigned char const *>(expected), expected_size));
    Logger::error_indent("             Mac calculated: ", bepaald::bytesToHexString(calculated_mac.get(), SHA256_DIGEST_LENGTH));
    return false;
  }
  return true;
}

// for aes 256, the key and iv have set sizes (32 and 16 resp)
inline std::pair<std::unique_ptr<unsigned char []>, size_t> bepaald::decrypt(EVP_CIPHER const *ciphertype,
                                                                             void const *key, void const *iv,
                                                                             void const *ciphertext, size_t ciphertext_size)
{
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (EVP_DecryptInit_ex(ctx.get(), ciphertype, nullptr, reinterpret_cast<unsigned char const *>(key), reinterpret_cast<unsigned char const *>(iv)) != 1) [[unlikely]]
  {
    Logger::error("CTX INIT FAILED");
    return {nullptr, 0};
  }

  std::pair<std::unique_ptr<unsigned char []>, size_t> plaintext{new unsigned char[ciphertext_size], ciphertext_size};
  int written = ciphertext_size;
  if (EVP_DecryptUpdate(ctx.get(),
                        plaintext.first.get(), &written,
                        reinterpret_cast<unsigned char const *>(ciphertext), ciphertext_size) != 1) [[unlikely]]
  {
    Logger::error("Failed to decrypt data");
    return {nullptr, 0};
  }

  int lastbits = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), plaintext.first.get() + written, &lastbits) != 1) [[unlikely]]
  {
    Logger::error("Failed to finalize decrypt");
    return {nullptr, 0};
  }
  plaintext.second = written + lastbits;

  return plaintext;
}

inline std::pair<std::unique_ptr<unsigned char []>, size_t> bepaald::decrypt_aes_256_ctr(void const *key, void const *iv,
                                                                                         void const *ciphertext, size_t ciphertext_size)
{
  return decrypt(EVP_aes_256_ctr(), key, iv, ciphertext, ciphertext_size);
}

inline std::pair<std::unique_ptr<unsigned char []>, size_t> bepaald::decrypt_aes_256_cbc(void const *key, void const *iv,
                                                                                         void const *ciphertext, size_t ciphertext_size)
{
  return decrypt(EVP_aes_256_cbc(), key, iv, ciphertext, ciphertext_size);
}

#endif
