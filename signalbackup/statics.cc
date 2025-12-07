/*
  Copyright (C) 2022-2025  Selwin van Dijk

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

/*
  struct DatabaseLink
  {
    std::string table;
    std::string column;
    std::vector<TableConnection> const connections;
    {
      std::string table;
      std::string column;
      std::string whereclause = std::string();
      std::string json_path = std::string();
      int flags = 0; // SET_UNIQUELY
      unsigned int mindbvversion = 0;
      unsigned int maxdbvversion = std::numeric_limits<unsigned int>::max();
    }
    int flags; // NO_COMPACT, SKIP, WARN
  };
*/

std::vector<SignalBackup::DatabaseLink> const SignalBackup::s_databaselinks // static
{
  {
    "thread",
    "_id",
    {
      {"sms", "thread_id"},
      {"mms", "thread_id"},    //       \ These are the same
      {"message", "thread_id"},//       /
      {"drafts", "thread_id"},
      {"mention", "thread_id"},
      {"name_collision", "thread_id"},
      {"chat_folder_membership", "thread_id"}
    },
    NO_COMPACT
  },
  {
    "sms",
    "_id",
    {
      {"msl_message", "message_id", "is_mms IS NOT 1", "", 0, 0, 167}, // is_mms is 'removed' from table (dbv 168?)
      {"msl_message", "message_id", "", "", 0, 168},
      {"reaction", "message_id", "is_mms IS NOT 1", "", 0, 0, 167},
      {"reaction", "message_id", "", "", 0, 168}
    },
    0
  },
  {
    "message",
    "_id",
    {
      {"part", "mid"},                // \ The same
      {"attachment", "message_id"},   // /
      {"group_receipts", "mms_id"},
      {"mention", "message_id"},
      {"msl_message", "message_id", "is_mms IS 1", "", 0, 0, 167}, // is_mms is 'removed' from table (dbv 168?)
      {"msl_message", "message_id", "", "", 0, 168},
      {"reaction", "message_id", "is_mms IS 1", "", 0, 0, 167},
      {"reaction", "message_id", "", "", 0, 168},
      {"story_sends", "message_id"},
      {"call", "message_id"},
      {"message", "latest_revision_id"},
      {"message", "original_message_id"},
      //{"message", "pinning_message_id", "pinning_message_id != 0"}, // column has default 0, but only refers to a message when != 0
      {"poll", "message_id"}
    },
    0
  },
  {
    "mms",
    "_id",
    {
      {"part", "mid"},                // \ The same
      {"attachment", "message_id"},   // /
      {"group_receipts", "mms_id"},
      {"mention", "message_id"},
      {"msl_message", "message_id", "is_mms IS 1", "", 0, 0, 167}, // is_mms is 'removed' from table (dbv 168?)
      {"msl_message", "message_id", "", "", 0, 168},
      {"reaction", "message_id", "is_mms IS 1", "", 0, 0, 167},
      {"reaction", "message_id", "", "", 0, 168},
      {"story_sends", "message_id"},
      {"call", "message_id"}
    },
    0
  },
  {
    "part",
    "_id",
    {
      {"message", "previews", "", "'$[0].attachmentId.rowId'"},      //       \ These are the same
      {"message", "link_previews", "", "'$[0].attachmentId.rowId'"}, //       /
      {"mms", "previews", "", "'$[0].attachmentId.rowId'"},     //       \ These are the same
      {"mms", "link_previews", "", "'$[0].attachmentId.rowId'"} //       /
    },
    0
  },
  {
    "attachment",
    "_id",
    {
      {"message", "previews", "", "'$[0].attachmentId.rowId'"},      //       \ These are the same
      {"message", "link_previews", "", "'$[0].attachmentId.rowId'"}, //       /
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
      {"message", "address"},           // \ These are one
      {"message", "recipient_id"},      // /
      {"message", "from_recipient_id"}, // | Also sort of
      {"message", "to_recipient_id"},   // /
      {"message", "quote_author"},
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
      {"pending_pni_signature_message", "recipient_id"},
      {"call", "peer"},
      {"call", "ringer"},
      {"group_membership", "recipient_id", "", "", SET_UNIQUELY},
      {"name_collision_membership", "recipient_id"},
      {"poll", "author_id"},
      {"poll_vote", "voter_id"}
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
    "push", // this table was dropped around dbv205
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
    "distribution_list",   /// WORK IN PROGRESS? other fields of distr_list are also unique: distibution_id, recipient_id
    "_id",
    {
      {"recipient", "distribution_list_id"},
      {"distribution_list_member", "list_id"}
      //{d_mms_table,"parent_story_id"}???
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
  {         // NOTE e164 is also UNIQUE, remove doubles (oldest?) beforehand
    "cds",  // 'Contact Discovery Service (v2)??'
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
  },
  {
    "call",
    "_id",
    {},
    0
  },
  {
    "group_membership",
    "_id",
    {},
    0
  },
  {
    "call_link",
    "_id",
    {
      {"call", "call_link"}
    },
    0
  },
  {
    "kyber_prekey",
    "_id",
    {
      {"last_resort_key_tuple", "kyber_prekey_id"}
    },
    0
  },
  {
    "name_collision",
    "_id",
    {
      {"name_collision_membership", "collision_id"}
    },
    0
  },
  {
    "name_collision_membership",
    "_id",
    {},
    0
  },
  {
    "in_app_payment",
    "_id",
    {},
    0
  },
  {
    "in_app_payment_subscriber",
    "_id",
    {},
    0
  },
  {
    "chat_folder",
    "_id",
    {
      {"chat_folder_membership", "chat_folder_id"}
    },
    0
  },
  {
    "chat_folder_membership",
    "_id",
    {},
    0
  },
  {
    "backup_media_snapshot",
    "_id",
    {},
    0
  },
  {
    "poll",
    "_id",
    {
      {"poll_option", "poll_id"},
      {"poll_vote", "poll_id"}
    },
    0
  },
  {
    "poll_option",
    "_id",
    {
      {"poll_vote", "poll_option_id"}
    },
    0
  },
  {
    "poll_vote",
    "_id",
    {},
    0
  },
  {
    "last_resort_key_tuple", // not sure about this one...
    "_id",
    {},
    0
  }
};


// in table FIRST, SECOND[n] used to be known as SECOND[n+1]
std::map<std::string, std::vector<std::vector<std::string>>> const SignalBackup::s_columnaliases //static
{
  std::make_pair("thread",
                 std::vector<std::vector<std::string>>{{"recipient_id", "thread_recipient_id", "recipient_ids"},
                                                       {"meaningful_messages", "message_count"},
                                                       {"has_delivery_receipt", "delivery_receipt_count"},
                                                       {"has_read_receipt", "read_receipt_count"},
                                                       {"pinned_order", "pinned"}}),

  std::make_pair("recipient",
                 std::vector<std::vector<std::string>>{{"aci", "uuid"},
                                                       {"e164", "phone"},
                                                       {"avatar_color", "color"},
                                                       {"system_joined_name", "system_display_name"},
                                                       {"profile_given_name", "signal_profile_name"},
                                                       {"storage_service_id", "storage_service_key"},
                                                       {"type", "group_type"},
                                                       {"sealed_sender_mode", "unidentified_access_mode"},
                                                       {"profile_avatar", "signal_profile_avatar"}}),

  std::make_pair("sms",
                 std::vector<std::vector<std::string>>{{"date_received", "date"},
                                                       {"recipient_id", "address"},
                                                       {"recipient_device_id", "address_device_id"}}),

  std::make_pair("message",
                 std::vector<std::vector<std::string>>{{"has_delivery_receipt", "delivery_receipt_count"},
                                                       {"has_read_receipt", "read_receipt_count"},
                                                       {"viewed", "viewed_receipt_count"},
                                                       {"date_sent", "date"},
                                                       {"message_ranges", "ranges"},
                                                       {"from_recipient_id", "recipient_id", "address"},
                                                       {"from_device_id", "recipient_device_id", "address_device_id"},
                                                       {"type", "msg_box"},
                                                       {"link_previews", "previews"}}),

  std::make_pair("mms",
                 std::vector<std::vector<std::string>>{{"has_delivery_receipt", "delivery_receipt_count"},
                                                       {"has_read_receipt", "read_receipt_count"},
                                                       {"viewed", "viewed_receipt_count"},
                                                       {"date_sent", "date"},
                                                       {"from_recipient_id", "recipient_id", "address"},
                                                       {"from_device_id", "recipient_device_id", "address_device_id"},
                                                       {"type", "msg_box"},
                                                       {"link_previews", "previews"}}),

  std::make_pair("groups",
                 std::vector<std::vector<std::string>>{{"unmigrated_v1_members", "former_v1_members"},
                                                       {"display_as_story", "show_as_story_state"}}),

  std::make_pair("attachment",
                 std::vector<std::vector<std::string>>{{"message_id", "mid"},
                                                       {"content_type", "ct"},
                                                       {"transfer_state", "pending_push"},
                                                       {"remote_key", "cd"},
                                                       {"remote_location", "cl"}})
};
