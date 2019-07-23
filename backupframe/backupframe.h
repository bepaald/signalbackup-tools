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

#ifndef BACKUPFRAME_H_
#define BACKUPFRAME_H_

#include <cstring>
#include <cstddef>
#include <vector>
#include <tuple>
#include <unordered_map>

#include "../common_be.h"
#include "../basedecryptor/basedecryptor.h"

enum FRAMETYPE : unsigned int
{
  HEADER = 1,
  SQLSTATEMENT = 2,
  SHAREDPREFERENCE = 3,
  ATTACHMENT = 4,
  DATABASEVERSION = 5,
  END = 6, // bool
  AVATAR = 7,
  STICKER = 8,
};

enum WIRETYPE : unsigned int
{
  VARINT = 0,
  FIXED64 = 1,
  LENGTHDELIM = 2,
  STARTTYPE = 3,
  ENDTYPE = 4,
  FIXED32 = 5
};

class BackupFrame
{
 protected:
  inline static std::unordered_map<FRAMETYPE, BackupFrame *(*)(unsigned char *, size_t, uint64_t)> &s_registry();

  struct Registrar
  {
    Registrar(FRAMETYPE ft, BackupFrame *(*func)(unsigned char *, size_t, uint64_t))
    {
      DEBUGOUT("Registering class type: ", ft);
      s_registry()[ft] = func;
    }
  };
  bool d_ok;
  std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> d_framedata; // field number, field data, length
  uint64_t d_count;
 public:
  explicit inline BackupFrame(uint64_t count);
  inline BackupFrame(unsigned char *data, size_t length, uint64_t count);
  inline BackupFrame(BackupFrame &&other);
  inline BackupFrame &operator=(BackupFrame &&other);
  inline BackupFrame(BackupFrame const &other);
  inline BackupFrame &operator=(BackupFrame const &other);
  inline virtual ~BackupFrame();
  inline bool ok();
  inline static int getFieldnumber(unsigned char head);
  inline static unsigned int wiretype(unsigned char head);
  inline static int64_t getLength(unsigned char *data, unsigned int *offset, unsigned int totallength);
  inline static int64_t getVarint(unsigned char *data, unsigned int *offset, unsigned int totallength);
  virtual FRAMETYPE frameType() const = 0;
  inline std::string frameTypeString() const;
  inline static BackupFrame *instantiate(FRAMETYPE, unsigned char *data, size_t length, uint64_t count = 0);
  virtual void printInfo() const = 0;
  inline uint64_t frameNumber() const;
  inline virtual uint32_t attachmentSize() const;
  inline virtual bool setAttachmentData(unsigned char *data);
  inline virtual std::pair<unsigned char *, uint64_t> getData() const;
  inline virtual std::string getHumanData() const;
  inline bool setNewData(unsigned int field, unsigned char *data, uint64_t size);
  inline virtual bool validate() const;
 protected:
  inline uint32_t bytesToUint32(unsigned char *data, size_t len) const;
  inline uint64_t bytesToUint64(unsigned char *data, size_t len) const;
  bool init(unsigned char *data, size_t length, std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> *framedata);
  template <typename T>
  inline void intTypeToBytes(T val, unsigned char *b);
  inline uint64_t putVarInt(uint64_t val, unsigned char *mem) const;
  inline uint64_t varIntSize(uint64_t val) const;
  inline uint64_t setFieldAndWire(unsigned int field, unsigned int type, unsigned char *mem) const;
  inline uint64_t setFrameSize(uint64_t totalsize, unsigned char *mem) const;
  inline uint64_t putLengthDelimType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;
  inline uint64_t putVarIntType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;
  inline uint64_t putFixed64Type(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;

 private:
  inline static int64_t getLengthOrVarint(unsigned char *data, unsigned int *offset, unsigned int totallength);
};

inline std::unordered_map<FRAMETYPE, BackupFrame *(*)(unsigned char *, size_t, uint64_t)> &BackupFrame::s_registry() // static
{
  static std::unordered_map<FRAMETYPE, BackupFrame *(*)(unsigned char *, size_t, uint64_t)> impl;
  return impl;
}

inline BackupFrame::BackupFrame(uint64_t num)
  :
  d_ok(false),
  d_count(num)
{}

inline BackupFrame::BackupFrame(unsigned char *data, size_t l, uint64_t num)
  :
  d_ok(false),
  d_count(num)
{
  DEBUGOUT("CREATING BACKUPFRAME!");
  DEBUGOUT("DATA: ", bepaald::bytesToHexString(data, l), " (", l, " bytes)");
  d_ok = init(data, l, &d_framedata);
}

inline BackupFrame::BackupFrame(BackupFrame &&other)
  :
  d_ok(std::move(other.d_ok)),
  d_framedata(std::move(other.d_framedata)),
  d_count(std::move(other.d_count))
{
  other.d_framedata.clear(); // clear other without delete[]ing, ~this will do it
}

inline BackupFrame &BackupFrame::operator=(BackupFrame &&other)
{
  if (this != &other)
  {
    // properly delete any data this is holding
    for (uint i = 0; i < d_framedata.size(); ++i)
      if (std::get<1>(d_framedata[i]))
        delete[] std::get<1>(d_framedata[i]);
    d_framedata.clear();

    d_ok = std::move(other.d_ok);
    d_framedata = std::move(other.d_framedata);
    other.d_framedata.clear();
    d_count = std::move(other.d_count);
  }
  return *this;
}

inline BackupFrame::BackupFrame(BackupFrame const &other)
{
  d_ok = other.d_ok;
  d_count = other.d_count;
  for (uint i = 0; i < other.d_framedata.size(); ++i)
  {
    unsigned char *datacpy = nullptr;
    if (std::get<1>(other.d_framedata[i]))
    {
      datacpy = new unsigned char[std::get<2>(other.d_framedata[i])];
      std::memcpy(datacpy, std::get<1>(other.d_framedata[i]), std::get<2>(other.d_framedata[i]));
    }
    d_framedata.emplace_back(std::make_tuple(std::get<0>(other.d_framedata[i]), datacpy, std::get<2>(other.d_framedata[i])));
  }
}

inline BackupFrame &BackupFrame::operator=(BackupFrame const &other)
{
  if (this != &other)
  {
    d_ok = other.d_ok;
    d_count = other.d_count;
    for (uint i = 0; i < other.d_framedata.size(); ++i)
    {
      unsigned char *datacpy = nullptr;
      if (std::get<1>(other.d_framedata[i]))
      {
        datacpy = new unsigned char[std::get<2>(other.d_framedata[i])];
        std::memcpy(datacpy, std::get<1>(other.d_framedata[i]), std::get<2>(other.d_framedata[i]));
      }
      d_framedata.emplace_back(std::make_tuple(std::get<0>(other.d_framedata[i]), datacpy, std::get<2>(other.d_framedata[i])));
    }
  }
  return *this;
}

inline BackupFrame::~BackupFrame()
{
  for (uint i = 0; i < d_framedata.size(); ++i)
    if (std::get<1>(d_framedata[i]))
      delete[] std::get<1>(d_framedata[i]);
  d_framedata.clear();
}

inline bool BackupFrame::ok()
{
  return d_ok;
}

inline int BackupFrame::getFieldnumber(unsigned char head) // static
{
  if (head & 0b10000000)
    return -1;
  return (head & 0b01111000) >> 3;
}

inline unsigned int BackupFrame::wiretype(unsigned char head) // static
{
  return (head & 0b00000111);
}

inline int64_t BackupFrame::getLength(unsigned char *data, unsigned int *offset, unsigned int totallength) // static
{
  return getLengthOrVarint(data, offset, totallength);
}

inline int64_t BackupFrame::getVarint(unsigned char *data, unsigned int *offset, unsigned int totallength) // static
{
  return getLengthOrVarint(data, offset, totallength);
}

inline std::string BackupFrame::frameTypeString() const
{
  switch (frameType())
  {
  case 1:
  {
   return "HeaderFrame";
  }
  case 2:
  {
   return "SqlStatementFrame";
  }
  case 3:
  {
   return "SharedPreferenceFrame";
  }
  case 4:
  {
   return "AttachmentFrame";
  }
  case 5:
  {
   return "DatabaseVersionFrame";
  }
  case 6:
  {
   return "EndFrame";
  }
  case 7:
  {
   return "AvatarFrame";
  }
  case 8:
  {
   return "StickerFrame";
  }
  }
  return "Unknown frame type";
}

inline int64_t BackupFrame::getLengthOrVarint(unsigned char *data, unsigned int *offset, unsigned int totallength) // static
{
  if (*offset >= totallength)
    return -1;

  uint64_t length = 0;
  uint64_t times = 0;
  while (*offset < totallength && (data[*offset]) & 0b10000000)
    length += ((static_cast<uint64_t>(data[(*offset)++]) & 0b01111111) << (times++ * 7));
  if (*offset >= totallength)
    return -1;
  length += ((static_cast<uint64_t>(data[(*offset)++]) & 0b01111111) << (times * 7));
  return length;
}


inline BackupFrame *BackupFrame::instantiate(FRAMETYPE ft, unsigned char *data, size_t length, uint64_t count) //static
{
  auto it = s_registry().find(ft);

  /*
  if (it == s_registry().end())
  {
    std::cout << s_registry().size() << std::endl;
    std::cout << ft << std::endl;
    std::cout << "END!" << std::endl;
  }
  */

  return it == s_registry().end() ? nullptr : (it->second)(data, length, count);
}

// maybe check endianness?
inline uint32_t BackupFrame::bytesToUint32(unsigned char *data, size_t len) const
{
  return static_cast<uint32_t>(data[len - 1] & 0xFF) |
    static_cast<uint32_t>(data[len - 2] & 0xFF) << 8 |
    static_cast<uint32_t>(data[len - 3] & 0xFF) << 16 |
    static_cast<uint32_t>(data[len - 4] & 0xFF) << 24;
}

inline uint64_t BackupFrame::bytesToUint64(unsigned char *data, size_t len) const
{
  return static_cast<uint64_t>(data[len - 1] & 0xFF) |
    static_cast<uint64_t>(data[len - 2] & 0xFF) << 8 |
    static_cast<uint64_t>(data[len - 3] & 0xFF) << 16 |
    static_cast<uint64_t>(data[len - 4] & 0xFF) << 24 |
    static_cast<uint64_t>(data[len - 5] & 0xFF) << 32 |
    static_cast<uint64_t>(data[len - 6] & 0xFF) << 40 |
    static_cast<uint64_t>(data[len - 7] & 0xFF) << 48 |
    static_cast<uint64_t>(data[len - 8] & 0xFF) << 56;
}

// inline void BackupFrame::printInfo() const // virtual
// {
//   DEBUGOUT("I AM A GENERIC BACKUPFRAME. IT SEEMS A BAD CHILD OF MINE HAS NOT OVERRIDDEN ME!");
//   std::cout << "Frame number: " << d_count << std::endl;
// }

inline uint64_t BackupFrame::frameNumber() const
{
  return d_count;
}

template <typename T>
inline void BackupFrame::intTypeToBytes(T val, unsigned char *b)
{
  int x = 4;
  unsigned char t[sizeof(int)];
  std::copy(static_cast<unsigned char*>(static_cast<void*>(&x)),
            static_cast<unsigned char*>(static_cast<void*>(&x)) + sizeof(int),
            t);
  DEBUGOUT("WRODKS? ", bepaald::bytesToHexString(t, sizeof(int)));
  DEBUGOUT("WROWR   ", bytesToUint32(t, sizeof(decltype(t))));

  for (size_t i = 0; i < sizeof(T); ++i)
    b[i] = (val >> ((sizeof(T) - i) * 8));

  DEBUGOUT("GOT: ", val);
  DEBUGOUT("GOT: ", bepaald::bytesToHexString(b, sizeof(T)));
}

inline uint32_t BackupFrame::attachmentSize() const
{
  return 0;
}

inline bool BackupFrame::setAttachmentData(unsigned char *) // virtual
{
  return false;
}

inline std::pair<unsigned char *, uint64_t> BackupFrame::getData() const
{
  return {nullptr, 0};
}

inline std::string BackupFrame::getHumanData() const
{
  return std::string();
}

// taken from techoverflow
inline uint64_t BackupFrame::putVarInt(uint64_t val, unsigned char *mem) const
{
  uint64_t outputSize = 0;
  //While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (val > 127)
  {
    //|128: Set the next byte flag
    mem[outputSize] = (static_cast<uint8_t>(val & 127)) | 128;
    //Remove the seven bits we just wrote
    val >>= 7;
    outputSize++;
  }
  mem[outputSize++] = (static_cast<uint8_t>(val)) & 127;
  return outputSize;
}

inline uint64_t BackupFrame::varIntSize(uint64_t value) const
{
  if (value <= 0x7f)
    return 1;
  if (value <= 0x3fff)
    return 2;
  if (value <= 0x1fffff)
    return 3;
  if (value <= 0xfffffff)
    return 4;
  if (value <= 0x7ffffffff)
    return 5;
  if (value <= 0x3ffffffffff)
    return 6;
  if (value <= 0x1ffffffffffff)
    return 7;
  if (value <= 0xffffffffffffff)
    return 8;
  if (value <= 0x7fffffffffffffff)
    return 9;
  return 10;
}

inline uint64_t BackupFrame::setFieldAndWire(unsigned int field, unsigned int type, unsigned char *mem) const
{
  mem[0] = 0x00 | (field << 3);
  mem[0] |= (type);
  return 1;
}

inline uint64_t BackupFrame::setFrameSize(uint64_t totalsize, unsigned char *mem) const
{
  // expand to varint size 10
  if (varIntSize(totalsize - 2) == 1)
    return putVarInt(totalsize - 2, mem);
  if (varIntSize(totalsize - 3) == 2)
    return putVarInt(totalsize - 3, mem);
  if (varIntSize(totalsize - 4) == 3)
    return putVarInt(totalsize - 4, mem);
  if (varIntSize(totalsize - 5) == 4)
    return putVarInt(totalsize - 5, mem);
  if (varIntSize(totalsize - 6) == 5)
    return putVarInt(totalsize - 6, mem);
  if (varIntSize(totalsize - 7) == 6)
    return putVarInt(totalsize - 7, mem);
  if (varIntSize(totalsize - 8) == 7)
    return putVarInt(totalsize - 8, mem);
  if (varIntSize(totalsize - 9) == 8)
    return putVarInt(totalsize - 9, mem);
  if (varIntSize(totalsize - 10) == 9)
    return putVarInt(totalsize - 10, mem);
  return putVarInt(totalsize - 11, mem);
}

inline uint64_t BackupFrame::putLengthDelimType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const
{
  uint64_t datapos = 0;
  datapos += setFieldAndWire(std::get<0>(data), WIRETYPE::LENGTHDELIM, mem + datapos);
  datapos += putVarInt(std::get<2>(data), mem + datapos);
  std::memcpy(mem + datapos, std::get<1>(data), std::get<2>(data));
  datapos += std::get<2>(data);
  return datapos;
}

inline uint64_t BackupFrame::putVarIntType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const
{
  uint64_t datapos = 0;
  uint64_t value = bytesToUint64(std::get<1>(data), std::get<2>(data));
  datapos += setFieldAndWire(std::get<0>(data), WIRETYPE::VARINT, mem + datapos);
  datapos += putVarInt(value, mem + datapos);
  return datapos;
}

inline uint64_t BackupFrame::putFixed64Type(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const
{
  uint64_t datapos = 0;
  datapos += setFieldAndWire(std::get<0>(data), WIRETYPE::FIXED64, mem + datapos);
  std::memcpy(mem + datapos, std::get<1>(data), std::get<2>(data));
  datapos += std::get<2>(data);
  return datapos;
}

inline bool BackupFrame::setNewData(unsigned int field, unsigned char *data, uint64_t size)
{
  d_framedata.emplace_back(std::make_tuple(field, data, size));
  return true;
}

inline bool BackupFrame::validate() const
{
  return true;
}

#endif
