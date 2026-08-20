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

#include "backupv2reader.h"

#include "../common_be.h"
#include "../common_crypto.h"
#include "../common_filesystem.h"
#include "../backupv2proto_typedef/backupv2proto_typedef.h"
#include "../protobufparser/protobufparser.h"

#define BACKUP_KEY_INFO "20240801_SIGNAL_BACKUP_KEY"
#define LOCAL_BACKUP_METADATA_KEY_INFO "20241011_SIGNAL_LOCAL_BACKUP_METADATA_KEY"

bool BackupV2Reader::getKeys()
{
  // derive backupkey from passphrase:
  auto [backupkey, backupkey_size] = bepaald::hkdf_sha256(d_passphrase.data(), d_passphrase.size(),
                                                          BACKUP_KEY_INFO, STRLEN(BACKUP_KEY_INFO),
                                                          32);
  if (!backupkey || backupkey_size == 0) [[unlikely]]
    return false;

  if (d_verbose) [[unlikely]]
    Logger::message("backupkey: ", bepaald::bytesToHexString(backupkey, backupkey_size));



  // derive metadatakey from backupkey
  auto [metadatakey, metadatakey_size] = bepaald::hkdf_sha256(backupkey.get(), backupkey_size,
                                                              LOCAL_BACKUP_METADATA_KEY_INFO, STRLEN(LOCAL_BACKUP_METADATA_KEY_INFO),
                                                              32);
  if (!metadatakey || metadatakey_size == 0) [[unlikely]]
    return false;

  if (d_verbose) [[unlikely]]
    Logger::message("  metadatakey: ", bepaald::bytesToHexString(metadatakey, metadatakey_size));



  // read metadata-file, get IV and pad it to 16 bytes, get encrypted backupkey
  auto [metadata_data, metadata_size] = bepaald::readFileFully(d_snapshotdir + "/metadata");
  if (!metadata_data || metadata_size == 0) [[unlikely]]
    return false;

  BackupV2::Metadata metadata(metadata_data.get(), metadata_size);
  //metadata.print();

  auto encryptedbackupid = metadata.getFieldView<2>();
  if (!encryptedbackupid.has_value()) [[unlikely]]
  {
    Logger::error("Failed to get encrypted backup_id from metadata");
    return false;
  }
  auto iv_field = encryptedbackupid.value().getFieldView<1>();
  auto encryptedbackupid_field = encryptedbackupid.value().getFieldView<2>();

  if (!iv_field.has_value() ||
      !encryptedbackupid_field.has_value()) [[unlikely]]
  {
    Logger::error("Failed to get required fields from metadata");
    return false;
  }

  auto [metadata_encryptedid, metadata_encryptedid_size] = encryptedbackupid_field.value();
  auto [metadata_iv, metadata_iv_size] = iv_field.value();
  if (metadata_iv_size != 12) [[unlikely]]
  {
    Logger::error("Unexpected IV size in metadata (", metadata_iv_size, ")");
    return false;
  }
  // iv needs to be 16 bytes for CTR mode, but only 12 bytes is in metadata (Signal adds 4 bytes for the counter themselves)
  int constexpr metadata_iv_padded_size = 16;
  std::unique_ptr<unsigned char []> metadata_iv_padded = std::make_unique_for_overwrite<unsigned char []>(metadata_iv_padded_size);
  std::memcpy(metadata_iv_padded.get(), metadata_iv, metadata_iv_size);
  std::memset(metadata_iv_padded.get() + metadata_iv_size, 0, 4);

  if (d_verbose) [[unlikely]]
  {
    Logger::message("  Metadata IV (padded): ", bepaald::bytesToHexString(metadata_iv_padded, metadata_iv_padded_size));
    Logger::message("  Metadata encryptedid: ", bepaald::bytesToHexString(metadata_encryptedid, metadata_encryptedid_size));
  }

  // now decrypt the backup_id with the metadatakey and IV:
  auto [backupid, backupid_size] = bepaald::decrypt_aes_256_ctr(metadatakey.get(), metadata_iv_padded.get(),
                                                                metadata_encryptedid, metadata_encryptedid_size);
  if (!backupid || backupid_size == 0) [[unlikely]]
  {
    Logger::error("Failed to decrypt backupid");
    return false;
  }

  if (d_verbose) [[unlikely]]
    Logger::message("backupId: ", bepaald::bytesToHexString(backupid, backupid_size));



  // now from the backupid, and the backupkey, we derive the aeskey and mackey:
  size_t constexpr hmackey_size = 32;
  size_t constexpr aeskey_size = 32;
  // this infoblock is always appended with 16 byte backupId, so we reserve 16 bytes at the end
  size_t constexpr ENCRYPT_MESSAGE_INFO_SIZE(STRLEN("20241007_SIGNAL_BACKUP_ENCRYPT_MESSAGE_BACKUP:") + 16);
  unsigned char ENCRYPT_MESSAGE_INFO[ENCRYPT_MESSAGE_INFO_SIZE] = {'2', '0', '2', '4', '1', '0', '0', '7', '_', 'S', 'I', 'G', 'N', 'A', 'L', '_',
                                                                   'B', 'A', 'C', 'K', 'U', 'P', '_', 'E', 'N', 'C', 'R', 'Y', 'P', 'T', '_', 'M',
                                                                   'E', 'S', 'S', 'A', 'G', 'E', '_', 'B', 'A', 'C', 'K', 'U', 'P', ':',
                                                                   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
                                                                   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
  std::memcpy(ENCRYPT_MESSAGE_INFO + (ENCRYPT_MESSAGE_INFO_SIZE - 16), backupid.get(), backupid_size);
  d_hmac_aes_keys = bepaald::hkdf_sha256(backupkey.get(), backupkey_size,
                                         ENCRYPT_MESSAGE_INFO, ENCRYPT_MESSAGE_INFO_SIZE,
                                         hmackey_size + aeskey_size);
  if (!d_hmac_aes_keys.first || d_hmac_aes_keys.second != d_hmackey_size + d_aeskey_size) [[unlikely]]
  {
    Logger::error("Failed to derive aes and mac keys from backupid");
    return false;
  }
  d_hmackey = d_hmac_aes_keys.first.get();
  d_aeskey = d_hmac_aes_keys.first.get() + d_hmackey_size;
  if (d_verbose) [[unlikely]]
  {
    Logger::message("hmacKey: ", bepaald::bytesToHexString(d_hmackey, d_hmackey_size));
    Logger::message("aesKey: ", bepaald::bytesToHexString(d_aeskey, d_aeskey_size));
  }

  return true;
}

#undef BACKUP_KEY_INFO
#undef LOCAL_BACKUP_METADATA_KEY_INFO

#endif
