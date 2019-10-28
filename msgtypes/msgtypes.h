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

#ifndef MSGTYPES_H_
#define MSGTYPES_H_

// see /src/org/thoughtcrime/securesms/database/MmsSmsColumns.java

struct Types
{
  static uint64_t constexpr BASE_TYPE_MASK                     = 0x1F;

  static uint64_t constexpr BASE_INBOX_TYPE                    = 20;
  static uint64_t constexpr BASE_OUTBOX_TYPE                   = 21;
  static uint64_t constexpr BASE_PENDING_INSECURE_SMS_FALLBACK = 26;

  static uint64_t constexpr GROUP_UPDATE_BIT            = 0x10000;
  static uint64_t constexpr GROUP_QUIT_BIT              = 0x20000;

 public:
  inline static bool isGroupUpdate(uint64_t type)
  {
    return (type & GROUP_UPDATE_BIT) != 0;
  }

  inline static bool isGroupQuit(uint64_t type)
  {
    return (type & GROUP_QUIT_BIT) != 0;
  }

  inline static bool isInboxType(uint64_t type)
  {
    return (type & BASE_TYPE_MASK) == BASE_INBOX_TYPE;
  }
};

#endif
