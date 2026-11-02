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

bool SignalBackup::handleChatFrame(BackupV2::Frame const &f, BackupV2ImportState *state)
{
  auto chat = f.getFieldView<CHAT_FRAMENUMBER>();
  if (!chat.has_value()) [[unlikely]]
    return false;

  auto chat_id = chat->getFieldView<1>();
  auto backup_rid = chat->getFieldView<2>();
  if (!chat_id.has_value() || !backup_rid.has_value()) [[unlikely]]
  {
    Logger::warning("Chat without id or recipient. Skipping...");
    return true;
  }

  auto rid_it = state->recipients.find(backup_rid.value());
  if (rid_it == state->recipients.end()) [[unlikely]]
  {
    Logger::warning("Recipient for chat not found (", backup_rid.value(), "). Skipping...");
    return true;
  }
  long long int rid = rid_it->second;

  std::any new_tid;
  if (!insertRow("thread",
                 {{"recipient_id", rid},
                  {"archived", chat->getFieldView<3>().value_or(false) ? 1 : 0},
                  {"pinned", static_cast<long long int>(chat->getFieldView<4>().value_or(0))},
                  {"read", chat->getFieldView<7>().value_or(false) ? 2 : 1}, // ForcedUnread : Read
                  {"active", 1}},
                 "_id", &new_tid)) [[unlikely]]
  {
    Logger::error("Failed to insert thread");
    return false;
  }
  long long int tid = std::any_cast<long long int>(new_tid);
  state->chats[chat_id.value()] = {tid, backup_rid.value()};

  if (!d_database.exec("UPDATE recipient SET "
                       "mute_until = ?, "
                       "message_expiration_time = ?, "
                       "message_expiration_time_version = ?, "
                       "mention_setting = ? "
                       "WHERE _id = ?",
                       {static_cast<long long int>(chat->getFieldView<6>().value_or(0)),
                        static_cast<long long int>(chat->getFieldView<5>().value_or(0) / 1000),
                        static_cast<long long int>(chat->getFieldView<10>().value_or(1)),
                        chat->getFieldView<8>().value_or(false) ? 1 : 0, // DO_NOT_NOTIFY : ALWAYS_NOTIFY
                        rid})) [[unlikely]]
    Logger::warning("Failed to update recipient settings for chat");

  return true;
}

#endif
