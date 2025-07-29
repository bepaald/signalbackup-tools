/*
  Copyright (C) 2025  Selwin van Dijk

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

#ifndef ADBBACKUPATTACHMENTREADER_H_
#define ADBBACKUPATTACHMENTREADER_H_

#include "../baseattachmentreader/baseattachmentreader.h"
#include "../common_filesystem.h"

#include <memory>
#include <cstring>

class FrameWithAttachment;

class AdbBackupAttachmentReader : public BaseAttachmentReader
{
  std::string d_path;
  std::unique_ptr<unsigned char[]> d_mackey;
  int d_mackey_length;
  std::unique_ptr<unsigned char[]> d_encryptionkey;
  int d_encryptionkey_length;
 public:
  inline AdbBackupAttachmentReader(std::string const &path,
                                   unsigned char *mackey, int mackey_lengt,
                                   unsigned char *encryptionkey, int encryptionkey_length);
  inline AdbBackupAttachmentReader(AdbBackupAttachmentReader const &other);
  inline AdbBackupAttachmentReader(AdbBackupAttachmentReader &&other) = default;
  inline AdbBackupAttachmentReader &operator=(AdbBackupAttachmentReader const &other);
  inline AdbBackupAttachmentReader &operator=(AdbBackupAttachmentReader &&other) = default;
  inline virtual ~AdbBackupAttachmentReader() override = default;
  inline virtual ReturnCode getAttachment(FrameWithAttachment *frame, bool verbose) override;
};

inline AdbBackupAttachmentReader::AdbBackupAttachmentReader(std::string const &path,
                                                            unsigned char *mackey, int mackey_length,
                                                            unsigned char *encryptionkey, int encryptionkey_length)
  :
  d_path(path),
  d_mackey_length(mackey_length),
  d_encryptionkey_length(encryptionkey_length)
{
  if (d_mackey_length == 0 ||
      d_encryptionkey_length == 0) [[unlikely]]
    return;

  d_mackey.reset(new unsigned char[d_mackey_length]);
  std::memcpy(d_mackey.get(), mackey, d_mackey_length);

  d_encryptionkey.reset(new unsigned char[d_encryptionkey_length]);
  std::memcpy(d_encryptionkey.get(), encryptionkey, d_encryptionkey_length);
}

inline AdbBackupAttachmentReader::AdbBackupAttachmentReader(AdbBackupAttachmentReader const &other)
  :
  d_path(other.d_path),
  d_mackey_length(other.d_mackey_length),
  d_encryptionkey_length(other.d_encryptionkey_length)
{
  if (d_mackey_length == 0 ||
      d_encryptionkey_length == 0) [[unlikely]]
  {
    d_mackey.reset();
    d_encryptionkey.reset();
    return;
  }

  d_mackey.reset(new unsigned char[d_mackey_length]);
  std::memcpy(d_mackey.get(), other.d_mackey.get(), d_mackey_length);

  d_encryptionkey.reset(new unsigned char[d_encryptionkey_length]);
  std::memcpy(d_encryptionkey.get(), other.d_encryptionkey.get(), d_encryptionkey_length);
}

inline AdbBackupAttachmentReader &AdbBackupAttachmentReader::operator=(AdbBackupAttachmentReader const &other)
{
  if (this != &other)
  {
    d_path = other.d_path;
    d_mackey_length = other.d_mackey_length;
    d_encryptionkey_length = other.d_encryptionkey_length;
    if (d_mackey_length == 0 ||
        d_encryptionkey_length == 0) [[unlikely]]
    {
      d_mackey.reset();
      d_encryptionkey.reset();
    }
    else
    {
      d_mackey.reset(new unsigned char[d_mackey_length]);
      std::memcpy(d_mackey.get(), other.d_mackey.get(), d_mackey_length);

      d_encryptionkey.reset(new unsigned char[d_encryptionkey_length]);
      std::memcpy(d_encryptionkey.get(), other.d_encryptionkey.get(), d_encryptionkey_length);
    }
  }
  return *this;
}

#endif
