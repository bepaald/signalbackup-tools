/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

#ifndef MESSAGERANGEPROTO_H_
#define MESSAGERANGEPROTO_H_

#include "../protobufparser/protobufparser.h"

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
