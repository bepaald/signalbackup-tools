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

#ifndef BACKUPFRAME_H_
#define BACKUPFRAME_H_

#include <cstring>
#include <cstddef>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <limits>

#include "../logger/logger.h"

class BackupFrame
{
 public:
  enum FRAMETYPE : std::uint8_t
  {
    HEADER = 1,
    SQLSTATEMENT = 2,
    SHAREDPREFERENCE = 3,
    ATTACHMENT = 4,
    DATABASEVERSION = 5,
    END = 6, // bool
    AVATAR = 7,
    STICKER = 8,
    KEYVALUE = 9,

    INVALID = std::numeric_limits<std::uint8_t>::max()
  };

  enum WIRETYPE : std::uint8_t
  {
    VARINT = 0,
    FIXED64 = 1,
    LENGTHDELIM = 2,
    STARTTYPE = 3,
    ENDTYPE = 4,
    FIXED32 = 5
  };

 protected:
  inline static std::unordered_map<FRAMETYPE, BackupFrame *(*)(unsigned char const *, size_t, uint64_t)> &s_registry();

  struct Registrar
  {
    Registrar(FRAMETYPE ft, BackupFrame *(*func)(unsigned char const *, size_t, uint64_t))
    {
      //Logger::message("Registering class type: ", ft);
      s_registry()[ft] = func;
    }
  };
  std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> d_framedata; // field number, field data, length
  uint64_t d_count;
  size_t d_constructedsize;
  bool d_ok;
 public:
  explicit inline BackupFrame(uint64_t count);
  inline BackupFrame(unsigned char const *data, size_t length, uint64_t count);
  inline BackupFrame(BackupFrame &&other) noexcept;
  inline BackupFrame &operator=(BackupFrame &&other) noexcept;
  inline BackupFrame(BackupFrame const &other);
  inline BackupFrame &operator=(BackupFrame const &other);
  inline virtual ~BackupFrame();
  inline virtual BackupFrame *clone() const = 0;
  inline virtual BackupFrame *move_clone() = 0;
  inline bool ok();
  inline static constexpr int getFieldnumber(unsigned char head);
  inline static constexpr unsigned int wiretype(unsigned char head);
  inline static constexpr int64_t getLength(unsigned char const *data, unsigned int *offset, unsigned int totallength);
  inline static constexpr int64_t getVarint(unsigned char const *data, unsigned int *offset, unsigned int totallength);
  virtual FRAMETYPE frameType() const = 0;
  inline std::string frameTypeString() const;
  inline static BackupFrame *instantiate(FRAMETYPE, unsigned char *data, size_t length, uint64_t count = 0);
  virtual void printInfo() const = 0;
  inline uint64_t frameNumber() const;
  inline virtual uint32_t attachmentSize() const;
  inline virtual bool setAttachmentDataBacked(unsigned char *data, long long int size);
  inline virtual std::pair<unsigned char *, uint64_t> getData() const;
  inline virtual std::string getHumanData() const;
  inline bool setNewData(unsigned int field, unsigned char *data, uint64_t size);
  inline virtual bool validate(uint64_t available) const;
  inline virtual uint64_t dataSize() const;
 protected:
  inline constexpr uint32_t bytesToUint32(unsigned char const *data, size_t len) const;
  inline constexpr uint64_t bytesToUint64(unsigned char const *data, size_t len) const;
  inline constexpr int32_t bytesToInt32(unsigned char const *data, size_t len) const;
  inline constexpr int64_t bytesToInt64(unsigned char const *data, size_t len) const;
  bool init(unsigned char const *data, size_t length, std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> *framedata);
  template <typename T>
  inline constexpr void intTypeToBytes(T val, unsigned char *b);
  inline constexpr uint64_t putVarInt(uint64_t val, unsigned char *mem) const;
  inline constexpr uint64_t varIntSize(uint64_t val) const;
  inline constexpr uint64_t setFieldAndWire(unsigned int field, unsigned int type, unsigned char *mem) const;
  inline constexpr uint64_t setFrameSize(uint64_t totalsize, unsigned char *mem) const;
  inline uint64_t putLengthDelimType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;
  inline uint64_t putVarIntType(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;
  inline uint64_t putFixed32Type(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;
  inline uint64_t putFixed64Type(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const;

 private:
  inline static constexpr int64_t getLengthOrVarint(unsigned char const *data, unsigned int *offset, unsigned int totallength);
};

inline std::unordered_map<BackupFrame::FRAMETYPE, BackupFrame *(*)(unsigned char const *, size_t, uint64_t)> &BackupFrame::s_registry() // static
{
  static std::unordered_map<FRAMETYPE, BackupFrame *(*)(unsigned char const *, size_t, uint64_t)> impl;
  return impl;
}

inline BackupFrame::BackupFrame(uint64_t num)
  :
  d_count(num),
  d_constructedsize(0),
  d_ok(false)
{}

inline BackupFrame::BackupFrame(unsigned char const *data, size_t l, uint64_t num)
  :
  d_count(num),
  d_constructedsize(l),
  d_ok(false)
{
  //std::cout << "CREATING BACKUPFRAME!" << std::endl;
  //Logger::message("DATA: ", bepaald::bytesToHexString(data, l), " (", l, " bytes)");
  d_ok = init(data, l, &d_framedata);
}

inline BackupFrame::BackupFrame(BackupFrame &&other) noexcept
  :
  d_framedata(std::move(other.d_framedata)),
  d_count(other.d_count),
  d_constructedsize(other.d_constructedsize),
  d_ok(other.d_ok)
{
  other.d_framedata.clear(); // clear other without delete[]ing, ~this will do it
}

inline BackupFrame &BackupFrame::operator=(BackupFrame &&other) noexcept
{
  if (this != &other)
  {
    // properly delete any data this is holding
    for (unsigned int i = 0; i < d_framedata.size(); ++i)
      if (std::get<1>(d_framedata[i]))
        delete[] std::get<1>(d_framedata[i]);
    d_framedata.clear();

    d_ok = other.d_ok;
    d_framedata = std::move(other.d_framedata);
    other.d_framedata.clear();
    d_count = other.d_count;
    d_constructedsize = other.d_constructedsize;
  }
  return *this;
}

inline BackupFrame::BackupFrame(BackupFrame const &other)
{
  d_ok = other.d_ok;
  d_count = other.d_count;
  d_constructedsize = other.d_constructedsize;
  for (unsigned int i = 0; i < other.d_framedata.size(); ++i)
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
    // properly delete any data this is holding
    for (unsigned int i = 0; i < d_framedata.size(); ++i)
      if (std::get<1>(d_framedata[i]))
        delete[] std::get<1>(d_framedata[i]);
    d_framedata.clear();

    d_ok = other.d_ok;
    d_count = other.d_count;
    d_constructedsize = other.d_constructedsize;
    for (unsigned int i = 0; i < other.d_framedata.size(); ++i)
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
  //std::cout << "DESTROYING BACKUPFRAME!" << std::endl;
  for (unsigned int i = 0; i < d_framedata.size(); ++i)
    if (std::get<1>(d_framedata[i]))
      delete[] std::get<1>(d_framedata[i]);
  d_framedata.clear();
}

inline bool BackupFrame::ok()
{
  return d_ok;
}

inline constexpr int BackupFrame::getFieldnumber(unsigned char head) // static
{
  if (head & 0b10000000) [[unlikely]]
    return -1;
  return (head & 0b01111000) >> 3;
}

inline constexpr unsigned int BackupFrame::wiretype(unsigned char head) // static
{
  return (head & 0b00000111);
}

inline constexpr int64_t BackupFrame::getLength(unsigned char const *data, unsigned int *offset, unsigned int totallength) // static
{
  return getLengthOrVarint(data, offset, totallength);
}

inline constexpr int64_t BackupFrame::getVarint(unsigned char const *data, unsigned int *offset, unsigned int totallength) // static
{
  return getLengthOrVarint(data, offset, totallength);
}

inline std::string BackupFrame::frameTypeString() const
{
  switch (frameType())
  {
    case FRAMETYPE::HEADER:
    {
      return "HeaderFrame";
    }
    case FRAMETYPE::SQLSTATEMENT:
    {
      return "SqlStatementFrame";
    }
    case FRAMETYPE::SHAREDPREFERENCE:
    {
      return "SharedPreferenceFrame";
    }
    case FRAMETYPE::ATTACHMENT:
    {
      return "AttachmentFrame";
    }
    case FRAMETYPE::DATABASEVERSION:
    {
      return "DatabaseVersionFrame";
    }
    case FRAMETYPE::END:
    {
      return "EndFrame";
    }
    case FRAMETYPE::AVATAR:
    {
      return "AvatarFrame";
    }
    case FRAMETYPE::STICKER:
    {
      return "StickerFrame";
    }
    case FRAMETYPE::KEYVALUE:
    {
      return "KeyValueFrame";
    }
    //case FRAMETYPE::INVALID:
    default:
    {
      return "InvalidFrame";
    }
  }
  return "Unknown frame type";
}

inline constexpr int64_t BackupFrame::getLengthOrVarint(unsigned char const *data, unsigned int *offset, unsigned int totallength) // static
{
  /*
  // This is a unrolled variant of the original below, in artificial testing, it appeared
  // faster (~70 millisecs, on 100 million inputs), but in practice, this causes to slow
  // opening backup file down (just single milliseconds, not significant but the speedup
  // was gone)

  // read first byte (if bytes available)
  uint64_t length = (*offset < totallength) ? data[*offset] & 0b0111'1111 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 7 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 14 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 21 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 28 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 35 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 42 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 49 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 56 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  length += (*offset < totallength) ? (static_cast<uint64_t>(data[*offset]) & 0b0111'1111) << 63 : 0;
  if (*offset < totallength && data[(*offset)++] < 0b1000'0000)
    return length;

  return 0;
  */

  if (*offset >= totallength) [[unlikely]]
    return 0;

  uint64_t length = 0;
  uint64_t times = 0;
  while (*offset < totallength && (data[*offset]) & 0b10000000)
    length += ((static_cast<uint64_t>(data[(*offset)++]) & 0b01111111) << (times++ * 7));
  if (*offset >= totallength) [[unlikely]]
    return 0;
  return static_cast<int64_t>(length + ((static_cast<uint64_t>(data[(*offset)++]) & 0b01111111) << (times * 7)));
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

  if (it == s_registry().end()) [[unlikely]]
  {
    //std::cout << "ERROR: Incorrect or unknown frametype (" << static_cast<unsigned int>(ft) << ")" << std::endl;
    return nullptr;
  }

  BackupFrame *ret = (it->second)(data, length, count);

  if (!ret->ok()) [[unlikely]]
  {
    //std::cout << "ERROR: BackupFrame::ok() failed" << std::endl;
    delete ret;
    return nullptr;
  }

  return ret;
}

// maybe check endianness?
inline constexpr uint32_t BackupFrame::bytesToUint32(unsigned char const *data, size_t len) const
{
  return static_cast<uint32_t>(data[len - 1] & 0xFF) |
    static_cast<uint32_t>(data[len - 2] & 0xFF) << 8 |
    static_cast<uint32_t>(data[len - 3] & 0xFF) << 16 |
    static_cast<uint32_t>(data[len - 4] & 0xFF) << 24;
}

inline constexpr uint64_t BackupFrame::bytesToUint64(unsigned char const *data, size_t len) const
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

inline constexpr int32_t BackupFrame::bytesToInt32(unsigned char const *data, size_t len) const
{
  return (data[len - 1] & 0xFF) |
    (data[len - 2] & 0xFF) << 8 |
    (data[len - 3] & 0xFF) << 16 |
    (data[len - 4] & 0xFF) << 24;
}

inline constexpr int64_t BackupFrame::bytesToInt64(unsigned char const *data, size_t len) const
{
  return static_cast<int64_t>(data[len - 1] & 0xFF) |
    static_cast<int64_t>(data[len - 2] & 0xFF) << 8 |
    static_cast<int64_t>(data[len - 3] & 0xFF) << 16 |
    static_cast<int64_t>(data[len - 4] & 0xFF) << 24 |
    static_cast<int64_t>(data[len - 5] & 0xFF) << 32 |
    static_cast<int64_t>(data[len - 6] & 0xFF) << 40 |
    static_cast<int64_t>(data[len - 7] & 0xFF) << 48 |
    static_cast<int64_t>(data[len - 8] & 0xFF) << 56;
}

inline uint64_t BackupFrame::frameNumber() const
{
  return d_count;
}

template <typename T>
inline constexpr void BackupFrame::intTypeToBytes(T val, unsigned char *b)
{
  for (size_t i = 0; i < sizeof(T); ++i)
    b[i] = (val >> ((sizeof(T) - (i + 1)) * 8)); // this may have a swap_endian builtin?
}

inline uint32_t BackupFrame::attachmentSize() const
{
  return 0;
}

inline bool BackupFrame::setAttachmentDataBacked(unsigned char *, long long int) // virtual
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
inline constexpr uint64_t BackupFrame::putVarInt(uint64_t val, unsigned char *mem) const
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

inline constexpr uint64_t BackupFrame::varIntSize(uint64_t value) const
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

inline constexpr uint64_t BackupFrame::setFieldAndWire(unsigned int field, unsigned int type, unsigned char *mem) const
{
  mem[0] = (field << 3);
  mem[0] |= type;
  return 1;
}

inline constexpr uint64_t BackupFrame::setFrameSize(uint64_t totalsize, unsigned char *mem) const
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

inline uint64_t BackupFrame::putFixed32Type(std::tuple<unsigned int, unsigned char *, size_t> const &data, unsigned char *mem) const
{
  uint64_t datapos = 0;
  datapos += setFieldAndWire(std::get<0>(data), WIRETYPE::FIXED32, mem + datapos);
  std::memcpy(mem + datapos, std::get<1>(data), std::get<2>(data));
  datapos += std::get<2>(data);
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

inline bool BackupFrame::validate(uint64_t) const
{
  return true;
}

inline uint64_t BackupFrame::dataSize() const
{
  return 0;
}

#endif
