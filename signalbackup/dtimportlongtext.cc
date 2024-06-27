/*
  Copyright (C) 2024  Selwin van Dijk

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

#include <openssl/sha.h>

void SignalBackup::dtImportLongText(std::string const &msgbody_full, long long int new_mms_id, long long int uniqueid)
{
  if (d_verbose) [[unlikely]] Logger::message_start("Insert LONG_TEXT attachment...");
  // gethash
  std::string hash;
  unsigned char rawhash[SHA256_DIGEST_LENGTH];
  bool fail = true;
  std::unique_ptr<EVP_MD_CTX, decltype(&::EVP_MD_CTX_free)> sha256(EVP_MD_CTX_new(), &::EVP_MD_CTX_free);
  if (sha256.get() && EVP_DigestInit_ex(sha256.get(), EVP_sha256(), nullptr) == 1)
  {
    fail = false;
    if (EVP_DigestUpdate(sha256.get(), msgbody_full.c_str(), msgbody_full.length()) != 1)
      fail = true;
    fail |= (EVP_DigestFinal_ex(sha256.get(), rawhash, nullptr) != 1);
  }
  hash = fail ? std::string() : Base64::bytesToBase64String(rawhash, SHA256_DIGEST_LENGTH);
  std::string filename = bepaald::toDateString(uniqueid / 1000, "signal-%Y-%m-%d-%H%M%S.txt");

  //std::cout << "SIZE: " << msgbody_full.length() << std::endl;
  //std::cout << "HASH: " << hash << std::endl;
  //std::cout << "FILENAME: " << filename << std::endl;

  std::any new_part_id_ret;
  if (!insertRow(d_part_table,
                 {{d_part_mid, new_mms_id},
                  {d_part_ct, "text/x-signal-plain"},
                  {d_part_pending, 0},
                  {"data_size", msgbody_full.length()},
                  {(d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : ""), uniqueid},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), hash},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), hash},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), hash},
                  {"upload_timestamp", uniqueid},
                  //{"cdn_number", results_attachment_data.value(0, "cdn_number")}, // will be 0 on sticker, usually 0 or 2, but I dont know what it means
                  {"file_name", filename}},
                 "_id", &new_part_id_ret))
    Logger::error("Failed to set LONG_TEXT attachment");
  else // set actual attachment data
  {
    long long int new_part_id = std::any_cast<long long int>(new_part_id_ret);
    DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame,
                            std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                     (d_database.tableContainsColumn(d_part_table, "unique_id") ? "ATTACHMENTID:uint64:" + bepaald::toString(uniqueid) : ""),
                                                     "LENGTH:uint32:" + bepaald::toString(msgbody_full.length())}))
    {
      new_attachment_frame->setAttachmentData(reinterpret_cast<unsigned char const *>(msgbody_full.c_str()), msgbody_full.length());
      d_attachments.emplace(std::make_pair(new_part_id, d_database.tableContainsColumn(d_part_table, "unique_id") ? uniqueid : -1), new_attachment_frame.release());
    }
    else
    {
      Logger::error("Failed to create AttachmentFrame for data");
      // try to remove the inserted part entry:
      d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
    }
  }
  if (d_verbose) [[unlikely]] Logger::message_end("done");
}
