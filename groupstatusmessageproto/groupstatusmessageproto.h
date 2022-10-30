/*
  Copyright (C) 2021-2022  Selwin van Dijk

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

#ifndef GROUPSTATUSMESSAGEPROTO_H_
#define GROUPSTATUSMESSAGEPROTO_H_

#include "../protobufparser/protobufparser.h"

/*
 * This uses the old(?) V1(?) group status update protobuf
 *
 * /Signal-Android/libsignal/service/src/main/proto/SignalService.proto
 *
 */

/*
message AttachmentPointer {
  enum Flags {
    VOICE_MESSAGE = 1;
  }

  optional fixed64 id          = 1;
  optional string  contentType = 2;
  optional bytes   key         = 3;
  optional uint32  size        = 4;
  optional bytes   thumbnail   = 5;
  optional bytes   digest      = 6;
  optional string  fileName    = 7;
  optional uint32  flags       = 8;
  optional uint32  width       = 9;
  optional uint32  height      = 10;
}
*/
typedef ProtoBufParser<protobuffer::optional::FIXED64, protobuffer::optional::STRING,
                       protobuffer::optional::BYTES, protobuffer::optional::UINT32,
                       protobuffer::optional::BYTES, protobuffer::optional::BYTES,
                       protobuffer::optional::STRING, protobuffer::optional::UINT32,
                       protobuffer::optional::UINT32, protobuffer::optional::UINT32> AttachmentPointer;

/*
message GroupContext {
  enum Type {
    UNKNOWN      = 0;
    UPDATE       = 1;
    DELIVER      = 2;
    QUIT         = 3;
    REQUEST_INFO = 4;
  }
  optional bytes             id      = 1;
  optional Type              type    = 2;
  optional string            name    = 3;
  repeated string            members = 4;
  optional AttachmentPointer avatar  = 5;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM,
                       protobuffer::optional::STRING, protobuffer::repeated::STRING,
                       AttachmentPointer> GroupContext;

#endif
