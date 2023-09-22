/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(std::pair<unsigned char *, uint64_t> const &data)
{
  return encryptFrame(data.first, data.second);
}

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(unsigned char *data, uint64_t length)
{
  if (!d_ok)
    return {nullptr, 0};

  // update iv:
  uintToFourBytes(d_iv, d_counter++);

  // encryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
  {
    std::cout << "CTX INIT FAILED" << std::endl;
    return {nullptr, 0};
  }

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[sizeof(uint32_t) + length + MACSIZE]);

  int encryptedframepos = 0;
  // in newer backup file versions, the length is encrypted
  if (d_backupfileversion >= 1) [[likely]]
  {
    int l = static_cast<int>(sizeof(int32_t) + length);
    int32_t length_data = bepaald::swap_endian<int32_t>(length + MACSIZE);

    if (d_verbose) [[unlikely]]
      std::cout << "Encrypting framelength: " << length << ", +macsize: " << (length + MACSIZE) << ", swap_endian: " << length_data << " -> ";

    if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get(), &l, reinterpret_cast<unsigned char *>(&length_data), sizeof(int32_t)) != 1)
    {
      std::cout << "ENCRYPT FAILED" << std::endl;
      return {nullptr, 0};
    }
    encryptedframepos = l;

    if (d_verbose) [[unlikely]]
      std::cout << bepaald::bytesToHexString(reinterpret_cast<unsigned char *>(encryptedframe.get()), 4) << std::endl;
  }
  else [[unlikely]] // old backup file format, had RAW frame length
  {
    int32_t rawlength = bepaald::swap_endian<int32_t>(length + MACSIZE);
    if (d_verbose) [[unlikely]]
      std::cout << "Writing raw framelength: " << length << ", +macsize: " << (length + MACSIZE) << ", swap_endian: " << rawlength << std::endl;
    std::memcpy(encryptedframe.get(), reinterpret_cast<unsigned char *>(&rawlength), sizeof(int32_t));
    encryptedframepos = 4;
  }

  int l = static_cast<int>(length);
  if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get() + encryptedframepos, &l, data, length) != 1)
  {
    std::cout << "ENCRYPT FAILED" << std::endl;
    return {nullptr, 0};
  }

  // calc mac
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size,
       encryptedframe.get() + (d_backupfileversion >= 1 ? 0 : sizeof(int32_t)),
       length + (d_backupfileversion >= 1 ? sizeof(int32_t) : 0),
       hash, &digest_size);
  std::memcpy(encryptedframe.get() + sizeof(int32_t) + length, hash, 10);

  //std::cout << "                                   : " << bepaald::bytesToHexString(hash, digest_size) << std::endl;

  return {encryptedframe.release(), sizeof(int32_t) + length + MACSIZE};
}
