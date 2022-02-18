/*
    Copyright (C) 2022  Selwin van Dijk

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

#include "signalbackup.ih"

std::vector<SignalBackup::DatabaseLink> const SignalBackup::d_databaselinks // static
{
  {
    "thread",
    "_id",
    {
      {"sms", "thread_id", "", 0},
      {"mms", "thread_id", "", 0},
      {"drafts", "thread_id", "", 0},
      {"mention", "thread_id", "", 0}
    },
    NO_COMPACT
  },
  {
    "sms",
    "_id",
    {
      {"msl_message", "message_id", "WHERE is_mms IS NOT 1", 0},
      {"reaction", "message_id", "WHERE is_mms IS NOT 1", 0}
    },
    0
  },
  {
    "mms",
    "_id",
    {
      {"part", "mid", "", 0},
      {"group_receipts", "mms_id", "", 0},
      {"mention", "message_id", "", 0},
      {"msl_message", "message_id", "WHERE is_mms IS 1", 0},
      {"reaction", "message_id", "WHERE is_mms IS 1", 0}
    },
    0
  },
  {
    "part",
    "_id",
    {},
    0
  },
  {
    "recipient_preferences", // for (very) old databases
    "_id",
    {},
    NO_COMPACT
  },
  {
    "recipient", // for (very) old databases
    "_id",
    {
      {"sms", "address", "", 0},
      {"mms", "address", "", 0},
      {"mms", "quote_author", "", 0},
      {"sessions", "address", "", 0},
      {"group_receipts", "address", "", 0},
      {"thread", "recipient_ids", "", 0},        //----> Only one of these will exist
      {"thread", "thread_recipient_id", "", 0},  //___/
      {"groups", "recipient_id", "", 0},
      {"remapped_recipients", "old_id", "", 0},
      {"remapped_recipients", "new_id", "", 0},
      {"mention", "recipient_id", "", 0},
      {"msl_recipient", "recipient_id", "", 0},
      {"reaction", "author_id", "", 0},
      {"notification_profile_allowed_members", "recipient_id", "", 0},
      {"payments", "recipient", "", 0},
      {"identities", "address", "", SET_UNIQUELY}
    },
    NO_COMPACT
  },
  {
    "groups",
    "_id",
    {},
    0
  },
  {
    "identities",
    "_id",
    {},
    0
  },
  {
    "group_receipts",
    "_id",
    {},
    0
  },
  {
    "drafts",
    "_id",
    {},
    0
  },
  {
    "sticker",
    "_id",
    {},
    0
  },
  {
    "msl_payload",
    "_id",
    {
      {"msl_recipient", "payload_id", "", 0},
      {"msl_message", "payload_id", "", 0}
    },
    0
  },
  {
    "msl_recipient",
    "_id",
    {},
    0
  },
  {
    "msl_message",
    "_id",
    {},
    0
  },
  {
    "group_call_ring",
    "_id",
    {},
    0
  },
  {
    "megaphone",
    "_id",
    {},
    0
  },
  {
    "remapped_recipients",
    "_id",
    {},
    0
  },
  {
    "remapped_threads",
    "_id",
    {},
    0
  },
  {
    "mention",
    "_id",
    {},
    0
  },
  {
    "reaction",
    "_id",
    {},
    0
  },
  {
    "notification_profile",
    "_id",
    {
      {"notification_profile_allowed_members", "notification_profile_id", "", 0},
      {"notification_profile_schedule", "notification_profile_id", "", 0}
    },
    0
  },
  {
    "notification_profile_allowed_members",
    "_id",
    {},
    0
  },
  {
    "notification_profile_schedule",
    "_id",
    {},
    0
  },
  {
    "payments",
    "_id",
    {},
    0
  },
  {
    "chat_colors",
    "",
    {},
    SKIP // deleted in importThread()
  },
  {
    "push",
    "_id",
    {},
    SKIP // cleared in importThread()
  },
  {
    "storage_key",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link definition may be incomplete (has 'key TEXT UNIQUE' field)
  },
  {
    "sender_key_shared",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link defenition may be incomplete (has 'address' field + multiple UNIQUE)
  },
  {
    "sender_keys",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link defenition may be incomplete (has UNIQUE 'address' field)
  },
  {
    "pending_retry_receipts",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link defenition may be incomplete (has UNIQUE 'author' field + more)
  },
  {
    "avatar_picker",
    "_id",
    {},
    WARN // I have never seen this table not-emptyy, this link defenition may be incomplete (has 'group_id' field)
  },
  {
    "emoji_search",
    "",
    {},
    SKIP // not sure, but i think this is skipped anyway, also does not seem to have any unique fields
  },
  {
    "job_spec",
    "",
    {},
    SKIP // cleared in importthread
  },
  {
    "constraint_spec",
    "",
    {},
    SKIP // cleared in importthread
  },
  {
    "dependency_spec",
    "",
    {},
    SKIP // cleared in importthread
  }
};
