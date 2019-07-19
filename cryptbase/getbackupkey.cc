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

bool CryptBase::getBackupKey(std::string const &passphrase)
{
  // convert passwords digits to unsigned char *
  size_t const passlength = 30;
  unsigned char pass[passlength];
  uint i = 0;
  uint j = 0;
  for (i = 0, j = 0; i < passlength && j < passphrase.size(); ++i, ++j)
  {
    while (!std::isdigit(passphrase[j]))
      ++j;
    pass[i] = passphrase[j];
  }

  while (j < passphrase.size() && !std::isdigit(passphrase[j])) // also eat any trailing non-digits
    ++j;

  if (i != passlength || j != passphrase.size()) // passlength == 30 && all chars in passphrase were processed
    return false;

  //DEBUGOUT("Passphrase: ", bepaald::bytesToHexString(pass, passlength));

  CryptoPP::SHA512 hash;

  CryptoPP::byte digest[CryptoPP::SHA512::DIGESTSIZE];
  std::memcpy(digest, pass, passlength);

  hash.Update(d_salt, d_salt_size);

  for (i = 0; i < 250000; ++i)
  {
    hash.Update(digest, i > 0 ? static_cast<uint>(CryptoPP::SHA512::DIGESTSIZE) : passlength); // update with digest, first time
    hash.CalculateDigest(digest, pass, passlength);                                            // it contains passphrase
  }

  d_backupkey_size = 32; // backupkey is digest trimmed to 32 bytes
  d_backupkey = new unsigned char[d_backupkey_size];
  std::memcpy(d_backupkey, digest, d_backupkey_size);

  return true;
}
