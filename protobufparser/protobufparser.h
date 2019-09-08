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

#ifndef PROTOBUFPARSER_H_
#define PROTOBUFPARSER_H_

#include <cstring>
#include <memory>
#include <vector>
#include <optional>

#include "../base64/base64.h"

template<typename T>
struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

struct ZigZag32
{};

struct ZigZag64
{};

namespace protobuffer
{
  namespace optional
  {
    typedef double DOUBLE;
    typedef float FLOAT;
    typedef int32_t ENUM;
    typedef int32_t INT32;
    typedef int64_t INT64;
    typedef uint32_t UINT32;
    typedef uint64_t UINT64;
    typedef ZigZag32 SINT32;
    typedef ZigZag64 SINT64;
    typedef uint32_t FIXED32;
    typedef uint64_t FIXED64;
    typedef int32_t SFIXED32;
    typedef int64_t SFIXED64;
    typedef bool BOOL;
    typedef std::string STRING;
    typedef unsigned char * BYTES;
  }
  namespace repeated
  {
    typedef std::vector<double> DOUBLE;
    typedef std::vector<float> FLOAT;
    typedef std::vector<int32_t> ENUM;
    typedef std::vector<int32_t> INT32;
    typedef std::vector<int64_t> INT64;
    typedef std::vector<uint32_t> UINT32;
    typedef std::vector<uint64_t> UINT64;
    typedef std::vector<ZigZag32> SINT32;
    typedef std::vector<ZigZag64> SINT64;
    typedef std::vector<uint32_t> FIXED32;
    typedef std::vector<uint64_t> FIXED64;
    typedef std::vector<int32_t> SFIXED32;
    typedef std::vector<int64_t> SFIXED64;
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
  struct item_return<char *, false>{ typedef std::optional<std::pair<char *, uint64_t>> type; };
  template <>
  struct item_return<unsigned char *, false>{ typedef std::optional<std::pair<unsigned char *, uint64_t>> type; };

  // for vectors:
  template <typename T>
  struct item_return<T, true> { typedef T type; };
  template <>
  struct item_return<std::vector<ZigZag32>, true>{ typedef std::vector<int32_t> type; };
  template <>
  struct item_return<std::vector<ZigZag64>, true>{ typedef std::vector<int64_t> type; };
  template <>
  struct item_return<std::vector<char *>, true>{ typedef std::vector<std::pair<char *, uint64_t>> type; };
  template <>
  struct item_return<std::vector<unsigned char *>, true>{ typedef std::vector<std::pair<unsigned char *, uint64_t>> type; };
}

template <typename... Spec>
class ProtoBufParser
{
  enum WIRETYPE : int
  {
   VARINT = 0,
   FIXED64 = 1,
   LENGTH_DELIMITED = 2,
   STARTGROUP = 3,
   ENDGROUP = 4,
   FIXED32 = 5
  };
  unsigned char *d_data;
  int64_t d_size;
  bool d_own;

 public:
  inline ProtoBufParser();
  ProtoBufParser(std::string const &base64);
  ProtoBufParser(unsigned char *data, int64_t size);
  ProtoBufParser(ProtoBufParser const &other);
  ProtoBufParser(ProtoBufParser &&other);
  ~ProtoBufParser();

  inline void setData(std::string const &base64);
  inline void setData(unsigned char *data, int64_t size);

  template <typename T>
  inline typename ProtoBufParserReturn::item_return<T, false>::type getFieldAs(int num) const;

  template <typename T>
  inline typename ProtoBufParserReturn::item_return<T, true>::type getFieldsAs(int num) const; // T must be std::vector<Something>

  template <int idx>
  inline auto getField() const -> typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type
  {
    if constexpr (!is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
      return getFieldAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>(idx);
    else
      return getFieldsAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>(idx);
  }

 private:
  int64_t readVarInt(int *pos, unsigned char *data, int size, bool zigzag = false) const;
  int64_t getVarIntFieldLength(int pos, unsigned char *data, int size) const;
  std::pair<unsigned char *, int64_t> getField(int num, bool *isvarint) const;
  std::pair<unsigned char *, int64_t> getField(int num, bool *isvarint, int *pos) const;
};

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser()
  :
  d_data(nullptr),
  d_size(0),
  d_own(false)
{}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(ProtoBufParser const &other)
  :
  d_data(other.d_own ? nullptr : other.d_data),
  d_size(other.d_own ? 0 : other.d_size),
  d_own(other.d_own)
{
  if (d_own)
  {
    d_data = new unsigned char[other.d_size];
    std::memcpy(d_data, other.d_data, other.d_size);
    d_size = other.d_size;
  }
}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(ProtoBufParser &&other)
  :
  d_data(other.d_data),
  d_size(other.d_size),
  d_own(other.d_own)
{
  other.d_data = nullptr;
  other.d_size = 0;
  other.d_own = false;
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::setData(std::string const &base64)
{
  // destroy old
  if (d_own && d_data)
    delete[] d_data;

  std::pair<unsigned char *, size_t> data = Base64::base64StringToBytes(base64);

  d_data = data.first;
  d_size = data.second;
  d_own = true;
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::setData(unsigned char *data, int64_t size)
{
  // destroy old
  if (d_own && d_data)
    delete[] d_data;

  d_size = size;
  d_data = data;
  d_own = false;
}


template <typename... Spec>
template <typename T>
inline typename ProtoBufParserReturn::item_return<T, false>::type ProtoBufParser<Spec...>::getFieldAs(int num) const
{
  bool varint = false;
  std::pair<unsigned char *, int64_t> fielddata(std::move(getField(num, &varint)));
  if (fielddata.first)
  {
    if constexpr (std::is_constructible<T, char *, int64_t>::value)
      return T(reinterpret_cast<char *>(fielddata.first), fielddata.second);
    else if constexpr (std::is_constructible<T, unsigned char *, int64_t>::value)
      return T(fielddata.first, fielddata.second);
    else if constexpr (std::is_same<T, char *>::value)
      return std::pair<char *, int64_t>{reinterpret_cast<char *>(fielddata.first), fielddata.second};
    else if constexpr (std::is_same<T, unsigned char *>::value)
      return fielddata;
    else // some numerical type (double / float / (u)int32/64 / bool / Enum)
    {
      if (/*[[likely]]*/ varint) // wiretype was varint -> raw data needs to be decoded into the actual number
      {
        if constexpr (std::is_same<T, ZigZag32>::value || std::is_same<T, ZigZag64>::value)
        {
          int pos = 0;
          return readVarInt(&pos, fielddata.first, fielddata.second, true);
        }
        else
        {
          int pos = 0;
          return readVarInt(&pos, fielddata.first, fielddata.second);
        }
      }
      else
      {
        if constexpr (!std::is_same<T, ZigZag32>::value && !std::is_same<T, ZigZag64>::value)
        {
          if (/*[[likely]]*/ sizeof(T) == fielddata.second)
          {
            T result;
            std::memcpy(reinterpret_cast<char *>(&result), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            return result;
          }
          else
          {
            std::cout << "ERROR REQUESTED TYPE TOO SMALL" << std::endl;
          }
        }
      }
    }
  }
  return {};
}

template <typename... Spec>
template <typename T>
inline typename ProtoBufParserReturn::item_return<T, true>::type ProtoBufParser<Spec...>::getFieldsAs(int num) const
{
  typename ProtoBufParserReturn::item_return<T, true>::type result; // == for example, for repeated::BYTES -> std::vector<std::pair<unsigned char *, size_t>>
  int pos = 0;
  while (true)
  {
    bool varint = false;
    std::pair<unsigned char *, int64_t> fielddata(std::move(getField(num, &varint, &pos)));
    if (fielddata.first)
    {
      if constexpr (std::is_constructible<typename ProtoBufParserReturn::item_return<T, true>::type::value_type, char *, int64_t>::value)
        result.emplace_back(typename ProtoBufParserReturn::item_return<T, true>::type::value_type(reinterpret_cast<char *>(fielddata.first), fielddata.second));
      else if constexpr (std::is_constructible<typename ProtoBufParserReturn::item_return<T, true>::type::value_type, unsigned char *, int64_t>::value)
        result.emplace_back(typename ProtoBufParserReturn::item_return<T, true>::type::value_type(fielddata.first, fielddata.second));
      else if constexpr (std::is_same<typename ProtoBufParserReturn::item_return<T, true>::type::value_type, char *>::value)
        result.emplace_back(std::pair<char *, int64_t>{reinterpret_cast<char *>(fielddata.first), fielddata.second});
      else if constexpr (std::is_same<typename ProtoBufParserReturn::item_return<T, true>::type::value_type, unsigned char *>::value)
        result.emplace_back(fielddata);
      else // maybe check return type is numerical? if constexpr (typename ProtoBufParserReturn::item_return<T, true>::type::value_type == numerical type);
      {
        if (/*[[likely]]*/ varint) // wiretype was varint -> raw data needs to be decoded into the actual number
        {
          if constexpr (std::is_same<typename T::value_type, ZigZag32>::value ||
                        std::is_same<typename T::value_type, ZigZag64>::value)
          {
            int pos2 = 0;
            result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second, true));
          }
          else
          {
            int pos2 = 0;
            result.push_back(readVarInt(&pos2, fielddata.first, fielddata.second));
          }
        }
        else
        {
          if (/*[[likely]]*/ sizeof(typename ProtoBufParserReturn::item_return<T, true>::type::value_type) == fielddata.second)
          {
            typename ProtoBufParserReturn::item_return<T, true>::type::value_type fixednumerical; // could be int32, int64, float or double
            std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            result.push_back(fixednumerical);
          }
          else
          {
            std::cout << "ERROR REQUESTED TYPE TOO SMALL" << std::endl;
          }
        }
      }
      pos += fielddata.second;
    }
    else
      break;
  }
  return result;
}

template <typename... Spec>
ProtoBufParser<Spec...>::ProtoBufParser(std::string const &base64)
  :
  d_data(nullptr),
  d_size(0),
  d_own(true)
{
  std::pair<unsigned char *, size_t> data = Base64::base64StringToBytes(base64);
  d_data = data.first;
  d_size = data.second;
}

template <typename... Spec>
ProtoBufParser<Spec...>::ProtoBufParser(unsigned char *data, int64_t size)
  :
  d_data(data),
  d_size(size),
  d_own(false)
{}

template <typename... Spec>
ProtoBufParser<Spec...>::~ProtoBufParser()
{
  if (d_own && d_data)
    delete[] d_data;
}

template <typename... Spec>
int64_t ProtoBufParser<Spec...>::readVarInt(int *pos, unsigned char *data, int size, bool zigzag) const
{
  uint64_t length = 0;
  uint64_t times = 0;
  while ((data[*pos]) & 0b10000000 && *pos < size)
    length |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << (times += 7));
  length |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << times);

  if (zigzag)
    length = -(length & 1) ^ (length >> 1);

  return length;
}

template <typename... Spec>
int64_t ProtoBufParser<Spec...>::getVarIntFieldLength(int pos, unsigned char *data, int size) const
{
  uint64_t length = 0;
  while ((data[pos]) & 0b10000000 && pos < size)
  {
    ++length;
    ++pos;
  }
  return ++length;
}

template <typename... Spec>
std::pair<unsigned char *, int64_t> ProtoBufParser<Spec...>::getField(int num, bool *isvarint) const
{
  int pos = 0;
  return getField(num, isvarint, &pos);
}

template <typename... Spec>
std::pair<unsigned char *, int64_t> ProtoBufParser<Spec...>::getField(int num, bool *isvarint, int *pos) const
{
  while (*pos < d_size)
  {
    int32_t field    = (d_data[*pos] >> 3) & 0b00000000000000000000000000001111;
    int32_t wiretype = d_data[*pos] & 0b00000000000000000000000000000111;
    ++(*pos);
    switch (wiretype)
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
        *isvarint = true;
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
        //std::cout << "Skipping 64 bit number for now" << std::endl;
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
          std::cout << "Skipping startgroup for now" << std::endl;
        break;
      }
    case WIRETYPE::ENDGROUP:
      {
        if (field == num)
          std::cout << "Skipping endgroup for now" << std::endl;
        break;
      }
    }
  }
  return std::pair<unsigned char *, int64_t>(nullptr, 0);
}

#endif
