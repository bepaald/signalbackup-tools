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

#ifndef STICKERFRAME_H_
#define STICKERFRAME_H_

#include "../framewithattachment/framewithattachment.h"

class StickerFrame : public FrameWithAttachment
{
  enum FIELD
  {
   INVALID = 0,
   ROWID = 1, // uint64
   LENGTH = 2, // uint32
  };

  static Registrar s_registrar;
 public:
  inline explicit StickerFrame(uint64_t count = 0);
  inline StickerFrame(unsigned char *data, size_t length, uint64_t count = 0);
  inline StickerFrame(StickerFrame &&other) = default;
  inline StickerFrame &operator=(StickerFrame &&other) = default;
  inline StickerFrame(StickerFrame const &other) = default;
  inline StickerFrame &operator=(StickerFrame const &other) = default;
  inline virtual ~StickerFrame() = default;
  inline virtual FRAMETYPE frameType() const override;
  inline static BackupFrame *create(unsigned char *data, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual uint32_t attachmentSize() const override;
  inline uint32_t length() const;
  inline uint64_t rowId() const;
  inline virtual std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string const &str) const;
 private:
  inline uint64_t dataSize() const;
};

inline StickerFrame::StickerFrame(uint64_t count)
  :
  FrameWithAttachment(count)
{}

inline StickerFrame::StickerFrame(unsigned char *data, size_t length, uint64_t count)
  :
  FrameWithAttachment(data, length, count)
{}

inline FRAMETYPE StickerFrame::frameType() const // virtual override
{
  return FRAMETYPE::STICKER;
}

inline BackupFrame *StickerFrame::create(unsigned char *data, size_t length, uint64_t count) // static
{
  return new StickerFrame(data, length, count);
}

inline void StickerFrame::printInfo() const // virtual override
{
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Type: STICKER" << std::endl;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::ROWID)
      std::cout << "         - row id          : " << bytesToUint64(std::get<1>(p), std::get<2>(p)) << " (" << std::get<2>(p) << " bytes)" << std::endl;
    else if (std::get<0>(p) == FIELD::LENGTH)
      std::cout << "         - length          : " << bytesToUint32(std::get<1>(p), std::get<2>(p)) << " (" << std::get<2>(p) << " bytes)" << std::endl;
  }
}

inline uint32_t StickerFrame::length() const
{
  if (!d_attachmentdata_size)
    for (auto const &p : d_framedata)
      if (std::get<0>(p) == FIELD::LENGTH)
        return bytesToUint32(std::get<1>(p), std::get<2>(p));
  return d_attachmentdata_size;
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

inline bool StickerFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::ROWID &&
        std::get<0>(p) != FIELD::LENGTH)
      return false;
  }
  return true;
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

inline unsigned int StickerFrame::getField(std::string const &str) const
{
  if (str == "ROWID")
    return FIELD::ROWID;
  if (str == "LENGTH")
    return FIELD::LENGTH;
  return FIELD::INVALID;
}

#endif
