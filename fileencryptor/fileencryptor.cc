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

FileEncryptor::FileEncryptor(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size, uint32_t backupfileversion)
  :
  d_passphrase(passphrase),
  d_backupfileversion(backupfileversion)
{
  d_ok = init(salt, salt_size, iv, iv_size);
}

FileEncryptor::FileEncryptor(std::string const &passphrase, uint32_t backupfileversion)
  :
  d_passphrase(passphrase),
  d_backupfileversion(backupfileversion)
{}

FileEncryptor::FileEncryptor()
{}

bool FileEncryptor::init(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size, uint32_t backupfileversion)
{
  d_passphrase = passphrase;
  d_backupfileversion = backupfileversion;
  return init(salt, salt_size, iv, iv_size);
}
