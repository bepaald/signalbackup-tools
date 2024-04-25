/*
  Copyright (C) 2021-2024  Selwin van Dijk

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
    BORDERLESS    = 2;
    reserved        3;
    GIF           = 4;
  }

  oneof attachment_identifier {
    fixed64        cdnId           = 1;
    string         cdnKey          = 15;
  }
  optional string  contentType       = 2;
  optional bytes   key               = 3;
  optional uint32  size              = 4;
  optional bytes   thumbnail         = 5;
  optional bytes   digest            = 6;
  optional bytes   incrementalDigest = 16;
  optional string  fileName          = 7;
  optional uint32  flags             = 8;
  optional uint32  width             = 9;
  optional uint32  height            = 10;
  optional string  caption           = 11;
  optional string  blurHash          = 12;
  optional uint64  uploadTimestamp   = 13;
  optional uint32  cdnNumber         = 14;
  // Next ID: 17
}
*/
// NOTE WEIRD ORDERING ABOVE
typedef ProtoBufParser<protobuffer::optional::FIXED64, protobuffer::optional::STRING, protobuffer::optional::BYTES,
                       protobuffer::optional::UINT32, protobuffer::optional::STRING, protobuffer::optional::STRING,
                       protobuffer::optional::STRING, protobuffer::optional::UINT32, protobuffer::optional::UINT32,
                       protobuffer::optional::UINT32, protobuffer::optional::STRING, protobuffer::optional::STRING,
                       protobuffer::optional::UINT32, protobuffer::optional::UINT64, protobuffer::optional::STRING,
                       protobuffer::optional::BYTES> AttachmentPointer;

/*
message GroupContext {
  enum Type {
    UNKNOWN      = 0;
    UPDATE       = 1;
    DELIVER      = 2;
    QUIT         = 3;
    REQUEST_INFO = 4;
  }

  message Member2 {
    reserved     / uuid / 1; // removed
    optional string e164  = 2;
  }

  optional bytes             id          = 1;
  optional Type              type        = 2;
  optional string            name        = 3;
  repeated string            membersE164 = 4;
  repeated Member2            members     = 6;
  optional AttachmentPointer avatar      = 5;
}
*/
typedef ProtoBufParser<protobuffer::optional::DUMMY, protobuffer::optional::STRING> Member2;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM, protobuffer::optional::STRING,
                       protobuffer::repeated::STRING, std::vector<Member2>, AttachmentPointer> GroupContext;

#endif
