/*
    Copyright (C) 2019  Selwin van Dijk

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

std::pair<unsigned char *, uint64_t> FileEncryptor::encryptAttachment(unsigned char *data, uint64_t length)
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
  hmac.Update(d_iv, d_iv_size);
  hmac.Update(encryptedframe, length);
  hmac.Final(ourMac);
  //std::cout << "OUR MAC: " << bepaald::bytesToHexString(ourMac, MACSIZE) << std::endl;

  std::memcpy(encryptedframe + length, ourMac, MACSIZE);

  return {encryptedframe, length + MACSIZE};
}
