/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

#ifdef USE_OPENSSL

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

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[length + MACSIZE]);
  int l = static_cast<int>(length);
  if (EVP_EncryptUpdate(ctx.get(), encryptedframe.get(), &l, data, length) != 1)
  {
    std::cout << "ENCRYPT FAILED" << std::endl;
    return {nullptr, 0};
  }

  // calc mac
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), length, hash, &digest_size);
  std::memcpy(encryptedframe.get() + length, hash, 10);

  return {encryptedframe.release(), length + MACSIZE};
}

#else

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptFrame(unsigned char *data, uint64_t length)
{
  if (!d_ok)
    return {nullptr, 0};

  unsigned char *encryptedframe = new unsigned char[length + MACSIZE];

  // update iv:
  uintToFourBytes(d_iv, d_counter++);

  CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption e(d_cipherkey, d_cipherkey_size, d_iv);
  e.ProcessData(encryptedframe, data, length);
  //std::cout << "Encrypted frame: " << bepaald::bytesToHexString(encryptedframe, length) << std::endl;

  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];
  CryptoPP::HMAC<CryptoPP::SHA256> hmac(d_mackey, d_mackey_size);
  hmac.Update(encryptedframe, length);
  hmac.Final(ourMac);
  //std::cout << "OUR MAC: " << bepaald::bytesToHexString(ourMac, MACSIZE) << std::endl;

  std::memcpy(encryptedframe + length, ourMac, MACSIZE);

  return {encryptedframe, length + MACSIZE};
}

#endif
