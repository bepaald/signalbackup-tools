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

#ifndef GROUPV2STATUSMESSAGEPROTO_H_
#define GROUPV2STATUSMESSAGEPROTO_H_

#include "../protobufparser/protobufparser.h"

/*
 * For GroupV2 status messages
 *
 * /Signal-Android/libsignal/service/src/main/proto/Groups.proto
 * /Signal-Android/libsignal/service/src/main/proto/DecryptedGroups.proto
 * /Signal-Android/libsignal/service/src/main/proto/SignalService.proto
 * /Signal-Android/app/src/main/proto/Database.proto
 */


/*
message AccessControl {
  enum AccessRequired {
    UNKNOWN       = 0;
    ANY           = 1;
    MEMBER        = 2;
    ADMINISTRATOR = 3;
    UNSATISFIABLE = 4;
  }

  AccessRequired attributes        = 1;
  AccessRequired members           = 2;
  AccessRequired addFromInviteLink = 3;
}
*/
typedef ProtoBufParser<protobuffer::optional::ENUM, protobuffer::optional::ENUM,
                       protobuffer::optional::ENUM> AccessControl;

/*
message Member {
  enum Role {
    UNKNOWN       = 0;
    DEFAULT       = 1;
    ADMINISTRATOR = 2;
  }

  bytes  userId           = 1;
  Role   role             = 2;
  bytes  profileKey       = 3;
  bytes  presentation     = 4;
  uint32 joinedAtRevision = 5;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM,
                       protobuffer::optional::BYTES, protobuffer::optional::BYTES,
                       protobuffer::optional::UINT32> Member;

/*
message DecryptedMember {
  bytes       uuid             = 1;
  Member.Role role             = 2;
  bytes       profileKey       = 3;
  uint32      joinedAtRevision = 5;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM,
                       protobuffer::optional::BYTES, protobuffer::optional::DUMMY,
                       protobuffer::optional::UINT32> DecryptedMember;

/*
message DecryptedPendingMember {
  bytes       uuid           = 1;
  Member.Role role           = 2;
  bytes       addedByUuid    = 3;
  uint64      timestamp      = 4;
  bytes       uuidCipherText = 5;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM,
                       protobuffer::optional::BYTES, protobuffer::optional::UINT64,
                       protobuffer::optional::BYTES> DecryptedPendingMember;

/*
message DecryptedRequestingMember {
  bytes  uuid       = 1;
  bytes  profileKey = 2;
  uint64 timestamp  = 4;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES,
                       protobuffer::optional::DUMMY, protobuffer::optional::UINT64> DecryptedRequestingMember;

/*
message DecryptedPendingMemberRemoval {
  bytes uuid           = 1;
  bytes uuidCipherText = 2;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES,  protobuffer::optional::BYTES> DecryptedPendingMemberRemoval;

/*
message DecryptedApproveMember {
  bytes       uuid = 1;
  Member.Role role = 2;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM> DecryptedApproveMember;

/*
message DecryptedModifyMemberRole {
  bytes       uuid = 1;
  Member.Role role = 2;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, Member> DecryptedModifyMemberRole;

/*
message DecryptedString {
  string value = 1;
}
*/
typedef ProtoBufParser<protobuffer::optional::STRING> DecryptedString;


/*
message DecryptedTimer {
  uint32 duration = 1;
}
*/
typedef ProtoBufParser<protobuffer::optional::UINT32> DecryptedTimer;

/*
message DecryptedGroupChange {
           bytes                         editor                   = 1;
           uint32                        revision                 = 2;
  repeated DecryptedMember               newMembers               = 3;
  repeated bytes                         deleteMembers            = 4;
  repeated DecryptedModifyMemberRole     modifyMemberRoles        = 5;
  repeated DecryptedMember               modifiedProfileKeys      = 6;
  repeated DecryptedPendingMember        newPendingMembers        = 7;
  repeated DecryptedPendingMemberRemoval deletePendingMembers     = 8;
  repeated DecryptedMember               promotePendingMembers    = 9;
           DecryptedString               newTitle                 = 10;
           DecryptedString               newAvatar                = 11;
           DecryptedTimer                newTimer                 = 12;
           AccessControl.AccessRequired  newAttributeAccess       = 13;
           AccessControl.AccessRequired  newMemberAccess          = 14;
           AccessControl.AccessRequired  newInviteLinkAccess      = 15;
  repeated DecryptedRequestingMember     newRequestingMembers     = 16;
  repeated bytes                         deleteRequestingMembers  = 17;
  repeated DecryptedApproveMember        promoteRequestingMembers = 18;
           bytes                         newInviteLinkPassword    = 19;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::UINT32,
                       std::vector<DecryptedMember>, protobuffer::repeated::BYTES,
                       std::vector<DecryptedModifyMemberRole>, std::vector<DecryptedMember>,
                       std::vector<DecryptedPendingMember>, std::vector<DecryptedPendingMemberRemoval>,
                       std::vector<DecryptedMember>, DecryptedString,
                       DecryptedString, DecryptedTimer,
                       protobuffer::optional::ENUM, protobuffer::optional::ENUM,
                       protobuffer::optional::ENUM, std::vector<DecryptedRequestingMember>,
                       protobuffer::repeated::BYTES, std::vector<DecryptedApproveMember>,
                       protobuffer::optional::BYTES> DecryptedGroupChange;

/*
message DecryptedGroup {
           string                    title                     = 2;
           string                    avatar                    = 3;
           DecryptedTimer            disappearingMessagesTimer = 4;
           AccessControl             accessControl             = 5;
           uint32                    revision                  = 6;
  repeated DecryptedMember           members                   = 7;
  repeated DecryptedPendingMember    pendingMembers            = 8;
  repeated DecryptedRequestingMember requestingMembers         = 9;
           bytes                     inviteLinkPassword        = 10;
}
*/
typedef ProtoBufParser<protobuffer::optional::DUMMY, protobuffer::optional::STRING,
                       protobuffer::optional::STRING, DecryptedTimer,
                       AccessControl, protobuffer::optional::UINT32,
                       std::vector<DecryptedMember>, std::vector<DecryptedPendingMember>,
                       std::vector<DecryptedRequestingMember>, protobuffer::optional::BYTES> DecryptedGroup;

/*
message GroupContextV2 {
  optional bytes  masterKey   = 1;
  optional uint32 revision    = 2;
  optional bytes  groupChange = 3;
}
*/
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::UINT32,
                       protobuffer::optional::BYTES> GroupContextV2;

/*
message DecryptedGroupV2Context {
    signalservice.GroupContextV2 context            = 1;
    DecryptedGroupChange         change             = 2;
    DecryptedGroup               groupState         = 3;
    DecryptedGroup               previousGroupState = 4;
}
*/
typedef ProtoBufParser<GroupContextV2, DecryptedGroupChange,
                       DecryptedGroup, DecryptedGroup> DecryptedGroupV2Context;

#endif
