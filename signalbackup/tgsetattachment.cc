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

#include "../rawfileattachmentreader/rawfileattachmentreader.h"
#include "../attachmentmetadata/attachmentmetadata.h"

bool SignalBackup::tgSetAttachment(SqliteDB::QueryResults const &message_data, std::string const &datapath,
                                   long long int r, long long int new_msg_id)
{
  std::string photo = message_data.valueAsString(r, "photo");
  std::string file = message_data.valueAsString(r, "file");
  std::string vcard = message_data.valueAsString(r, "contact_vcard");

  if (photo.empty() && file.empty() && vcard.empty())
    return true;

  bool attachmentadded = false;
  for (std::string const &a : {photo, file, vcard})
  {
    if (a.empty())
      continue;

    //std::cout << "Attachment: " << datapath << a << std::endl;
    std::string filename_for_db(std::filesystem::path(a).filename().string());
    //std::cout << filename_for_db << std::endl;

    AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(datapath + a);
    if (amd.filename.empty() || amd.filesize == 0)
    {
      Logger::warning("Failed to get attachment data. Skipping.");
      continue;
    }

    std::string ct = message_data.valueAsString(r, "mime_type");
    if (ct.empty())
      ct = amd.filetype;
    if (a == vcard)
      ct = "text/vcard";
    if (ct.empty())
    {
      Logger::warning("Attachment has no mime_type. Skipping.");
      continue;
    }

    long long int w = message_data.valueAsInt(r, "width", 0);
    long long int h = message_data.valueAsInt(r, "height", 0);

    long long int voice_note = 0;
    if (message_data.valueAsString(r, "media_type") == "voice_message")
      voice_note = 1;

    // get unqiue id / or -1
    long long int unique_id = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE _id = ?", new_msg_id, -1);
    if (d_database.tableContainsColumn(d_part_table, "unique_id") && unique_id == -1)
    {
      Logger::warning("Failed to get unique_id for attachment. Skipping.");
      continue;
    }

    //insert into part
    std::any retval;
    if (!insertRow(d_part_table,
                   {{d_part_mid, new_msg_id},
                    {d_part_ct, ct},
                    {d_part_pending, 0},
                    {"data_size", amd.filesize},
                    {(d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : ""), unique_id},
                    {(!filename_for_db.empty() ? "file_name" : ""), filename_for_db},
                    {"voice_note", voice_note},
                    {"width", amd.width == -1 ? w : amd.width},
                    {"height", amd.height == -1 ? h : amd.height},
                    {"quote", 0},
                    {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), amd.hash},
                    {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), amd.hash},
                    {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), amd.hash},
                    //{"upload_timestamp", results_attachment_data.value(0, "upload_timestamp")},      // will be 0 on sticker
                    //{"cdn_number", results_attachment_data.value(0, "cdn_number")}, // will be 0 on sticker, usually 0 or 2, but I dont know what it means
                    //{"file_name", results_attachment_data.value(0, "file_name")}},
                   },
                   "_id", &retval))
    {
      Logger::error("Inserting part-data");
      continue;
    }
    long long int new_part_id = std::any_cast<long long int>(retval);

    // now the actual attachment
    DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>
                            {"ROWID:uint64:" + bepaald::toString(new_part_id),
                             (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                              "ATTACHMENTID:uint64:" + bepaald::toString(unique_id) : ""),
                             "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
    {
      //new_attachment_frame->setLazyDataRAW(amd.filesize, datapath + a);
      new_attachment_frame->setReader(new RawFileAttachmentReader(datapath + a));
      d_attachments.emplace(std::make_pair(new_part_id, (d_database.tableContainsColumn(d_part_table, "unique_id") ? unique_id : -1)), new_attachment_frame.release());
    }
    else
    {
      Logger::error("Failed to create AttachmentFrame for data");
      Logger::error_indent("rowid       : ", new_part_id);
      Logger::error_indent("attachmentid: ", unique_id);
      Logger::error_indent("length      : ", amd.filesize);
      Logger::error_indent("path        : ", datapath, a);

      // try to remove the inserted part entry:
      d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
      continue;
    }

    //std::cout << "ADDED ATTACHMENT TO MESSAGE:" << std::endl;
    //d_database.prettyPrint("SELECT _id, body, " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE _id = ?", new_msg_id);

    attachmentadded = true;
  }

  return attachmentadded;
}
