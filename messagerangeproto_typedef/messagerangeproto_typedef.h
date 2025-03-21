/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

#ifndef MESSAGERANGEPROTO_TYPEDEF_H_
#define MESSAGERANGEPROTO_TYPEDEF_H_

struct ZigZag32;
struct ZigZag64;
struct Fixed32;
struct Fixed64;
struct SFixed32;
struct SFixed64;
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
template <typename... Spec>
class ProtoBufParser;

// protospec (app/src/main/proto/Database.proto):
// message BodyRangeList {
//     message BodyRange {
//         enum Style {
//             BOLD          = 0;
//             ITALIC        = 1;
//             SPOILER       = 2;
//             STRIKETHROUGH = 3;
//             MONOSPACE     = 4;
//         }
//
//         message Button {
//             string label  = 1;
//             string action = 2;
//         }
//
//         int32 start  = 1;
//         int32 length = 2;
//
//         oneof associatedValue {
//             string mentionUuid = 3;
//             Style  style       = 4;
//             string link        = 5;
//             Button button      = 6;
//         }
//     }
//     repeated BodyRange ranges = 1;
// }

typedef ProtoBufParser<protobuffer::optional::INT32, // int32 start
                       protobuffer::optional::INT32, // int32 length
                       protobuffer::optional::STRING,                   // \.
                       protobuffer::optional::ENUM,                     //  \.
                       protobuffer::optional::STRING,                   //   > ONE OF
                       ProtoBufParser<protobuffer::optional::STRING,    //  /
                                      protobuffer::optional::STRING>>   // /
BodyRange;

typedef ProtoBufParser<std::vector<BodyRange>> BodyRanges;

#endif
