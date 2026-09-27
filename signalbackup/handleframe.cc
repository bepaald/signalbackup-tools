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
  // recipientframe = oneof(contact, group, distributionlistitem, self, releasenotes, calllink);

  auto contact = f.getFieldView<RECIPIENT_FRAMENUMBER, 2>();
  if (contact.has_value())
  {
    std::cout << "(contact)" << std::endl;
    f.print();
    return true;


    auto aci = contact->getFieldView<1>();
    if (aci.has_value())
      std::cout << "Aci: " << bepaald::bytesToHexString(aci.value()) << std::endl;

    auto profilekey = contact->getFieldView<9>();
    if (profilekey.has_value())
      std::cout << "profilekey: " << Base64::bytesToBase64String(profilekey.value()) << std::endl;

    auto e164 = contact->getFieldView<4>();
    if (e164.has_value())
      std::cout << "e164: " << e164.value() << std::endl;

    return true;
  }

  auto group = f.getFieldView<RECIPIENT_FRAMENUMBER, 3>();
  if (group.has_value())
  {
    std::cout << "(group)" << std::endl;
    auto masterkey = group->getFieldView<1>();
    if (masterkey.has_value())
      std::cout << "Masterkey: " << Base64::bytesToBase64String(masterkey.value()) << std::endl;

    return true;
  }

  auto distributionlistitem = f.getFieldView<RECIPIENT_FRAMENUMBER, 4>();
  if (distributionlistitem.has_value())
  {
    std::cout << "(distributionlistitem)" << std::endl;
    return true;
  }

  auto self = f.getFieldView<RECIPIENT_FRAMENUMBER, 5>();
  if (self.has_value())
  {
    std::cout << "(self)" << std::endl;
    return true;
  }

  auto releasenotes = f.getFieldView<RECIPIENT_FRAMENUMBER, 6>();
  if (releasenotes.has_value())
  {
    std::cout << "(releasenotes)" << std::endl;
    return true;
  }

  auto calllink = f.getFieldView<RECIPIENT_FRAMENUMBER, 7>();
  if (calllink.has_value())
  {
    std::cout << "(calllink)" << std::endl;
    return true;
  }

  return false;
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
  auto accountdata_frame = f.getFieldView<ACCOUNTDATA_FRAMENUMBER>();
  if (!accountdata_frame.has_value()) [[unlikely]]
    return false;

  f.print();
  return true;

  auto profile_given_name = accountdata_frame->getFieldView<4>();

  auto profile_family_name = accountdata_frame->getFieldView<5>();

  std::optional<std::string> profile_joined_name;
  if (profile_given_name && profile_family_name)
    profile_joined_name->append(profile_given_name.value()).append(" ").append(profile_family_name.value());
  else if (profile_family_name)
    profile_joined_name->append(profile_family_name.value());
  else if (profile_given_name)
    profile_joined_name->append(profile_given_name.value());

  auto profile_avatar = accountdata_frame->getFieldView<6>();

  int registered{1};

  bool profile_sharing{true};

  int unregistered_timestamp{0};

  /*
  // recipientExtras:

  message RecipientExtras {
  bool  manuallyShownAvatar = 1;
  bool  hideStory           = 2;
  int64 lastStoryView       = 3;
  }
  */

  auto profile_key_raw = accountdata_frame->getFieldView<1>();
  if (!profile_key_raw)
  {
    Logger::error("Missing porfile key for self!");
    return false;
  }
  std::string profile_key_b64(Base64::bytesToBase64String(profile_key_raw.value()));

  std::cout << "Got profile_key_b64: " << profile_key_b64 << std::endl;

  auto username = accountdata_frame->getFieldView<2>();

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
  BackupV2::Frame f(data, size, ProtoBufParserBase::MEMTYPE::VIEWONLY);
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
