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

class AdbBackupAttachmentReader : public AttachmentReader<AdbBackupAttachmentReader>
{
  std::string d_path;
  std::unique_ptr<unsigned char[]> d_mackey;
  int d_mackey_length;
  std::unique_ptr<unsigned char[]> d_encryptionkey;
  int d_encryptionkey_length;
  int64_t d_size;
 public:
  inline AdbBackupAttachmentReader(std::string const &path,
                                   unsigned char const *mackey, int mackey_length,
                                   unsigned char const *encryptionkey, int encryptionkey_length);
  inline AdbBackupAttachmentReader(AdbBackupAttachmentReader const &other);
  inline AdbBackupAttachmentReader(AdbBackupAttachmentReader &&other) = default;
  inline AdbBackupAttachmentReader &operator=(AdbBackupAttachmentReader const &other);
  inline AdbBackupAttachmentReader &operator=(AdbBackupAttachmentReader &&other) = default;
  inline virtual ~AdbBackupAttachmentReader() override = default;
  virtual ReturnCode getAttachment(FrameWithAttachment *frame, bool verbose) override;
  ReturnCode getAttachmentData(unsigned char **data, bool verbose);
  inline int64_t size() const;
};

inline AdbBackupAttachmentReader::AdbBackupAttachmentReader(std::string const &path,
                                                            unsigned char const *mackey, int mackey_length,
                                                            unsigned char const *encryptionkey, int encryptionkey_length)
  :
  d_path(path),
  d_mackey_length(mackey_length),
  d_encryptionkey_length(encryptionkey_length),
  d_size(-1)
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
  d_encryptionkey_length(other.d_encryptionkey_length),
  d_size(other.d_size)
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
    d_size = other.d_size;
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

inline int64_t AdbBackupAttachmentReader::size() const
{
  return d_size;
}

#endif
