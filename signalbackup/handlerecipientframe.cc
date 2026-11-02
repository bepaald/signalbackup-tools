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
#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"

namespace
{
  // Backup.proto 'enum AvatarColor' -> Android AvatarColor.serialize()
  std::any avatarColorToString(std::optional<int64_t> const &color)
  {
    static std::array<std::string_view, 12> constexpr colors{"A100", "A110", "A120", "A130", "A140", "A150",
                                                              "A160", "A170", "A180", "A190", "A200", "A210"};
    if (!color.has_value() || color.value() < 0 || color.value() >= static_cast<int64_t>(colors.size()))
      return std::any(nullptr);
    return std::string(colors[color.value()]);
  }

  std::any optionalString(std::optional<std::string_view> const &s)
  {
    if (!s.has_value() || s->empty())
      return std::any(nullptr);
    return std::string(s.value());
  }

  std::string joinNames(std::optional<std::string_view> const &given, std::optional<std::string_view> const &family)
  {
    std::string joined(given.value_or(std::string_view()));
    if (family.has_value() && !family->empty())
      joined += (joined.empty() ? "" : " ") + std::string(family.value());
    return joined;
  }
}

bool SignalBackup::handleRecipientFrame(BackupV2::Frame const &f, BackupV2ImportState *state)
{
  // recipientframe = id + oneof(contact, group, distributionlistitem, self, releasenotes, calllink);
  auto recipient = f.getFieldView<RECIPIENT_FRAMENUMBER>();
  if (!recipient.has_value()) [[unlikely]]
    return false;

  auto backup_id = recipient->getFieldView<1>();
  if (!backup_id.has_value()) [[unlikely]]
  {
    Logger::warning("Recipient frame without id. Skipping...");
    ++state->skippedrecipients;
    return true;
  }

  auto contact = recipient->getFieldView<2>();
  if (contact.has_value())
  {
    std::string aci = contact->getFieldView<1>().has_value() ? bv2UuidFromBytes(contact->getFieldView<1>().value()) : std::string();
    std::string pni = contact->getFieldView<2>().has_value() ? bv2UuidFromBytes(contact->getFieldView<2>().value()) : std::string();
    auto e164 = contact->getFieldView<4>();

    if (aci.empty() && pni.empty() && !e164.has_value())
    {
      Logger::warning("Recipient without any identifier. Skipping...");
      ++state->skippedrecipients;
      return true;
    }

    auto nickname = contact->getFieldView<16>();
    std::optional<std::string_view> nick_given;
    std::optional<std::string_view> nick_family;
    if (nickname.has_value())
    {
      nick_given = nickname->getFieldView<1>();
      nick_family = nickname->getFieldView<2>();
    }

    long long int registered = 0; // UNKNOWN
    long long int unregistered_timestamp = 0;
    if (contact->getFieldView<7>().has_value())
      registered = 1;             // REGISTERED
    else if (auto notregistered = contact->getFieldView<8>(); notregistered.has_value())
    {
      registered = 2;             // NOT_REGISTERED
      unregistered_timestamp = notregistered->getFieldView<1>().value_or(0);
    }

    std::any new_rid;
    if (!insertRow("recipient",
                   {{"type", 0},
                    {"aci", aci.empty() ? std::any(nullptr) : aci},
                    {"pni", pni.empty() ? std::any(nullptr) : "PNI:" + pni},
                    {"e164", e164.has_value() ? "+" + bepaald::toString(e164.value()) : std::any(nullptr)},
                    {"username", optionalString(contact->getFieldView<3>())},
                    {"blocked", contact->getFieldView<5>().value_or(false) ? 1 : 0},
                    {"hidden", static_cast<long long int>(contact->getFieldView<6>().value_or(0))},
                    {"registered", registered},
                    {"unregistered_timestamp", unregistered_timestamp},
                    {"profile_key", contact->getFieldView<9>().has_value() ? Base64::bytesToBase64String(contact->getFieldView<9>().value()) : std::any(nullptr)},
                    {"profile_sharing", contact->getFieldView<10>().value_or(false) ? 1 : 0},
                    {"profile_given_name", optionalString(contact->getFieldView<11>())},
                    {"profile_family_name", optionalString(contact->getFieldView<12>())},
                    {"profile_joined_name", joinNames(contact->getFieldView<11>(), contact->getFieldView<12>())},
                    {"nickname_given_name", optionalString(nick_given)},
                    {"nickname_family_name", optionalString(nick_family)},
                    {"nickname_joined_name", nickname.has_value() ? std::any(joinNames(nick_given, nick_family)) : std::any(nullptr)},
                    {"note", optionalString(contact->getFieldView<17>())},
                    {"system_given_name", optionalString(contact->getFieldView<18>())},
                    {"system_family_name", optionalString(contact->getFieldView<19>())},
                    {"system_joined_name", (contact->getFieldView<18>().has_value() || contact->getFieldView<19>().has_value()) ?
                                           std::any(joinNames(contact->getFieldView<18>(), contact->getFieldView<19>())) : std::any(nullptr)},
                    {"system_nickname", optionalString(contact->getFieldView<20>())},
                    {"avatar_color", avatarColorToString(contact->getFieldView<21>())}},
                   "_id", &new_rid)) [[unlikely]]
    {
      Logger::error("Failed to insert contact");
      return false;
    }
    long long int rid = std::any_cast<long long int>(new_rid);
    state->recipients[backup_id.value()] = rid;

    // identity key
    auto identity_key = contact->getFieldView<14>();
    if (identity_key.has_value() && (!aci.empty() || !pni.empty()))
      if (!insertRow("identities",
                     {{"address", !aci.empty() ? aci : "PNI:" + pni},
                      {"identity_key", Base64::bytesToBase64String(identity_key.value())},
                      {"verified", static_cast<long long int>(contact->getFieldView<15>().value_or(0))}})) [[unlikely]]
        Logger::warning("Failed to insert identity key for recipient");

    return true;
  }

  auto group = recipient->getFieldView<3>();
  if (group.has_value())
  {
    auto masterkey = group->getFieldView<1>();
    if (!masterkey.has_value())
    {
      Logger::warning("Group without master key. Skipping...");
      ++state->skippedrecipients;
      return true;
    }
    std::string group_id = bv2GroupIdFromMasterKey(masterkey.value());
    if (group_id.empty()) [[unlikely]]
    {
      Logger::error("Failed to derive group id from master key");
      return false;
    }

    std::any new_rid;
    if (!insertRow("recipient",
                   {{"type", 3}, // GV2
                    {"group_id", group_id},
                    {"profile_sharing", group->getFieldView<2>().value_or(false) ? 1 : 0},
                    {"blocked", group->getFieldView<6>().value_or(false) ? 1 : 0},
                    {"avatar_color", avatarColorToString(group->getFieldView<7>())}},
                   "_id", &new_rid)) [[unlikely]]
    {
      Logger::error("Failed to insert group recipient");
      return false;
    }
    long long int rid = std::any_cast<long long int>(new_rid);
    state->recipients[backup_id.value()] = rid;

    // The backup contains the (decrypted) group state, convert it to the
    // DecryptedGroup that Android stores in groups.decrypted_group.
    DecryptedGroup decrypted_group;
    std::string title;
    std::vector<std::pair<std::string, long long int>> members;
    long long int revision = 0;
    auto snapshot = group->getFieldView<5>();
    if (snapshot.has_value())
    {
      if (auto title_blob = snapshot->getFieldView<2>(); title_blob.has_value())
        title = title_blob->getFieldView<1>().value_or(std::string_view());
      decrypted_group.addField<2>(title);

      if (auto avatar = snapshot->getFieldView<3>(); avatar.has_value() && !avatar->empty())
        decrypted_group.addField<3>(std::string(avatar.value()));

      if (auto timer_blob = snapshot->getFieldView<4>(); timer_blob.has_value())
      {
        DecryptedTimer timer;
        timer.addField<1>(timer_blob->getFieldView<3>().value_or(0));
        decrypted_group.addField<4>(timer);
      }

      if (auto accesscontrol = snapshot->getFieldView<5>(); accesscontrol.has_value())
        decrypted_group.addField<5>(::AccessControl(accesscontrol->data(), accesscontrol->size())); // identical message

      revision = snapshot->getFieldView<6>().value_or(0);
      decrypted_group.addField<6>(static_cast<uint32_t>(revision));

      for (auto const &m : snapshot->getFieldView<7>())
      {
        auto userid = m.getFieldView<1>();
        if (!userid.has_value())
          continue;
        DecryptedMember dm;
        dm.addField<1>(userid.value());
        long long int role = m.getFieldView<2>().value_or(0);
        dm.addField<2>(role);
        if (auto joined = m.getFieldView<5>(); joined.has_value())
          dm.addField<5>(joined.value());
        if (auto labelemoji = m.getFieldView<6>(); labelemoji.has_value())
          dm.addField<7>(std::string(labelemoji.value()));
        if (auto labelstring = m.getFieldView<7>(); labelstring.has_value())
          dm.addField<8>(std::string(labelstring.value()));
        decrypted_group.addField<7>(dm);
        members.emplace_back(bv2UuidFromBytes(userid.value()), role);
      }

      for (auto const &pm : snapshot->getFieldView<8>())
      {
        auto member = pm.getFieldView<1>();
        if (!member.has_value())
          continue;
        DecryptedPendingMember dpm;
        if (auto userid = member->getFieldView<1>(); userid.has_value())
          dpm.addField<1>(userid.value());
        dpm.addField<2>(static_cast<long long int>(member->getFieldView<2>().value_or(0)));
        if (auto addedby = pm.getFieldView<2>(); addedby.has_value())
          dpm.addField<3>(addedby.value());
        dpm.addField<4>(pm.getFieldView<3>().value_or(0));
        decrypted_group.addField<8>(dpm);
      }

      for (auto const &rm : snapshot->getFieldView<9>())
      {
        DecryptedRequestingMember drm;
        if (auto userid = rm.getFieldView<1>(); userid.has_value())
          drm.addField<1>(userid.value());
        drm.addField<4>(rm.getFieldView<4>().value_or(0));
        decrypted_group.addField<9>(drm);
      }

      if (auto invitelinkpassword = snapshot->getFieldView<10>(); invitelinkpassword.has_value())
        decrypted_group.addField<10>(invitelinkpassword.value());

      if (auto description_blob = snapshot->getFieldView<11>(); description_blob.has_value())
        if (auto description = description_blob->getFieldView<4>(); description.has_value() && !description->empty())
          decrypted_group.addField<11>(std::string(description.value()));

      decrypted_group.addField<12>(snapshot->getFieldView<12>().value_or(false) ? 1 : 2); // EnabledState ENABLED : DISABLED

      for (auto const &bm : snapshot->getFieldView<13>())
      {
        DecryptedBannedMember dbm;
        if (auto userid = bm.getFieldView<1>(); userid.has_value())
          dbm.addField<1>(userid.value());
        dbm.addField<2>(bm.getFieldView<2>().value_or(0));
        decrypted_group.addField<13>(dbm);
      }

      if (snapshot->getFieldView<14>().value_or(false))
        decrypted_group.addField<14>(true);
    }

    // 'active' (self is a member) is set after the recipients are finalized
    if (!insertRow("groups",
                   {{"group_id", group_id},
                    {"recipient_id", rid},
                    {"title", title},
                    {"master_key", std::pair<unsigned char *, unsigned long long>(masterkey->first, masterkey->second)},
                    {"revision", revision},
                    {"decrypted_group", std::pair<unsigned char *, unsigned long long>(decrypted_group.data(), decrypted_group.size())},
                    {"active", 0}})) [[unlikely]]
    {
      Logger::error("Failed to insert group");
      return false;
    }

    // members are added later, after we know which ACI belongs to self
    state->groupmembers.emplace_back(group_id, std::move(members));
    return true;
  }

  auto distributionlistitem = recipient->getFieldView<4>();
  if (distributionlistitem.has_value())
  {
    // story distribution lists: stories are not in the backup, nothing to do
    return true;
  }

  auto self = recipient->getFieldView<5>();
  if (self.has_value())
  {
    if (d_selfid == -1) [[unlikely]]
    {
      Logger::error("Got 'Self'-recipient, but AccountData has not been processed");
      return false;
    }
    state->selfbackupid = backup_id.value();
    state->recipients[backup_id.value()] = d_selfid;
    if (auto avatarcolor = self->getFieldView<1>(); avatarcolor.has_value())
      d_database.exec("UPDATE recipient SET avatar_color = ? WHERE _id = ?", {avatarColorToString(avatarcolor), d_selfid});
    return true;
  }

  auto releasenotes = recipient->getFieldView<6>();
  if (releasenotes.has_value())
  {
    // the 'Signal' release notes channel
    std::any new_rid;
    if (!insertRow("recipient",
                   {{"type", 0},
                    {"profile_given_name", std::string("Signal")},
                    {"profile_joined_name", std::string("Signal")},
                    {"profile_sharing", 1}},
                   "_id", &new_rid)) [[unlikely]]
    {
      Logger::error("Failed to insert release notes recipient");
      return false;
    }
    state->recipients[backup_id.value()] = std::any_cast<long long int>(new_rid);
    return true;
  }

  auto calllink = recipient->getFieldView<7>();
  if (calllink.has_value())
  {
    // call links only have ad hoc calls (not supported), no messages
    return true;
  }

  Logger::warning("Unsupported recipient type. Skipping...");
  ++state->skippedrecipients;
  return true;
}

#endif
