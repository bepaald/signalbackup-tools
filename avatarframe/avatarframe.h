/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

#ifndef AVATARFRAME_H_
#define AVATARFRAME_H_

#include <cstring>

#include "../framewithattachment/framewithattachment.h"

class AvatarFrame : public FrameWithAttachment
{
  enum FIELD
  {
   INVALID = 0,
   NAME = 1, // string
   LENGTH = 2, // uint32
   RECIPIENT = 3, //string
  };

  static Registrar s_registrar;
 public:
  inline explicit AvatarFrame(uint64_t count = 0);
  inline AvatarFrame(unsigned char *bytes, size_t length, uint64_t count = 0);
  // inline AvatarFrame(AvatarFrame &&other);
  // inline AvatarFrame &operator=(AvatarFrame &&other);
  // inline AvatarFrame(AvatarFrame const &other) = delete;
  // inline AvatarFrame &operator=(AvatarFrame const &other) = delete;
  inline virtual ~AvatarFrame() = default;
  inline static BackupFrame *create(unsigned char *bytes, size_t length, uint64_t count);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline uint32_t length() const;
  inline virtual uint32_t attachmentSize() const override;
  inline std::string name() const;
  inline std::string recipient() const;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string const &str) const;
 private:
  inline uint64_t dataSize() const;
};

inline AvatarFrame::AvatarFrame(uint64_t count)
  :
  FrameWithAttachment(count)
{}

inline AvatarFrame::AvatarFrame(unsigned char *bytes, size_t length, uint64_t count)
  :
  FrameWithAttachment(bytes, length, count)
{}


inline BackupFrame *AvatarFrame::create(unsigned char *bytes, size_t length, uint64_t count) // static
{
  return new AvatarFrame(bytes, length, count);
}

inline void AvatarFrame::printInfo() const // virtual override
{
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Size: " << d_constructedsize << std::endl;
  std::cout << "        Type: AVATAR" << std::endl;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::NAME)
      std::cout << "         - name      : " << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << " (" << std::get<2>(p) << " bytes)" << std::endl;
    else if (std::get<0>(p) == FIELD::RECIPIENT)
      std::cout << "         - recipient : " << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << " (" << std::get<2>(p) << " bytes)" << std::endl;
    else if (std::get<0>(p) == FIELD::LENGTH)
      std::cout << "         - length    : " << bytesToUint32(std::get<1>(p), std::get<2>(p)) << " (" << std::get<2>(p) << " bytes)" << std::endl;
  }
  if (d_attachmentdata)
  {
    uint32_t size = length();
    if (size < 25)
      std::cout << "         - attachment: " << bepaald::bytesToHexString(d_attachmentdata, size) << std::endl;
    else
      std::cout << "         - attachment: " << bepaald::bytesToHexString(d_attachmentdata, 25) << " ... (" << size << " bytes total)" << std::endl;
  }
}

inline BackupFrame::FRAMETYPE AvatarFrame::frameType() const // virtual override
{
  return FRAMETYPE::AVATAR;
}

inline uint32_t AvatarFrame::length() const
{
  if (!d_attachmentdata_size)
    for (auto const &p : d_framedata)
      if (std::get<0>(p) == FIELD::LENGTH)
        return  bytesToUint32(std::get<1>(p), std::get<2>(p));
  return d_attachmentdata_size;
}

inline uint32_t AvatarFrame::attachmentSize() const // virtual override
{
  return length();
}

inline std::string AvatarFrame::name() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::NAME)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
  return std::string();
}

inline std::string AvatarFrame::recipient() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::RECIPIENT)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
  return std::string();
}

inline uint64_t AvatarFrame::dataSize() const
{
  uint64_t size = 0;

  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
    case FIELD::NAME:
    case FIELD::RECIPIENT:
      {
        uint64_t stringsize = std::get<2>(fd);
        size += varIntSize(stringsize);
        size += stringsize + 1; // +1 for fieldtype + wiretype
        break;
      }
    case FIELD::LENGTH:
      {
        uint64_t value = bytesToUint64(std::get<1>(fd), std::get<2>(fd));
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

inline std::pair<unsigned char *, uint64_t> AvatarFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::AVATAR, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
    case FIELD::NAME:
      datapos += putLengthDelimType(fd, data + datapos);
      break;
    case FIELD::LENGTH:
      datapos += putVarIntType(fd, data + datapos);
      break;
    case FIELD::RECIPIENT:
      datapos += putLengthDelimType(fd, data + datapos);
      break;
    }
  }
  return {data, size};
}

inline bool AvatarFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  int foundlength = 0;
  int foundname_or_recipient = 0;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::NAME &&
        std::get<0>(p) != FIELD::RECIPIENT &&
        std::get<0>(p) != FIELD::LENGTH)
      return false;

    // a valid avatar frame must contain length AND (recipient (newer backups) XOR name (older backups))
    if (std::get<0>(p) == FIELD::LENGTH)
      ++foundlength;
    else if (std::get<0>(p) == FIELD::RECIPIENT || std::get<0>(p) != FIELD::NAME)
      ++foundname_or_recipient;
  }

  return foundlength == 1 && foundname_or_recipient == 1;
}

inline std::string AvatarFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::NAME)
      data += "NAME:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::RECIPIENT)
      data += "RECIPIENT:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::LENGTH)
      data += "LENGTH:uint32:" + bepaald::toString(bytesToUint32(std::get<1>(p), std::get<2>(p))) + "\n";
  }
  return data;
}

inline unsigned int AvatarFrame::getField(std::string const &str) const
{
  if (str == "RECIPIENT")
    return FIELD::RECIPIENT;
  if (str == "NAME")
    return FIELD::NAME;
  if (str == "LENGTH")
    return FIELD::LENGTH;
  return FIELD::INVALID;
}

#endif
