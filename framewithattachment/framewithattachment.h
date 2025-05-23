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

#ifndef FRAMEWITHATTACHMENT_H_
#define FRAMEWITHATTACHMENT_H_

#include <cstring>
#include <memory>
#include <fstream>

#include "../common_be.h"
#include "../backupframe/backupframe.h"
#include "../baseattachmentreader/baseattachmentreader.h"

class FrameWithAttachment : public BackupFrame
{
 protected:
  unsigned char *d_attachmentdata;
  BaseAttachmentReader *d_attachmentreader;
  uint32_t d_attachmentdata_size;
  bool d_noclear;

 public:
  inline explicit FrameWithAttachment(uint64_t count = 0);
  inline FrameWithAttachment(unsigned char const *bytes, size_t length, uint64_t count = 0);
  inline FrameWithAttachment(FrameWithAttachment &&other) noexcept;
  inline FrameWithAttachment &operator=(FrameWithAttachment &&other) noexcept;
  inline FrameWithAttachment(FrameWithAttachment const &other);
  inline FrameWithAttachment &operator=(FrameWithAttachment const &other);
  inline virtual ~FrameWithAttachment() override;
  bool setAttachmentDataBacked(unsigned char *data, long long int length) override;
  bool setAttachmentDataUnbacked(unsigned char const *data, long long int length);
  template <int lengthfield>
  inline uint32_t length() const;
  //inline void setLength(int32_t l);
  inline void setReader(BaseAttachmentReader *reader);
  inline BaseAttachmentReader *reader() const;
  inline virtual unsigned char *attachmentData(bool verbose, bool *badmac = nullptr);
  inline void clearData();
};

inline FrameWithAttachment::FrameWithAttachment(uint64_t count)
  :
  BackupFrame(count),
  d_attachmentdata(nullptr),
  d_attachmentreader(nullptr),
  d_attachmentdata_size(0),
  d_noclear(false)
{}

inline FrameWithAttachment::FrameWithAttachment(unsigned char const *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count),
  d_attachmentdata(nullptr),
  d_attachmentreader(nullptr),
  d_attachmentdata_size(0),
  d_noclear(false)
{}

inline FrameWithAttachment::FrameWithAttachment(FrameWithAttachment &&other) noexcept
  :
  BackupFrame(std::move(other)),
  d_attachmentdata(other.d_attachmentdata),
  d_attachmentreader(other.d_attachmentreader),
  d_attachmentdata_size(other.d_attachmentdata_size),
  d_noclear(other.d_noclear)
{
  other.d_attachmentdata = nullptr;
  other.d_attachmentreader = nullptr;
  other.d_attachmentdata_size = 0;
}

inline FrameWithAttachment &FrameWithAttachment::operator=(FrameWithAttachment &&other) noexcept
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    if (d_attachmentreader)
      delete d_attachmentreader;

    BackupFrame::operator=(std::move(other));
    d_attachmentdata = other.d_attachmentdata;
    d_attachmentdata_size = other.d_attachmentdata_size;
    d_attachmentreader = other.d_attachmentreader;
    d_noclear = other.d_noclear;

    other.d_attachmentdata = nullptr;
    other.d_attachmentreader = nullptr;
    other.d_attachmentdata_size = 0;
  }
  return *this;
}

inline FrameWithAttachment::FrameWithAttachment(FrameWithAttachment const &other)
  :
  BackupFrame(other),
  d_attachmentdata(nullptr),
  d_attachmentreader(nullptr),
  d_attachmentdata_size(other.d_attachmentdata_size),
  d_noclear(other.d_noclear)
{
  if (other.d_attachmentdata)
  {
    d_attachmentdata = new unsigned char[d_attachmentdata_size];
    std::memcpy(d_attachmentdata, other.d_attachmentdata, d_attachmentdata_size);
  }

  if (other.d_attachmentreader)
    d_attachmentreader = other.d_attachmentreader->clone();

}

inline FrameWithAttachment &FrameWithAttachment::operator=(FrameWithAttachment const &other)
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    if (d_attachmentreader)
      delete d_attachmentreader;

    BackupFrame::operator=(other);
    d_attachmentdata_size = other.d_attachmentdata_size;
    d_noclear = other.d_noclear;

    if (other.d_attachmentreader)
      d_attachmentreader = other.d_attachmentreader->clone();

    if (other.d_attachmentdata)
    {
      d_attachmentdata = new unsigned char[d_attachmentdata_size];
      std::memcpy(d_attachmentdata, other.d_attachmentdata, d_attachmentdata_size);
    }
  }
  return *this;
}

inline FrameWithAttachment::~FrameWithAttachment()
{
  bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
  if (d_attachmentreader)
    delete d_attachmentreader;
}

template <int lengthfield>
inline uint32_t FrameWithAttachment::length() const
{
  if (!d_attachmentdata_size)
    for (auto const &p : d_framedata)
      if (std::get<0>(p) == lengthfield)
        return bytesToUint32(std::get<1>(p), std::get<2>(p));
  return d_attachmentdata_size;
}

inline void FrameWithAttachment::setReader(BaseAttachmentReader *reader)
{
  d_attachmentreader = reader;
}

inline BaseAttachmentReader *FrameWithAttachment::reader() const
{
  return d_attachmentreader;
}

inline unsigned char *FrameWithAttachment::attachmentData(bool verbose, bool *badmac)
{
  if (!d_attachmentdata)
  {
    if (d_attachmentreader)
    {
      BaseAttachmentReader::ReturnCode result = d_attachmentreader->getAttachment(this, verbose);
      if (result == BaseAttachmentReader::ReturnCode::BADMAC) [[unlikely]]
      {
        if (badmac)
          *badmac = true;
        return nullptr;
      }
      if (result == BaseAttachmentReader::ReturnCode::ERROR) [[unlikely]]
        return nullptr;
    }
    else [[unlikely]]
    {
      Logger::error("Asked for attachment data, but no reader was set");
      return nullptr;
    }
  }
  return d_attachmentdata;
}

inline void FrameWithAttachment::clearData()
{
  if (d_noclear) [[unlikely]]
  {
    //if (d_verbose) [[unlikely]]
    // maybe remove this warning
    Logger::message("Ignoring request to clear attachmentdata as it is not backed by file");
    return;
  }

  if (d_attachmentdata) // do not use bepaald::destroyPtr, it will set size to zero
  {
    delete[] d_attachmentdata;
    d_attachmentdata = nullptr;
  }

  // to allow the attachmentreader to do its own cleanup
  d_attachmentreader->clearData();
}

#endif
