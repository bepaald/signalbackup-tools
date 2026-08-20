/*
  Copyright (C) 2019-2026  Selwin van Dijk

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

#include "../protobufparserbase/protobufparserbase.h"

template <typename... Spec>
class ProtoBufParser : public ProtoBufParserBase
{
  template <typename... Spec2> friend class ProtoBufParser;
 private:
  template <bool viewonly, int idx>
  static inline auto constexpr getDeepType();
  template <bool viewonly, int idx, int idx2, int... rest>
  static inline auto constexpr getDeepType();

 public:
  inline ProtoBufParser() = default;
  inline explicit ProtoBufParser(std::string const &base64);
  inline explicit ProtoBufParser(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data);
  inline ProtoBufParser(unsigned char const *data, uint64_t size);
  inline ProtoBufParser(unsigned char *data, uint64_t size, ProtoBufParserBase::MEMTYPE viewonly) : ProtoBufParserBase(data, size, viewonly) {};
  inline ProtoBufParser(ProtoBufParser const &other) = default;
  inline ProtoBufParser &operator=(ProtoBufParser const &other) = default;
  inline explicit ProtoBufParser(ProtoBufParser &&other) noexcept = default;
  inline ProtoBufParser &operator=(ProtoBufParser &&other) noexcept = default;
  inline ~ProtoBufParser() = default;

  template <int idx>
  inline auto getField() const -> typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type;
  template <int idx>
  inline auto getFieldView() const -> typename ProtoBufParserReturn::item_return_view<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type;
  template <int idx, int idx2, int... rest>
  inline auto getField() const;// -> decltype(getDeepType<idx, idx2, rest...>());
  template <int idx, int idx2, int... rest>
  inline auto getFieldView() const;// -> decltype(getDeepType<idx, idx2, rest...>());

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

  inline void checkBufferFields(std::string_view pb_messagename, std::vector<int> &fields) const;
  inline void checkBufferFields(std::string_view pb_messagename = std::string_view()) const;

 private:
  template <unsigned int idx, typename T>
  inline bool addFieldInternal(T const &value);
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
inline ProtoBufParser<Spec...>::ProtoBufParser(std::string const &base64)
  :
  ProtoBufParserBase(base64)
{}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(std::pair<std::shared_ptr<unsigned char []>, size_t> const &data)
  :
  ProtoBufParserBase(data.first.get(), data.second)
{}

template <typename... Spec>
inline ProtoBufParser<Spec...>::ProtoBufParser(unsigned char const *data, uint64_t size)
  :
  ProtoBufParserBase(data, size)
{}

template <typename... Spec>
template <int idx>
inline auto ProtoBufParser<Spec...>::getField() const -> typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type
{
  if constexpr (!is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
    return getFieldAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, false>(idx);
  else
    return getFieldsAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, false>(idx);
}

template <typename... Spec>
template <int idx>
inline auto ProtoBufParser<Spec...>::getFieldView() const -> typename ProtoBufParserReturn::item_return_view<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type
{
  if constexpr (!is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
    return getFieldAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, true>(idx);
  else
    return getFieldsAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, true>(idx);
  //return getFieldsViewAs<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>(idx);
}

template <typename... Spec>
template <int idx, int idx2, int... rest>
inline auto ProtoBufParser<Spec...>::getField() const// -> decltype(getDeepType<idx, idx2, rest...>())
{
  static_assert(is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}, "Trying to recurse into flat type");

  auto firstfield = getFieldView<idx>();
  if (firstfield.has_value())
    return firstfield->template getField<idx2, rest...>();

  return getDeepType<false, idx, idx2, rest...>();
}

template <typename... Spec>
template <int idx, int idx2, int... rest>
inline auto ProtoBufParser<Spec...>::getFieldView() const// -> decltype(getDeepType<idx, idx2, rest...>())
{
  static_assert(is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}, "Trying to recurse into flat type");

  auto firstfield = getFieldView<idx>();
  if (firstfield.has_value())
    return firstfield->template getFieldView<idx2, rest...>();

  return getDeepType<true, idx, idx2, rest...>();
}

template <typename... Spec>
template <bool viewonly, int idx>
inline auto constexpr ProtoBufParser<Spec...>::getDeepType()
{
  if constexpr (viewonly)
    return typename ProtoBufParserReturn::item_return_view<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type{};
  else
    return typename ProtoBufParserReturn::item_return<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type, is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}>::type{};
  // if constexpr (is_vector<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{})
  //   return typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type{};
  // return std::optional<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{};
}

template <typename... Spec>
template <bool viewonly, int idx, int idx2, int... rest>
inline auto constexpr ProtoBufParser<Spec...>::getDeepType() //static
{
  static_assert(is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>{}, "Trying to recurse into flat type");

  // Wowsers!
  return decltype(std::declval<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type>().template getDeepType<viewonly, idx2, rest...>()){};
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
                     protobuffer::repeated::SINT64>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                     protobuffer::optional::BOOL>::value ||
                     std::is_same<typename std::remove_reference<decltype(std::get<idx - 1>(std::tuple<Spec...>()))>::type,
                     protobuffer::repeated::BOOL>::value)
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
                     protobuffer::repeated::SFIXED32>::value)
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
  {
    if constexpr (std::is_same<T, ZigZag32>::value)
      fielddatasize = varIntSize((static_cast<uint32_t>(value) << 1) ^ static_cast<uint32_t>(value >> 31));
    else if constexpr (std::is_same<T, ZigZag64>::value)
      fielddatasize = varIntSize((static_cast<uint64_t>(value) << 1) ^ static_cast<uint64_t>(value >> 63));
    else
      fielddatasize = varIntSize(value);
  }
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
    {
      if constexpr (std::is_same<T, ZigZag32>::value)
        varint = ((static_cast<uint32_t>(value) << 1) ^ static_cast<uint32_t>(value >> 31));
      else if constexpr (std::is_same<T, ZigZag64>::value)
        varint = ((static_cast<uint64_t>(value) << 1) ^ static_cast<uint64_t>(value >> 63));
      else
        varint = value;
    }
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
    else // string(_view)
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
inline void ProtoBufParser<Spec...>::checkBufferFields(std::string_view pb_messagename, std::vector<int> &fields) const
{
  std::vector<int> presentfields;
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

    //std::cout << "Found field " << field << std::endl;
    presentfields.push_back(field);

    if (static_cast<uint64_t>(field) > sizeof...(Spec)) [[unlikely]]
    {
      Logger::warning("Unknown field in ", (pb_messagename.empty() ? "protobuffer" : pb_messagename), " at: ", fields, (fields.size() ? "," : ""), field);
    }

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

  // iterate known fields,
  // if vector of ProtobufParser -> recurse all...
  // if ProtobufParser -> recurse
  // if dummy -> check present
  bepaald::constexpr_for<0, sizeof...(Spec), 1>([&](auto idx)
  {
    if constexpr (is_vector<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
    {
      if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type::value_type>{})
      {
        auto rec = getFieldView<idx + 1>();
        fields.push_back(idx + 1);
        for (unsigned int i = 0; i < rec.size(); ++i)
          rec[i].checkBufferFields(pb_messagename, fields);
        fields.pop_back();
      }
    }
    else if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
    {
      //std::cout << "YES PROTOBUFFER" << std::endl;
      auto rec = getFieldView<idx + 1>();
      if (rec.has_value())
      {
        //std::cout << "With value!" << std::endl;
        fields.push_back(idx + 1);
        rec->checkBufferFields(pb_messagename, fields);
        fields.pop_back();
      }
    }
    else if constexpr (std::is_same<protobuffer::DUMMY, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>::value)
    {
      if (std::find(presentfields.begin(), presentfields.end(), idx + 1) != presentfields.end()) [[unlikely]]
        Logger::warning("Unknown DUMMY with value in ", (pb_messagename.empty() ? "protobuffer" : pb_messagename), " at: ", fields, (fields.size() ? "," : ""), idx + 1);
    }
  });
}

template <typename... Spec>
inline void ProtoBufParser<Spec...>::checkBufferFields(std::string_view pb_messagename) const
{
  std::vector<int> fields;
  checkBufferFields(pb_messagename, fields);
}

template <typename... Spec>
template<std::size_t idx>
inline void ProtoBufParser<Spec...>::printSingle(int indent, std::string const &typestring) const
{
  auto tmp = getFieldView<idx + 1>();
  if (tmp.has_value())
    Logger::message(std::string(indent, ' '), "Field ", idx + 1, " ", typestring, ": ", tmp.value());
}

template <typename... Spec>
template <std::size_t idx, typename T>
inline void ProtoBufParser<Spec...>::printRepeated(int indent, std::string const &typestring) const
{
  std::vector<T> tmp = getFieldView<idx + 1>();
  for (unsigned int i = 0; i < tmp.size(); ++i)
    Logger::message(std::string(indent, ' '), "Field ", idx + 1, " ", typestring, " (", i + 1 , "/", tmp.size(), "): ", tmp[i]);
  return;
}

template <typename... Spec>
template <std::size_t idx>
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
  //   auto tmp = getFieldView<idx + 1>();
  //   if (tmp.has_value())
  //     std::cout << std::string(indent, ' ') << "Field " << idx + 1 << " (optional::STRING): " << tmp.value() << std::endl;
  //   return;
  // }

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::INT32>::value)
    return printSingle<idx>(indent, "(optional::int32)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::INT64>::value)
    return printSingle<idx>(indent, "(optional::int64)");

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

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::ENUM>::value)
    return printSingle<idx>(indent, "(optional::enum)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::BOOL>::value)
  {
    auto tmp = getFieldView<idx + 1>();
    if (tmp.has_value())
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::bool): ", std::boolalpha, tmp.value());
    return;
  }

  // bytes
  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::optional::BYTES>::value)
  {
    std::optional<std::pair<unsigned char *, size_t>> tmp = getFieldView<idx + 1>();
    if (tmp.has_value())
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::bytes[", tmp.value().second, "]): ", bepaald::bytesToHexString(tmp.value().first, tmp.value().second));
    return;
  }

  // single protobuffer
  if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
  {
    if (getFieldView<idx + 1>().has_value())
    {
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (optional::protobuf):");
      return getFieldView<idx + 1>().value().print(indent + 2);
    }
  }

  // REPEATED

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::STRING>::value)
    return printRepeated<idx, std::string_view>(indent, "(repeated::string)");

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

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::ENUM>::value)
    return printRepeated<idx, Enum>(indent, "(repeated::enum)");

  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BOOL>::value)
  {
    std::vector<bool> tmp = getFieldView<idx + 1>();
    for (unsigned int i = 0; i < tmp.size(); ++i)
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::bool) (", i + 1 , "/", tmp.size(), "): ", std::boolalpha, tmp[i]);
    return;
  }

  // bytes
  if constexpr (std::is_same<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type, protobuffer::repeated::BYTES>::value)
  {
    std::vector<std::pair<unsigned char *, uint64_t>> tmp = getFieldView<idx + 1>();
    for (unsigned int i = 0; i < tmp.size(); ++i)
      Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::bytes[", tmp[i].second, "]) (", i + 1 , "/", tmp.size(), "): ",
                      bepaald::bytesToHexString(tmp[i].first, tmp[i].second));
    return;
  }

  // protobuf message
  if constexpr (is_vector<typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type>{})
    if constexpr (is_specialization_of<ProtoBufParser, typename std::remove_reference<decltype(std::get<idx>(std::tuple<Spec...>()))>::type::value_type>{})
    {
      auto tmp = getFieldView<idx + 1>();
      for (unsigned int i = 0; i < tmp.size(); ++i)
      {
        Logger::message(std::string(indent, ' '), "Field ", idx + 1, " (repeated::protobuf) (", i + 1 , "/", tmp.size(), "):");
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


// template <typename... Spec>
// template <int idx, typename... rest>
// inline auto ProtoBufParser<Spec...>::getField2() const
// {
//   auto firstfield = getField<idx>();
//   if (firstfield.has_value())
//     return firstfield.value().getField<rest...>();


// }

#endif
