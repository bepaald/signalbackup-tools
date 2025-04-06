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

#ifndef ATTACHMENTFRAME_H_
#define ATTACHMENTFRAME_H_

#include <cstring>
#include <string_view>

#include "../common_bytes.h"
#include "../framewithattachment/framewithattachment.h"

class AttachmentFrame : public FrameWithAttachment
{
  enum FIELD
  {
   INVALID = 0,
   ROWID = 1, // uint64
   ATTACHMENTID = 2, // uint64
   LENGTH = 3 // uint32
  };

  static Registrar s_registrar;
 public:
  inline explicit AttachmentFrame(uint64_t count = 0);
  inline AttachmentFrame(unsigned char const *bytes, size_t length, uint64_t count = 0);
  inline AttachmentFrame(AttachmentFrame &&other) = default;
  inline AttachmentFrame &operator=(AttachmentFrame &&other) = default;
  inline AttachmentFrame(AttachmentFrame const &other) = default;
  inline AttachmentFrame &operator=(AttachmentFrame const &other) = default;
  inline virtual ~AttachmentFrame() override = default;
  inline virtual AttachmentFrame *clone() const override;
  inline virtual AttachmentFrame *move_clone() override;
  inline static BackupFrame *create(unsigned char const *bytes, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline uint32_t length() const;
  inline void setLength(uint32_t l);
  inline virtual uint32_t attachmentSize() const override;
  inline uint64_t rowId() const;
  inline void setRowId(uint64_t rid);
  inline uint64_t attachmentId() const;
  inline void setAttachmentId(uint64_t rid);
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate(uint64_t available) const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string_view const &str) const;
  inline void setLengthField(uint32_t newlength);
 private:
  inline uint64_t dataSize() const override;
};

inline AttachmentFrame::AttachmentFrame(uint64_t count)
  :
  FrameWithAttachment(count)
{}

inline AttachmentFrame::AttachmentFrame(unsigned char const *bytes, size_t length, uint64_t count)
  :
  FrameWithAttachment(bytes, length, count)
{}

inline AttachmentFrame *AttachmentFrame::clone() const
{
  return new AttachmentFrame(*this);
}

inline AttachmentFrame *AttachmentFrame::move_clone()
{
  return new AttachmentFrame(std::move(*this));
}

inline BackupFrame *AttachmentFrame::create(unsigned char const *bytes, size_t length, uint64_t count) // static
{
  return new AttachmentFrame(bytes, length, count);
}

inline void AttachmentFrame::printInfo() const // virtual override
{
  Logger::message("Frame number: ", d_count);
  Logger::message("        Size: ", d_constructedsize);
  Logger::message("        Type: ATTACHMENT");
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::ROWID)
      Logger::message("         - row id          : ", bytesToUint64(std::get<1>(p), std::get<2>(p)), " (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::ATTACHMENTID)
      Logger::message("         - attachment id   : ", bytesToUint64(std::get<1>(p), std::get<2>(p)), " (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::LENGTH)
      Logger::message("         - length          : ", bytesToUint32(std::get<1>(p), std::get<2>(p)), " (", std::get<2>(p), " bytes)");
  }
  if (d_attachmentdata)
  {
    uint32_t size = length();
    if (size < 25)
      Logger::message("         - attachment      : ", bepaald::bytesToHexString(d_attachmentdata, size));
    else
      Logger::message("         - attachment      : ", bepaald::bytesToHexString(d_attachmentdata, 25), " ... (", size, " bytes total)");
  }
}

inline BackupFrame::FRAMETYPE AttachmentFrame::frameType() const // virtual override
{
  return FRAMETYPE::ATTACHMENT;
}

inline uint32_t AttachmentFrame::length() const
{
  return FrameWithAttachment::length<FIELD::LENGTH>();
}

inline void AttachmentFrame::setLength(uint32_t l)
{
  for (auto &p : d_framedata)
    if (std::get<0>(p) == FIELD::LENGTH)
    {
      uint64_t val = bepaald::swap_endian(static_cast<uint64_t>(l));
      std::memcpy(std::get<1>(p), reinterpret_cast<unsigned char *>(&val), sizeof(val));
      d_attachmentdata_size = l;
      return;
    }
}

inline uint32_t AttachmentFrame::attachmentSize() const // virtual override
{
  return length();
}

inline uint64_t AttachmentFrame::rowId() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::ROWID)
      return bytesToUint64(std::get<1>(p), std::get<2>(p));
  return 0;
}

inline void AttachmentFrame::setRowId(uint64_t rid)
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

inline uint64_t AttachmentFrame::attachmentId() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::ATTACHMENTID)
      return bytesToUint64(std::get<1>(p), std::get<2>(p));
  return 0;
}

inline void AttachmentFrame::setAttachmentId(uint64_t aid)
{
  for (auto &p : d_framedata)
    if (std::get<0>(p) == FIELD::ATTACHMENTID)
    {
      if (sizeof(aid) != std::get<2>(p)) [[unlikely]]
      {
        //std::cout << "       ************        DAMN!        **********        " << std::endl;
        return;
      }
      uint64_t val = bepaald::swap_endian(aid);
      std::memcpy(std::get<1>(p), reinterpret_cast<unsigned char *>(&val), sizeof(val));
      return;
    }
}

inline uint64_t AttachmentFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
    uint64_t value = bytesToUint64(std::get<1>(fd), std::get<2>(fd));
    size += varIntSize(value);
    size += 1; // for fieldtype + wiretype
  }
  // for size of this frame.
  size += varIntSize(size);
  return ++size;  // for frametype and wiretype
}

inline std::pair<unsigned char *, uint64_t> AttachmentFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::ATTACHMENT, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  /*
  ROWID = 1, // uint64
    ATTACHMENTID = 2, // uint64
    LENGTH = 3 // uint32
  */

  for (auto const &fd : d_framedata)
    datapos += putVarIntType(fd, data + datapos);

  return {data, size};

}

inline bool AttachmentFrame::validate(uint64_t available) const
{
  if (d_framedata.empty())
    return false;

  int foundrowid = 0;
  int rowid_fieldsize = 0;
  int foundlength = 0;
  int length_fieldsize = 0;
  unsigned int len = 0;
  int foundattachmentid = 0;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::ROWID &&
        std::get<0>(p) != FIELD::ATTACHMENTID &&
        std::get<0>(p) != FIELD::LENGTH)
      return false;

    if (std::get<0>(p) == FIELD::ROWID)
    {
      ++foundrowid;
      rowid_fieldsize += std::get<2>(p);
    }
    else if (std::get<0>(p) == FIELD::ATTACHMENTID)
      ++foundattachmentid; // zero for newer backups...
    else if (std::get<0>(p) == FIELD::LENGTH)
    {
      ++foundlength;
      len += bytesToUint32(std::get<1>(p), std::get<2>(p));
      length_fieldsize += std::get<2>(p);
    }
  }

  return foundlength == 1 && foundattachmentid <= 1 && foundrowid == 1 &&
    length_fieldsize <= 8 && rowid_fieldsize <= 8 &&
    len <= available &&
    len < 1 * 1024 * 1024 * 1024; // lets cap a valid attachment size at 1 gigabyte.
  // From what I've found, the current (theoretical) maximum is 500Mb for video on
  // Android.
  // From reading the source, the real maximum 100Mb (even less, as this is the
  // length of the padded ciphertext), and 500Mb video is only allowed _to be transcoded
  // to a smaller size_. (https://github.com/signalapp/Signal-Android/blob/28c280947fd75c48268200638bb80117647ce5cf/app/src/main/java/org/thoughtcrime/securesms/util/RemoteConfig.kt#L867)
  // Obviously these values have changed in the past, and will likely change in the future.
  // But this frame validation is only relevant to older databases (with even older limits) anyway
}

inline std::string AttachmentFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::ROWID)
      data += "ROWID:uint64:" + bepaald::toString(bytesToUint64(std::get<1>(p), std::get<2>(p))) + "\n";
    else if (std::get<0>(p) == FIELD::ATTACHMENTID)
      data += "ATTACHMENTID:uint64:" + bepaald::toString(bytesToUint64(std::get<1>(p), std::get<2>(p))) + "\n";
    else if (std::get<0>(p) == FIELD::LENGTH)
      data += "LENGTH:uint32:" + bepaald::toString(bytesToUint32(std::get<1>(p), std::get<2>(p))) + "\n";
  }
  return data;
}

inline unsigned int AttachmentFrame::getField(std::string_view const &str) const
{
  if (str == "ROWID")
    return FIELD::ROWID;
  if (str == "ATTACHMENTID")
    return FIELD::ATTACHMENTID;
  if (str == "LENGTH")
    return FIELD::LENGTH;
  return FIELD::INVALID;
}

inline void AttachmentFrame::setLengthField(uint32_t newlength)
{
  for (auto &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::LENGTH)
    {
      // out with the old
      if (std::get<1>(p))
        delete[] std::get<1>(p);

      // in with the new
      uint32_t tmp = bepaald::swap_endian(newlength);
      std::get<1>(p) = new unsigned char[sizeof(uint32_t)];
      std::memcpy(std::get<1>(p), reinterpret_cast<unsigned char *>(&tmp), sizeof(uint32_t));
      std::get<2>(p) = sizeof(uint32_t);
    }
  }
}

#endif
