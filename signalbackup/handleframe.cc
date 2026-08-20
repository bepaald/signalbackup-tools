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

#include "signalbackup.h"

#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../protobufparser/protobufparser.h"

bool SignalBackup::handleChatItemFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleRecipientFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleChatFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleStickerPackFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleAccountDataFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleAdHocCallFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleNotifcationProfileFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleChatFolderFrame(BackupV2::Frame const &f) const
{
  return true;
}

bool SignalBackup::handleFrame(unsigned char *const data, size_t size) const
{
  BackupV2::Frame f(data, size, ProtoBufParserBase::VIEWONLY::TRUE);
  f.checkBufferFields();


  int32_t frame_type = f.getFirstFieldNumber();

  switch (frame_type)
  {
    case CHATITEM_FRAMENUMBER:
    {
      std::cout << "Got ChatItem frame" << std::endl;
      return handleChatItemFrame(f);
    }
    case RECIPIENT_FRAMENUMBER:
    {
      std::cout << "Got Recipient frame" << std::endl;
      return handleRecipientFrame(f);
    }
    case CHAT_FRAMENUMBER:
    {
      std::cout << "Got Chat frame" << std::endl;
      return handleChatFrame(f);
    }
    case STICKERPACK_FRAMENUMBER:
    {
      std::cout << "Got StickerPack frame" << std::endl;
      return handleStickerPackFrame(f);
    }
    case ACCOUNTDATA_FRAMENUMBER:
    {
      std::cout << "Got AccountData frame" << std::endl;
      return handleAccountDataFrame(f);
    }
    case ADHOCCALL_FRAMENUMBER:
    {
      std::cout << "Got AdHocCall frame" << std::endl;
      return handleAdHocCallFrame(f);
    }
    case NOTIFICATIONPROFILE_FRAMENUMBER:
    {
      std::cout << "Got NotificationProfile frame" << std::endl;
      return handleNotifcationProfileFrame(f);
    }
    case CHATFOLDER_FRAMENUMBER:
    {
      std::cout << "Got ChatFolder frame" << std::endl;
      return handleChatFolderFrame(f);
    }
    default:
    {
      Logger::error("Unhandled frame type: ", frame_type);
      return false;
    }
  }

  return true;
}

#endif
