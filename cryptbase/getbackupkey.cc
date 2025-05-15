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

#include "cryptbase.ih"

bool CryptBase::getBackupKey(std::string const &passphrase)
{
  // convert passwords digits to unsigned char *
  size_t constexpr passlength = 30;
  unsigned char pass[passlength];
  unsigned int i = 0;
  unsigned int j = 0;
  while (i < passlength && j < passphrase.size())
  {
    if (!std::isdigit(passphrase[j])) // skip non digits
      ++j;
    else
      pass[i++] = passphrase[j++];
  }

  while (j < passphrase.size() && !std::isdigit(passphrase[j])) // also eat any trailing non-digits
    ++j;

  if (i != passlength)
  {
    Logger::error("Failed to parse passphrase from string '", passphrase, "' : passphrase too short! "
                  "Need ", passlength, " digits, ", i, " provided");
    return false;
  }

  if (j != passphrase.size()) // passlength == 30 && all chars in passphrase were processed
  {
    Logger::error("Failed to parse passphrase from string '", passphrase, "' : passphrase too long! "
                  "Need ", passlength, " digits, ", passphrase.size(), " provided");
    return false;
  }

  //std::cout << "Passphrase: " << bepaald::bytesToHexString(pass, passlength) << std::endl;

  std::unique_ptr<EVP_MD_CTX, decltype(&::EVP_MD_CTX_free)> mdctx(EVP_MD_CTX_create(), &::EVP_MD_CTX_free);

  if (mdctx.get() == nullptr ||
      EVP_DigestInit_ex(mdctx.get(), EVP_sha512(), nullptr) != 1)
  {
    Logger::error("Failed to create message digest context");
    return false;
  }

  EVP_DigestUpdate(mdctx.get(), d_salt, d_salt_size);

  unsigned long digest_size = EVP_MD_size(EVP_sha512());
  std::unique_ptr<unsigned char[]> digest(new unsigned char[digest_size]);

  for (i = 0; i < 250000; ++i)
  {
    if (i > 0)
      EVP_DigestUpdate(mdctx.get(), digest.get(), digest_size);
    else
      EVP_DigestUpdate(mdctx.get(), pass, passlength);
    EVP_DigestUpdate(mdctx.get(), pass, passlength);

    EVP_DigestFinal(mdctx.get(), digest.get(), nullptr);
    if (EVP_MD_CTX_reset(mdctx.get()) != 1 ||
        EVP_DigestInit_ex(mdctx.get(), EVP_sha512(), nullptr) != 1)
    {
      Logger::error("Failed to reset digest context");
      return false;
    }
  }

  d_backupkey_size = 32; // backupkey is digest trimmed to 32 bytes
  d_backupkey = new unsigned char[d_backupkey_size];
  std::memcpy(d_backupkey, digest.get(), d_backupkey_size);

  return true;
}
