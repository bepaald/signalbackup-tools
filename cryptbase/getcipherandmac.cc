/*
  Copyright (C) 2019-2026  Selwin van Dijk

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
#include "../common_crypto.h"

bool CryptBase::getCipherAndMac(unsigned int hashoutputsize, size_t outputsize)
{
  unsigned int const info_size = 13;
  unsigned char info[info_size] = {'B','a','c','k','u','p',' ','E','x','p','o','r','t'};

  auto [derived, size] = bepaald::hkdf_sha256(d_backupkey, d_backupkey_size, info, info_size, outputsize);
  if (!derived || size == 0) [[unlikely]]
    return false;

  d_cipherkey_size = hashoutputsize;
  d_cipherkey = new unsigned char[d_cipherkey_size];
  std::memcpy(d_cipherkey, derived.get(), hashoutputsize);

  d_mackey_size = hashoutputsize;
  d_mackey = new unsigned char[d_mackey_size];
  std::memcpy(d_mackey, derived.get() + hashoutputsize, hashoutputsize);

  return true;
}
