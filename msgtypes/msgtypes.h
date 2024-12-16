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

#ifndef MSGTYPES_H_
#define MSGTYPES_H_

// see /src/org/thoughtcrime/securesms/database/MmsSmsColumns.java
// app/src/main/java/org/thoughtcrime/securesms/database/MmsSmsColumns.java
// app/src/main/java/org/thoughtcrime/securesms/database/MessageTypes.java

struct Types
{
  static uint64_t constexpr BASE_TYPE_MASK                     = 0x1F;

  static uint64_t constexpr INCOMING_CALL_TYPE                 = 1; // LATER: INCOMING_AUDIO_CALL_TYPE
  static uint64_t constexpr INCOMING_AUDIO_CALL_TYPE           = 1;
  static uint64_t constexpr OUTGOING_CALL_TYPE                 = 2; // LATER: OUTGOING_AUDIO_CALL_TYPE
  static uint64_t constexpr OUTGOING_AUDIO_CALL_TYPE           = 2;
  static uint64_t constexpr MISSED_CALL_TYPE                   = 3; // LATER: MISSED_AUDIO_CALL_TYPE
  static uint64_t constexpr MISSED_AUDIO_CALL_TYPE             = 3;
  static uint64_t constexpr JOINED_TYPE                        = 4;
  static uint64_t constexpr UNSUPPORTED_MESSAGE_TYPE           = 5;
  static uint64_t constexpr INVALID_MESSAGE_TYPE               = 6;
  static uint64_t constexpr PROFILE_CHANGE_TYPE                = 7;
  static uint64_t constexpr MISSED_VIDEO_CALL_TYPE             = 8;
  static uint64_t constexpr GV1_MIGRATION_TYPE                 = 9;
  static uint64_t constexpr INCOMING_VIDEO_CALL_TYPE           = 10;
  static uint64_t constexpr OUTGOING_VIDEO_CALL_TYPE           = 11;
  static uint64_t constexpr GROUP_CALL_TYPE                    = 12;
  static uint64_t constexpr BAD_DECRYPT_TYPE                   = 13;
  static uint64_t constexpr CHANGE_NUMBER_TYPE                 = 14;
  static uint64_t constexpr BOOST_REQUEST_TYPE                 = 15;
  static uint64_t constexpr THREAD_MERGE_TYPE                  = 16;


  static uint64_t constexpr BASE_INBOX_TYPE                    = 20;
  static uint64_t constexpr BASE_OUTBOX_TYPE                   = 21;
  static uint64_t constexpr BASE_SENDING_TYPE                  = 22;
  static uint64_t constexpr BASE_SENT_TYPE                     = 23;
  static uint64_t constexpr BASE_SENT_FAILED_TYPE              = 24;
  static uint64_t constexpr BASE_PENDING_SECURE_SMS_FALLBACK   = 25;
  static uint64_t constexpr BASE_PENDING_INSECURE_SMS_FALLBACK = 26;
  static uint64_t constexpr BASE_DRAFT_TYPE                    = 27;

  static uint64_t constexpr OUTGOING_MESSAGE_TYPES[] = {BASE_OUTBOX_TYPE, BASE_SENT_TYPE,
                                                        BASE_SENDING_TYPE, BASE_SENT_FAILED_TYPE,
                                                        BASE_PENDING_SECURE_SMS_FALLBACK,
                                                        BASE_PENDING_INSECURE_SMS_FALLBACK,
                                                        OUTGOING_CALL_TYPE, OUTGOING_VIDEO_CALL_TYPE};

  static uint64_t constexpr KEY_EXCHANGE_MASK                  = 0xFF00;
  static uint64_t constexpr KEY_EXCHANGE_BIT                   = 0x8000;
  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_VERIFIED_BIT = 0x4000;
  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_DEFAULT_BIT  = 0x2000;
  static uint64_t constexpr KEY_EXCHANGE_CORRUPTED_BIT         = 0x1000;
  static uint64_t constexpr KEY_EXCHANGE_INVALID_VERSION_BIT   = 0x800;
  static uint64_t constexpr KEY_EXCHANGE_BUNDLE_BIT            = 0x400;
  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_UPDATE_BIT   = 0x200;
  static uint64_t constexpr KEY_EXCHANGE_CONTENT_FORMAT        = 0x100;

  static uint64_t constexpr GROUP_UPDATE_BIT            = 0x10000;
  static uint64_t constexpr GROUP_LEAVE_BIT             = 0x20000;
  static uint64_t constexpr GROUP_QUIT_BIT              = GROUP_LEAVE_BIT;
  static uint64_t constexpr EXPIRATION_TIMER_UPDATE_BIT = 0x40000;
  static uint64_t constexpr GROUP_V2_BIT                = 0x80000;
  static uint64_t constexpr GROUP_V2_LEAVE_BITS         = GROUP_V2_BIT | GROUP_LEAVE_BIT | GROUP_UPDATE_BIT;

  static uint64_t constexpr SECURE_MESSAGE_BIT = 0x800000;
  static uint64_t constexpr END_SESSION_BIT    = 0x400000;
  static uint64_t constexpr PUSH_MESSAGE_BIT   = 0x200000;

  static uint64_t constexpr ENCRYPTION_REMOTE_NO_SESSION_BIT = 0x08000000;

  static uint64_t constexpr SPECIAL_TYPES_MASK                     = 0xF00000000L;
  static uint64_t constexpr SPECIAL_TYPE_STORY_REACTION            = 0x100000000L;
  static uint64_t constexpr SPECIAL_TYPE_GIFT_BADGE                = 0x200000000L;
  static uint64_t constexpr SPECIAL_TYPE_PAYMENTS_NOTIFICATION     = 0x300000000L;
  static uint64_t constexpr SPECIAL_TYPE_PAYMENTS_ACTIVATE_REQUEST = 0x400000000L;
  static uint64_t constexpr SPECIAL_TYPE_REPORTED_SPAM             = 0x500000000L;
  static uint64_t constexpr SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED  = 0x600000000L;
  static uint64_t constexpr SPECIAL_TYPE_PAYMENTS_ACTIVATED        = 0x800000000L;
  static uint64_t constexpr SPECIAL_TYPE_PAYMENTS_TOMBSTONE        = 0x900000000L;

  inline static bool isGroupUpdate(uint64_t type)
  {
    return (type & GROUP_UPDATE_BIT) != 0;
  }

  inline static bool isGroupV2(uint64_t type)
  {
    return (type & GROUP_V2_BIT) != 0;
  }

  inline static bool isGroupQuit(uint64_t type)
  {
    return (type & GROUP_QUIT_BIT) != 0;
  }

  inline static bool isOutgoing(uint64_t type)
  {
    for (uint64_t const outgoingType : OUTGOING_MESSAGE_TYPES)
    {
      if ((type & BASE_TYPE_MASK) == outgoingType)
        return true;
    }
    return false;
  }

  inline static bool isInboxType(uint64_t type)
  {
    return (type & BASE_TYPE_MASK) == BASE_INBOX_TYPE;
  }

  inline static bool isCallType(uint64_t type)
  {
    return isIncomingCall(type) ||
      isOutgoingCall(type) ||
      isMissedCall(type) ||
      isIncomingVideoCall(type) ||
      isOutgoingVideoCall(type) ||
      isMissedVideoCall(type) ||
      isGroupCall(type);
  }

  inline static bool isGroupCall(uint64_t type)
  {
    return type == GROUP_CALL_TYPE;
  }

  inline static bool isIncomingCall(uint64_t type)
  {
    return type == INCOMING_CALL_TYPE;
  }

  inline static bool isOutgoingCall(uint64_t type)
  {
    return type == OUTGOING_CALL_TYPE;
  }

  inline static bool isMissedCall(uint64_t type)
  {
    return type == MISSED_CALL_TYPE;
  }

  inline static bool isIncomingVideoCall(uint64_t type)
  {
    return type == INCOMING_VIDEO_CALL_TYPE;
  }

  inline static bool isOutgoingVideoCall(uint64_t type)
  {
    return type == OUTGOING_VIDEO_CALL_TYPE;
  }

  inline static bool isMissedVideoCall(uint64_t type)
  {
    return type == MISSED_VIDEO_CALL_TYPE;
  }

  inline static bool isJoined(long type)
  {
    return (type & BASE_TYPE_MASK) == JOINED_TYPE;
  }

  inline static bool isNumberChange(long type)
  {
    return type == CHANGE_NUMBER_TYPE;
  }

  inline static bool isDonationRequest(long type)
  {
    return type == BOOST_REQUEST_TYPE;
  }

  inline static bool isExpirationTimerUpdate(long type)
  {
    return (type & EXPIRATION_TIMER_UPDATE_BIT) != 0;
  }

  inline static bool isIdentityUpdate(long type)
  {
    return (type & KEY_EXCHANGE_MASK) == KEY_EXCHANGE_IDENTITY_UPDATE_BIT;
  }

  inline static bool isIdentityVerified(long type)
  {
    return (type & KEY_EXCHANGE_MASK) == KEY_EXCHANGE_IDENTITY_VERIFIED_BIT;
  }

  inline static bool isIdentityDefault(long type)
  {
    return (type & KEY_EXCHANGE_MASK) == KEY_EXCHANGE_IDENTITY_DEFAULT_BIT;
  }

  inline static bool isSecureType(long type)
  {
    return (type & SECURE_MESSAGE_BIT) != 0;
  }

  inline static bool isEndSession(long type)
  {
    return (type & END_SESSION_BIT) != 0;
  }

  inline static bool isProfileChange(long type)
  {
    return (type & BASE_TYPE_MASK) == PROFILE_CHANGE_TYPE;
  }

  inline static bool isMessageRequestAccepted(long type)
  {
    return (type & SPECIAL_TYPES_MASK) == SPECIAL_TYPE_MESSAGE_REQUEST_ACCEPTED;
  }

  inline static bool isStatusMessage(long type)
  {
    return isCallType(type) || isGroupUpdate(type) || isGroupV2(type) ||
      isGroupQuit(type) || isIdentityUpdate(type) || isIdentityVerified(type) ||
      isIdentityDefault(type) || isExpirationTimerUpdate(type) || isJoined(type) ||
      isProfileChange(type) || isEndSession(type) || type == Types::GV1_MIGRATION_TYPE ||
      isNumberChange(type) || isDonationRequest(type) || isMessageRequestAccepted(type);
  }
};

#endif
