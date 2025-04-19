/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef DESKTOPATTACHMENTREADER_H_
#define DESKTOPATTACHMENTREADER_H_

#include "../baseattachmentreader/baseattachmentreader.h"
#include "../framewithattachment/framewithattachment.h"
#include "../base64/base64.h"
#include "../rawfileattachmentreader/rawfileattachmentreader.h"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

/*
  version < 2 (or unset):
    attachment data is just in the file
  version 2:
    File is encrypted:
      [=16 bytes iv=][=AES-256-CBC encrypted file, padded with zeros to some binned size, padded with AES-CBC padding to next nearest multiple of 16=][=32 bytes HMAC=]
    the passed in key ('localKey') is base64 encoded concatenation of 32 bytes AES key + 32 bytes MAC key
*/

class DesktopAttachmentReader : public AttachmentReader<DesktopAttachmentReader>
{
  std::string d_path;
  std::string d_key;
  uint64_t d_size;
  int d_version;
 public:
  inline explicit DesktopAttachmentReader(std::string const &path);
  inline DesktopAttachmentReader(int version, std::string const &path, std::string const &key, uint64_t size);
  inline DesktopAttachmentReader(DesktopAttachmentReader const &other) = default;
  inline DesktopAttachmentReader(DesktopAttachmentReader &&other) = default;
  inline DesktopAttachmentReader &operator=(DesktopAttachmentReader const &other) = default;
  inline DesktopAttachmentReader &operator=(DesktopAttachmentReader &&other) = default;
  inline virtual ~DesktopAttachmentReader() override = default;
  inline virtual ReturnCode getAttachment(FrameWithAttachment *frame, bool verbose) override;
  ReturnCode getAttachmentData(unsigned char **data, bool verbose);
  //decryptdata
 private:
  ReturnCode getEncryptedAttachment(FrameWithAttachment *frame, bool verbose);
  inline ReturnCode getRawAttachment(FrameWithAttachment *frame, bool verbose);
};

inline DesktopAttachmentReader::DesktopAttachmentReader(std::string const &path)
  :
  DesktopAttachmentReader(1, path, std::string(), 0)
{}

inline DesktopAttachmentReader::DesktopAttachmentReader(int version, std::string const &path, std::string const &key, uint64_t size)
  :
  d_path(path),
  d_key(key),
  d_size(size),
  d_version(version)
{}

inline BaseAttachmentReader::ReturnCode DesktopAttachmentReader::getAttachment(FrameWithAttachment *frame, bool verbose)
{
  if (d_version >= 2) [[likely]]
    return getEncryptedAttachment(frame, verbose);
  return getRawAttachment(frame, verbose);
}

inline BaseAttachmentReader::ReturnCode DesktopAttachmentReader::getRawAttachment(FrameWithAttachment *frame, bool verbose)
{
  if (verbose) [[unlikely]]
    Logger::message("Starting get raw DesktopAttachment data");
  RawFileAttachmentReader raw(d_path);
  return raw.getAttachment(frame, verbose);
}

#endif
