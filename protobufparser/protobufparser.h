/*
  Copyright (C) 2019-2024  Selwin van Dijk

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
#include "../common_be.h"
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
{};

struct ZigZag64
{};

struct Fixed32
{
  uint32_t value;
};

struct Fixed64
{
  uint64_t value;
};

struct SFixed32
{
  int32_t value;
};

struct SFixed64
{
  int64_t value;
};

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
    typedef Fixed32 FIXED32;
    typedef Fixed64 FIXED64;
    typedef SFixed32 SFIXED32;
    typedef SFixed64 SFIXED64;
    typedef bool BOOL;
    typedef std::string STRING;
    typedef unsigned char *BYTES;
    typedef int DUMMY;
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
    typedef std::vector<Fixed32> FIXED32;
    typedef std::vector<Fixed64> FIXED64;
    typedef std::vector<SFixed32> SFIXED32;
    typedef std::vector<SFixed64> SFIXED64;
    typedef std::vector<bool> BOOL;
    typedef std::vector<std::string> STRING;
    typedef std::vector<unsigned char *> BYTES;
    typedef int DUMMY;
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
  struct item_return<std::vector<Fixed32>, true>{ typedef std::vector<uint32_t> type; };
  template <>
  struct item_return<std::vector<Fixed64>, true>{ typedef std::vector<uint64_t> type; };
  template <>
  struct item_return<std::vector<SFixed32>, true>{ typedef std::vector<int32_t> type; };
  template <>
  struct item_return<std::vector<SFixed64>, true>{ typedef std::vector<int64_t> type; };
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

 public:
  inline ProtoBufParser();
  inline ProtoBufParser(std::string const &base64);
  explicit ProtoBufParser(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data);
  explicit ProtoBufParser(unsigned char *data, int64_t size);
  inline ProtoBufParser(ProtoBufParser const &other);
  inline ProtoBufParser &operator=(ProtoBufParser const &other);
  inline ProtoBufParser(ProtoBufParser &&other);
  inline ProtoBufParser &operator=(ProtoBufParser &&other);
  ~ProtoBufParser();
  inline bool operator==(ProtoBufParser const &other) const;
  inline bool operator!=(ProtoBufParser const &other) const;

  inline int64_t size() const;
  inline unsigned char *data() const;
  inline std::string getDataString() const;
  inline void setData(std::string const &base64);
  inline void setData(unsigned char *data, int64_t size);

  template <typename T>
  inline typename ProtoBufParserReturn::item_return<T, false>::type getFieldAs(int num) const;

  template <typename T>
  inline typename ProtoBufParserReturn::item_return<T, true>::type getFieldsAs(int num) const; // T must be std::vector<Something>

  template <int idx>
  inline auto getField() const -> typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type;

  template <typename T = std::nullptr_t>
  int deleteFields(int num, T const *value = nullptr);

  template <typename T = std::nullptr_t>
  bool deleteFirstField(int num, T const *value = nullptr);

  // add repeated things
  template <unsigned int idx>
  inline bool addField(typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type::value_type const &value);
  template <unsigned int idx>
  inline typename std::enable_if<std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BYTES>::value, bool>::type addField(std::pair<unsigned char *, uint64_t> const &value); // specialization for repeated BYTES

  // add optional things
  template <unsigned int idx>
  inline bool addField(typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type const &value);
  template <unsigned int idx>
  inline typename std::enable_if<std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, protobuffer::optional::BYTES>::value, bool>::type addField(std::pair<unsigned char *, uint64_t> const &value); // specialization for optional BYTES

  inline void print(int indent = 0) const;

 protected:
  inline void clear();

 private:
  template <unsigned int idx, typename T>
  inline bool addFieldInternal(T const &value);
  int64_t readVarInt(int *pos, unsigned char *data, int size, bool zigzag = false) const;
  int64_t getVarIntFieldLength(int pos, unsigned char *data, int size) const;
  std::pair<unsigned char *, int64_t> getField(int num, bool *isvarint) const;
  std::pair<unsigned char *, int64_t> getField(int num, bool *isvarint, int *pos) const;
  void getPosAndLengthForField(int num, int startpos, int *pos, int *fieldlength) const;
  bool fieldExists(int num) const;
  template <int idx>
  inline constexpr uint64_t fieldSize() const;
  inline uint64_t varIntSize(uint64_t value) const;
  template <int idx>
  static inline constexpr unsigned int getType();

  // printing this horrorshow...
  template<std::size_t N, typename Indices = std::make_index_sequence<N>>
  inline void printHelper1(int indent) const;
  template<std::size_t... I>
  inline void printHelper2(std::index_sequence<I...>, int indent) const;
  template<std::size_t Idx>
  struct printHelperWrapper
  {
    void printHelper3(ProtoBufParser const *ptr, int indent) const
    {
      ptr->printHelper4<Idx>(indent);
    }
  };
  template<std::size_t Idx>
  inline void printHelper4(int indent) const;
  template<std::size_t idx>
  inline void printSingle(int indent, std::string const &typestring) const;
  template<std::size_t idx, typename T>
  inline void printRepeated(int indent, std::string const &typestring) const;

};

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser()
  :
  d_data(nullptr),
  d_size(0)
{}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(ProtoBufParser const &other)
  :
  d_data(nullptr),
  d_size(0)
{
  d_data = new unsigned char[other.d_size];
  if (other.d_data)
    std::memcpy(d_data, other.d_data, other.d_size);
  d_size = other.d_size;
}

template <typename... Spec>
inline ProtoBufParser<Spec...> &ProtoBufParser<Spec...>::operator=(ProtoBufParser const &other)
{
  if (this != &other)
  {
    if (d_data)
      bepaald::destroyPtr(&d_data, &d_size);

    d_data = new unsigned char[other.d_size];
    if (other.d_data)
      std::memcpy(d_data, other.d_data, other.d_size);
    d_size = other.d_size;
  }
  return *this;
}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(ProtoBufParser &&other)
  :
  d_data(other.d_data),
  d_size(other.d_size)
{
  other.d_data = nullptr;
  other.d_size = 0;
}

template <typename... Spec>
inline ProtoBufParser<Spec...> &ProtoBufParser<Spec...>::operator=(ProtoBufParser &&other)
{
  if (this != &other)
  {
    if (d_data)
      bepaald::destroyPtr(&d_data, &d_size);

    d_data = other.d_data;
    d_size = other.d_size;

    other.d_data = nullptr;
    other.d_size = 0;
  }
  return *this;
}

template <typename... Spec>
ProtoBufParser<Spec...>::ProtoBufParser(std::string const &base64)
  :
  d_data(nullptr),
  d_size(0)
{
  std::pair<unsigned char *, size_t> data = Base64::base64StringToBytes(base64);
  d_data = data.first;
  d_size = data.second;

  //std::cout << "INPUT: " << bepaald::bytesToHexString(d_data, d_size) << std::endl;
}

template <typename... Spec>
ProtoBufParser<Spec...>::ProtoBufParser(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data)
  :
  ProtoBufParser(data.first.get(), data.second)
{}

template <typename... Spec>
ProtoBufParser<Spec...>::ProtoBufParser(unsigned char *data, int64_t size)
  :
  d_data(nullptr),
  d_size(0)
{
  d_data = new unsigned char[size];
  if (data)
    std::memcpy(d_data, data, size);
  d_size = size;
}

template <typename... Spec>
ProtoBufParser<Spec...>::~ProtoBufParser()
{
  bepaald::destroyPtr(&d_data, &d_size);
}


template <typename... Spec>
inline void ProtoBufParser<Spec...>::clear()
{
  bepaald::destroyPtr(&d_data, &d_size);
}

template <typename... Spec>
inline bool ProtoBufParser<Spec...>::operator==(ProtoBufParser const &other) const
{
  return d_size == other.d_size &&
    std::memcmp(d_data, other.d_data, d_size) == 0;
}

template <typename... Spec>
inline bool ProtoBufParser<Spec...>::operator!=(ProtoBufParser const &other) const
{
  return d_size != other.d_size ||
    std::memcmp(d_data, other.d_data, d_size) != 0;
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::setData(std::string const &base64)
{
  // destroy old
  if (d_data)
    delete[] d_data;

  std::pair<unsigned char *, size_t> data = Base64::base64StringToBytes(base64);

  d_data = data.first;
  d_size = data.second;
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::setData(unsigned char *data, int64_t size)
{
  // destroy old
  if (d_data)
    delete[] d_data;

  d_data = new unsigned char[size];
  if (data)
    std::memcpy(d_data, data, size);
  d_size = size;
}

template <typename... Spec>
inline int64_t ProtoBufParser<Spec...>::size() const
{
  return d_size;
}

template <typename... Spec>
inline unsigned char *ProtoBufParser<Spec...>::data() const
{
  return d_data;
}

template <typename... Spec>
inline std::string ProtoBufParser<Spec...>::getDataString() const
{
  if (d_size)
    return Base64::bytesToBase64String(d_data, d_size);
  return std::string();
}

// for optional?
template <typename... Spec>
template <typename T>
inline typename ProtoBufParserReturn::item_return<T, false>::type ProtoBufParser<Spec...>::getFieldAs(int num) const
{
  bool varint = false;
  std::pair<unsigned char *, int64_t> fielddata(getField(num, &varint));
  if (fielddata.first)
  {
    if constexpr (std::is_constructible<T, char *, int64_t>::value) // this handles std::string and ProtoBufParser<U...> ?
      return T(reinterpret_cast<char *>(fielddata.first), fielddata.second);
    else if constexpr (std::is_constructible<T, unsigned char *, int64_t>::value)
      return T(fielddata.first, fielddata.second);
    else if constexpr (std::is_same<T, char *>::value) // binary blob
      return std::pair<char *, int64_t>{reinterpret_cast<char *>(fielddata.first), fielddata.second};
    else if constexpr (std::is_same<T, unsigned char *>::value)
      return fielddata;
    else // some numerical type (double / float / (u)int32/64 / bool / Enum)
    {
      if (varint) [[likely]] // wiretype was varint -> raw data needs to be decoded into the actual number
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
        if constexpr (std::is_same<T, Fixed32>::value || std::is_same<T, Fixed64>::value ||
                      std::is_same<T, SFixed32>::value || std::is_same<T, SFixed64>::value)
        {
          if (sizeof(T) == fielddata.second) [[likely]]
          {
            typename ProtoBufParserReturn::item_return<T, false>::type::value_type result; // ie.: uint32_t result; (stripped off std::optional
            std::memcpy(reinterpret_cast<char *>(&result), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            return result;
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (1)");
          }

        }
        else if constexpr (!std::is_same<T, ZigZag32>::value && !std::is_same<T, ZigZag64>::value)
        {
          if (sizeof(T) == fielddata.second) [[likely]]
          {
            T result;
            std::memcpy(reinterpret_cast<char *>(&result), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            return result;
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (2): ", fielddata.second, " ", sizeof(T));
          }
        }
      }
    }
  }
  return {};
}

// for repeated
template <typename... Spec>
template <typename T>
inline typename ProtoBufParserReturn::item_return<T, true>::type ProtoBufParser<Spec...>::getFieldsAs(int num) const
{
  typename ProtoBufParserReturn::item_return<T, true>::type result; // == for example, for repeated::BYTES -> std::vector<std::pair<unsigned char *, size_t>>
  int pos = 0;
  while (true)
  {
    bool varint = false;
    std::pair<unsigned char *, int64_t> fielddata(getField(num, &varint, &pos));
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
        if (varint) [[likely]] // wiretype was varint -> raw data needs to be decoded into the actual number
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
          if (sizeof(typename ProtoBufParserReturn::item_return<T, true>::type::value_type) == fielddata.second) [[likely]]
          {
            typename ProtoBufParserReturn::item_return<T, true>::type::value_type fixednumerical; // could be int32, int64, float or double
            std::memcpy(reinterpret_cast<char *>(&fixednumerical), reinterpret_cast<char *>(fielddata.first), fielddata.second);
            result.push_back(fixednumerical);
          }
          else
          {
            Logger::error("ProtoBufParser: REQUESTED TYPE TOO SMALL (3)");
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
template <int idx>
inline auto ProtoBufParser<Spec...>::getField() const -> typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type
{
  if constexpr (!is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
    return getFieldAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>(idx);
  else
    return getFieldsAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>(idx);
}

template <typename... Spec>
template <typename T>
int ProtoBufParser<Spec...>::deleteFields(int num, T const *value)
{
  int deleted = 0;
  while (deleteFirstField(num, value))
    ++deleted;
  return deleted;
}

template <typename... Spec>
template <typename T>
bool ProtoBufParser<Spec...>::deleteFirstField(int num, T const *value [[maybe_unused]])
{
  int startpos = 0;
  int pos = -1;
  int fieldlength = -1;

  while (startpos < d_size)
  {
    getPosAndLengthForField(num, startpos, &pos, &fieldlength);

    if (pos == -1 || fieldlength == -1)
      return false;

    //std::cout << "DATA: " << bepaald::bytesToHexString(d_data, d_size) << std::endl;
    //std::cout << "Got requested field at pos " << pos << " (length " << fieldlength << ")" << std::endl;
    //std::cout << "FIELD: " << bepaald::bytesToHexString(d_data + pos, fieldlength) << std::endl;

    if constexpr (!std::is_same<T, std::nullptr_t>::value)
    {

      //std::cout << "Asked to delete specific: " << *value << std::endl;

      bool del = false;
      int tmppos = pos;
      bool isvarint = false;

      if constexpr (std::is_constructible<T, char *, int64_t>::value) // meant for probably for std::strings
      {
        std::pair<unsigned char *, uint64_t> data = getField(num, &isvarint, &tmppos);
        T tmp(reinterpret_cast<char *>(data.first), data.second);

        //std::cout << "Created tmp1: " << tmp << std::endl;

        if (tmp == *value)
          del = true;
      }
      else if constexpr (std::is_same<T, std::pair<char *, uint64_t>>::value)
      {
        std::pair<unsigned char *, uint64_t> data = getField(num, &isvarint, &tmppos);
        if (value->second == data.second && std::memcmp(reinterpret_cast<char *>(value->first), data.first, data.second) == 0)
          del = true;
      }
      else if constexpr (std::is_same<T, std::pair<unsigned char *, uint64_t>>::value)
      {
        std::pair<unsigned char *, uint64_t> data = getField(num, &isvarint, &tmppos);
        if (value->second == data.second && std::memcmp(value->first, data.first, data.second) == 0)
          del = true;
      }
      else if constexpr (is_specialization_of<ProtoBufParser, T>::value)
      {
        //std::cout << "YO666" << std::endl;
        std::pair<unsigned char *, uint64_t> data = getField(num, &isvarint, &tmppos);
        T tmp(data.first, data.second);
        if (tmp == *value)
          del = true;
      }
      else if constexpr (std::is_integral<T>::value)
      {
        std::pair<unsigned char *, uint64_t> data = getField(num, &isvarint, &tmppos);
        if (isvarint)
        {
          int lpos = 0;
          T vint = readVarInt(&lpos, data.first, data.second, false); // zigzag not (yet) supported
          if (vint == *value)
            del = true;
        }
        else // fixed numerical (int32 (enum), int64, float or double)
        {
          T tmp = 0;
          std::memcpy(reinterpret_cast<char *>(&tmp), reinterpret_cast<char *>(data.first), data.second);
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

// field is different from varint because first byte only has 4 bits left...
template <typename... Spec>
template <int idx>
inline constexpr uint64_t ProtoBufParser<Spec...>::fieldSize() const
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

template <typename... Spec>
inline uint64_t ProtoBufParser<Spec...>::varIntSize(uint64_t value) const
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

template <typename... Spec>
template <int idx>
inline constexpr unsigned int ProtoBufParser<Spec...>::getType() //static
{
  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                             protobuffer::optional::STRING>::value ||
                std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                             protobuffer::repeated::STRING>::value ||
                std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                             protobuffer::optional::BYTES>::value ||
                std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                             protobuffer::repeated::BYTES>::value)
    return WIRETYPE::LENGTH_DELIMITED;
  else if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::ENUM>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::ENUM>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::INT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::INT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::INT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::INT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::UINT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::UINT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::UINT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::UINT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::SINT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::SINT32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::SINT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::SINT64>::value)
    return WIRETYPE::VARINT;
  else if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::FLOAT>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::FLOAT>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::FIXED32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::FIXED32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::SFIXED32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::SFIXED32>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::BOOL>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::BOOL>::value)
    return WIRETYPE::FIXED32;
  else if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::DOUBLE>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::DOUBLE>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::FIXED64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::FIXED64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::optional::SFIXED64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                                  protobuffer::repeated::SFIXED64>::value)
    return WIRETYPE::FIXED64;
  else if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
    return WIRETYPE::LENGTH_DELIMITED;
  else if constexpr (is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
    if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type::value_type>{})
      return WIRETYPE::LENGTH_DELIMITED;

}

template <typename... Spec>
template <unsigned int idx, typename T>
inline bool ProtoBufParser<Spec...>::addFieldInternal(T const &value)
{
  unsigned int field = idx;
  unsigned int constexpr type = getType<idx>();
  unsigned int fielddatasize = 0;
  if constexpr (type == WIRETYPE::LENGTH_DELIMITED)
  {
    if constexpr (is_specialization_of<std::pair, T>{}) // bytes
      fielddatasize = value.second;
    else // string
      fielddatasize = value.size();
  }
  else if constexpr (type == WIRETYPE::VARINT)
    fielddatasize = varIntSize(value);
  else if constexpr (type == WIRETYPE::FIXED32)
    fielddatasize = 4;
  else if constexpr (type == WIRETYPE::FIXED64)
    fielddatasize = 8;

  int size = fieldSize<idx>() + (type == WIRETYPE::LENGTH_DELIMITED ? varIntSize(fielddatasize) : 0) + fielddatasize;
  unsigned char *mem = new unsigned char[size]{};

  // set first byte of field (+ wire)
  unsigned int mempos = 0;
  mem[mempos] = (field << 3) & 0b00000000'00000000'00000000'01111000;
  mem[mempos] |= (type);

  field >>= 4; // shift out the used part of idx
  while (field)
  {
    // previous byte add 0b1000000;
    mem[mempos++] |= 0b10000000;
    mem[mempos] = field & 0b00000000'00000000'00000000'01111111;
    field >>= 7; // shift out the used part of idx
  }

  // put length (as varint) if type is length_delim, or put actual value if type is varint
  ++mempos;
  if constexpr (type == WIRETYPE::LENGTH_DELIMITED || type == WIRETYPE::VARINT)
  {
    uint64_t varint = 0;
    if constexpr (type == WIRETYPE::LENGTH_DELIMITED)
      varint = fielddatasize;
    else if constexpr (type == WIRETYPE::VARINT)
      varint = value;
    while (varint > 127)
    {
      mem[mempos] = (static_cast<uint8_t>(varint & 127)) | 128;
      varint >>= 7;
      ++mempos;
    }
    mem[mempos++] = (static_cast<uint8_t>(varint)) & 127;
  }

  // put actual data
  if constexpr (type == WIRETYPE::LENGTH_DELIMITED)
  {
    if constexpr (is_specialization_of<std::pair, T>{}) // bytes
      std::memcpy(mem + mempos, value.first, fielddatasize);
    else // string
      std::memcpy(mem + mempos, value.data(), fielddatasize);
  }
  else if constexpr (type == WIRETYPE::FIXED32 || type == WIRETYPE::FIXED64)
    std::memcpy(mem + mempos, reinterpret_cast<unsigned char const *>(&value), sizeof(value));

  //std::cout << "Adding: " << bepaald::bytesToHexString(mem, size) << std::endl;

  // build the new protobuf data, by copying old plus new
  unsigned char *newdata = new unsigned char[d_size + size];
  if (d_data) // when creating a new ProtoBuf from nothing, d_data starts empty, which is UB for memcpy
    std::memcpy(newdata, d_data, d_size);
  std::memcpy(newdata + d_size, mem, size);

  delete[] mem;
  if (d_data)
    delete[] d_data;
  d_data = newdata;
  d_size = d_size + size;

  //std::cout << "OUTPUT: " << bepaald::bytesToHexString(d_data, d_size) << std::endl;

  return true;
}

// add repeated things
template <typename... Spec>
template <unsigned int idx>
inline bool ProtoBufParser<Spec...>::addField(typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type::value_type const &value)
{
  //std::cout << "Repeated -> go!" << std::endl;
  return addFieldInternal<idx>(value);
}

// specialization for repeated BYTES
template <typename... Spec>
template <unsigned int idx>
inline typename std::enable_if<std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BYTES>::value, bool>::type ProtoBufParser<Spec...>::addField(std::pair<unsigned char *, uint64_t> const &value)
{
  //std::cout << "Repeated -> go!" << std::endl;
  return addFieldInternal<idx>(value);
}

// add optional things
template <typename... Spec>
template <unsigned int idx>
inline bool ProtoBufParser<Spec...>::addField(typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type const &value)
{
  //std::cout << "Optional -> go!" << std::endl;

  if (fieldExists(idx))
  {
    //std::cout << "FIELD NOT REPEATED AND ALREADY SET! NOT ADDING!" << std::endl;
    return false;
  }
  return addFieldInternal<idx>(value);
}

// specialization for optional BYTES
template <typename... Spec>
template <unsigned int idx>
inline typename std::enable_if<std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, protobuffer::optional::BYTES>::value, bool>::type ProtoBufParser<Spec...>::addField(std::pair<unsigned char *, uint64_t> const &value)
{
  //std::cout << "Optional -> go!" << std::endl;

  if (fieldExists(idx))
  {
    //std::cout << "FIELD NOT REPEATED AND ALREADY SET! NOT ADDING!" << std::endl;
    return false;
  }
  return addFieldInternal<idx>(value);
}

template <typename... Spec>
int64_t ProtoBufParser<Spec...>::readVarInt(int *pos, unsigned char *data, int size, bool zigzag) const
{
  uint64_t value = 0;
  uint64_t times = 0;
  while ((data[*pos]) & 0b10000000 && *pos < size)
    value |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << (times++ * 7));
  value |= ((static_cast<uint64_t>(data[(*pos)++]) & 0b01111111) << (times * 7));

  if (zigzag)
    value = -(value & 1) ^ (value >> 1);

  return value;
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
void ProtoBufParser<Spec...>::getPosAndLengthForField(int num, int startpos, int *pos, int *fieldlength) const
{
  int localpos = startpos;
  while (localpos < d_size)
  {
    int32_t field    = (d_data[localpos] & 0b00000000000000000000000001111000) >> 3;
    int32_t wiretype = d_data[localpos] & 0b00000000000000000000000000000111;
    int fieldshift = 4;
    unsigned int localpos2 = localpos;
    while (d_data[localpos2] & 0b00000000000000000000000010000000 && // skipping the shift
           localpos2 < d_size - 1)
    {
      field |= (d_data[++localpos2] & 0b00000000000000000000000001111111) << fieldshift;
      fieldshift += 7;
    }
    int nextpos = localpos2 + 1;

    switch (wiretype)
    {
      case WIRETYPE::LENGTH_DELIMITED:
      {
        uint64_t localfieldlength = readVarInt(&nextpos, d_data, d_size);
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
        uint64_t localfieldlength = getVarIntFieldLength(nextpos, d_data, d_size);
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
        uint64_t localfieldlength = 8;
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + 1;
          return;
        }
        localpos = nextpos + localfieldlength;
        break;
      }
      case WIRETYPE::FIXED32:
      {
        uint64_t localfieldlength = 4;
        if (field == num)
        {
          *pos = localpos;
          *fieldlength = localfieldlength + 1;
          return;
        }
        localpos = nextpos + localfieldlength;
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
      default:
      {
        Logger::error("Unknown wiretype: ", wiretype);
        return;
        break;
      }
    }
  }
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
    //std::cout << "AT: " << *pos << " : " << "0x" << std::hex << static_cast<int>(d_data[*pos] & 0xff) << std::dec << std::endl;
    int32_t field    = (d_data[*pos] & 0b00000000000000000000000001111000) >> 3;
    int32_t wiretype = d_data[*pos] & 0b00000000000000000000000000000111;
    int fieldshift = 4;
    while (d_data[*pos] & 0b00000000000000000000000010000000 && // skipping the shift
           *pos < d_size - 1)
    {
      //std::cout << "Adding byte to varint field number" << std::endl;
      field |= (d_data[++(*pos)] & 0b00000000000000000000000001111111) << fieldshift;
      fieldshift += 7;
    }
    // std::cout << "field: " << field << std::endl;
    // std::cout << "wiret: " << wiretype << std::endl;

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

template <typename... Spec>
bool ProtoBufParser<Spec...>::fieldExists(int num) const
{
  int pos = 0;
  while (pos < d_size)
  {
    int32_t field    = (d_data[pos] & 0b00000000000000000000000001111000) >> 3;
    int32_t wiretype = d_data[pos] & 0b00000000000000000000000000000111;
    int fieldshift = 4;
    while (d_data[pos] & 0b00000000000000000000000010000000 && // skipping the shift
           pos < d_size - 1)
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
      {
        break;
      }
      case WIRETYPE::ENDGROUP: // deprecated/not implemented yet
      {
        break;
      }
    }
  }
  return false;
}

template <typename... Spec>
template<std::size_t idx>
inline void ProtoBufParser<Spec...>::printSingle(int indent, std::string const &typestring) const
{
  auto tmp = getField<idx + 1>();
  if (tmp.has_value())
    Logger::message(std::string(indent, ' '), "Field ", idx + 1, " ", typestring, ": ", tmp.value());
}

template <typename... Spec>
template<std::size_t idx, typename T>
inline void ProtoBufParser<Spec...>::printRepeated(int indent, std::string const &typestring) const
{
  std::vector<T> tmp = getField<idx + 1>();
  for (uint i = 0; i < tmp.size(); ++i)
    Logger::message(std::string(indent, ' '), "Field ", idx + 1, " ", typestring, " (", i + 1 , "/", tmp.size(), "): ", tmp[i]);
  return;
}

template <typename... Spec>
template<std::size_t idx>
inline void ProtoBufParser<Spec...>::printHelper4(int indent) const
{
  // std::cout << "Dealing with field " << idx + 1 << std::endl;
  if (!fieldExists(idx + 1))
  {
    // std::cout << "Not present" << std::endl;
    return;
  }

  //std::cout << std::string(indent, ' ') << "Dealing with field " << idx + 1 << std::endl;

  // SINGLE

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::STRING>::value)
    return printSingle<idx>(indent, "(optional::string)");
  // {
  //   auto tmp = getField<idx + 1>();
  //   if (tmp.has_value())
  //     std::cout << std::string(indent, ' ') << "Field " << idx + 1 << " (optional::STRING): " << tmp.value() << std::endl;
  //   return;
  // }

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::ENUM>::value)
    return printSingle<idx>(indent, "(optional::enum)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::INT32>::value)
    return printSingle<idx>(indent, "(optional::int32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::INT64>::value)
    return printSingle<idx>(indent, "(optional::int32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::UINT32>::value)
    return printSingle<idx>(indent, "(optional::uint32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::UINT64>::value)
    return printSingle<idx>(indent, "(optional::uint64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::SINT32>::value)
    return printSingle<idx>(indent, "(optional::sint32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::SINT64>::value)
    return printSingle<idx>(indent, "(optional::sint64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::FLOAT>::value)
    return printSingle<idx>(indent, "(optional::float)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::FIXED32>::value)
    return printSingle<idx>(indent, "(optional::fixed32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::SFIXED32>::value)
    return printSingle<idx>(indent, "(optional::sfixed32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::DOUBLE>::value)
    return printSingle<idx>(indent, "(optional::double)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::FIXED64>::value)
    return printSingle<idx>(indent, "(optional::fixed64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::SFIXED64>::value)
    return printSingle<idx>(indent, "(optional::sfixed64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::BOOL>::value)
  {
    auto tmp = getField<idx + 1>();
    if (tmp.has_value())
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::bool): ", std::boolalpha, tmp.value());
    return;
  }

  // bytes
  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::BYTES>::value)
  {
    std::optional<std::pair<unsigned char *, size_t>> tmp = getField<idx + 1>();
    if (tmp.has_value())
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::bytes[", tmp.value().second, "]): ", bepaald::bytesToHexString(tmp.value().first, tmp.value().second));
    return;
  }

  // single protobuffer
  if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
  {
    if (getField<idx + 1>().has_value())
    {
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::protobuf): ");
      return getField<idx + 1>().value().print(indent + 2);
    }
  }


  // REPEATED

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::STRING>::value)
    return printRepeated<idx, std::string>(indent, "(repeated::string)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::ENUM>::value)
    return printRepeated<idx, int32_t>(indent, "(repeated::enum)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::INT32>::value)
    return printRepeated<idx, int32_t>(indent, "(repeated::int32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::INT64>::value)
    return printRepeated<idx, int64_t>(indent, "(repeated::int64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::UINT32>::value)
    return printRepeated<idx, uint32_t>(indent, "(repeated::uint32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::UINT64>::value)
    return printRepeated<idx, uint64_t>(indent, "(repeated::uint64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::SINT32>::value)
    return printRepeated<idx, int32_t>(indent, "(repeated::sint32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::SINT64>::value)
    return printRepeated<idx, int64_t>(indent, "(repeated::sint64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::FLOAT>::value)
    return printRepeated<idx, float>(indent, "(repeated::float)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::FIXED32>::value)
    return printRepeated<idx, uint32_t>(indent, "(repeated::fixed32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::SFIXED32>::value)
    return printRepeated<idx, int32_t>(indent, "(repeated::sfixed32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::DOUBLE>::value)
    return printRepeated<idx, double>(indent, "(repeated::double)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::FIXED64>::value)
    return printRepeated<idx, uint64_t>(indent, "(repeated::fixed64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::SFIXED64>::value)
    return printRepeated<idx, int64_t>(indent, "(repeated::sfixed64)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BOOL>::value)
  {
    std::vector<bool> tmp = getField<idx + 1>();
    for (uint i = 0; i < tmp.size(); ++i)
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::bool) (", i + 1 , "/", tmp.size(), "): ", std::boolalpha, tmp[i]);
    return;
  }

  // bytes
  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BYTES>::value)
  {
    std::vector<std::pair<unsigned char *, uint64_t>> tmp = getField<idx + 1>();
    for (uint i = 0; i < tmp.size(); ++i)
    {
      std::pair<unsigned char *, size_t> tmp2 = tmp[i];
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::bytes) (", i + 1 , "/", tmp.size(), "): ",
                      bepaald::bytesToHexString(tmp2.first, tmp2.second));
      return;
    }
    return;
  }

  // protobuf message
  if constexpr (is_vector<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
    if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type::value_type>{})
    {
      auto tmp = getField<idx + 1>();
      for (uint i = 0; i < tmp.size(); ++i)
      {
        Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::protobuf) (", i + 1 , "/", tmp.size(), "): ");
        tmp.at(i).print(indent + 2);
      }
      return;
    }

  Logger::message(std::string(indent, ' '), "Field ", idx + 1, ": (unhandled protobuf type)");
}

template <typename... Spec>
template<std::size_t... Idx>
inline void ProtoBufParser<Spec...>::printHelper2(std::index_sequence<Idx...>, int indent [[maybe_unused]]) const
{
  (printHelperWrapper<Idx>().printHelper3(this, indent), ...);
}

template <typename... Spec>
template<std::size_t N, typename Indices>
inline void ProtoBufParser<Spec...>::printHelper1(int indent) const
{
  printHelper2(Indices(), indent);
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::print(int indent) const
{
  printHelper1<sizeof...(Spec)>(indent);
}

#endif
