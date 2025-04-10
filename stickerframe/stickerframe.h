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

#ifndef STICKERFRAME_H_
#define STICKERFRAME_H_

#include <optional>
#include <string_view>

#include "../framewithattachment/framewithattachment.h"
#include "../attachmentmetadata/attachmentmetadata.h"

#include "../common_be.h"
#include "../common_bytes.h"

class StickerFrame : public FrameWithAttachment
{
  enum FIELD : int
  {
    INVALID = 0,
    ROWID = 1, // uint64
    LENGTH = 2, // uint32
  };

  std::optional<std::string> d_mimetype;
  static Registrar s_registrar;
 public:
  inline explicit StickerFrame(uint64_t count = 0);
  inline StickerFrame(unsigned char const *data, size_t length, uint64_t count = 0);
  inline StickerFrame(StickerFrame &&other) = default;
  inline StickerFrame &operator=(StickerFrame &&other) = default;
  inline StickerFrame(StickerFrame const &other) = default;
  inline StickerFrame &operator=(StickerFrame const &other) = default;
  inline virtual ~StickerFrame() override = default;
  inline virtual StickerFrame *clone() const override;
  inline virtual StickerFrame *move_clone() override;
  inline virtual FRAMETYPE frameType() const override;
  inline static BackupFrame *create(unsigned char const *data, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual uint32_t attachmentSize() const override;
  inline uint32_t length() const;
  inline uint64_t rowId() const;
  inline void setRowId(uint64_t rid);
  inline virtual std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate(uint64_t available) const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string_view const &str) const;
  inline std::optional<std::string> mimetype() const;
  inline unsigned char *attachmentData(bool *badmac = nullptr, bool verbose = false) override;
 private:
  inline uint64_t dataSize() const override;
};

inline StickerFrame::StickerFrame(uint64_t count)
  :
  FrameWithAttachment(count)
{}

inline StickerFrame::StickerFrame(unsigned char const *data, size_t length, uint64_t count)
  :
  FrameWithAttachment(data, length, count)
{}

inline StickerFrame *StickerFrame::clone() const
{
  return new StickerFrame(*this);
}

inline StickerFrame *StickerFrame::move_clone()
{
  return new StickerFrame(std::move(*this));
}

inline BackupFrame::FRAMETYPE StickerFrame::frameType() const // virtual override
{
  return FRAMETYPE::STICKER;
}

inline BackupFrame *StickerFrame::create(unsigned char const *data, size_t length, uint64_t count) // static
{
  return new StickerFrame(data, length, count);
}

inline void StickerFrame::printInfo() const // virtual override
{
  Logger::message("Frame number: ", d_count);
  Logger::message("        Size: ", d_constructedsize);
  Logger::message("        Type: STICKER");
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::ROWID)
      Logger::message("         - row id          : ", bytesToUint64(std::get<1>(p), std::get<2>(p)), " (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::LENGTH)
      Logger::message("         - length          : ", bytesToUint32(std::get<1>(p), std::get<2>(p)), " (", std::get<2>(p), " bytes)");
  }
}

inline uint32_t StickerFrame::length() const
{
  return FrameWithAttachment::length<FIELD::LENGTH>();
}

inline uint32_t StickerFrame::attachmentSize() const // virtual override
{
  return length();
}

inline uint64_t StickerFrame::rowId() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::ROWID)
      return bytesToUint64(std::get<1>(p), std::get<2>(p));
  return 0;
}

inline void StickerFrame::setRowId(uint64_t rid)
{
  for (auto &p : d_framedata)
    if (std::get<0>(p) == FIELD::ROWID)
    {
      if (sizeof(rid) != std::get<2>(p)) [[unlikely]]
      {
        //std::cout << "       ************        DAMN!        **********        " << std::endl;
        return;
      }
      uint64_t val = bepaald::swap_endian(rid);
      std::memcpy(std::get<1>(p), reinterpret_cast<unsigned char *>(&val), sizeof(val));
      return;
    }
}

inline uint64_t StickerFrame::dataSize() const
{
  uint64_t size = 0;

  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::ROWID:
      {
        uint64_t value = bytesToUint64(std::get<1>(fd), std::get<2>(fd));
        size += varIntSize(value);
        size += 1; // +1 for fieldtype + wiretype
        break;
      }
      case FIELD::LENGTH:
      {
        uint32_t value = bytesToUint32(std::get<1>(fd), std::get<2>(fd));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
        break;
      }
    }
  }

  // for size of this entire frame.
  size += varIntSize(size);
  return ++size;  // for frametype and wiretype
}

inline std::pair<unsigned char *, uint64_t> StickerFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::STICKER, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::ROWID:
        datapos += putVarIntType(fd, data + datapos);
        break;
      case FIELD::LENGTH:
        datapos += putVarIntType(fd, data + datapos);
        break;
    }
  }
  return {data, size};
}

inline bool StickerFrame::validate(uint64_t available) const
{
  if (d_framedata.empty())
    return false;

  int foundrowid = 0;
  int rowid_fieldsize = 0;
  int foundlength = 0;
  int length_fieldsize = 0;
  unsigned int len = 0;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::ROWID &&
        std::get<0>(p) != FIELD::LENGTH)
      return false;

    if (std::get<0>(p) == FIELD::ROWID)
    {
      ++foundrowid;
      rowid_fieldsize += std::get<2>(p);
    }
    else if (std::get<0>(p) == FIELD::LENGTH)
    {
      ++foundlength;
      len += bytesToUint32(std::get<1>(p), std::get<2>(p));
      length_fieldsize += std::get<2>(p);
    }
  }
  return foundlength == 1 && foundrowid == 1 &&
    length_fieldsize <= 8 &&
    rowid_fieldsize <= 8 &&
    len <= available &&
    len < 1 * 1024 * 1024; // If size is more than 1MB, it's not right... From
                              // https://support.signal.org/hc/en-us/articles/360031836512-Stickers :
                              // "Each sticker has a size limit of 300kb"
}

inline std::string StickerFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::ROWID)
      data += "ROWID:uint64:" + bepaald::toString(bytesToUint64(std::get<1>(p), std::get<2>(p))) + "\n";
    else if (std::get<0>(p) == FIELD::LENGTH)
      data += "LENGTH:uint32:" + bepaald::toString(bytesToUint32(std::get<1>(p), std::get<2>(p))) + "\n";
  }
  return data;
}

inline unsigned int StickerFrame::getField(std::string_view const &str) const
{
  if (str == "ROWID")
    return FIELD::ROWID;
  if (str == "LENGTH")
    return FIELD::LENGTH;
  return FIELD::INVALID;
}

inline std::optional<std::string> StickerFrame::mimetype() const
{
  return d_mimetype;
}

inline unsigned char *StickerFrame::attachmentData(bool *badmac, bool verbose)
{
  unsigned char *data = FrameWithAttachment::attachmentData(badmac, verbose);
  if (data && !d_mimetype) // try to get mimetype
  {
    AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(std::string(), data, d_attachmentdata_size, true/*skiphash*/);
    if (!amd.filetype.empty())
      d_mimetype = amd.filetype;
  }
  return data;
}

#endif
