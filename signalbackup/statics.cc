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
      {"sms", "thread_id"},
      {"mms", "thread_id"},
      {"drafts", "thread_id"},
      {"mention", "thread_id"}
    },
    NO_COMPACT
  },
  {
    "sms",
    "_id",
    {
      {"msl_message", "message_id", "is_mms IS NOT 1"},
      {"reaction", "message_id", "is_mms IS NOT 1"}
    },
    0
  },
  {
    "mms",
    "_id",
    {
      {"part", "mid"},
      {"group_receipts", "mms_id"},
      {"mention", "message_id"},
      {"msl_message", "message_id", "is_mms IS 1"},
      {"reaction", "message_id", "is_mms IS 1"},
      {"story_sends", "message_id"}
    },
    0
  },
  {
    "part",
    "_id",
    {
      {"mms", "previews", "", "'$[0].attachmentId.rowId'"},     //       \ These are the same
      {"mms", "link_previews", "", "'$[0].attachmentId.rowId'"} //       /
    },
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
      {"sms", "address"},      // \ These are one
      {"sms", "recipient_id"}, // /
      {"mms", "address"},      // \ These are one
      {"mms", "recipient_id"}, // /
      {"mms", "quote_author"},
      {"sessions", "address"},
      {"group_receipts", "address"},
      {"thread", "recipient_ids"},        //---\ Only one of these will exist
      {"thread", "thread_recipient_id"},  //   /
      {"thread", "recipient_id"},         //__/
      {"groups", "recipient_id"},
      {"remapped_recipients", "old_id"}, // should actually be cleared, but ...
      {"remapped_recipients", "new_id"}, // this can't hurt
      {"mention", "recipient_id"},
      {"msl_recipient", "recipient_id"},
      {"reaction", "author_id"},
      {"notification_profile_allowed_members", "recipient_id"},
      {"payments", "recipient"},
      {"identities", "address", "", "", SET_UNIQUELY},  // identities.address has UNIQUE constraint
                                                        // when I can assume c++20, sometime in the future, change this to
                                                        // {.table = "identities", .column = "address', .flags = SET_UNIQUELY}
                                                        // this is much more explicit and looks cleaner without the empty
                                                        // fields. (give missing fields default init in header)
      {"distribution_list", "recipient_id"},
      {"distribution_list_member", "recipient_id"},
      {"story_sends", "recipient_id"},
      {"pending_pni_signature_message", "recipient_id"}
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
      {"msl_recipient", "payload_id"},
      {"msl_message", "payload_id"}
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
      {"notification_profile_allowed_members", "notification_profile_id"},
      {"notification_profile_schedule", "notification_profile_id"}
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
    0 //WARN // I have never seen this table not-empty, this link definition may be incomplete (has 'key TEXT UNIQUE' field)
    // see #76, should be fixed (existing 'key' entries are deleted in importThread()
  },
  {
    "sender_key_shared",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link definition may be incomplete (has 'address' field + multiple UNIQUE)
  },
  {
    "sender_keys",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link definition may be incomplete (has UNIQUE 'address' field)
  },
  {
    "pending_retry_receipts",
    "_id",
    {},
    WARN // I have never seen this table not-empty, this link definition may be incomplete (has UNIQUE 'author' field + more)
  },
  {
    "avatar_picker",
    "_id",
    {},
    WARN // I have never seen this table not-emptyy, this link definition may be incomplete (has 'group_id' field)
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
  },
  {
    "distribution_list",   /// WORK IN PROGRESS? All other fields of distr_list are also unique.
    "_id",
    {
      {"recipient", "distribution_list_id"},
      {"distribution_list_member", "list_id"}
        //{"mms","parent_story_id"}???
      //distribution_id TEXT UNIQUE NOT NULL
    },
    0
  },
  {
    "distribution_list_member",
    "_id",
    {},
    0
  },
  {
    "donation_receipt",
    "_id",
    {},
    0
  },
  {
    "story_sends",
    "_id",
    //distribution_id TEXT NOT NULL REFERENCES distribution_list (distribution_id) ON DELETE CASCADE
    {},
    0
  },
  {
    "key_value",
    "_id",
    {},
    0
  },
  {         // NOTE e164 is also UNIQUE, remove doubles (oldeest?) beforehand
    "cds",  // 'Caontact Discovery Service (v2)??'
    "_id",
    {},
    0
  },
  {         // Remove double (UNIQUE) uuid beforehand
    "remote_megaphone",
    "_id",
    {},
    0
  },
  {
    "pending_pni_signature_message",
    "_id",
    {},
    0
  }
};
