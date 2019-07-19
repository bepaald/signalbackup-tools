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

#include "cryptbase.ih"

bool CryptBase::getCipherAndMac(uint hashoutputsize, uint outputsize)
{
  unsigned char *localsalt = new unsigned char[hashoutputsize]();
  unsigned char *derived = new unsigned char[outputsize]();
  unsigned int const info_size = 13;
  unsigned char info[info_size] = {'B','a','c','k','u','p',' ','E','x','p','o','r','t'};

  CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
  hkdf.DeriveKey(derived, outputsize, d_backupkey, d_backupkey_size, localsalt, hashoutputsize, info, info_size);

  delete[] localsalt;

  d_cipherkey_size = hashoutputsize;
  d_cipherkey = new unsigned char[d_cipherkey_size];
  std::memcpy(d_cipherkey, derived, hashoutputsize);

  d_mackey_size = hashoutputsize;
  d_mackey = new unsigned char[d_mackey_size];
  std::memcpy(d_mackey, derived + hashoutputsize, hashoutputsize);

  delete[] derived;
  return true;
}
