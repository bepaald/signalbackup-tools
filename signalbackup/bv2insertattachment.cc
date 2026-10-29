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

#include <openssl/evp.h>

#include "../common_filesystem.h"
#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../protobufparser/protobufparser.h"
#include "../desktopattachmentreader/desktopattachmentreader.h"

/*
  message FilePointer {                           message LocatorInfo {
    optional string contentType = 4;                bytes key = 1;
    optional bytes incrementalMac = 5;              uint32 size = 3;  // plaintext size
    optional uint32 incrementalMacChunkSize = 6;    optional string transitCdnKey = 4;
    optional string fileName = 7;                   optional uint32 transitCdnNumber = 5;
    optional uint32 width = 8;                      optional uint64 transitTierUploadTimestamp = 6;
    optional uint32 height = 9;                     optional uint32 mediaTierCdnNumber = 7;
    optional string caption = 10;                   optional bytes localKey = 9;
    optional string blurHash = 11;                  oneof integrityCheck {
    LocatorInfo locatorInfo = 13;                     bytes plaintextHash = 10;
    ...                                               bytes encryptedDigest = 11;
  }                                                 }
                                                  }

  In a local backup, the attachment data is in [backupdir]/files/XX/[mediaName], where
  mediaName = hex(sha256(plaintextHash || localKey)) and XX are the first two characters
  of the mediaName. The file is encrypted the standard way for Signal attachments
  (iv + aes-256-cbc(padded data) + hmac-sha256, localKey = aeskey + mackey), the same as
  Signal Desktop's encrypted attachments, so the DesktopAttachmentReader can read it.

  Returns the new attachment._id, or -1 on error.
*/
long long int SignalBackup::bv2InsertAttachment(BackupV2::FilePointer const &fp, long long int mid, int display_order, int flag,
                                                bool quote, std::string const &contenttype_override, BackupV2ImportState *state)
{
  auto locatorinfo = fp.getFieldView<13>();

  uint64_t size = 0;
  std::string remote_key;
  std::string plaintexthash_b64;
  std::string datapath;
  std::string localkey_b64;
  if (locatorinfo.has_value())
  {
    size = locatorinfo->getFieldView<3>().value_or(0);
    if (auto key = locatorinfo->getFieldView<1>(); key.has_value())
      remote_key = Base64::bytesToBase64String(key.value());

    auto localkey = locatorinfo->getFieldView<9>();
    auto plaintexthash = locatorinfo->getFieldView<10>();
    if (plaintexthash.has_value())
      plaintexthash_b64 = Base64::bytesToBase64String(plaintexthash.value());

    if (localkey.has_value() && plaintexthash.has_value())
    {
      std::unique_ptr<unsigned char []> namedata(new unsigned char[plaintexthash->second + localkey->second]);
      std::memcpy(namedata.get(), plaintexthash->first, plaintexthash->second);
      std::memcpy(namedata.get() + plaintexthash->second, localkey->first, localkey->second);
      unsigned char hash[EVP_MAX_MD_SIZE];
      unsigned int hashsize = 0;
      if (EVP_Digest(namedata.get(), plaintexthash->second + localkey->second, hash, &hashsize, EVP_sha256(), nullptr) == 1) [[likely]]
      {
        std::string medianame = bepaald::bytesToHexString(hash, hashsize, true);
        datapath = state->filesdir + "/" + medianame.substr(0, 2) + "/" + medianame;
        localkey_b64 = Base64::bytesToBase64String(localkey.value());
      }
    }
  }

  bool havedata = !datapath.empty() && size > 0 && bepaald::isRegularFile(datapath);

  std::string contenttype = contenttype_override;
  if (contenttype.empty())
    contenttype = fp.getFieldView<4>().value_or("application/octet-stream");

  std::any new_aid;
  if (!insertRow("attachment",
                 {{"message_id", mid},
                  {"content_type", contenttype},
                  {"remote_key", remote_key.empty() ? std::any(nullptr) : remote_key},
                  {"remote_location", locatorinfo.has_value() && locatorinfo->getFieldView<4>().has_value() ?
                                      std::any(std::string(locatorinfo->getFieldView<4>().value())) : std::any(nullptr)},
                  {"cdn_number", locatorinfo.has_value() ? static_cast<long long int>(locatorinfo->getFieldView<5>().value_or(0)) : 0},
                  {"upload_timestamp", locatorinfo.has_value() ? static_cast<long long int>(locatorinfo->getFieldView<6>().value_or(0)) : 0},
                  {"transfer_state", havedata ? 0 : 2}, // DONE : PENDING
                  {"data_size", static_cast<long long int>(size)},
                  {"file_name", fp.getFieldView<7>().has_value() ? std::any(std::string(fp.getFieldView<7>().value())) : std::any(nullptr)},
                  {"voice_note", flag == 1 ? 1 : 0},
                  {"borderless", flag == 2 ? 1 : 0},
                  {"video_gif", flag == 3 ? 1 : 0},
                  {"quote", quote ? 1 : 0},
                  {"quote_target_content_type", quote && fp.getFieldView<4>().has_value() ?
                                                std::any(std::string(fp.getFieldView<4>().value())) : std::any(nullptr)},
                  {"width", static_cast<long long int>(fp.getFieldView<8>().value_or(0))},
                  {"height", static_cast<long long int>(fp.getFieldView<9>().value_or(0))},
                  {"caption", fp.getFieldView<10>().has_value() ? std::any(std::string(fp.getFieldView<10>().value())) : std::any(nullptr)},
                  {"blur_hash", fp.getFieldView<11>().has_value() ? std::any(std::string(fp.getFieldView<11>().value())) : std::any(nullptr)},
                  {"display_order", display_order},
                  {"data_hash_start", plaintexthash_b64.empty() ? std::any(nullptr) : plaintexthash_b64},
                  {"data_hash_end", plaintexthash_b64.empty() ? std::any(nullptr) : plaintexthash_b64}},
                 "_id", &new_aid)) [[unlikely]]
  {
    Logger::error("Failed to insert attachment");
    return -1;
  }
  long long int aid = std::any_cast<long long int>(new_aid);

  ++state->attachments;
  if (!havedata)
  {
    if (size > 0)
      ++state->missingattachments;
    return aid;
  }

  DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
  if (!setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(aid),
                                                                           "LENGTH:uint32:" + bepaald::toString(size)})) [[unlikely]]
  {
    Logger::error("Failed to create AttachmentFrame for data");
    return aid;
  }
  new_attachment_frame->setReader(new DesktopAttachmentReader(2, datapath, localkey_b64, size));
  d_attachments.emplace(std::make_pair(aid, -1), new_attachment_frame.release());

  return aid;
}

#endif
