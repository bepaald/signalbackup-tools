/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

#ifndef FILEENCRYPTOR_H_
#define FILEENCRYPTOR_H_

#include <cstring>
#include <utility>

#include "../cryptbase/cryptbase.h"
#include "../common_be.h"

class FileEncryptor : public CryptBase
{
  std::string d_passphrase;
 public:
  FileEncryptor(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size);
  explicit FileEncryptor(std::string const &passphrase);
  FileEncryptor();
  FileEncryptor(FileEncryptor const &other) = delete;
  FileEncryptor operator=(FileEncryptor const &other) = delete;
  bool init(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size);
  bool init(unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size);
  std::pair<unsigned char *, uint64_t> encryptFrame(std::pair<unsigned char *, uint64_t> const &data);
  std::pair<unsigned char *, uint64_t> encryptFrame(unsigned char *data, uint64_t length);
  std::pair<unsigned char *, uint64_t> encryptAttachment(unsigned char *data, uint64_t length);
 private:
  //bool getCipherAndMac(uint hashoutputsize, uint outputsize); // MOVE TO BASE
  //bool getBackupKey(std::string const &passphrase);           // MOVE TO BASE
};

#endif
