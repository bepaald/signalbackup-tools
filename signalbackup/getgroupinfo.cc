/*
  Copyright (C) 2023-2025  Selwin van Dijk

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

#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"

void SignalBackup::getGroupInfo(long long int rid, GroupInfo *groupinfo) const
{
  std::pair<std::shared_ptr<unsigned char []>, size_t> groupdata =
    d_database.tableContainsColumn("groups", "decrypted_group") ?
    d_database.getSingleResultAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>("SELECT decrypted_group FROM groups WHERE recipient_id = ?", rid, {nullptr, 0}) :
    std::make_pair(nullptr, 0);

  if (!groupdata.first || !groupdata.second)
    return;

/*
message DecryptedGroup {
           string                    title                     = 2;
           string                    avatar                    = 3;
           DecryptedTimer            disappearingMessagesTimer = 4;
           AccessControl             accessControl             = 5;
           uint32                    revision                  = 6;
  repeated DecryptedMember           members                   = 7;
  repeated DecryptedPendingMember    pendingMembers            = 8;
  repeated DecryptedRequestingMember requestingMembers         = 9;
           bytes                     inviteLinkPassword        = 10;
           string                    description               = 11;
           EnabledState              isAnnouncementGroup       = 12;
  repeated DecryptedBannedMember     bannedMembers             = 13;
}
*/
  DecryptedGroup group_info(groupdata);

  //group_info.print();

  // get announcementgroup
  auto group_info_isannouncement_group = group_info.getField<12>();
  if (group_info_isannouncement_group.has_value())
  {
    /*
      enum EnabledState {
      UNKNOWN  = 0;
      ENABLED  = 1;
      DISABLED = 2;
      }
    */
    long long int state = group_info_isannouncement_group.value();

    if (state == 2)
      groupinfo->isannouncementgroup = false;
    else if (state == 1)
      groupinfo->isannouncementgroup = true;

    // 0 = unknown => false?
  }

  // get timer value:
  //std::cout << "=== TIMER:" << std::endl;
  auto group_info_timer = group_info.getField<4>();
  if (group_info_timer.has_value())
  {
    /*
      message DecryptedTimer {
      uint32 duration = 1;
      }
    */
    auto timerdata = group_info_timer.value();
    long long int timer = -1;
    auto duration = timerdata.getField<1>();
    if (duration.has_value())
      timer = duration.value();
    //std::cout << "Timer: " << timer << std::endl;
    if (timer != -1)
      groupinfo->expiration_timer = timer;
  }
  //std::cout << "===" << std::endl << std::endl;


  // get access control:
  //std::cout << "=== ACCESS CONTROL:" << std::endl;
  auto group_info_accesscontrol = group_info.getField<5>();
  if (group_info_accesscontrol.has_value())
  {
    /*
message AccessControl {
  enum AccessRequired {
    UNKNOWN       = 0;
    ANY           = 1;
    MEMBER        = 2;
    ADMINISTRATOR = 3;
    UNSATISFIABLE = 4;
  }

  AccessRequired attributes        = 1;
  AccessRequired members           = 2;
  AccessRequired addFromInviteLink = 3;
}
    */

    auto enumToString = [] (int i) STATICLAMBDA
    {
      switch (i)
      {
        case 1:
          return "Anyone";
        case 2:
          return "All members";
        case 3:
          return "Only admins";
        case 4:
          return "No one";
        case 0:
        default:
          return "Unknown";
      }
    };

    auto acdata = group_info_accesscontrol.value();

    long long int attributes = 0;
    auto accesscontrol_attributes = acdata.getField<1>();
    if (accesscontrol_attributes.has_value())
      attributes = accesscontrol_attributes.value();
    groupinfo->access_control_attributes = enumToString(attributes);


    long long int members = 0;
    auto accesscontrol_members = acdata.getField<2>();
    if (accesscontrol_members.has_value())
      members = accesscontrol_members.value();
    groupinfo->access_control_members = enumToString(members);

    long long int addfrominvitelink = 0;
    auto accesscontrol_addfrominvitelink = acdata.getField<3>();
    if (accesscontrol_addfrominvitelink.has_value())
      addfrominvitelink = accesscontrol_addfrominvitelink.value();
    groupinfo->access_control_addfromlinkinvite = enumToString(addfrominvitelink);

    //std::cout << "Access control: " << attributes << " - " << members << " - " << addfrominvitelink << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get members:
  {
    //std::cout << "=== MEMBERS:" << std::endl;
    auto newmembers = group_info.getField<7>();
    for (unsigned int i = 0; i < newmembers.size(); ++i)
    {
      /*
        message DecryptedMember {
        bytes       uuid             = 1;
        Member.Role role             = 2;
        bytes       profileKey       = 3;
        uint32      joinedAtRevision = 5;
        bytes       pni              = 6;
        }
        enum Role {
        UNKNOWN       = 0;
        DEFAULT       = 1;
        ADMINISTRATOR = 2;
        }
      */
      // uuid
      auto [uuid, uuid_size] = newmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
      if (uuid_size < 16)
        continue;
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      // role
      long long int role = -1;
      auto newmember_role = newmembers[i].getField<2>();
      if (newmember_role.has_value())
        role = newmember_role.value();

      //std::cout << uuidstr << " (" << role << ")" << std::endl;

      if (role == 2) // ADMIN
      {
        long long int id = getRecipientIdFromUuidMapped(uuidstr, nullptr);
        if (id != -1)
          groupinfo->admin_ids.push_back(id);
      }
    }
    //std::cout << "===" << std::endl << std::endl;
  }



  // get pending members:
  {
    //std::cout << "=== PENDING MEMBERS:" << std::endl;
    auto pendingmembers = group_info.getField<8>();
    for (unsigned int i = 0; i < pendingmembers.size(); ++i)
    {
      /*
        message DecryptedPendingMember {
        bytes       uuid           = 1;
        Member.Role role           = 2;
        bytes       addedByUuid    = 3;
        uint64      timestamp      = 4;
        bytes       uuidCipherText = 5;
        }
      */
      // uuid
      auto [uuid, uuid_size] = pendingmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
      if (uuid_size < 16)
        continue;
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      // // role
      // long long int role = -1;
      // if (pendingmembers[i].getField<2>().has_value())
      //   role = pendingmembers[i].getField<2>().value();

      long long int id = getRecipientIdFromUuidMapped(uuidstr, nullptr);
      if (id != -1)
        groupinfo->pending_members.push_back(id);

      //std::cout << uuidstr << " (" << role << ")" << std::endl;
    }
    //std::cout << "===" << std::endl << std::endl;
  }



  // get requesting members:
  {
    //std::cout << "=== REQUESTING MEMBERS:" << std::endl;
    auto requestingmembers = group_info.getField<9>();
    for (unsigned int i = 0; i < requestingmembers.size(); ++i)
    {
      /*
        message DecryptedRequestingMember {
        bytes  uuid       = 1;
        bytes  profileKey = 2;
        uint64 timestamp  = 4;
        }
      */
      // uuid
      auto [uuid, uuid_size] = requestingmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
      if (uuid_size < 16)
        continue;
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      long long int id = getRecipientIdFromUuidMapped(uuidstr, nullptr);
      if (id != -1)
        groupinfo->requesting_members.push_back(id);

      //std::cout << uuidstr << std::endl;
    }
    //std::cout << "===" << std::endl << std::endl;
  }




  // get banned members:
  {
    //std::cout << "=== BANNED MEMBERS:" << std::endl;
    auto bannedmembers = group_info.getField<13>();
    for (unsigned int i = 0; i < bannedmembers.size(); ++i)
    {
      /*
        message DecryptedBannedMember {
        bytes  uuid      = 1;
        uint64 timestamp = 2;
        }
      */
      // uuid
      auto [uuid, uuid_size] = bannedmembers[i].getField<1>().value_or(std::make_pair(nullptr, 0)); // bytes
      if (uuid_size < 16)
        continue;
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      long long int id = getRecipientIdFromUuidMapped(uuidstr, nullptr);
      if (id != -1)
        groupinfo->banned_members.push_back(id);

      //std::cout << uuidstr << std::endl;
    }
    //std::cout << "===" << std::endl << std::endl;
  }




  // get description
  //std::cout << "=== DESCRIPTION:" << std::endl;
  auto group_info_description = group_info.getField<11>();
  if (group_info_description.has_value())
  {
    std::string desc = (group_info_description.value());
    groupinfo->description = desc;
    //std::cout << desc << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get group invite password?
  //std::cout << "=== INVITE PW:" << std::endl;
  auto group_info_invitepw = group_info.getField<10>();
  if (group_info_invitepw.has_value())
  {
    auto [pw, pwsize] = group_info_invitepw.value();
    //std::cout << bepaald::bytesToHexString(pw, pwsize) << std::endl;
    //std::cout << "(base64:) " << Base64::bytesToBase64String(pw, pwsize) << std::endl;
    if (pwsize)
      groupinfo->link_invite_enabled = true;
  }
  //std::cout << "===" << std::endl << std::endl;
}
