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

bool SignalBackup::tgSetAttachment(SqliteDB::QueryResults const &message_data, std::string const &datapath,
                                   long long int r, long long int new_msg_id)
{
  std::string photo = message_data.valueAsString(r, "photo");
  std::string file = message_data.valueAsString(r, "file");

  if (photo.empty() && file.empty())
    return true;

  bool attachmentadded = false;
  for (std::string const &a : {photo, file})
  {
    if (a.empty())
      continue;

    //std::cout << "Attachment: " << datapath << a << std::endl;

    AttachmentMetadata amd = getAttachmentMetaData(datapath + a);
    if (amd.filename.empty() || amd.filesize == 0)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get attachment data. Skipping." << std::endl;
      continue;
    }

    std::string ct = message_data.valueAsString(r, "mime_type");
    if (ct.empty())
      ct = amd.filetype;
    if (ct.empty())
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Attachment has no mime_type. Skipping." << std::endl;
      continue;
    }

    long long int w = message_data.valueAsInt(r, "width", 0);
    long long int h = message_data.valueAsInt(r, "height", 0);

    long long int voice_note = 0;
    if (message_data.valueAsString(r, "media_type") == "voice_message")
      voice_note = 1;

    // get unqiue id
    long long int unique_id = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE _id = ?", new_msg_id, -1);
    if (unique_id == -1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get unique_id for attachment. Skipping." << std::endl;
      continue;
    }

    //insert into part
    std::any retval;
    if (!insertRow("part",
                   {{"mid", new_msg_id},
                    {"ct", ct},
                    {"pending_push", 0},
                    {"data_size", amd.filesize},
                    {"unique_id", unique_id},
                    {"voice_note", voice_note},
                    {"width", amd.width == -1 ? w : amd.width},
                    {"height", amd.height == -1 ? h : amd.height},
                    {"quote", 0},
                    {"data_hash", amd.hash}
                    //{"upload_timestamp", results_attachment_data.value(0, "upload_timestamp")},      // will be 0 on sticker
                    //{"cdn_number", results_attachment_data.value(0, "cdn_number")}, // will be 0 on sticker, usually 0 or 2, but I dont know what it means
                    //{"file_name", results_attachment_data.value(0, "file_name")}},
                   },
                   "_id", &retval))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting part-data" << std::endl;
      continue;
    }
    long long int new_part_id = std::any_cast<long long int>(retval);

    // now the actual attachment
    std::unique_ptr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                                            "ATTACHMENTID:uint64:" + bepaald::toString(unique_id),
                                                                            "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
    {
      new_attachment_frame->setLazyDataRAW(amd.filesize, datapath + a);
      d_attachments.emplace(std::make_pair(new_part_id, unique_id), new_attachment_frame.release());
    }
    else
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create AttachmentFrame for data" << std::endl;
      std::cout << "       rowid       : " << new_part_id << std::endl;
      std::cout << "       attachmentid: " << unique_id << std::endl;
      std::cout << "       length      : " << amd.filesize << std::endl;
      std::cout << "       path        : " << datapath << a << std::endl;

      // try to remove the inserted part entry:
      d_database.exec("DELETE FROM part WHERE _id = ?", new_part_id);
      continue;
    }

    //std::cout << "ADDED ATTACHMENT TO MESSAGE:" << std::endl;
    //d_database.prettyPrint("SELECT _id, body, " + d_mms_date_sent + " FROM " + d_mms_table + " WHERE _id = ?", new_msg_id);

    attachmentadded = true;
  }

  return attachmentadded;
}
