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

/*
  Decodes a (backup.)GroupChangeChatUpdate, as found in MessageExtras.gv2UpdateDescription.groupChangeUpdate.
  This is how group updates are stored in messages restored from a backup V2 (and the only
  form they exist in in the backup itself).
*/
std::string SignalBackup::decodeGroupChangeChatUpdate(GroupChangeChatUpdate const &groupchange, IconType *icon) const
{
  auto setIcon = [icon](IconType i)
  {
    if (icon && *icon == IconType::NONE)
      *icon = i;
  };

  auto isSelf = [this](std::optional<std::pair<unsigned char *, uint64_t>> const &aci)
  {
    if (!aci.has_value() || aci->second != 16 || d_selfuuid.empty())
      return false;
    std::string uuid = bepaald::bytesToHexString(aci->first, aci->second, true);
    uuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
    return uuid == d_selfuuid;
  };

  // returns name for aci, "You"/"you" if aci is self. Empty if aci is not set.
  auto name = [this, &isSelf](std::optional<std::pair<unsigned char *, uint64_t>> const &aci, bool capitalize = true) -> std::string
  {
    if (!aci.has_value() || aci->second != 16)
      return std::string();
    if (isSelf(aci))
      return capitalize ? "You" : "you";
    std::string uuid = bepaald::bytesToHexString(aci->first, aci->second, true);
    uuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
    std::string n = getNameFromUuid(uuid, true);
    return n.empty() ? "Unknown" : n;
  };

  auto accessLevel = [](long long int level) STATICLAMBDA -> std::string
  {
    // UNKNOWN = 0 (Interpret as "Unsatisfiable"), ANY = 1, MEMBER = 2, ADMINISTRATOR = 3, UNSATISFIABLE = 4
    switch (level)
    {
      case 1: return "Anyone";
      case 2: return "All members";
      case 3: return "Only admins";
      default: return "Unknown";
    }
  };

  auto duration = [](uint64_t seconds) STATICLAMBDA -> std::string
  {
    if (seconds < 60) // less than full minute
      return bepaald::toString(seconds) + " second" + (seconds > 1 ? "s" : "");
    if (seconds < 60 * 60) // less than full hour
      return bepaald::toString(seconds / 60) + " minute" + ((seconds / 60) > 1 ? "s" : "");
    if (seconds < 24 * 60 * 60) // less than full day
      return bepaald::toString(seconds / (60 * 60)) + " hour" + ((seconds / (60 * 60)) > 1 ? "s" : "");
    if (seconds < 7 * 24 * 60 * 60) // less than full week
      return bepaald::toString(seconds / (24 * 60 * 60)) + " day" + ((seconds / (24 * 60 * 60)) > 1 ? "s" : "");
    return bepaald::toString(seconds / (7 * 24 * 60 * 60)) + " week" + ((seconds / (7 * 24 * 60 * 60)) > 1 ? "s" : "");
  };

  auto people = [](uint64_t count) STATICLAMBDA -> std::string
  {
    return count == 1 ? "1 person" : bepaald::toString(count) + " people";
  };

  std::string statusmsg;
  auto add = [&statusmsg](std::string const &msg)
  {
    statusmsg += (statusmsg.empty() ? "" : "\n") + msg;
  };

  for (auto const &update : groupchange.getFieldView<1>())
  {
    if (auto u = update.getFieldView<1>(); u.has_value()) // generic
    {
      std::string updater = name(u->getFieldView<1>());
      add(updater.empty() ? "The group was updated." : updater + " updated the group.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<2>(); u.has_value()) // creation
    {
      std::string updater = name(u->getFieldView<1>());
      if (updater.empty())
        add("The group was created.");
      else if (isSelf(u->getFieldView<1>()))
        add("You created the group.");
      else
        add(updater + " added you to the group.");
      setIcon(IconType::MEMBERS);
      continue;
    }

    if (auto u = update.getFieldView<3>(); u.has_value()) // name
    {
      std::string updater = name(u->getFieldView<1>());
      std::string newname(u->getFieldView<2>().value_or(std::string_view()));
      add(updater.empty() ? "The group name has changed to \"" + newname + "\"." :
          updater + " changed the group name to \"" + newname + "\".");
      setIcon(IconType::PENCIL);
      continue;
    }

    if (auto u = update.getFieldView<4>(); u.has_value()) // avatar
    {
      std::string updater = name(u->getFieldView<1>());
      bool removed = u->getFieldView<2>().value_or(false);
      if (updater.empty())
        add(removed ? "The group avatar has been removed." : "The group avatar has been changed.");
      else
        add(updater + (removed ? " removed the group avatar." : " changed the group avatar."));
      setIcon(IconType::AVATAR_UPDATE);
      continue;
    }

    if (auto u = update.getFieldView<5>(); u.has_value()) // description
    {
      std::string updater = name(u->getFieldView<1>());
      bool removed = u->getFieldView<2>().value_or(std::string_view()).empty();
      if (updater.empty())
        add(removed ? "The group description has been removed." : "The group description has been changed.");
      else
        add(updater + (removed ? " removed the group description." : " changed the group description."));
      setIcon(IconType::PENCIL);
      continue;
    }

    if (auto u = update.getFieldView<6>(); u.has_value()) // membership access
    {
      std::string updater = name(u->getFieldView<1>());
      std::string level = accessLevel(u->getFieldView<2>().value_or(0));
      add(updater.empty() ? "Who can edit group membership has been changed to \"" + level + "\"." :
          updater + " changed who can edit group membership to \"" + level + "\".");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<7>(); u.has_value()) // attributes access
    {
      std::string updater = name(u->getFieldView<1>());
      std::string level = accessLevel(u->getFieldView<2>().value_or(0));
      add(updater.empty() ? "Who can edit group info has been changed to \"" + level + "\"." :
          updater + " changed who can edit group info to \"" + level + "\".");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<8>(); u.has_value()) // announcement only
    {
      std::string updater = name(u->getFieldView<1>());
      bool announcementonly = u->getFieldView<2>().value_or(false);
      std::string setting(announcementonly ? "only allow admins to send messages." : "allow all members to send messages.");
      add(updater.empty() ? "The group settings were changed to " + setting : updater + " changed the group settings to " + setting);
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<9>(); u.has_value()) // admin status
    {
      std::string updater = name(u->getFieldView<1>());
      std::string member = name(u->getFieldView<2>(), false);
      bool granted = u->getFieldView<3>().value_or(false);
      if (updater.empty())
        add(isSelf(u->getFieldView<2>()) ? (granted ? "You are now an admin." : "You are no longer an admin.") :
            name(u->getFieldView<2>()) + (granted ? " is now an admin." : " is no longer an admin."));
      else
        add(granted ? updater + " made " + member + " an admin." : updater + " revoked admin privileges from " + member + ".");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<10>(); u.has_value()) // member left
    {
      add(name(u->getFieldView<1>()) + " left the group.");
      setIcon(IconType::GROUP_QUIT);
      continue;
    }

    if (auto u = update.getFieldView<11>(); u.has_value()) // member removed
    {
      std::string remover = name(u->getFieldView<1>());
      if (remover.empty())
        add(isSelf(u->getFieldView<2>()) ? "You are no longer in the group." : name(u->getFieldView<2>()) + " is no longer in the group.");
      else
        add(remover + " removed " + (isSelf(u->getFieldView<2>()) ? "you from the group" : name(u->getFieldView<2>())) + ".");
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<12>(); u.has_value()) // self invited
    {
      std::string inviter = name(u->getFieldView<1>());
      add(inviter.empty() ? "You were invited to the group." : inviter + " invited you to the group.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<13>(); u.has_value()) // self invited other
    {
      add("You invited " + name(u->getFieldView<1>()) + " to the group.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<14>(); u.has_value()) // unknown invitee
    {
      std::string inviter = name(u->getFieldView<1>());
      uint32_t count = u->getFieldView<2>().value_or(0);
      add(inviter.empty() ? people(count) + (count == 1 ? " was" : " were") + " invited to the group." :
          inviter + " invited " + people(count) + " to the group.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<15>(); u.has_value()) // invitation accepted
    {
      std::string inviter = name(u->getFieldView<1>());
      if (isSelf(u->getFieldView<2>()))
        add(inviter.empty() ? "You accepted the invitation to the group." : "You accepted an invitation to the group from " + inviter + ".");
      else
        add(name(u->getFieldView<2>()) + " accepted an invitation to the group" + (inviter.empty() ? "." : " from " + name(u->getFieldView<1>(), false) + "."));
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<16>(); u.has_value()) // invitation declined
    {
      if (isSelf(u->getFieldView<2>()))
        add("You declined the invitation to the group.");
      else if (u->getFieldView<2>().has_value())
        add(name(u->getFieldView<2>()) + " declined the invitation to the group.");
      else
        add("1 person declined the invitation to the group.");
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<17>(); u.has_value()) // member joined
    {
      add(name(u->getFieldView<1>()) + " joined the group.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<18>(); u.has_value()) // member added
    {
      std::string updater = name(u->getFieldView<1>());
      if (updater.empty())
        add(name(u->getFieldView<2>()) + " joined the group.");
      else
        add(updater + " added " + (isSelf(u->getFieldView<2>()) ? "you to the group" : name(u->getFieldView<2>())) + ".");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<19>(); u.has_value()) // self invitation revoked
    {
      std::string revoker = name(u->getFieldView<1>());
      add(revoker.empty() ? "An admin revoked your invitation to the group." : revoker + " revoked your invitation to the group.");
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<20>(); u.has_value()) // invitations revoked
    {
      std::string updater = name(u->getFieldView<1>());
      size_t count = u->getFieldView<2>().size();
      std::string invitations(count == 1 ? "an invitation" : bepaald::toString(count) + " invitations");
      add(updater.empty() ? (count == 1 ? "An invitation" : bepaald::toString(count) + " invitations") + std::string(" to the group ") +
          (count == 1 ? "was" : "were") + " revoked." :
          updater + " revoked " + invitations + " to the group.");
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<21>(); u.has_value()) // join request
    {
      add(isSelf(u->getFieldView<1>()) ? "You sent a request to join the group." :
          name(u->getFieldView<1>()) + " requested to join via the group link.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (auto u = update.getFieldView<22>(); u.has_value()) // join request approval
    {
      std::string updater = name(u->getFieldView<2>());
      bool approved = u->getFieldView<3>().value_or(false);
      if (isSelf(u->getFieldView<1>()))
        add(approved ? "Your request to join the group has been approved." : "Your request to join the group has been denied by an admin.");
      else if (updater.empty())
        add("A request to join the group from " + name(u->getFieldView<1>()) + (approved ? " was approved." : " was denied."));
      else
        add(updater + (approved ? " approved" : " denied") + " a request to join the group from " + name(u->getFieldView<1>()) + ".");
      setIcon(approved ? IconType::MEMBER_APPROVED : IconType::MEMBER_REJECTED);
      continue;
    }

    if (auto u = update.getFieldView<23>(); u.has_value()) // join request canceled
    {
      add(isSelf(u->getFieldView<1>()) ? "You canceled your request to join the group." :
          name(u->getFieldView<1>()) + " canceled their request to join the group.");
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<24>(); u.has_value()) // invite link reset
    {
      std::string updater = name(u->getFieldView<1>());
      add(updater.empty() ? "The group link has been reset." : updater + " reset the group link.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<25>(); u.has_value()) // invite link enabled
    {
      std::string updater = name(u->getFieldView<1>());
      std::string approval(u->getFieldView<2>().value_or(false) ? "on" : "off");
      add(updater.empty() ? "The group link has been turned on with admin approval " + approval + "." :
          updater + " turned on the group link with admin approval " + approval + ".");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<26>(); u.has_value()) // invite link admin approval
    {
      std::string updater = name(u->getFieldView<1>());
      std::string approval(u->getFieldView<2>().value_or(false) ? "on" : "off");
      add(updater.empty() ? "The admin approval for the group link has been turned " + approval + "." :
          updater + " turned " + approval + " admin approval for the group link.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<27>(); u.has_value()) // invite link disabled
    {
      std::string updater = name(u->getFieldView<1>());
      add(updater.empty() ? "The group link has been turned off." : updater + " turned off the group link.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<28>(); u.has_value()) // joined by link
    {
      add(name(u->getFieldView<1>()) + " joined the group via the group link.");
      setIcon(IconType::MEMBER_ADD);
      continue;
    }

    if (update.getFieldView<29>().has_value()) // gv2 migration
    {
      add("This group was updated to a New Group.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (update.getFieldView<30>().has_value()) // gv2 migration, self invited
    {
      add("You couldn't be added to the New Group and have been invited to join.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<31>(); u.has_value()) // gv2 migration, invited members
    {
      uint32_t count = u->getFieldView<1>().value_or(0);
      add(bepaald::toString(count) + (count == 1 ? " member" : " members") + " couldn't be added to the New Group and " +
          (count == 1 ? "has" : "have") + " been invited to join.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<32>(); u.has_value()) // gv2 migration, dropped members
    {
      uint32_t count = u->getFieldView<1>().value_or(0);
      add(bepaald::toString(count) + (count == 1 ? " member" : " members") + " couldn't be added to the New Group and " +
          (count == 1 ? "has" : "have") + " been removed.");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<33>(); u.has_value()) // sequence of requests and cancels
    {
      uint32_t count = u->getFieldView<2>().value_or(0);
      add(name(u->getFieldView<1>()) + " requested and canceled their request to join via the group link" +
          (count > 1 ? " " + bepaald::toString(count) + " times." : "."));
      setIcon(IconType::MEMBER_REMOVE);
      continue;
    }

    if (auto u = update.getFieldView<34>(); u.has_value()) // expiration timer
    {
      std::string updater = name(u->getFieldView<2>());
      uint64_t seconds = u->getFieldView<1>().value_or(0) / 1000;
      if (seconds == 0)
        add(updater.empty() ? "Disappearing messages disabled." : updater + " disabled disappearing messages.");
      else
        add(updater.empty() ? "The disappearing message timer has been set to " + duration(seconds) + "." :
            updater + " set the disappearing message timer to " + duration(seconds) + ".");
      setIcon(seconds == 0 ? IconType::TIMER_DISABLE : IconType::TIMER_UPDATE);
      continue;
    }

    if (auto u = update.getFieldView<35>(); u.has_value()) // member label access
    {
      std::string updater = name(u->getFieldView<1>());
      std::string level = accessLevel(u->getFieldView<2>().value_or(0));
      add(updater.empty() ? "Who can add member labels has been changed to \"" + level + "\"." :
          updater + " changed who can add member labels to \"" + level + "\".");
      setIcon(IconType::MEGAPHONE);
      continue;
    }

    if (auto u = update.getFieldView<36>(); u.has_value()) // group terminated
    {
      std::string updater = name(u->getFieldView<1>());
      add(updater.empty() ? "The group was ended." : updater + " ended the group.");
      setIcon(IconType::GROUP_TERMINATE);
      continue;
    }
  }

  return statusmsg;
}
