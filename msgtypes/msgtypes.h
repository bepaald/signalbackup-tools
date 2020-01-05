/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

#define MSGTYPES_H_

// see /src/org/thoughtcrime/securesms/database/MmsSmsColumns.java

struct Types
{
  static uint64_t constexpr BASE_TYPE_MASK                     = 0x1F;

  static uint64_t constexpr INCOMING_CALL_TYPE                 = 1;
  static uint64_t constexpr OUTGOING_CALL_TYPE                 = 2;
  static uint64_t constexpr MISSED_CALL_TYPE                   = 3;
  static uint64_t constexpr JOINED_TYPE                        = 4;

  static uint64_t constexpr BASE_INBOX_TYPE                    = 20;
  static uint64_t constexpr BASE_OUTBOX_TYPE                   = 21;
  static uint64_t constexpr BASE_SENDING_TYPE                  = 22;
  static uint64_t constexpr BASE_SENT_TYPE                     = 23;
  static uint64_t constexpr BASE_SENT_FAILED_TYPE              = 24;
  static uint64_t constexpr BASE_PENDING_SECURE_SMS_FALLBACK   = 25;
  static uint64_t constexpr BASE_PENDING_INSECURE_SMS_FALLBACK = 26;
  static uint64_t constexpr BASE_DRAFT_TYPE                    = 27;

  static uint64_t constexpr OUTGOING_MESSAGE_TYPES[7] = {BASE_OUTBOX_TYPE, BASE_SENT_TYPE,
                                                         BASE_SENDING_TYPE, BASE_SENT_FAILED_TYPE,
                                                         BASE_PENDING_SECURE_SMS_FALLBACK,
                                                         BASE_PENDING_INSECURE_SMS_FALLBACK,
                                                         OUTGOING_CALL_TYPE};

  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_VERIFIED_BIT = 0x4000;
  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_DEFAULT_BIT  = 0x2000;
  static uint64_t constexpr KEY_EXCHANGE_IDENTITY_UPDATE_BIT   = 0x200;

  static uint64_t constexpr GROUP_UPDATE_BIT            = 0x10000;
  static uint64_t constexpr GROUP_QUIT_BIT              = 0x20000;
  static uint64_t constexpr EXPIRATION_TIMER_UPDATE_BIT = 0x40000;

  static uint64_t constexpr SECURE_MESSAGE_BIT = 0x800000;

 public:
  inline static bool isGroupUpdate(uint64_t type)
  {
    return (type & GROUP_UPDATE_BIT) != 0;
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

  inline static bool isJoined(long type)
  {
    return (type & BASE_TYPE_MASK) == JOINED_TYPE;
  }

  inline static bool isExpirationTimerUpdate(long type)
  {
    return (type & EXPIRATION_TIMER_UPDATE_BIT) != 0;
  }

  inline static bool isIdentityUpdate(long type)
  {
    return (type & KEY_EXCHANGE_IDENTITY_UPDATE_BIT) != 0;
  }

  inline static bool isIdentityVerified(long type)
  {
    return (type & KEY_EXCHANGE_IDENTITY_VERIFIED_BIT) != 0;
  }

  inline static bool isIdentityDefault(long type)
  {
    return (type & KEY_EXCHANGE_IDENTITY_DEFAULT_BIT) != 0;
  }

  inline static bool isSecureType(long type)
  {
    return (type & SECURE_MESSAGE_BIT) != 0;
  }

};

#endif
