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

bool SignalBackup::handleStickerPackFrame(BackupV2::Frame const &) const
{
  // only pack id and key are in the backup, the sticker data itself
  // would need to be downloaded. Stickers used in messages are stored
  // as attachments of those messages.
  return true;
}

bool SignalBackup::handleAccountDataFrame(BackupV2::Frame const &f)
{
  auto accountdata_frame = f.getFieldView<ACCOUNTDATA_FRAMENUMBER>();
  if (!accountdata_frame.has_value()) [[unlikely]]
    return false;

  // The account data is the only place the profile of 'self' is stored. There is
  // also a 'Self'-recipient frame, but that only contains an id (and avatar color).
  // AccountData is always the first frame, so we create the self recipient now,
  // the Self-recipientframe will map to it later.
  auto profile_key = accountdata_frame->getFieldView<1>();
  auto username = accountdata_frame->getFieldView<2>();
  auto given_name = accountdata_frame->getFieldView<4>();
  auto family_name = accountdata_frame->getFieldView<5>();
  auto bio_text = accountdata_frame->getFieldView<13>();
  auto bio_emoji = accountdata_frame->getFieldView<14>();

  std::string joined_name(given_name.value_or(std::string_view()));
  if (family_name.has_value() && !family_name->empty())
    joined_name += (joined_name.empty() ? "" : " ") + std::string(family_name.value());

  std::any new_rid;
  if (!insertRow("recipient",
                 {{"type", 0},
                  {"registered", 1},
                  {"profile_sharing", 1},
                  {"profile_key", profile_key.has_value() ? Base64::bytesToBase64String(profile_key.value()) : std::any(nullptr)},
                  {"username", username.has_value() && !username->empty() ? std::string(username.value()) : std::any(nullptr)},
                  {"profile_given_name", std::string(given_name.value_or(std::string_view()))},
                  {"profile_family_name", std::string(family_name.value_or(std::string_view()))},
                  {"profile_joined_name", joined_name},
                  {"about", bio_text.has_value() ? std::string(bio_text.value()) : std::any(nullptr)},
                  {"about_emoji", bio_emoji.has_value() ? std::string(bio_emoji.value()) : std::any(nullptr)}},
                 "_id", &new_rid)) [[unlikely]]
  {
    Logger::error("Failed to insert recipient for self");
    return false;
  }
  d_selfid = std::any_cast<long long int>(new_rid);
  return true;
}

bool SignalBackup::handleAdHocCallFrame(BackupV2::Frame const &) const
{
  // call log entries for call links, not (yet) supported
  return true;
}

bool SignalBackup::handleNotifcationProfileFrame(BackupV2::Frame const &) const
{
  // settings, not relevant for this tool
  return true;
}

bool SignalBackup::handleChatFolderFrame(BackupV2::Frame const &) const
{
  // settings, not relevant for this tool
  return true;
}

bool SignalBackup::handleFrame(unsigned char *const data, size_t size, BackupV2ImportState *state)
{
  BackupV2::Frame f(data, size, ProtoBufParserBase::MEMTYPE::VIEWONLY);
  if (d_verbose) [[unlikely]]
    f.checkBufferFields();

  int32_t frame_type = f.getFirstFieldNumber();

  // all recipients come before the first chat, finish setting up the recipients
  // (that need information from all recipient frames) before handling anything else
  if (frame_type != RECIPIENT_FRAMENUMBER && frame_type != ACCOUNTDATA_FRAMENUMBER &&
      !state->recipientsfinalized)
    if (!bv2FinalizeRecipients(state)) [[unlikely]]
      return false;

  switch (frame_type)
  {
    case CHATITEM_FRAMENUMBER:
      return handleChatItemFrame(f, state);
    case RECIPIENT_FRAMENUMBER:
      return handleRecipientFrame(f, state);
    case CHAT_FRAMENUMBER:
      return handleChatFrame(f, state);
    case STICKERPACK_FRAMENUMBER:
      return handleStickerPackFrame(f);
    case ACCOUNTDATA_FRAMENUMBER:
      return handleAccountDataFrame(f);
    case ADHOCCALL_FRAMENUMBER:
      return handleAdHocCallFrame(f);
    case NOTIFICATIONPROFILE_FRAMENUMBER:
      return handleNotifcationProfileFrame(f);
    case CHATFOLDER_FRAMENUMBER:
      return handleChatFolderFrame(f);
    default:
    {
      Logger::warning("Unhandled frame type: ", frame_type, ". Skipping...");
      return true;
    }
  }

  return true;
}

#endif
