/*
  Copyright (C) 2023  Selwin van Dijk

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

void SignalBackup::getGroupInfo(long long int rid, GroupInfo *groupinfo) const
{
  std::pair<std::shared_ptr<unsigned char []>, size_t> groupdata =
    d_database.getSingleResultAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>("SELECT decrypted_group FROM groups WHERE recipient_id = ?", rid, {nullptr, 0});

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

  // get timer value:
  //std::cout << "=== TIMER:" << std::endl;
  if (group_info.getField<4>().has_value())
  {
    /*
      message DecryptedTimer {
      uint32 duration = 1;
      }
    */
    DecryptedTimer timerdata(group_info.getField<4>().value());
    long long int timer [[maybe_unused]] = -1;
    if (timerdata.getField<1>().has_value())
      timer = timerdata.getField<1>().value();
    //std::cout << "Timer: " << timer << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;


  // get access control:
  //std::cout << "=== ACCESS CONTROL:" << std::endl;
  if (group_info.getField<5>().has_value())
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
    AccessControl acdata(group_info.getField<5>().value());

    long long int attributes [[maybe_unused]] = -1;
    if (acdata.getField<1>().has_value())
      attributes = acdata.getField<1>().value();

    long long int members [[maybe_unused]] = -1;
    if (acdata.getField<1>().has_value())
      members = acdata.getField<1>().value();

    long long int addfrominvitelink [[maybe_unused]] = -1;
    if (acdata.getField<1>().has_value())
      addfrominvitelink = acdata.getField<1>().value();

    //std::cout << "Access control: " << attributes << " - " << members << " - " << addfrominvitelink << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get members:
  //std::cout << "=== MEMBERS:" << std::endl;
  auto newmembers = group_info.getField<7>();
  for (uint i = 0; i < newmembers.size(); ++i)
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
    std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
    uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

    // role
    long long int role [[maybe_unused]] = -1;
    if (newmembers[i].getField<2>().has_value())
      role = newmembers[i].getField<2>().value();

    //std::cout << uuidstr << " (" << role << ")" << std::endl;

    if (role == 2) // ADMIN
    {
      long long int id = getRecipientIdFromUuid(uuidstr, nullptr);
      if (id != -1)
        groupinfo->admin_ids.push_back(id);
    }
  }
  //std::cout << "===" << std::endl << std::endl;

  // get pending members:
  //std::cout << "=== PENDING MEMBERS:" << std::endl;
  auto pendingmembers = group_info.getField<8>();
  for (uint i = 0; i < pendingmembers.size(); ++i)
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
    std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
    uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

    // role
    long long int role [[maybe_unused]] = -1;
    if (newmembers[i].getField<2>().has_value())
      role = newmembers[i].getField<2>().value();

    //std::cout << uuidstr << " (" << role << ")" << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get requesting members:
  //std::cout << "=== REQUESTING MEMBERS:" << std::endl;
  auto requestingmembers = group_info.getField<9>();
  for (uint i = 0; i < requestingmembers.size(); ++i)
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
    std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
    uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

    //std::cout << uuidstr << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get description
  //std::cout << "=== DESCRIPTION:" << std::endl;
  if (group_info.getField<11>().has_value())
  {
    std::string desc = (group_info.getField<11>().value());
    groupinfo->description = desc;
    //std::cout << desc << std::endl;
  }
  //std::cout << "===" << std::endl << std::endl;

  // get group invite password?
  //std::cout << "=== INVITE PW:" << std::endl;
  if (group_info.getField<10>().has_value())
  {
    auto [pw, pwsize] = group_info.getField<10>().value();
    // std::cout << bepaald::bytesToHexString(pw, pwsize) << std::endl;
    // std::cout << "(base64:) " << Base64::bytesToBase64String(pw, pwsize) << std::endl;
    if (pwsize)
      groupinfo->link_invite_enabled = true;
  }
  //std::cout << "===" << std::endl << std::endl;

}
