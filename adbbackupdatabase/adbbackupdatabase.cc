/*
  Copyright (C) 2025  Selwin van Dijk

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

#include "adbbackupdatabase.ih"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <cstring>

#include "../common_bytes.h"
#include "../logger/logger.h"
#include "../xmldocument/xmldocument.h"

#define KEY_MATERIAL_ID 1
#define IV_ID 2
#define MAC_ID 3

AdbBackupDatabase::AdbBackupDatabase(std::string const &backupdir, std::string const &passphrase_given, bool verbose)
  :
  d_db(backupdir + "/db/messages.db"),
  d_backuproot(backupdir),
  d_encryption_secret_length(0),
  d_encryption_secret(nullptr),
  d_mac_secret_length(0),
  d_mac_secret(nullptr),
  d_ok(false),
  d_verbose(verbose)
{
  if (!d_db.ok()) [[unlikely]]
  {
    Logger::error("Failed to open Sqlite database");
    return;
  }

  d_db.prettyPrint(false, "SELECT _id AS thread_id, message_count AS 'N messages' FROM thread");
  d_db.transactionState();

  //  d_db.prettyPrint(false, "SELECT name FROM sqlite_master WHERE type = 'table'");

  // attach the 'canonical_address.db' database
  if (!d_db.exec("ATTACH DATABASE ? AS ca", "file:" + d_backuproot + "/db/canonical_address.db?mode=ro"))
  {
    Logger::error("Failed to attach 'canonical_address.db'");
    return;
  }

  //  d_db.prettyPrint(false, "SELECT name FROM sqlite_master WHERE type = 'table' UNION ALL SELECT name FROM ca.sqlite_master WHERE type = 'table'");

  /*
    SELECT DISTINCT thread_id,mms.address AS msg_address,canonical_addresses.address AS can_address,recipient_ids FROM mms LEFT JOIN thread ON thread_id = thread._id LEFT JOIN ca.canonical_addresses ON ca.canonical_addresses._id = thread.recipient_ids UNION SELECT DISTINCT thread_id,sms.address AS msg_address,canonical_addresses.address AS can_address,recipient_ids FROM sms LEFT JOIN thread ON thread._id = sms.thread_id LEFT JOIN ca.canonical_addresses ON ca.canonical_addresses._id = thread.recipient_ids
  */

  // get self phone
  XmlDocument securesms_preferences(d_backuproot + "/sp/org.thoughtcrime.securesms_preferences.xml");
  if (!securesms_preferences.ok()) [[unlikely]]
    Logger::warning("Failed to open/parse preference file: '", d_backuproot, "/sp/org.thoughtcrime.securesms_preferences.xml");
  else
    d_selfphone = securesms_preferences.getTextContentsFromNode(0, "string", {{"name", "pref_local_number"}}).value_or(std::string());




  // set passphrase
  std::string passphrase(passphrase_given);
  if (passphrase.empty())
  {
    if (!securesms_preferences.ok()) [[unlikely]]
      Logger::warning("Failed to open/parse preference file: '", d_backuproot, "/sp/org.thoughtcrime.securesms_preferences.xml");
    else
    {
      std::string passphrase_disabled = securesms_preferences.getAttributeValueFromNode("value", "boolean", {{"name", "pref_disable_passphrase"}}).value_or(std::string());
      if (passphrase_disabled != "true")
        Logger::warning("No passphrase given (using default), but passphrase does not appear disabled in preferences");
    }
    passphrase = "unencrypted";
  }
  if (d_verbose) [[unlikely]]
    Logger::message("ADB backup: passphrase=", passphrase);





  // this pref file contains the needed data
  XmlDocument securesms_prefs(d_backuproot + "/sp/SecureSMS-Preferences.xml");
  if (!securesms_prefs.ok())
  {
    Logger::error("ADB backup: Filed to open/parse XML preferences '", d_backuproot, "/sp/SecureSMS-Preferences.xml");
    return;
  }





  // get iterations
  long long int iterations = bepaald::toNumber<long long int>(securesms_prefs.getAttributeValueFromNode("value", "int", {std::make_pair("name", "passphrase_iterations")}).value_or("100"));
  if (d_verbose) [[unlikely]]
    Logger::message("ADB backup: iterations=", iterations);




  // get kek-mac salt
  std::string mac_salt_b64 = securesms_prefs.getTextContentsFromNode(0, "string", {{"name", "mac_salt"}}).value_or(std::string());
  if (mac_salt_b64.empty())
  {
    Logger::error("ADB backup: Failed to get mac_salt from preferences");
    return;
  }
  auto mac_salt = Base64::base64StringToBytes(mac_salt_b64);
  ScopeGuard mac_salt_guard([&](){ if (mac_salt.first) delete[] mac_salt.first; });
  if (verbose) [[unlikely]]
    Logger::message("ADB backup: mac_salt: ", bepaald::bytesToHexString(mac_salt));




  // create the kek-mac key from the salt, iterations and passphrase
  int kek_mackey_length = 16;
  std::unique_ptr<unsigned char []> kek_mackey(new unsigned char[kek_mackey_length]);
  if (!PKCS12_key_gen_utf8(passphrase.data(), passphrase.length(), mac_salt.first, mac_salt.second,
                           KEY_MATERIAL_ID, iterations, kek_mackey_length, kek_mackey.get(), EVP_sha1())) [[unlikely]]
  {
    Logger::error("Failed to generate mac-key with pkcs12");
    return;
  }
  if (d_verbose) [[unlikely]]
    Logger::message("ADB backup: MAC-key (KEK): ", bepaald::bytesToHexString(kek_mackey, kek_mackey_length));




  // get encrypted master-secret
  std::string master_secret_b64 = securesms_prefs.getTextContentsFromNode(0, "string", {{"name", "master_secret"}}).value_or(std::string());
  if (master_secret_b64.empty())
  {
    Logger::error("ADB backup: Failed to get master_secret from preferences");
    return;
  }
  auto master_secret = Base64::base64StringToBytes(master_secret_b64);
  ScopeGuard master_secret_guard([&](){ if (master_secret.first) delete[] master_secret.first; });
  if (verbose) [[unlikely]]
    Logger::message("ADB backup: master_secret: ", bepaald::bytesToHexString(master_secret));




  // check the master secret HMAC
  unsigned int digest_size = SHA_DIGEST_LENGTH;
  std::unique_ptr<unsigned char []> hash(new unsigned char[digest_size]);
  if (HMAC(EVP_sha1(), kek_mackey.get(), kek_mackey_length, master_secret.first,
           master_secret.second - digest_size, hash.get(), &digest_size) == nullptr) [[unlikely]]
  {
    Logger::error("Failed to calculate HMAC for master_secret");
    return;
  }
  if (std::memcmp(master_secret.first + (master_secret.second - digest_size), hash.get(), digest_size) != 0) [[unlikely]]
  {
    Logger::error("HMAC check failed. Mac read: ", bepaald::bytesToHexString(master_secret.first + (master_secret.second - SHA_DIGEST_LENGTH), SHA_DIGEST_LENGTH));
    Logger::error_indent("             Mac calculated: ", bepaald::bytesToHexString(hash, digest_size));
    return;
  }



  // get the encryption salt
  std::string encryption_salt_b64 = securesms_prefs.getTextContentsFromNode(0, "string", {{"name", "encryption_salt"}}).value_or(std::string());
  if (encryption_salt_b64.empty())
  {
    Logger::error("ADB backup: Failed to get encryption_salt from preferences");
    return;
  }
  auto encryption_salt = Base64::base64StringToBytes(encryption_salt_b64);
  ScopeGuard encryption_salt_guard([&](){ if (encryption_salt.first) delete[] encryption_salt.first; });
  if (verbose) [[unlikely]]
    Logger::message("ADB backup: encryption_salt: ", bepaald::bytesToHexString(encryption_salt));



  // generate cipherkey and iv from passphrase, iterations and encryption_salt
  // cipherkey:
  int cipherkey_length = 16;
  std::unique_ptr<unsigned char []> cipherkey(new unsigned char[cipherkey_length]);
  if (!PKCS12_key_gen_utf8(passphrase.data(), passphrase.length(), encryption_salt.first, encryption_salt.second,
                           KEY_MATERIAL_ID, iterations, cipherkey_length, cipherkey.get(), EVP_sha1())) [[unlikely]]
  {
    Logger::error("ADB backup: Failed to generate cipherkey with pkcs12");
    return;
  }
  if (d_verbose) [[unlikely]]
    Logger::message("ADB backup: cipherkey: ", bepaald::bytesToHexString(cipherkey, cipherkey_length));

  // iv:
  int iv_length = 16;
  std::unique_ptr<unsigned char []> iv(new unsigned char[iv_length]);
  if (!PKCS12_key_gen_utf8(passphrase.data(), passphrase.length(), encryption_salt.first, encryption_salt.second,
                           IV_ID, iterations, iv_length, iv.get(), EVP_sha1()))
  {
    Logger::error("ADB backup: Failed to generate IV with pkcs12");
    return;
  }
  if (d_verbose) [[unlikely]]
    Logger::message("ADB backup: IV: ", bepaald::bytesToHexString(iv, iv_length));



  // decrypt master_secret into the (combined) mac_secret and encryption_secret
  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), nullptr, cipherkey.get(), iv.get()) != 1) [[unlikely]]
  {
    Logger::error("CTX INIT FAILED");
    return;
  }

  // decrypt master_secret (without MAC)
  int combined_secret_length = master_secret.second - SHA_DIGEST_LENGTH;
  d_combined_secret.reset(new unsigned char[combined_secret_length]);
  if (EVP_DecryptUpdate(ctx.get(), d_combined_secret.get(), &combined_secret_length,
                        master_secret.first, master_secret.second - SHA_DIGEST_LENGTH) != 1) [[unlikely]]
  {
    Logger::error("Failed to decrypt master_secret");
    return;
  }

  // finalize (check and discard the padding)
  int lastbit = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), d_combined_secret.get() + combined_secret_length, &lastbit) != 1) [[unlikely]]
  {
    Logger::error("Failed to finalize decryption of master_secret");
    return;
  }
  combined_secret_length += lastbit;

  d_mac_secret_length = SHA_DIGEST_LENGTH; // (= 20)
  d_encryption_secret_length = 16;

  d_encryption_secret = d_combined_secret.get();
  d_mac_secret = d_combined_secret.get() + d_encryption_secret_length;

  if (d_verbose) [[unlikely]]
  {
    Logger::message("ADB backup: Encryption secret: ", bepaald::bytesToHexString(d_encryption_secret, d_encryption_secret_length));
    Logger::message("                   Mac secret: ", bepaald::bytesToHexString(d_mac_secret, d_mac_secret_length));
  }

  // SqliteDB::QueryResults res;
  // d_db.exec("SELECT body FROM sms WHERE body IS NOT NULL", &res);
  // for (unsigned int i = 0; i < res.rows(); ++i)
  // {
  //   auto body = decryptMessageBody(res(i, "body"));
  //   if (body.has_value())
  //     Logger::message("Body: \"", body.value(), "\"");
  //   else
  //     Logger::message("(nobody)");
  // }
  // d_db.exec("SELECT body FROM mms WHERE body IS NOT NULL", &res);
  // for (unsigned int i = 0; i < res.rows(); ++i)
  // {
  //   auto body = decryptMessageBody(res(i, "body"));
  //   if (body.has_value())
  //     Logger::message("Body: \"", body.value(), "\"");
  //   else
  //     Logger::message("(nobody)");
  // }

  d_ok = true;
}

#undef KEY_MATERIAL_ID
#undef IV_ID
#undef MAC_ID
