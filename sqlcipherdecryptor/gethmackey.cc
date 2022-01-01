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

bool SqlCipherDecryptor::getHmacKey()
{
  // initialize hmac salt to salt
  unsigned int hmac_saltsize = d_saltsize;
  std::unique_ptr<unsigned char[]> hmac_salt(new unsigned char[hmac_saltsize]);
  std::memcpy(hmac_salt.get(), d_salt, d_saltsize);

  // then switch it up by xoring with mask
  for (uint i = 0; i < hmac_saltsize; ++i)
    hmac_salt[i] ^= s_saltmask;

  d_hmackeysize = 32;
  d_hmackey = new unsigned char[d_hmackeysize];

#ifndef USE_CRYPTOPP
  return PKCS5_PBKDF2_HMAC(reinterpret_cast<char *>(d_key), d_keysize, hmac_salt.get(), hmac_saltsize, 2, d_digest, d_hmackeysize, d_hmackey) == 1;
#else
  d_pbkdf->DeriveKey(d_hmackey, d_hmackeysize, d_key, d_keysize,
                     CryptoPP::MakeParameters(CryptoPP::Name::Salt(), CryptoPP::ConstByteArrayParameter(reinterpret_cast<CryptoPP::byte const *>(hmac_salt.get()), hmac_saltsize))("Iterations", 2));
#endif

  return true;
}
