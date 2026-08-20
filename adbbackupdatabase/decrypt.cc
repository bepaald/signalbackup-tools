/*
  Copyright (C) 2025-2026  Selwin van Dijk

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

#include "adbbackupdatabase.ih"

#include "../common_crypto.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>

std::optional<std::pair<std::unique_ptr<unsigned char[]>, int>> AdbBackupDatabase::decrypt(unsigned char const *encdata, int enclength,
                                                                                           unsigned char *mackey, int maclength,
                                                                                           unsigned char *key, int keylength [[maybe_unused]]) //static
{
  // check HMAC
  unsigned int digest_size = SHA_DIGEST_LENGTH;
  std::unique_ptr<unsigned char []> hash(new unsigned char[digest_size]);
  if (!HMAC(EVP_sha1(), mackey, maclength,
            encdata, enclength - SHA_DIGEST_LENGTH,
            hash.get(), &digest_size) ||
      std::memcmp(hash.get(), encdata + (enclength - SHA_DIGEST_LENGTH), SHA_DIGEST_LENGTH) != 0) [[unlikely]]
  {
    Logger::error("HMAC failed for message body");
    return std::optional<std::pair<std::unique_ptr<unsigned char[]>, int>>();
  }

  auto decrypted = bepaald::decrypt_aes_128_cbc(key, encdata, encdata + 16, enclength - (16 + SHA_DIGEST_LENGTH));
  if (!decrypted.first || decrypted.second == 0) [[unlikely]]
    return std::nullopt;
  return decrypted;
}
