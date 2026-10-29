/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "signalbackup.ih"

#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../protobufparser/protobufparser.h"

bool SignalBackup::handleChatItemFrame(BackupV2::Frame const &f, BackupV2ImportState *state)
{
  auto chatitem = f.getFieldView<CHATITEM_FRAMENUMBER>();
  if (!chatitem.has_value()) [[unlikely]]
    return false;

  auto chat_id = chatitem->getFieldView<1>();
  auto author_id = chatitem->getFieldView<2>();
  uint64_t date_sent = chatitem->getFieldView<3>().value_or(0);

  auto chat_it = chat_id.has_value() ? state->chats.find(chat_id.value()) : state->chats.end();
  if (chat_it == state->chats.end()) [[unlikely]]
  {
    Logger::warning("Chat not found for message (date_sent: ", date_sent, "). Skipping...");
    ++state->skippedchatitems;
    return true;
  }
  auto [thread_id, chat_backup_rid] = chat_it->second;
  long long int chat_rid = state->recipients[chat_backup_rid];

  auto author_it = author_id.has_value() ? state->recipients.find(author_id.value()) : state->recipients.end();
  if (author_it == state->recipients.end()) [[unlikely]]
  {
    Logger::warning("Author not found for message (date_sent: ", date_sent, "). Skipping...");
    ++state->skippedchatitems;
    return true;
  }
  long long int author_rid = author_it->second;

  // Like Android, do not restore messages that have already expired
  uint64_t expire_start = chatitem->getFieldView<4>().value_or(0);
  uint64_t expires_in = chatitem->getFieldView<5>().value_or(0);
  if (expire_start > 0 && expires_in > 0 &&
      expire_start + expires_in < static_cast<uint64_t>(state->now))
  {
    ++state->expiredchatitems;
    return true;
  }

  // edited messages: the older revisions are nested inside the latest one. Android
  // stores them as separate messages, oldest first, all pointing at the latest.
  auto revisions = chatitem->getFieldView<6>();
  std::vector<long long int> revision_ids;
  for (auto const &rev_data : revisions)
  {
    BackupV2::ChatItem revision(rev_data.first, rev_data.second, ProtoBufParserBase::MEMTYPE::VIEWONLY);
    long long int rev_mid = -1;
    if (!bv2InsertChatItem(revision, thread_id, chat_rid, chat_backup_rid, author_rid, state, &rev_mid)) [[unlikely]]
      return false;
    if (rev_mid != -1)
      revision_ids.push_back(rev_mid);
  }

  long long int mid = -1;
  if (!bv2InsertChatItem(chatitem.value(), thread_id, chat_rid, chat_backup_rid, author_rid, state, &mid)) [[unlikely]]
    return false;

  if (mid != -1 && !revision_ids.empty())
  {
    for (unsigned int i = 0; i < revision_ids.size(); ++i)
      if (!d_database.exec("UPDATE message SET latest_revision_id = ?, original_message_id = ?, revision_number = ? WHERE _id = ?",
                           {mid, i == 0 ? std::any(nullptr) : std::any(revision_ids[0]), static_cast<long long int>(i), revision_ids[i]})) [[unlikely]]
        return false;
    if (!d_database.exec("UPDATE message SET original_message_id = ?, revision_number = ? WHERE _id = ?",
                         {revision_ids[0], static_cast<long long int>(revision_ids.size()), mid})) [[unlikely]]
      return false;
  }

  return true;
}

#endif
