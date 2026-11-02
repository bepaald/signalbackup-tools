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

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../messagerangeproto_typedef/messagerangeproto_typedef.h"
#include "../protobufparser/protobufparser.h"

// 16 raw bytes -> "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
std::string SignalBackup::bv2UuidFromBytes(std::pair<unsigned char *, uint64_t> const &bytes)
{
  if (!bytes.first || bytes.second != 16) [[unlikely]]
    return std::string();

  std::string hex = bepaald::bytesToHexString(bytes.first, bytes.second, true);
  return hex.substr(0, 8) + '-' + hex.substr(8, 4) + '-' + hex.substr(12, 4) + '-' + hex.substr(16, 4) + '-' + hex.substr(20);
}

/*
  The group id is derived from the group master key through zkgroup's
  GroupSecretParams::derive_from_master_key(), which uses Sho (ShoHmacSha256):

    sho = Sho::new("Signal_ZKGroup_20200424_GroupMasterKey_GroupSecretParams_DeriveFromMasterKey", masterkey);
    group_id = sho.squeeze(32);

  ShoHmacSha256:
    new(label)        : cv = 0^32; absorb_and_ratchet(label)
    absorb_and_ratchet: cv = HMAC(key = cv, data || 0x00)
    squeeze(n)        : output block i = HMAC(key = cv, BE64(i) || 0x01)
                        (then cv = HMAC(key = cv, BE64(n) || 0x02), not needed here)

  See https://github.com/signalapp/libsignal/blob/main/rust/poksho/src/shohmacsha256.rs
*/
std::string SignalBackup::bv2GroupIdFromMasterKey(std::pair<unsigned char *, uint64_t> const &masterkey)
{
  if (!masterkey.first || masterkey.second != 32) [[unlikely]]
    return std::string();

  auto hmac = [](unsigned char const *key, unsigned char const *data, size_t datasize, unsigned char *out) STATICLAMBDA
  {
    unsigned int outsize = SHA256_DIGEST_LENGTH;
    return HMAC(EVP_sha256(), key, SHA256_DIGEST_LENGTH, data, datasize, out, &outsize) != nullptr;
  };

  auto absorb_and_ratchet = [&hmac](unsigned char *cv, unsigned char const *data, size_t datasize)
  {
    std::unique_ptr<unsigned char []> input(new unsigned char[datasize + 1]);
    std::memcpy(input.get(), data, datasize);
    input[datasize] = 0x00;
    return hmac(cv, input.get(), datasize + 1, cv);
  };

  unsigned char cv[SHA256_DIGEST_LENGTH] = {0};
  std::string_view constexpr label("Signal_ZKGroup_20200424_GroupMasterKey_GroupSecretParams_DeriveFromMasterKey");
  if (!absorb_and_ratchet(cv, reinterpret_cast<unsigned char const *>(label.data()), label.size()) ||
      !absorb_and_ratchet(cv, masterkey.first, masterkey.second)) [[unlikely]]
    return std::string();

  // first (and only needed) output block: HMAC(cv, BE64(0) || 0x01)
  unsigned char squeezeinput[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0x01};
  unsigned char groupid[SHA256_DIGEST_LENGTH];
  if (!hmac(cv, squeezeinput, sizeof(squeezeinput), groupid)) [[unlikely]]
    return std::string();

  return "__signal_group__v2__!" + bepaald::bytesToHexString(groupid, SHA256_DIGEST_LENGTH, true);
}

long long int SignalBackup::bv2GetRecipientIdForAci(std::string const &aci, bool create) const
{
  if (aci.empty()) [[unlikely]]
    return -1;

  long long int rid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE aci = ?", aci, -1);
  if (rid != -1 || !create)
    return rid;

  // Android creates a recipient for any unknown service id it encounters
  std::any new_rid;
  if (!insertRow("recipient", {{"aci", aci}, {"type", 0}}, "_id", &new_rid)) [[unlikely]]
    return -1;
  return std::any_cast<long long int>(new_rid);
}

/*
  Backup:                                  Android (BodyRangeList, message_ranges/quote_mentions):
  message BodyRange {                      message BodyRange {
    enum Style {                             enum Style {
      NONE = 0;                                BOLD          = 0;
      BOLD = 1;                                ITALIC        = 1;
      ITALIC = 2;                              SPOILER       = 2;
      SPOILER = 3;                             STRIKETHROUGH = 3;
      STRIKETHROUGH = 4;                       MONOSPACE     = 4;
      MONOSPACE = 5;                         }
    }                                        int32 start  = 1;
    optional uint32 start = 1;               int32 length = 2;
    optional uint32 length = 2;              oneof associatedValue {
    oneof associatedValue {                    string mentionUuid = 3;
      bytes mentionAci = 3;                    Style  style       = 4;
      Style style = 4;                         ...
    }                                        }
  }                                        }

  Puts the encoded BodyRangeList in *out, returns false (and leaves *out empty) if there are no applicable ranges
*/
bool SignalBackup::bv2BodyRangesToMessageRanges(std::vector<BackupV2::BodyRange> const &ranges, bool includementions,
                                                std::pair<std::unique_ptr<unsigned char []>, size_t> *out) const
{
  BodyRanges brl;
  bool any = false;
  for (auto const &r : ranges)
  {
    auto mentionaci = r.getFieldView<3>();
    auto style = r.getFieldView<4>();
    if (mentionaci.has_value() && !includementions)
      continue;
    if (!mentionaci.has_value() && (!style.has_value() || style.value() <= 0 || style.value() > 5))
      continue;

    BodyRange br;
    br.addField<1>(static_cast<int32_t>(r.getFieldView<1>().value_or(0)));
    br.addField<2>(static_cast<int32_t>(r.getFieldView<2>().value_or(0)));
    if (mentionaci.has_value())
      br.addField<3>(bv2UuidFromBytes(mentionaci.value()));
    else
      br.addField<4>(style.value() - 1);
    brl.addField<1>(br);
    any = true;
  }

  if (!any)
  {
    out->first.reset();
    out->second = 0;
    return false;
  }

  out->first.reset(new unsigned char[brl.size()]);
  std::memcpy(out->first.get(), brl.data(), brl.size());
  out->second = brl.size();
  return true;
}

#endif
