/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifndef PROTOBUFPARSERBASE_H_
#define PROTOBUFPARSERBASE_H_

#include <cstring>
#include <memory>
#include <vector>
#include <optional>

#include "../common_be.h"
#include "../base64/base64.h"
#include "../logger/logger.h"

template<typename T>
struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template<typename T>
struct is_optional : public std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : public std::true_type {};

template < template <typename...> class Template, typename T >
struct is_specialization_of : std::false_type {};

template < template <typename...> class Template, typename... Args >
struct is_specialization_of< Template, Template<Args...> > : std::true_type {};

struct ZigZag32
{
  int32_t value{0}; // this holds the decoded (non-zigzag) value
  operator int32_t() const { return value; }
  ZigZag32() = default;
  ZigZag32(uint32_t v) : value(static_cast<int32_t>(v)) {}
};

struct ZigZag64
{
  int64_t value{0}; // this holds the decoded (non-zigzag) value
  operator int64_t() const { return value; }
  ZigZag64() = default;
  ZigZag64(uint64_t v) : value(static_cast<int64_t>(v)) {}
};

struct Fixed32
{
  uint32_t value{0};
  operator uint32_t() const { return value; }
  Fixed32() = default;
  Fixed32(uint32_t v) : value(v) {}
};

struct Fixed64
{
  uint64_t value{0};
  operator uint64_t() const { return value; }
  Fixed64() = default;
  Fixed64(uint64_t v) : value(v) {}
};

struct SFixed32
{
  int32_t value{0};
  operator int32_t() const { return value; }
  SFixed32() = default;
  SFixed32(int32_t v) : value(v) {}
};

struct SFixed64
{
  int64_t value{0};
  operator int64_t() const { return value; }
  SFixed64() = default;
  SFixed64(int64_t v) : value(v) {}
};

struct Enum
{
  int32_t value{0};
  operator int32_t() const { return value; }
  Enum() = default;
  Enum(int32_t v) : value(v) {}
};

struct Dummy
{};

namespace protobuffer
{
  typedef Dummy DUMMY;
  namespace optional
  {
    typedef double DOUBLE;
    typedef float FLOAT;
    typedef int32_t INT32;
    typedef Enum ENUM;
    typedef int64_t INT64;
    typedef uint32_t UINT32;
    typedef uint64_t UINT64;
    typedef ZigZag32 SINT32;
    typedef ZigZag64 SINT64;
    typedef Fixed32 FIXED32;
    typedef Fixed64 FIXED64;
    typedef SFixed32 SFIXED32;
    typedef SFixed64 SFIXED64;
    typedef bool BOOL;
    typedef std::string STRING;
    typedef unsigned char *BYTES;
  }
  namespace repeated
  {
    typedef std::vector<double> DOUBLE;
    typedef std::vector<float> FLOAT;
    typedef std::vector<int32_t> INT32;
    typedef std::vector<Enum> ENUM;
    typedef std::vector<int64_t> INT64;
    typedef std::vector<uint32_t> UINT32;
    typedef std::vector<uint64_t> UINT64;
    typedef std::vector<ZigZag32> SINT32;
    typedef std::vector<ZigZag64> SINT64;
    typedef std::vector<Fixed32> FIXED32;
    typedef std::vector<Fixed64> FIXED64;
    typedef std::vector<SFixed32> SFIXED32;
    typedef std::vector<SFixed64> SFIXED64;
    typedef std::vector<bool> BOOL;
    typedef std::vector<std::string> STRING;
    typedef std::vector<unsigned char *> BYTES;
  }
}

// these might be able to go back into Protobufparser when gcc bug is fixed: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
namespace ProtoBufParserReturn
{
  // primary
  template <typename T, bool vec>
  struct item_return {};

  // for optionals
  template <typename T>
  struct item_return<T, false> { typedef std::optional<T> type; };
  template <>
  struct item_return<ZigZag32, false>{ typedef std::optional<int32_t> type; };
  template <>
  struct item_return<ZigZag64, false>{ typedef std::optional<int64_t> type; };
  template <>
  struct item_return<Fixed32, false>{ typedef std::optional<uint32_t> type; };
  template <>
  struct item_return<Fixed64, false>{ typedef std::optional<uint64_t> type; };
  template <>
  struct item_return<SFixed32, false>{ typedef std::optional<int32_t> type; };
  template <>
  struct item_return<SFixed64, false>{ typedef std::optional<int64_t> type; };
  // template <>
  // struct item_return<char *, false>{ typedef std::optional<std::pair<std::unique_ptr<char []>, uint64_t>> type; };
  template <>
  struct item_return<unsigned char *, false>{ typedef std::optional<std::pair<std::unique_ptr<unsigned char []>, uint64_t>> type; };

  // for vectors:
  template <typename T>
  struct item_return<T, true> { typedef T type; };
  template <>
  struct item_return<std::vector<ZigZag32>, true>{ typedef std::vector<int32_t> type; };
  template <>
  struct item_return<std::vector<ZigZag64>, true>{ typedef std::vector<int64_t> type; };
  template <>
  struct item_return<std::vector<Fixed32>, true>{ typedef std::vector<uint32_t> type; };
  template <>
  struct item_return<std::vector<Fixed64>, true>{ typedef std::vector<uint64_t> type; };
  template <>
  struct item_return<std::vector<SFixed32>, true>{ typedef std::vector<int32_t> type; };
  template <>
  struct item_return<std::vector<SFixed64>, true>{ typedef std::vector<int64_t> type; };
  // template <>
  // struct item_return<std::vector<char *>, true>{ typedef std::vector<std::pair<std::unique_ptr<char []>, uint64_t>> type; };
  template <>
  struct item_return<std::vector<unsigned char *>, true>{ typedef std::vector<std::pair<std::unique_ptr<unsigned char []>, uint64_t>> type; };

  // primary
  template <typename T, bool vec>
  struct item_return_view {};

  // for optionals
  template <typename T>
  struct item_return_view<T, false> { typedef typename item_return<T, false>::type type; };
  template <>
  struct item_return_view<std::string, false>{ typedef std::optional<std::string_view> type; };
  // template <>
  // struct item_return_view<char *, false>{ typedef std::optional<std::pair<char *, uint64_t>> type; };
  template <>
  struct item_return_view<unsigned char *, false>{ typedef std::optional<std::pair<unsigned char *, uint64_t>> type; };

  // for vectors:
  template <typename T>
  struct item_return_view<T, true> { typedef typename item_return<T, true>::type type;  };
  template <>
  struct item_return_view<std::vector<std::string>, true>{ typedef std::vector<std::string_view> type; };
  // template <>
  // struct item_return_view<std::vector<char *>, true>{ typedef std::vector<std::pair<char *, uint64_t>> type; };
  template <>
  struct item_return_view<std::vector<unsigned char *>, true>{ typedef std::vector<std::pair<unsigned char *, uint64_t>> type; };
}

class ProtoBufParserBase
{
 protected:
  enum WIRETYPE : std::uint8_t
  {
    VARINT = 0,
    FIXED64 = 1,
    LENGTH_DELIMITED = 2,
    STARTGROUP = 3,
    ENDGROUP = 4,
    FIXED32 = 5
  };

  unsigned char *d_data;
  uint64_t d_size;
  bool d_viewonly;
 public:
  inline ProtoBufParserBase();
  inline explicit ProtoBufParserBase(std::string const &base64);
  inline ProtoBufParserBase(unsigned char const *data, uint64_t size);
  inline ProtoBufParserBase(unsigned char *data, uint64_t size, bool viewonly);
  inline explicit ProtoBufParserBase(ProtoBufParserBase const &other);
  inline ProtoBufParserBase &operator=(ProtoBufParserBase const &other);
  inline ProtoBufParserBase(ProtoBufParserBase &&other) noexcept;
  inline ProtoBufParserBase &operator=(ProtoBufParserBase &&other) noexcept;
  ~ProtoBufParserBase();

  inline bool operator==(ProtoBufParserBase const &other) const;
  inline bool operator!=(ProtoBufParserBase const &other) const;

  inline int64_t size() const;
  inline unsigned char *data() const;
  inline std::string getDataString() const;
  inline void setData(std::string const &base64);
  inline void setData(unsigned char const *data, int64_t size);
  inline int32_t getFirstFieldNumber() const;

  template <typename T = std::nullptr_t>
  int deleteFields(int num, T const *value = nullptr);
  template <typename T = std::nullptr_t>
  bool deleteFirstField(int num, T const *value = nullptr);

 protected:
  inline void clear();
  template <int idx>
  inline static constexpr uint64_t fieldSize();
  inline static uint64_t varIntSize(uint64_t value);
  inline static int64_t readVarInt(unsigned int *pos, unsigned char const *data, unsigned int size, bool zigzag = false);
  inline static int64_t getVarIntFieldLength(int pos, unsigned char const *data, int size);
  inline std::pair<unsigned char *, uint64_t> getFieldData(int num, int32_t *wiretype) const;
  inline std::pair<unsigned char *, uint64_t> getFieldData(int num, int32_t *wiretype, unsigned int *pos) const;
  inline void getPosAndLengthForField(int num, int startpos, int64_t *pos, int64_t *fieldlength) const;
  inline bool fieldExists(int num) const;
  template <typename T, bool asview>
  inline auto getFieldAs(int num) const;
  template <typename T, bool asview>
  inline auto getFieldsAs(int num) const; // T must be std::vector<Something>
  // template <typename T>
  // inline typename ProtoBufParserReturn::item_return_view<T, false>::type getFieldViewAs(int num) const;
  // template <typename T>
  // inline typename ProtoBufParserReturn::item_return_view<T, true>::type getFieldsViewAs(int num) const; // T must be std::vector<Something>
};

inline ProtoBufParserBase::ProtoBufParserBase()
  :
  d_data(nullptr),
  d_size(0),
  d_viewonly(false)
{}

inline ProtoBufParserBase::ProtoBufParserBase(ProtoBufParserBase const &other)
  :
  d_data(other.d_viewonly ? other.d_data : nullptr),
  d_size(other.d_size),
  d_viewonly(other.d_viewonly)
{
  if (!d_viewonly)
  {
    d_data = new unsigned char[d_size];
    if (d_size)
      std::memcpy(d_data, other.d_data, d_size);
  }
}

inline ProtoBufParserBase &ProtoBufParserBase::operator=(ProtoBufParserBase const &other)
{
  if (this != &other)
  {
    clear();

    d_size = other.d_size;
    d_viewonly = other.d_viewonly;
    if (d_viewonly)
    {
      d_data = new unsigned char[d_size];
      if (d_size)
        std::memcpy(d_data, other.d_data, d_size);
    }
    else
      d_data = other.d_data;
  }
  return *this;
}

inline ProtoBufParserBase::ProtoBufParserBase(ProtoBufParserBase &&other) noexcept
  :
  d_data(other.d_data),
  d_size(other.d_size),
  d_viewonly(other.d_viewonly)
{
  other.d_data = nullptr;
  other.d_size = 0;
}

inline ProtoBufParserBase &ProtoBufParserBase::operator=(ProtoBufParserBase &&other) noexcept
{
  if (this != &other)
  {
    clear();

    d_data = other.d_data;
    d_size = other.d_size;
    d_viewonly = other.d_viewonly;

    other.d_data = nullptr;
    other.d_size = 0;
  }
  return *this;
}

inline ProtoBufParserBase::ProtoBufParserBase(std::string const &base64)
  :
  d_data(nullptr),
  d_size(0),
  d_viewonly(false)
{
  std::pair<unsigned char *, size_t> l_data = Base64::base64StringToBytes(base64);
  d_data = l_data.first;
  d_size = l_data.second;

  //std::cout << "INPUT: " << bepaald::bytesToHexString(d_data, d_size) << std::endl;
}

inline ProtoBufParserBase::ProtoBufParserBase(unsigned char const *data, uint64_t size)
  :
  d_data(nullptr),
  d_size(size),
  d_viewonly(false)
{
  d_data = new unsigned char[d_size];
  if (d_size)
    std::memcpy(d_data, data, d_size);
}

inline ProtoBufParserBase::ProtoBufParserBase(unsigned char *data, uint64_t size, bool viewonly)
  :
  d_data(viewonly ? data : nullptr),
  d_size(size),
  d_viewonly(viewonly)
{
  if (!viewonly) [[unlikely]]
  {
    d_data = new unsigned char[d_size];
    if (d_size)
      std::memcpy(d_data, data, d_size);
  }
}

inline ProtoBufParserBase::~ProtoBufParserBase()
{
  clear();
}

inline void ProtoBufParserBase::clear()
{
  if (!d_viewonly)
    bepaald::destroyPtr(&d_data, &d_size);
}

inline bool ProtoBufParserBase::operator==(ProtoBufParserBase const &other) const
{
  return d_size == other.d_size &&
    std::memcmp(d_data, other.d_data, d_size) == 0;
}

inline bool ProtoBufParserBase::operator!=(ProtoBufParserBase const &other) const
{
  return d_size != other.d_size ||
    std::memcmp(d_data, other.d_data, d_size) != 0;
}

inline int64_t ProtoBufParserBase::size() const
{
  return d_size;
}

inline unsigned char *ProtoBufParserBase::data() const
{
  return d_data;
}

inline std::string ProtoBufParserBase::getDataString() const
{
  if (d_size)
    return Base64::bytesToBase64String(d_data, d_size);
  return std::string();
}

inline void ProtoBufParserBase::setData(std::string const &base64)
{
  // destroy old
  if (d_data)
    delete[] d_data;

  std::pair<unsigned char *, size_t> l_data = Base64::base64StringToBytes(base64);

  d_data = l_data.first;
  d_size = l_data.second;
}

inline void ProtoBufParserBase::setData(unsigned char const *d, int64_t s)
{
  // destroy old
  if (d_data)
    delete[] d_data;

  d_data = new unsigned char[s];
  if (d)
    std::memcpy(d_data, d, s);
  d_size = s;
}

int32_t ProtoBufParserBase::getFirstFieldNumber() const
{
  unsigned int pos = 0;
  while (pos < d_size)
  {
    int32_t field = (d_data[pos] & 0b00000000000000000000000001111000) >> 3;
    int fieldshift = 4;
    while (pos < d_size - 1 &&
           d_data[pos] & 0b00000000000000000000000010000000) // skipping the shift
    {
      field |= (d_data[++pos] & 0b00000000000000000000000001111111) << fieldshift;
      fieldshift += 7;
    }
    return field;
  }
  return 0;
}

template <typename T>
int ProtoBufParserBase::deleteFields(int num, T const *value)
{
  int deleted = 0;
  while (deleteFirstField(num, value))
    ++deleted;
  return deleted;
}

template <typename T>
bool ProtoBufParserBase::deleteFirstField(int num, T const *value [[maybe_unused]])
{
  int64_t startpos = 0;
  int64_t pos = -1;
  int64_t fieldlength = -1;

  while (startpos < static_cast<int64_t>(d_size))
  {
    getPosAndLengthForField(num, startpos, &pos, &fieldlength);

    //std::cout << "DATA: " << bepaald::bytesToHexString(d_data, d_size) << std::endl;
    //std::cout << "Got requested field at pos " << pos << " (length " << fieldlength << ")" << std::endl;
    if (pos == -1 || fieldlength == -1)
      return false;
    //std::cout << "FIELD: " << bepaald::bytesToHexString(d_data + pos, fieldlength) << std::endl;

    if constexpr (!std::is_same<T, std::nullptr_t>::value)
    {

      //std::cout << "Asked to delete specific: " << *value << std::endl;

      bool del = false;
      unsigned int tmppos = pos;
      int32_t wiretype;

      if constexpr (std::is_constructible<T, char *, int64_t>::value) // meant probably for std::strings
      {
        std::pair<unsigned char *, uint64_t> l_data = getFieldData(num, &wiretype, &tmppos);
        T tmp(reinterpret_cast<char *>(l_data.first), l_data.second);

        //std::cout << "Created tmp1: " << tmp << std::endl;

        if (tmp == *value)
          del = true;
      }
      else if constexpr (std::is_same<T, std::pair<char *, uint64_t>>::value)
      {
        std::pair<unsigned char *, uint64_t> l_data = getFieldData(num, &wiretype, &tmppos);
        if (value->second == l_data.second && std::memcmp(reinterpret_cast<char *>(value->first), l_data.first, l_data.second) == 0)
          del = true;
      }
      else if constexpr (std::is_same<T, std::pair<unsigned char *, uint64_t>>::value)
      {
        std::pair<unsigned char *, uint64_t> l_data = getFieldData(num, &wiretype, &tmppos);
        if (value->second == l_data.second && std::memcmp(value->first, l_data.first, l_data.second) == 0)
          del = true;
      }
      else if constexpr (std::is_base_of<ProtoBufParserBase, T>::value)
      {
        //std::cout << "YO666" << std::endl;
        std::pair<unsigned char *, uint64_t> l_data = getFieldData(num, &wiretype, &tmppos);
        T tmp(l_data.first, l_data.second);
        if (tmp == *value)
          del = true;
      }
      else if constexpr (std::is_integral<T>::value)
      {
        std::pair<unsigned char *, uint64_t> l_data = getFieldData(num, &wiretype, &tmppos);
        if (wiretype == WIRETYPE::VARINT)
        {
          unsigned int lpos = 0;
          T vint = readVarInt(&lpos, l_data.first, l_data.second, false);
          if (vint == *value)
            del = true;
        }
        else // fixed numerical (int32 (enum), int64, float or double)
        {
          T tmp = 0;
          std::memcpy(reinterpret_cast<char *>(&tmp), reinterpret_cast<char *>(l_data.first), l_data.second);
          if (tmp == *value)
            del = true;
        }
      }

      if (del)
      {
        //std::cout << "GOT HIT!" << std::endl;
      }
      else
      {
        //std::cout << "First find is no hit, looping!" << std::endl;
        startpos = pos + fieldlength;
        pos = -1;
        fieldlength = -1;
        continue;
      }
    }

    // std::cout << "Got field " << num << " at pos " << pos << " (length " << fieldlength << ")" << std::endl;

    unsigned char *newdata = new unsigned char[d_size - fieldlength];
    std::memcpy(newdata, d_data, pos);
    std::memcpy(newdata + pos, d_data + pos + fieldlength, d_size - (pos + fieldlength));

    delete[] d_data;
    d_data = newdata;
    d_size = d_size - fieldlength;

    //std::cout << "After delete" << std::endl;
    //std::cout << "DATA:  " << bepaald::bytesToHexString(d_data, d_size) << std::endl;

    return true;
  }
  return false;
}

template <int idx>
inline constexpr uint64_t ProtoBufParserBase::fieldSize() // static
{
  if constexpr (idx <= 0xf)
    return 1;
  if constexpr (idx <= 0x7ff)
    return 2;
  if constexpr (idx <= 0x3ffff)
    return 3;
  if constexpr (idx <= 0x1ffffff)
    return 4;
  if constexpr (idx <= 0xffffffff)
    return 5;
  if constexpr (idx <= 0x7fffffffff)
    return 6;
  if constexpr (idx <= 0x3fffffffffff)
    return 7;
  if constexpr (idx <= 0x1fffffffffffff)
    return 8;
  if constexpr (idx <= 0xfffffffffffffff)
    return 9;
  return 10;
}

inline uint64_t ProtoBufParserBase::varIntSize(uint64_t value) // static
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

int64_t ProtoBufParserBase::readVarInt(unsigned int *pos, unsigned char const *data, unsigned int size, bool zigzag) // static
{
  uint64_t value = 0;
  uint64_t times = 0;
  while (*pos < size && (data[*pos]) & 0b10000000)
    value |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << (times++ * 7));
  value |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << (times * 7));

  if (zigzag)
    value = ((value >> 1) ^ (~(value & 1) + 1));

  return static_cast<int64_t>(value);
}

int64_t ProtoBufParserBase::getVarIntFieldLength(int pos, unsigned char const *data, int size) // static
{
  int64_t length = 0;
  while (pos < size && (data[pos]) & 0b10000000)
  {
    ++length;
    ++pos;
  }
  return ++length;
}

std::pair<unsigned char *, uint64_t> ProtoBufParserBase::getFieldData(int num, int32_t *wiretype) const
{
  unsigned int pos = 0;
  return getFieldData(num, wiretype, &pos);
}

std::pair<unsigned char *, uint64_t> ProtoBufParserBase::getFieldData(int num, int32_t *wiretype, unsigned int *pos) const
{
  while (*pos < d_size)
  {
    //std::cout << "AT: " << *pos << " : " << "0x" << std::hex << static_cast<int>(d_data[*pos] & 0xff) << std::dec << std::endl;
    int32_t field = (d_data[*pos] & 0b0111'1000) >> 3;
    *wiretype     =  d_data[*pos] & 0b0000'0111;
    int fieldshift = 4;
    while (d_data[*pos] & 0b1000'0000 && // skipping the shift
           *pos < d_size - 1)
    {
      //std::cout << "Adding byte to varint field number" << std::endl;
      field |= (d_data[++(*pos)] & 0b0111'1111) << fieldshift;
      fieldshift += 7;
    }
    // std::cout << "field: " << field << std::endl;
    // std::cout << "wiret: " << wiretype << std::endl;

    ++(*pos);
    switch (*wiretype)
    {
      case WIRETYPE::LENGTH_DELIMITED:
      {
        uint64_t fieldlength = readVarInt(pos, d_data, d_size);
        if (field == num)
          return std::make_pair(d_data + *pos, fieldlength);
        *pos += fieldlength;
        break;
      }
      case WIRETYPE::VARINT:
      {
        //std::cout << "AT: " << *pos << " : " << "0x" << std::hex << static_cast<int>(d_data[*pos] & 0xff) << std::dec << std::endl;
        uint64_t fieldlength = getVarIntFieldLength(*pos, d_data, d_size);
        if (field == num)
          return std::make_pair(d_data + *pos, fieldlength);
        *pos += fieldlength;
        break;
      }
      case WIRETYPE::FIXED64:
      {
        if (field == num)
          return std::make_pair(d_data + *pos, 8);
        *pos += 8;
        break;
      }
      case WIRETYPE::FIXED32:
      {
        if (field == num)
          return std::make_pair(d_data + *pos, 4);
        *pos += 4;
        break;
      }
      case WIRETYPE::STARTGROUP:
      {
        if (field == num)
          Logger::warning("Skipping startgroup for now");
        break;
      }
      case WIRETYPE::ENDGROUP:
      {
        if (field == num)
          Logger::warning("Skipping endgroup for now");
        break;
      }
    }
  }
  return std::pair<unsigned char *, int64_t>(nullptr, 0);
}

inline void ProtoBufParserBase::getPosAndLengthForField(int num, int startpos, int64_t *pos, int64_t *fieldlength) const
{
  int64_t localpos = startpos;
  while (localpos < static_cast<int64_t>(d_size))
  {
    int32_t field    = (d_data[localpos] & 0b0111'1000) >> 3;
    int32_t wiretype =  d_data[localpos] & 0b0000'0111;
    int fieldshift = 4;
    int64_t localpos2 = localpos;
    while (localpos2 < static_cast<int64_t>(d_size) - 1 &&
           d_data[localpos2] & 0b10000000) // skipping the shift
    {
      field |= (d_data[++localpos2] & 0b01111111) << fieldshift;
      fieldshift += 7;
    }
    unsigned int nextpos = static_cast<uint64_t>(localpos2) + 1;

    //std::cout << "F: " << field << std::endl;
    //std::cout << "W: " << wiretype << std::endl;

    switch (wiretype)
    {
      case WIRETYPE::LENGTH_DELIMITED:
      {
        int64_t localfieldlength = readVarInt(&nextpos, d_data, d_size);
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + nextpos - localpos;
          return;
        }
        localpos = nextpos + localfieldlength;
        break;
      }
      case WIRETYPE::VARINT:
      {
        int64_t localfieldlength = getVarIntFieldLength(nextpos, d_data, d_size);
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + nextpos - localpos;
          return;
        }
        localpos = nextpos + localfieldlength;
        break;
      }
      case WIRETYPE::FIXED64:
      {
        int64_t localfieldlength = 8;
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + nextpos - localpos;
          return;
        }
        localpos = nextpos + localfieldlength;
        break;
      }
      case WIRETYPE::FIXED32:
      {
        int64_t localfieldlength = 4;
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + nextpos - localpos;
          return;
        }
        localpos = nextpos + localfieldlength;
        break;
      }
      case WIRETYPE::STARTGROUP:
      {
        if (field == num)
          Logger::warning("Skipping startgroup for now (deprecated)");
        break;
      }
      case WIRETYPE::ENDGROUP:
      {
        if (field == num)
          Logger::warning("Skipping endgroup for now (deprecated)");
        break;
      }
      default:
      {
        Logger::error("Unknown wiretype: ", wiretype);
        return;
      }
    }
  }
}

bool ProtoBufParserBase::fieldExists(int num) const
{
  unsigned int pos = 0;
  while (pos < d_size)
  {
    int32_t field    = (d_data[pos] & 0b00000000000000000000000001111000) >> 3;
    int32_t wiretype = d_data[pos] & 0b00000000000000000000000000000111;
    int fieldshift = 4;
    while (pos < d_size - 1 &&
           d_data[pos] & 0b00000000000000000000000010000000) // skipping the shift
    {
      field |= (d_data[++pos] & 0b00000000000000000000000001111111) << fieldshift;
      fieldshift += 7;
    }

    if (field == num)
      return true;

    ++pos;

    switch (wiretype)
    {
      case WIRETYPE::LENGTH_DELIMITED:
      {
        uint64_t fieldlength = readVarInt(&pos, d_data, d_size);
        pos += fieldlength;
        break;
      }
      case WIRETYPE::VARINT:
      {
        uint64_t fieldlength = getVarIntFieldLength(pos, d_data, d_size);
        pos += fieldlength;
        break;
      }
      case WIRETYPE::FIXED64:
      {
        pos += 8;
        break;
      }
      case WIRETYPE::FIXED32:
      {
        pos += 4;
        break;
      }
      case WIRETYPE::STARTGROUP: // deprecated/not implemented yet
      case WIRETYPE::ENDGROUP: // deprecated/not implemented yet
      {
        break;
      }
    }
  }
  return false;
}

// template <typename T>
// constexpr void printType()
// {
//   std::string_view name = __PRETTY_FUNCTION__;
//   std::string_view prefix = "constexpr auto type_name() [with T = ";
//   std::string_view suffix = "]";

//   name.remove_prefix(prefix.size());
//   name.remove_suffix(suffix.size());
//   std::cout << name << std::endl;
// }

// for optional
template <typename T, bool asview>
inline auto ProtoBufParserBase::getFieldAs(int num) const
{

  typedef typename std::conditional<asview,
                                    typename ProtoBufParserReturn::item_return_view<T, false>::type,
                                    typename ProtoBufParserReturn::item_return<T, false>::type>::type ReturnType;
  typedef typename std::conditional<asview,
                                    typename ProtoBufParserReturn::item_return_view<T, false>::type::value_type,
                                    typename ProtoBufParserReturn::item_return<T, false>::type::value_type>::type ReturnType_held;

  // std::cout << "TYPE: " << std::endl;
  // printType<T>();
  // printType<ReturnType>();

  int32_t wiretype;
  std::pair<unsigned char *, uint64_t> fielddata(getFieldData(num, &wiretype));
  if (fielddata.first)
  {

    if constexpr (std::is_base_of<ProtoBufParserBase, T>::value) // this handles std::string and ProtoBufParser<U...> ?
    {
      //if constexpr (asview)
      return ReturnType({fielddata.first, fielddata.second, asview});
      //else
      //  return ReturnType({fielddata.first, fielddata.second});
    }
    else if constexpr (std::is_same<T, std::string>::value)
    {
      //if constexpr (asview)
      return ReturnType({reinterpret_cast<char *>(fielddata.first), fielddata.second});
      //else
      //  return ReturnType({reinterpret_cast<char *>(fielddata.first), fielddata.second});
    }
    else if constexpr (std::is_same<T, unsigned char *>::value)
    {
      if constexpr (asview)
        return ReturnType({fielddata.first, fielddata.second});
      else
      {
        std::unique_ptr<unsigned char []> tmp(new unsigned char[fielddata.second]);
        std::memcpy(tmp.get(), fielddata.first, fielddata.second);
        return ReturnType({std::move(tmp), fielddata.second});
      }
    }
    else // some numerical type (double / float / (u)int32/64 / bool / Enum)
    {
      if (wiretype == WIRETYPE::VARINT) [[likely]] // wiretype was varint -> raw data needs to be decoded into the actual number
      {
        if constexpr (std::is_same<T, ZigZag32>::value || std::is_same<T, ZigZag64>::value)
        {
          unsigned int pos = 0;
          return ReturnType({static_cast<ReturnType_held>(readVarInt(&pos, fielddata.first, fielddata.second, true))});
        }
        else
        {
          unsigned int pos = 0;
          return ReturnType({static_cast<ReturnType_held>(readVarInt(&pos, fielddata.first, fielddata.second))});
        }
      }
      else
      {
        if constexpr (std::is_same<T, Fixed32>::value || std::is_same<T, Fixed64>::value ||
                      std::is_same<T, SFixed32>::value || std::is_same<T, SFixed64>::value)
        {
          if (sizeof(ReturnType_held) == fielddata.second) [[likely]]
          {
            ReturnType_held result; // ie.: uint32_t result; (stripped off std::optional
            std::memcpy(reinterpret_cast<char *>(&result), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            return std::make_optional<ReturnType_held>(std::move(result));
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (1)");
          }

        }
        else if constexpr (!std::is_same<T, ZigZag32>::value && !std::is_same<T, ZigZag64>::value) // float and double and bool...
        {
          if (sizeof(ReturnType_held) == fielddata.second) [[likely]]
          {
            ReturnType_held result; // ie.: uint32_t result; (stripped off std::optional
            std::memcpy(reinterpret_cast<char *>(&result), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            return std::make_optional<ReturnType_held>(std::move(result));
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (2): ", fielddata.second, " ", sizeof(T));
          }
        }
      }
    }
  }
  return ReturnType{};
}

// for repeated
template <typename T, bool asview>
inline auto ProtoBufParserBase::getFieldsAs(int num) const
{
  typedef typename ProtoBufParserReturn::item_return<T, true>::type::value_type HeldType_valuetype;
  typedef typename std::conditional<asview,
                                    typename ProtoBufParserReturn::item_return_view<T, true>::type,          // == for example, for repeated::BYTES -> std::vector<std::pair<unsigned char *, size_t>>
                                    typename ProtoBufParserReturn::item_return<T, true>::type>::type ReturnType; // == for example, for repeated::BYTES -> std::vector<std::pair<std::unique_ptr<unsined char []>, size_t>>
  typedef typename std::conditional<asview,
                                    typename ProtoBufParserReturn::item_return_view<T, true>::type::value_type,
                                    typename ProtoBufParserReturn::item_return<T, true>::type::value_type>::type ReturnType_held;

  // std::cout << "TYPE: " << std::endl;
  // printType<T>();
  // printType<HeldType_valuetype>();
  // printType<ReturnType>();
  // printType<ReturnType_held>();

  unsigned int pos = 0;
  ReturnType result;
  while (true)
  {
    int32_t wiretype;
    std::pair<unsigned char *, uint64_t> fielddata(getFieldData(num, &wiretype, &pos));
    if (!fielddata.first)
      break;

    if constexpr (std::is_base_of<ProtoBufParserBase, HeldType_valuetype>::value) // this handles std::string and ProtoBufParser<U...> ?
    {
      //if constexpr (asview)
      result.emplace_back(ReturnType_held{fielddata.first, fielddata.second, asview});
      //else
      //  result.emplace_back(ReturnType_held{fielddata.first, fielddata.second});
    }
    else if constexpr (std::is_same<HeldType_valuetype, std::string>::value)
    {
      //if constexpr (asview)
      result.emplace_back(ReturnType_held{reinterpret_cast<char *>(fielddata.first), fielddata.second});
      //else
      //  result.emplace_back(ReturnType_held{reinterpret_cast<char *>(fielddata.first), fielddata.second});
    }
    else if constexpr (std::is_same<T, std::vector<unsigned char *>>::value)
    {
      if constexpr (asview)
        result.emplace_back(std::make_pair(fielddata.first, fielddata.second));
      else
      {
        std::unique_ptr<unsigned char []> tmp(new unsigned char[fielddata.second]);
        std::memcpy(tmp.get(), fielddata.first, fielddata.second);
        result.emplace_back(std::make_pair(std::move(tmp), fielddata.second));
      }
    }
    else // maybe check return type is numerical? if constexpr (held_type == numerical type);
    {
      if (wiretype == WIRETYPE::VARINT) [[likely]] // wiretype was varint -> raw data needs to be decoded into the actual number
      {
        if constexpr (std::is_same<typename T::value_type, ZigZag32>::value ||
                      std::is_same<typename T::value_type, ZigZag64>::value)
        {
          unsigned int pos2 = 0;
          result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, true));
        }
        else
        {
          unsigned int pos2 = 0;
          result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second));
        }
      }
      else // wiretype not varint -> [S]FIXED[32|64]
      {
        if (sizeof(ReturnType_held) == fielddata.second &&
            (wiretype == WIRETYPE::FIXED64 || wiretype == WIRETYPE::FIXED32)) [[likely]] // [float/double/fixed32/fixed64]
        {
          ReturnType_held fixednumerical;
          std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first), fielddata.second);
          result.push_back(fixednumerical);
        }
        else
        {
          // if any type, that is normally VARINT ([U|S]INT[32|64]+BOOL), is in a LENGTH_DELIMITED field
          // and is a repeated::-type, this indicates a 'packed' field: the wiretype and length are omitted
          // after the first value, instead the length indicates the total length of the pack, and values
          // are concatenated one after the other...
          if (wiretype == WIRETYPE::LENGTH_DELIMITED)
          {
            // Logger::message("Data: ", bepaald::bytesToHexString(fielddata), "(size: ", fielddata.second, ")");
            unsigned int pos2 = 0;
            while (pos2 < fielddata.second)
            {
              if constexpr (std::is_same<typename T::value_type, ZigZag32>::value ||
                            std::is_same<typename T::value_type, ZigZag64>::value)
                //std::cout << "Readin varint from fielddata... pos: " << pos2 << " total length: " << fielddata.second << std::endl;
                result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, true));
              else if constexpr (std::is_same<typename T::value_type, Fixed32>::value ||
                                 std::is_same<typename T::value_type, SFixed32>::value ||
                                 std::is_same<typename T::value_type, float>::value)
              {
                if (fielddata.second < pos2 + 4) [[unlikely]]
                  break;
                ReturnType_held fixednumerical; // could be int32, int64, float or double
                std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first + pos2), 4);
                pos2 += 4;
                result.push_back(fixednumerical);
              }
              else if constexpr (std::is_same<typename T::value_type, Fixed64>::value ||
                                 std::is_same<typename T::value_type, SFixed64>::value ||
                                 std::is_same<typename T::value_type, double>::value)
              {
                if (fielddata.second < pos2 + 8) [[unlikely]]
                  break;
                ReturnType_held fixednumerical; // could be int32, int64, float or double
                std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first + pos2), 8);
                pos2 += 8;
                result.push_back(fixednumerical);
              }
              else // VARINT ([S|U]INT[32|64]/BOOL)
                result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, false));
              //std::cout << "GOT : " << result.back() << std::endl;
              //std::cout << "Pos now: " << pos2 << std::endl;
            }
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (3). Field data size: ", fielddata.second);
            Logger::error_indent("Field data: ", bepaald::bytesToHexString(fielddata));
          }
        }
      }
    }
    pos += fielddata.second;
  }
  return result;
}

// // for repeated view
// template <typename T>
// inline typename ProtoBufParserReturn::item_return_view<T, true>::type ProtoBufParserBase::getFieldsViewAs(int num) const
// {
//   typename ProtoBufParserReturn::item_return_view<T, true>::type result; // == for example, for repeated::BYTES -> std::vector<std::pair<unsigned char *, size_t>>
//   typedef typename ProtoBufParserReturn::item_return_view<T, true>::type::value_type held_type; //  == for example, for repeated::BYTES -> std::pair<unsigned char *, size_t>

//   unsigned int pos = 0;
//   while (true)
//   {
//     int32_t wiretype;
//     std::pair<unsigned char *, uint64_t> fielddata(getFieldData(num, &wiretype, &pos));
//     if (fielddata.first)
//     {
//       if constexpr (std::is_base_of<ProtoBufParserBase, held_type>::value) // this handles std::string and ProtoBufParser<U...> ?
//         result.emplace_back(fielddata.first, fielddata.second, true);
//       else if constexpr (std::is_same<held_type, ProtoBufParserReturn::item_return_view<protobuffer::repeated::BYTES, true>::type::value_type>::value)
//         result.emplace_back(fielddata.first, fielddata.second);
//       else if constexpr (std::is_constructible<held_type, char *, int64_t>::value)
//         result.emplace_back(held_type(reinterpret_cast<char *>(fielddata.first), fielddata.second));
//       else if constexpr (std::is_constructible<held_type, unsigned char *, int64_t>::value)
//          result.emplace_back(held_type(fielddata.first, fielddata.second));
//       else // maybe check return type is numerical? if constexpr (held_type == numerical type);
//       {
//         if (wiretype == WIRETYPE::VARINT) [[likely]] // wiretype was varint -> raw data needs to be decoded into the actual number
//         {
//           if constexpr (std::is_same<typename T::value_type, ZigZag32>::value ||
//                         std::is_same<typename T::value_type, ZigZag64>::value)
//           {
//             unsigned int pos2 = 0;
//             result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, true));
//           }
//           else
//           {
//             unsigned int pos2 = 0;
//             result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second));
//           }
//         }
//         else // wiretype not varint -> [S]FIXED[32|64]
//         {
//           if (sizeof(held_type) == fielddata.second &&
//               (wiretype == WIRETYPE::FIXED64 || wiretype == WIRETYPE::FIXED32)) [[likely]] // [float/double/fixed32/fixed64]
//           {
//             held_type fixednumerical;
//             std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first), fielddata.second);
//             result.push_back(fixednumerical);
//           }
//           else
//           {
//             // if any type, that is normally VARINT ([U|S]INT[32|64]+BOOL), is in a LENGTH_DELIMITED field
//             // and is a repeated::-type, this indicates a 'packed' field: the wiretype and length are omitted
//             // after the first value, instead the length indicates the total length of the pack, and values
//             // are concatenated one after the other...
//             if (wiretype == WIRETYPE::LENGTH_DELIMITED)
//             {
//               // Logger::message("Data: ", bepaald::bytesToHexString(fielddata), "(size: ", fielddata.second, ")");
//               unsigned int pos2 = 0;
//               while (pos2 < fielddata.second)
//               {
//                 if constexpr (std::is_same<typename T::value_type, ZigZag32>::value ||
//                               std::is_same<typename T::value_type, ZigZag64>::value)
//                   //std::cout << "Readin varint from fielddata... pos: " << pos2 << " total length: " << fielddata.second << std::endl;
//                   result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, true));
//                 else if constexpr (std::is_same<typename T::value_type, Fixed32>::value ||
//                                    std::is_same<typename T::value_type, SFixed32>::value ||
//                                    std::is_same<typename T::value_type, float>::value)
//                 {
//                   if (fielddata.second < pos2 + 4) [[unlikely]]
//                     break;
//                   held_type fixednumerical; // could be int32, int64, float or double
//                   std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first + pos2), 4);
//                   pos2 += 4;
//                   result.push_back(fixednumerical);
//                 }
//                 else if constexpr (std::is_same<typename T::value_type, Fixed64>::value ||
//                                    std::is_same<typename T::value_type, SFixed64>::value ||
//                                    std::is_same<typename T::value_type, double>::value)
//                 {
//                   if (fielddata.second < pos2 + 8) [[unlikely]]
//                     break;
//                   held_type fixednumerical; // could be int32, int64, float or double
//                   std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first + pos2), 8);
//                   pos2 += 8;
//                   result.push_back(fixednumerical);
//                 }
//                 else // VARINT ([S|U]INT[32|64]/BOOL)
//                   result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, false));
//                 //std::cout << "GOT : " << result.back() << std::endl;
//                 //std::cout << "Pos now: " << pos2 << std::endl;
//               }
//             }
//             else
//             {
//               Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (3). Field data size: ", fielddata.second);
//               Logger::error_indent("Field data: ", bepaald::bytesToHexString(fielddata));
//             }
//           }
//         }
//       }
//       pos += fielddata.second;
//     }
//     else
//       break;
//   }
//   return result;
// }

#endif
