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

#include "fileencryptor.ih"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

#include <memory>
#include <cstring>

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptAttachment(unsigned char *data, uint64_t length)
{
  if (!d_ok)
    return {nullptr, 0};

  if (length == 0) [[unlikely]]
  {
    Logger::warning("Asked to encrypt a zero sized attachment.");
    //return {nullptr, 0};
  }

  if (d_verbose) [[unlikely]]
    Logger::message_start("Encrypting attachment. Length: ", length, "...");

  // update iv:
  uintToFourBytes(d_iv, d_counter++);

  // encryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1) [[unlikely]]
  {
    Logger::error("CTX INIT FAILED");
    return {nullptr, 0};
  }

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[length + MACSIZE]);
  int l = static_cast<int>(length);
  if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get(), &l, data, length) != 1) [[unlikely]]
  {
    Logger::error("ENCRYPT FAILED");
    return {nullptr, 0};
  }

  // calc mac
  unsigned char hash[SHA256_DIGEST_LENGTH];
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  std::unique_ptr<EVP_MAC, decltype(&::EVP_MAC_free)> mac(EVP_MAC_fetch(nullptr, "hmac", nullptr), &::EVP_MAC_free);
  std::unique_ptr<EVP_MAC_CTX, decltype(&::EVP_MAC_CTX_free)> hctx(EVP_MAC_CTX_new(mac.get()), &::EVP_MAC_CTX_free);
  char digest[] = "SHA256";
  OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("digest", digest, 0), OSSL_PARAM_construct_end()};
  if (EVP_MAC_init(hctx.get(), d_mackey, d_mackey_size, params) != 1) [[unlikely]]
  {
    Logger::error("Failed to initialize HMAC");
    return {nullptr, 0};
  }
  if (EVP_MAC_update(hctx.get(), d_iv, d_iv_size) != 1 ||
      EVP_MAC_update(hctx.get(), encryptedframe.get(), length) != 1 ||
      EVP_MAC_final(hctx.get(), hash, nullptr, SHA256_DIGEST_LENGTH) != 1) [[unlikely]]
  {
    Logger::error("Failed to update/finalize hmac");
    return {nullptr, 0};
  }
#else
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
  if (HMAC_Init_ex(hctx.get(), d_mackey, d_mackey_size, EVP_sha256(), nullptr) != 1) [[unlikely]]
  {
    Logger::error("Failed to initialize HMAC context");
    return {nullptr, 0};
  }
  if (HMAC_Update(hctx.get(), d_iv, d_iv_size) != 1 ||
      HMAC_Update(hctx.get(), encryptedframe.get(), length) != 1 ||
      HMAC_Final(hctx.get(), hash, &digest_size) != 1) [[unlikely]]
  {
    Logger::error("Failed to update/finalize hmac");
    return {nullptr, 0};
  }
#endif
  std::memcpy(encryptedframe.get() + length, hash, 10);

  if (d_verbose) [[unlikely]]
    Logger::message_end("done!");

  return {encryptedframe.release(), length + MACSIZE};
}
