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

#include "signalbackup.ih"

#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"
#include "../msgtypes/msgtypes.h"

std::string SignalBackup::decodeGroupV2UpdateMessage(DecryptedGroupV2Context const &groupv2ctx, long long int type, std::string const &contactname, IconType *icon) const
{

  if (Types::isGroupQuit(type)) // special case, otherwise the gv2ctx protobuf gets parsed leading to things like 'Joe removed Joe'
  {
    if (Types::isOutgoing(type))
      return "You left the group.";
    return bepaald::concat(contactname, " left the group.");
  }

  //groupv2ctx.print();
  std::string statusmsg;

  auto context_groupchange = groupv2ctx.getFieldView<2>();
  if (context_groupchange.has_value())
  {
    DecryptedGroupChange groupchange = context_groupchange.value();
    //std::cout << bepaald::bytesToHexString(groupchange.data(), groupchange.size()) << std::endl;
    //groupchange.print();

    // invite link changed:
    auto groupchange_invitelink_changed = groupchange.getFieldView<15>();
    if (groupchange_invitelink_changed.has_value())
    {
      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;

      // new value: 0 unknown, 1 any, 2 member, 3 admin, 4 unsatisfiable
      int accesscontrol = groupchange_invitelink_changed.value();

      // get editor
      std::string editoruuid;
      auto groupchange_editor = groupchange.getFieldView<1>();
      if (groupchange_editor.has_value())
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        editoruuid = bepaald::bytesToHexString(uuid, uuid_size, true);
        editoruuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      // previous accesscontrol: 0 unknown, 1 any, 2 member, 3 admin, 4 unsatisfiable
      int old_accesscontrol = 0;
      auto context_field4_5_3 = groupv2ctx.getFieldView<4, 5, 3>();
      if (context_field4_5_3.has_value())
        old_accesscontrol = context_field4_5_3.value();

      if (editoruuid == d_selfuuid)
      {
        if (accesscontrol == 4)
          return "You turned off the group link.";
        if (accesscontrol == 3)
        {
          if (old_accesscontrol == 1)
            return "You turned on admin approval for the group link.";
          else
            return "You turned on the group link with admin approval on.";
        }
        if (accesscontrol == 1)
        {
          if (old_accesscontrol == 3)
            return "You turned off admin approval for the group link.";
          else
            return "You turned on the group link with admin approval off.";
        }
      }

      std::string editorname = editoruuid.empty() ? editoruuid : getNameFromUuid(editoruuid);
      if (editorname.empty())
      {
        if (accesscontrol == 4)
          return "The group link has been turned off.";
        if (accesscontrol == 3)
        {
          if (old_accesscontrol == 1)
            return "The admin approval for the group link has been turned on.";
          else
            return "The group link has been turned on with admin approval on.";
        }
        if (accesscontrol == 1)
        {
          if (old_accesscontrol == 3)
            return "The admin approval for the group link has been turned off.";
          else
            return "The group link has been turned on with admin approval off.";
        }
      }
      else
      {
        if (accesscontrol == 4)
          return editorname + " turned off the group link.";
        if (accesscontrol == 3)
        {
          if (old_accesscontrol == 1)
            return editorname + " turned on admin approval for the group link.";
          else
            return editorname + " turned on the group link with admin approval on.";
        }
        if (accesscontrol == 1)
        {
          if (old_accesscontrol == 3)
            return editorname + " turned off admin approval for the group link.";
          else
            return editorname + " turned on the group link with admin approval off.";
        }
        // if (accesscontrol...
      }
    }


    // for accepting invites: check DecryptedGroupChange<1>: (editor)
    // and                         (DecryptedGroupChange<9>[]:DecryptedMember<1>: uuid (promotePendingMembers)
    //                              OR
    //                              DecryptedGroupChange<24>[]:DecryptedMember<1>: uuid (promotePendingPniAciMembers))
    //
    // if editor == self
    //   if (promotependingmembers contains self)
    //     "You accepted the invitation to the group."
    //   else
    //     "You added invited member Bob."
    // else
    //   if (promotependingmembers contains self)
    //     "Bob added you to the group." (or if editor is unknown: "You joined the group.");
    //   else if (promotependingmembers contains editor)
    //     "Bob accepted an invitation to the group."
    //   else
    //     "Bob added invited member Alice." (or if editor is unknown: "Alice joined the group.");

    auto groupchange_editor = groupchange.getFieldView<1>();
    auto const &groupchange_promotedmembers = groupchange.getFieldView<9>();
    auto const &groupchange_promotedpniacimembers = groupchange.getFieldView<24>();
    if (groupchange_editor.has_value() &&
        (groupchange_promotedmembers.size() || groupchange_promotedpniacimembers.size()))
    {
      // editor
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      std::vector<std::string> promotedmemberuuids;
      for (unsigned int i = 0; i < groupchange_promotedmembers.size(); ++i)
      {
        DecryptedMember dm = groupchange_promotedmembers[i];
        auto promotedmember_uuid = dm.getFieldView<1>();
        if (promotedmember_uuid.has_value())
        {
          auto [tmpuuid, tmpuuid_size] = promotedmember_uuid.value();
          std::string pmus = bepaald::bytesToHexString(tmpuuid, tmpuuid_size, true);
          pmus.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
          promotedmemberuuids.emplace_back(std::move(pmus));
        }
      }
      for (unsigned int i = 0; i < groupchange_promotedpniacimembers.size(); ++i)
      {
        DecryptedMember dm = groupchange_promotedpniacimembers[i];
        auto promotedmember_uuid = dm.getFieldView<1>();
        if (promotedmember_uuid.has_value())
        {
          auto [tmpuuid, tmpuuid_size] = promotedmember_uuid.value();
          std::string pmus = bepaald::bytesToHexString(tmpuuid, tmpuuid_size, true);
          pmus.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
          promotedmemberuuids.emplace_back(std::move(pmus));
        }
      }

      if (bepaald::contains(promotedmemberuuids, uuidstr)) // the editor is promoted -> someone accepted an invite
      {
        if (icon && *icon == IconType::NONE)
          *icon = IconType::MEMBER_APPROVED;

        if (uuidstr == d_selfuuid)
          return "You accepted the invitation to the group.";
        else
        {
          std::string promotedmember = getNameFromUuid(uuidstr);
          if (!promotedmember.empty())
            return promotedmember + " accepted an invitation to the group.";
          else
            return "A new member accepted an invitation to the group.";
        }
      }
      else // the editor is not one of the promoted members
      {
        if (icon && *icon == IconType::NONE)
          *icon = IconType::MEMBER_ADD;
        if (uuidstr == d_selfuuid)
        {
          if (promotedmemberuuids.size() == 1)
          {
            std::string promotedmember = getNameFromUuid(promotedmemberuuids[0]);
            if (!promotedmember.empty())
              return "You added invited member " + promotedmember;
            else
              return "You added an invited member";
          }
          else
            return "You added " + bepaald::toString(promotedmemberuuids.size()) + "invited members";
        }
        else if (bepaald::contains(promotedmemberuuids, d_selfuuid)) // you were not editor, but were promoted
        {
          std::string editorname = getNameFromUuid(uuidstr);
          if (!editorname.empty())
            return editorname + "added you to the group.";
          else
            return "You joined the group.";
        }
        else // you were not editor AND not the promoted member
        {
          std::string editorname = getNameFromUuid(uuidstr);
          if (!editorname.empty())
          {
            if (promotedmemberuuids.size() == 1)
            {
              std::string promotedmember = getNameFromUuid(promotedmemberuuids[0]);
              if (!promotedmember.empty())
                return editorname + " added invited member " + promotedmember + ".";
              else
                return editorname + " added an invited member.";
            }
            else
              return editorname + " added " + bepaald::toString(promotedmemberuuids.size()) + " invited members.";
          }
          else
          {
            if (promotedmemberuuids.size() == 1)
            {
              std::string promotedmember = getNameFromUuid(promotedmemberuuids[0]);
              if (!promotedmember.empty())
                return promotedmember + " joined the group.";
              else
                return "An invited member joined the group.";
            }
            else
              return bepaald::toString(promotedmemberuuids.size()) + " members joined the group.";
          }
        }
      }
    }

    // if no previous state is given, and this is revision 0 and new title -> created new group
    if (!groupv2ctx.getFieldView<4>().has_value()) // no previous state
    {
      auto groupctx = groupv2ctx.getFieldView<1>();
      if (groupctx.has_value() &&
          groupctx.value().getFieldView<2>().value_or(-1) == 0 &&
          groupchange_editor.has_value()) // revision == 0
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

        if (Types::isOutgoing(type))
        {
          if (icon && *icon == IconType::NONE)
            *icon = IconType::MEMBERS;
          return "You created the group.";
        }
        //else
        if (icon && *icon == IconType::NONE)
          *icon = IconType::MEMBER_ADD;
        return contactname + " added you to the group.";
      }
    }

    // check group title changed
    auto groupchange_decryptedtitle = groupchange.getFieldView<10>();
    if (groupchange_decryptedtitle.has_value())
    {
      auto groupchange_newtitle = groupchange_decryptedtitle.value().getFieldView<1>();
      if (groupchange_newtitle.has_value())
      {
        statusmsg.append((Types::isOutgoing(type) ? "You" : contactname)).append(" changed the group name to \"").append(groupchange_newtitle.value()).append("\".");
        if (icon)
          *icon = IconType::PENCIL;
      }
    }

    // check group description changed
    auto groupchange_descriptionchange = groupchange.getFieldView<20>();
    if (groupchange_descriptionchange.has_value()/* && groupchange.getField<20>().value().getField<1>().has_value()*/)
    {
      statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " changed the group description.";
      if (icon)
        *icon = IconType::PENCIL;
    }

    // check group avatar changed
    auto groupchange_avatarchange = groupchange.getFieldView<11>();
    if (groupchange_avatarchange.has_value()/* && groupchange.getField<11>().value().getField<1>().has_value()*/)
    {
      statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " changed the group avatar.";
      if (icon && *icon == IconType::NONE)
        *icon = IconType::AVATAR_UPDATE;
    }

    // check group timer changed : THIS TIMER IS IN SECONDS (message.expires_in, for non-group messages is in milliseconds)
    auto groupchange_timerchange = groupchange.getFieldView<12>();
    if (groupchange_timerchange.has_value()/* && groupchange.getField<12>().value().getField<1>().has_value()*/)
    {
      uint32_t newexp = groupchange_timerchange.value().getFieldView<1>().value_or(0);
      std::string time;
      if (newexp == 0)
        time = "Off";
      else if (newexp < 60) // less than full minute
        time = bepaald::toString(newexp) + " second" + (newexp > 1 ? "s" : "");
      else if (newexp < 60 * 60) // less than full hour
        time = bepaald::toString(newexp / 60) + " minute" + ((newexp / 60) > 1 ? "s" : "");
      else if (newexp < 24 * 60 * 60) // less than full day
        time = bepaald::toString(newexp / (60 * 60)) + " hour" + ((newexp / (60 * 60)) > 1 ? "s" : "");
      else if (newexp < 7 * 24 * 60 * 60) // less than full week
        time = bepaald::toString(newexp / (24 * 60 * 60)) + " day" + ((newexp / (24 * 60 * 60)) > 1 ? "s" : "");
      else // show newexp in number of weeks
        time = bepaald::toString(newexp / (7 * 24 * 60 * 60)) + " week" + ((newexp / (7 * 24 * 60 * 60)) > 1 ? "s" : "");
      statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " set the disappearing message timer to " + time + ".";
      if (icon && *icon == IconType::NONE)
        *icon = (newexp == 0) ? IconType::TIMER_DISABLE : IconType::TIMER_UPDATE;
    }

    // check memberrole change
    auto const &memberrolechanges = groupchange.getFieldView<5>();
    for (unsigned int i = 0; i < memberrolechanges.size(); ++i) // I dont know how this can be more than size() == 1
    {
      DecryptedModifyMemberRole const &mr = memberrolechanges[i];

      std::string uuidstr;
      auto memberrole_uuid = mr.getFieldView<1>();
      if (memberrole_uuid.has_value())
      {
        auto [uuid, uuid_size] = memberrole_uuid.value();
        uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }
      /*
        message Member {
        enum Role {
        UNKNOWN       = 0;
        DEFAULT       = 1;
        ADMINISTRATOR = 2;
        }*/
      int newrole = mr.getField<2>().value_or(0);

      if (newrole == 2)
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " made " + (uuidstr == d_selfuuid ? "you" : getNameFromUuid(uuidstr)) + " an admin.";
      else
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " revoked admin privileges from " + getNameFromUuid(uuidstr) + ".";
      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;
    }

    // check new member:
    auto const &newmembers = groupchange.getFieldView<3>();
    if (newmembers.size())
    {
      // get editor of this group change
      std::string editoruuid;
      if (groupchange_editor.has_value())
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        editoruuid = bepaald::bytesToHexString(uuid, uuid_size, true);
        editoruuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      for (unsigned int i = 0; i < newmembers.size(); ++i)
      {
        auto [uuid, uuid_size] = newmembers[i].getFieldView<1>().value_or(std::make_pair(nullptr, 0)); // bytes
        std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

        if (uuidstr == editoruuid) // you can't add yourself to the group, if this happens
        {                          // you joined via invite link (without approval)
          statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " joined the group via the group link";
          if (icon && *icon == IconType::NONE)
            *icon = IconType::MEMBER_APPROVED;
        }
        else
        {
          statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " added " +
            (uuidstr == d_selfuuid ? "you to the group" : getNameFromUuid(uuidstr)) + ".";
          if (icon && *icon == IconType::NONE)
            *icon = IconType::MEMBER_ADD;
        }
      }
    }

    // check members left:
    auto const &deletedmembers = groupchange.getFieldView<4>();
    for (unsigned int i = 0; i < deletedmembers.size(); ++i) // I dont know how this can be more than size() == 1
    {
      auto [uuid, uuid_size] = deletedmembers[i]; // bytes
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      if (Types::isOutgoing(type) && uuidstr == d_selfuuid)
      {
        statusmsg += (!statusmsg.empty() ? "\n" : "") + "You left the group."s;
        if (icon && *icon == IconType::NONE)
          *icon = IconType::GROUP_QUIT;
      }
      // else if editor == uuidstr -> "name left the group." ?
      else
      {
        // if (contactname.empty() && !Types::isOutgoing(type))
        //   statusmsg += (!statusmsg.empty() ? "\n" : "") + (uuidstr == d_selfuuid ? "You were" : getNameFromUuid(uuidstr) + "was") + " removed from the group.";
        // else
        statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " removed " +
          (uuidstr == d_selfuuid ? "you from the group" : getNameFromUuid(uuidstr)) + ".";
        if (icon && *icon == IconType::NONE)
          *icon = IconType::MEMBER_REMOVE;
      }
    }

    // // check members left:
    // auto deletedmembers = groupchange.getField<4>();
    // for (unsigned int i = 0; i < deletedmembers.size(); ++i) // I dont know how this can be more than size() == 1
    // {
    //   auto [uuid, uuid_size] = deletedmembers[i]; // bytes
    //   std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
    //   uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
    //   statusmsg += (!statusmsg.empty() ? "\n" : "") + (Types::isOutgoing(type) ? "You" : contactname) + " removed " + getNameFromUuid(uuidstr) + ".";
    // }

    // field 19 == newInviteLinkPassword
    auto groupchange_newinvitelinkpw = groupchange.getFieldView<19>();
    if (groupchange_newinvitelinkpw.has_value() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;

      // field 15 == newInviteLinkAccess (== always 1 (== admin approval off) on creation?)
      auto groupchange_newinvitelinkpw_accesscontrol = groupchange.getFieldView<15>();
      if (!groupchange_newinvitelinkpw_accesscontrol.has_value())
        statusmsg +=  (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " reset the group link.";
      else
      {
        int accesscontrol = groupchange_newinvitelinkpw_accesscontrol.value();
        statusmsg += (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " turned on the group link with admin approval " +
          (accesscontrol == 3 ? "on." : "off."); // never 3/on at creation
      }
    }

    // Field 21 'newIsAnnouncementGroup'
    auto groupchange_newannouncementgroup = groupchange.getFieldView<21>();
    if (groupchange_newannouncementgroup.has_value())
    {
      std::string uuidstr;
      if (groupchange_editor.has_value())
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
        uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;
      /*
        enum EnabledState {
        UNKNOWN  = 0;
        ENABLED  = 1;
        DISABLED = 2;
        }
      */
      int enabledstate = groupchange_newannouncementgroup.value();
      if (enabledstate == 2)
      {
        if (uuidstr.empty())
          statusmsg += "The group settings were changed to allow all members to send messages.";
        else
          statusmsg += (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " changed the group settings to allow all members to send messages.";
      }
      else
      {
        if (uuidstr.empty())
          statusmsg += "The group settings were changed to only allow all admins to send messages.";
        else
          statusmsg += (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " changed the group settings to only allow admins to send messages.";
      }
    }

    // Field 13 'newAttributeAccess' : who can edit group info
    auto groupchange_newattribute_access = groupchange.getFieldView<13>();
    if (groupchange_newattribute_access.has_value() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;
      /*
        UNKNOWN       = 0;
        ANY           = 1;
        MEMBER        = 2;
        ADMINISTRATOR = 3;
        UNSATISFIABLE = 4;
      */
      int accesscontrol = groupchange_newattribute_access.value();
      statusmsg += (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " changed who can edit group info to \"" +
        (accesscontrol == 3 ? "Only admins" : "All members") + "\".";
    }

    // Field 14 'newmemberaccess' : who can edit group membership
    auto groupchange_newmember_access = groupchange.getFieldView<14>();
    if (groupchange.getFieldView<14>().has_value() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;
      /*
        UNKNOWN       = 0;
        ANY           = 1;
        MEMBER        = 2;
        ADMINISTRATOR = 3;
        UNSATISFIABLE = 4;
      */
      int accesscontrol = groupchange_newmember_access.value();
      statusmsg += (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " changed who can edit group membership to \"" +
        (accesscontrol == 3 ? "Only admins" : "All members") + "\".";
    }

    // Field 16 'requesting member' : want to join
    auto const &groupchange_requestingmembers = groupchange.getFieldView<16>();
    if (groupchange_requestingmembers.size() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBERS;

      // if field 17 'deletereqestingmembers' is also present (and same uuid as reqesting member?)
      // the memebrs has cancelled their request.
      std::string cancelleduuidstr;
      auto groupchange_deletereqestingmembers = groupchange.getFieldView<17>();
      if (groupchange_deletereqestingmembers.size())
      {
        auto [cancelleduuid, cancelleduuid_size] = groupchange_deletereqestingmembers[0];
        cancelleduuidstr = bepaald::bytesToHexString(cancelleduuid, cancelleduuid_size, true);
        cancelleduuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      if (cancelleduuidstr == uuidstr)
      {
        statusmsg += (statusmsg.empty() ? "" : " ") +
          getNameFromUuid(uuidstr) + " requested and cancelled their request to join via the group link.";
      }
      else
      {
        // field 16 is (vector of) requesting members (= [uuid, profilekey, timestamp]).
        // But all that's needed is the uuid and we have that from <1> already
        statusmsg += (statusmsg.empty() ? "" : " ") +
          getNameFromUuid(uuidstr) + " requested to join via the group link.";
      }
    }

    // Field 18 'approved member' : added after request
    auto const &groupchange_approvedmembers = groupchange.getFieldView<18>();
    if (groupchange_approvedmembers.size() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBER_APPROVED;

      for (unsigned int i = 0; i < groupchange_approvedmembers.size(); ++i)
      {
        if (!groupchange_approvedmembers[i].getFieldView<1>().has_value())
          continue;
        auto [uuid2, uuid_size2] = groupchange_approvedmembers[i].getFieldView<1>().value();
        std::string uuidstr2 = bepaald::bytesToHexString(uuid2, uuid_size2, true);
        uuidstr2.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        std::string newmem = getNameFromUuid(uuidstr2);
        if (!newmem.empty())
          statusmsg += (statusmsg.empty() ? "" : " ") +
            (Types::isOutgoing(type) ? "You" : getNameFromUuid(uuidstr)) + " approved a request to join the group from " + newmem + ".";
      }
    }

    // field 22 'new banned member' , when combined with 17 ('delete reqeusting members) : request denied
    auto const &groupchange_bannedmembers = groupchange.getFieldView<22>();
    if (groupchange_bannedmembers.size() && groupchange_editor.has_value())
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBER_REJECTED;

      DecryptedBannedMember decryptedbannedmember = groupchange_bannedmembers[0];
      std::string banneduuidstr;
      auto decryptedbannedmember_uuid = decryptedbannedmember.getFieldView<1>();
      if (decryptedbannedmember_uuid.has_value())
      {
        auto [banneduuid, banneduuid_size] = decryptedbannedmember_uuid.value();
        banneduuidstr = bepaald::bytesToHexString(banneduuid, banneduuid_size, true);
        banneduuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      // if field 17 'deletereqestingmembers' is also present (and same uuid as reqesting member?)
      // the members request was denied
      std::string cancelleduuidstr;
      auto const &deniedmembers = groupchange.getFieldView<17>();
      if (deniedmembers.size())
      {
        auto [cancelleduuid, cancelleduuid_size] = deniedmembers[0];
        cancelleduuidstr = bepaald::bytesToHexString(cancelleduuid, cancelleduuid_size, true);
        cancelleduuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      }

      if (cancelleduuidstr == banneduuidstr)
      {
        statusmsg += (statusmsg.empty() ? "" : " ") +
          getNameFromUuid(uuidstr) + " denied a request to join the group from " + getNameFromUuid(banneduuidstr);
      }
      else // non-requesting member was banned???
      {
      }
    }

    // if memberlabels-accesscontrol was changed:
    if (groupchange.getFieldView<27>().has_value())
    {
      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEGAPHONE;

      // new value: 0 unknown, 1 any, 2 member, 3 admin, 4 unsatisfiable
      auto accesscontrol = groupchange.getFieldView<27>().value();

      // get editor
      std::string editoruuid;
      if (groupchange_editor.has_value())
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        if (uuid_size == 16) //likely
        {
          editoruuid = bepaald::bytesToHexString(uuid, uuid_size, true);
          editoruuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        }
      }

      std::string editorname(editoruuid == d_selfuuid ? "You " : editoruuid.empty() ? editoruuid : getNameFromUuid(editoruuid));
      if (editorname.empty())
        editorname = "An admin";

      if (accesscontrol == 3)
        return editorname + " changed who can add member labels to \"Only admins\".";
      else if (accesscontrol == 2)
        return editorname + " changed who can add member labels to \"All members\".";
      else [[unlikely]]
        return editorname + " changed who can add member labels."; // cant happen (?)
    }

    // if group was ended
    if (groupchange.getFieldView<28>().has_value() &&
        groupchange.getFieldView<28>().value() == true)
    {
      //std::cout << "GROUP TERMINATED!" << std::endl;
      if (icon && *icon == IconType::NONE)
        *icon = IconType::GROUP_TERMINATE;

      // get editor
      std::string editoruuid;
      if (groupchange_editor.has_value())
      {
        auto [uuid, uuid_size] = groupchange_editor.value();
        if (uuid_size == 16) //likely
        {
          editoruuid = bepaald::bytesToHexString(uuid, uuid_size, true);
          editoruuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
        }
      }

      std::string editorname(editoruuid == d_selfuuid ? "You" : editoruuid.empty() ? editoruuid : getNameFromUuid(editoruuid));
      if (editorname.empty()) [[unlikely]]
        return "The group has been ended.";
      else
        return bepaald::concat(editorname, " ended the group.");
    }


    // if groupchange has editor, but nothing else : editor added you to the group:
    if (groupchange_editor.has_value() &&
        !(groupchange.getFieldView<2>().has_value() ||    // 2
          newmembers.size() ||                            // 3
          deletedmembers.size() ||                        // 4
          memberrolechanges.size() ||                     // 5
          groupchange.getFieldView<6>().size() ||         // 6
          groupchange.getFieldView<7>().size() ||         // 7
          groupchange.getFieldView<8>().size() ||         // 8
          groupchange_promotedmembers.size() ||           // 9
          groupchange_decryptedtitle.has_value() ||       // 10
          groupchange_avatarchange.has_value() ||         // 11
          groupchange_timerchange.has_value() ||          // 12
          groupchange_newattribute_access.has_value() ||  // 13
          groupchange_newmember_access.has_value() ||     // 14
          groupchange_invitelink_changed.has_value() ||   // 15
          groupchange_requestingmembers.size() ||         // 16
          groupchange.getFieldView<17>().size() ||        // 17
          groupchange_approvedmembers.size() ||           // 18
          groupchange_newinvitelinkpw.has_value() ||      // 19
          groupchange_descriptionchange.has_value() ||    // 20
          groupchange_newannouncementgroup.has_value() || // 21
          groupchange_bannedmembers.size() ||             // 22
          groupchange.getFieldView<23>().size() ||        // 23
          groupchange.getFieldView<24>().size()))         // 24
    {
      auto [uuid, uuid_size] = groupchange_editor.value();
      std::string uuidstr = bepaald::bytesToHexString(uuid, uuid_size, true);
      uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');

      statusmsg = getNameFromUuid(uuidstr) + " added you to the group.";
      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBER_ADD;
    }
  } // if groupchange (groupv2ctx.field2)

  if (!groupv2ctx.getFieldView<2>().has_value() && // no GroupChange
      groupv2ctx.getFieldView<3>().has_value() &&  // yes current state, but...
      !groupv2ctx.getFieldView<4>().has_value() && // no previous state
      Types::isOutgoing(type))
  {
    auto groupctx = groupv2ctx.getFieldView<1>();
    if (!groupctx.has_value() ||
        groupctx.value().getFieldView<2>().value_or(-1) > 0) // revision > 0
    {                                                        // we just joined!
      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBER_ADD;
      return "You joined the group.";
    }
  }

  // maybe someone was invited, check for pending memebers...
  auto currentdecryptedgroup = groupv2ctx.getFieldView<3>();
  if (currentdecryptedgroup.has_value())
  {
    auto const &pendingmembers = currentdecryptedgroup->getFieldView<8>();
    if (pendingmembers.size())
    {
      std::vector<std::pair<std::string, std::string>> all_invitedmembers; // <invited_uuid, invited_by_uuid>
      std::vector<std::string> old_invitedmembers;

      for (unsigned int i = 0; i < pendingmembers.size(); ++i)
      {
        DecryptedPendingMember pm(pendingmembers[i]);
        //pm.print();
        auto pm_uuid = pm.getFieldView<1>();
        if (pm_uuid.has_value())
        {
          auto [inv_uuid, inv_uuid_size] = pm_uuid.value();
          std::string invited_uuidstr = bepaald::bytesToHexString(inv_uuid, inv_uuid_size, true);
          invited_uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
          std::string invited_by_uuidstr;
          auto invitedby_uuid = pm.getFieldView<3>();
          if (invitedby_uuid.has_value())
          {
            auto [inv_by_uuid, inv_by_uuid_size] = invitedby_uuid.value();
            invited_by_uuidstr = bepaald::bytesToHexString(inv_by_uuid, inv_by_uuid_size, true);
            invited_by_uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
          }
          all_invitedmembers.emplace_back(std::move(invited_uuidstr), std::move(invited_by_uuidstr));
        }
      }

      auto const &prevpendingmembers = groupv2ctx.getFieldView<4, 8>();
      if (prevpendingmembers.size())
      {
        for (unsigned int i = 0; i < prevpendingmembers.size(); ++i)
        {
          DecryptedPendingMember pm(prevpendingmembers[i]);
          //pm.print();
          auto pm_uuid = pm.getFieldView<1>();
          if (pm_uuid.has_value())
          {
            auto [inv_uuid, inv_uuid_size] = pm_uuid.value();
            std::string invited_uuidstr = bepaald::bytesToHexString(inv_uuid, inv_uuid_size, true);
            invited_uuidstr.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
            old_invitedmembers.emplace_back(std::move(invited_uuidstr));
          }
        }
      }

      // move new invited members for this group change into vector
      std::vector<std::pair<std::string, std::string>> invitedmembers; // <invited_uuid, invited_by_uuid>
      std::copy_if(std::make_move_iterator(all_invitedmembers.begin()), std::make_move_iterator(all_invitedmembers.end()),
                   std::back_inserter(invitedmembers), [&](auto const &mem){ return !bepaald::contains(old_invitedmembers, mem.first); } );

      if (icon && *icon == IconType::NONE)
        *icon = IconType::MEMBER_ADD;

      if (invitedmembers.size() > 0)
      {
        std::string return_message;
        // assert all were invited by same person
        if (std::all_of(invitedmembers.begin(), invitedmembers.end(), [&](auto const &p){ return p.second == invitedmembers.begin()->second; })) [[likely]]
        {
          std::string inviter_name = invitedmembers.begin()->second == d_selfuuid ? "You" : getNameFromUuid(invitedmembers[0].second);

          int self_is_invited = std::count_if(invitedmembers.begin(), invitedmembers.end(), [&](auto const &p){ return p.first == d_selfuuid; });
          int others_invited = invitedmembers.size() - self_is_invited;

          if (self_is_invited)
          {
            if (!inviter_name.empty())
              return_message = inviter_name + " invited you to the group.";
            else
              return_message = "You were invited to the group.";

            // self was invited, but more people were invited, add new line
            if (others_invited > 0)
              return_message += "\n";
          }

          if (others_invited == 1)
          {
            auto invited_it = std::find_if(invitedmembers.begin(), invitedmembers.end(), [&](auto const &p){ return p.first != d_selfuuid; });
            std::string invited_name = getNameFromUuid(invited_it->first);

            if (!inviter_name.empty())
            {
              if (!invited_name.empty())
                return_message += inviter_name + " invited " + invited_name + " to the group.";
              else
                return_message += inviter_name + " invited 1 person to the group.";
            }
            else
            {
              if (!invited_name.empty())
                return_message += invited_name + " was invited to the group.";
              else
                return_message += "1 person was invited to the group.";
            }
          }
          if (others_invited > 1)
          {
            if (!inviter_name.empty())
              return_message += inviter_name + " invited " + bepaald::toString(others_invited) + " people to the group.";
            else
              return_message += bepaald::toString(others_invited) + " people were invited to the group.";
          }

          if (!return_message.empty())
            return return_message;
        }
        else
        {
          Logger::warning("Unsupported group change: multiple people invited by multiple inviters in a single GroupChange");
        }
      }
    }
  }

  if (statusmsg.empty())
  {
    // std::cout << "" << std::endl;
    // std::cout << "  ********" << std::endl;
    // std::cout << body << std::endl;
    // //groupv2ctx.print();
    // groupv2ctx.getField<2>().value().print();
    // std::cout << "  ********" << std::endl;
    // std::cout << "" << std::endl;
    return "(group V2 update)";
  }
  return statusmsg;
}
