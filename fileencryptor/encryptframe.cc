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

#include "fileencryptor.ih"

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(unsigned char *data, uint64_t length)
{
  if (!d_ok) [[unlikely]]
    return {nullptr, 0};

  if (length == 0) [[unlikely]]
  {
    Logger::warning("Asked to encrypt a zero sized frame.");
    //return {nullptr, 0};
  }

  // update iv:
  uintToFourBytes(d_iv, d_counter++);

  // encryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
  {
    Logger::error("CTX INIT FAILED");
    return {nullptr, 0};
  }

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[sizeof(uint32_t) + length + MACSIZE]);

  int encryptedframepos = 0;
  // in newer backup file versions, the length is encrypted
  if (d_backupfileversion >= 1) [[likely]]
  {
    int l = static_cast<int>(sizeof(uint32_t) + length);
    uint32_t length_data = bepaald::swap_endian<uint32_t>(length + MACSIZE);

    if (d_verbose) [[unlikely]]
      Logger::message_start("Encrypting frame. Length: ", length, ", +macsize: ", (length + MACSIZE), ", swap_endian: ", length_data, " -> ");

    if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get(), &l, reinterpret_cast<unsigned char *>(&length_data), sizeof(uint32_t)) != 1)
    {
      Logger::error("ENCRYPT FAILED");
      return {nullptr, 0};
    }
    encryptedframepos = l;

    if (d_verbose) [[unlikely]]
      Logger::message(bepaald::bytesToHexString(reinterpret_cast<unsigned char *>(encryptedframe.get()), 4));
  }
  else [[unlikely]] // old backup file format, had RAW frame length
  {
    uint32_t rawlength = bepaald::swap_endian<uint32_t>(length + MACSIZE);
    if (d_verbose) [[unlikely]]
      Logger::message("Writing raw framelength: ", length, ", +macsize: ", (length + MACSIZE), ", swap_endian: ", rawlength);
    std::memcpy(encryptedframe.get(), reinterpret_cast<unsigned char *>(&rawlength), sizeof(uint32_t));
    encryptedframepos = 4;
  }

  int l = static_cast<int>(length);
  if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get() + encryptedframepos, &l, data, length) != 1)
  {
    Logger::error("ENCRYPT FAILED");
    return {nullptr, 0};
  }

  // calc mac
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size,
       encryptedframe.get() + (d_backupfileversion >= 1 ? 0 : sizeof(uint32_t)),
       length + (d_backupfileversion >= 1 ? sizeof(uint32_t) : 0),
       hash, &digest_size);
  std::memcpy(encryptedframe.get() + sizeof(uint32_t) + length, hash, 10);

  //std::cout << "                                   : " << bepaald::bytesToHexString(hash, digest_size) << std::endl;

  return {encryptedframe.release(), sizeof(uint32_t) + length + MACSIZE};
}
