/*
  Copyright (C) 2022  Selwin van Dijk

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

bool SignalBackup::insertAttachments(long long int mms_id, long long int unique_id, int numattachments,
                                     SqliteDB const &ddb, std::string const &where, std::string const &databasedir,
                                     bool isquote)
{
  if (numattachments == -1) // quote attachments, number not known yet
  {
    SqliteDB::QueryResults res;
    if (!ddb.exec("SELECT IFNULL(json_array_length(json, '$.attachments'), 0) AS numattachments FROM messages " + where, &res) ||
        (res.rows() != 1 && res.columns() != 1))
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get number of attachment in quoted message. Skipping" << std::endl;
      return false;
    }
    numattachments = res.getValueAs<long long int>(0, "numattachments");
  }

  //if (numattachments)
  //  std::cout << "  " << numattachments << " attachments" << (isquote ? " (in quote)" : "") << std::endl;

  SqliteDB::QueryResults results_attachment_data;
  for (int k = 0; k < numattachments; ++k)
  {
    //std::cout << "  Attachment " << k + 1 << "/" << numattachments << ": " << std::flush;

    if (!ddb.exec("SELECT "
                  "json_extract(json, '$.attachments[" + bepaald::toString(k) + "].contentType') AS content_type,"
                  "json_extract(json, '$.attachments[" + bepaald::toString(k) + "].fileName') AS file_name,"
                  "json_extract(json, '$.attachments[" + bepaald::toString(k) + "].size') AS size,"
                  "IFNULL(json_extract(json, '$.attachments[" + bepaald::toString(k) + "].cdnNumber'), 0) AS cdn_number,"
                  "json_extract(json, '$.attachments[" + bepaald::toString(k) + "].flags') AS flags," // currently, the only flag implemented in Signal is:  VOICE_NOTE = 1
                  //"json_extract(json, '$.attachments[" + bepaald::toString(k) + "].cdnKey') AS cdn_key,"
                  "IFNULL(json_extract(json, '$.attachments[" + bepaald::toString(k) + "].uploadTimestamp'), 0) AS upload_timestamp,"
                  "json_extract(json, '$.attachments[" + bepaald::toString(k) + "].path') AS path"
                  " FROM messages " + where, &results_attachment_data))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to get attachment data from desktop database" << std::endl;
      continue;
    }

    AttachmentMetadata amd = getAttachmentMetaData(databasedir + "/attachments.noindex/" + results_attachment_data.valueAsString(0, "path"));
    // PROBABLY JUST NOT AN IMAGE, WE STILL WANT THE HASH
    // if (!amd)
    // {
    //   std::cout << "Failed to get metadata on new attachment: "
    //             << databasedir << "/attachments.noindex/" << results_attachment_data.valueAsString(0, "path") << std::endl;
    // }

    // attachmentdata.emplace_back(getAttachmentMetaData(configdir + "/attachments.noindex/" + results_attachment_data.valueAsString(0, "path")));
    // if (!results_attachment_data.isNull(0, "file_name"))
    //   attachmentdata.back().filename = results_attachment_data.valueAsString(0, "file_name");

    //insert into part
    std::any retval;
    if (!insertRow("part",
                   {{"mid", mms_id},
                    {"ct", results_attachment_data.value(0, "content_type")},
                    {"pending_push", 0},
                    {"data_size", results_attachment_data.value(0, "size")},
                    {"file_name", results_attachment_data.value(0, "file_name")},
                    {"unique_id", unique_id},
                    {"voice_note", results_attachment_data.isNull(0, "flags") ? 0 : (results_attachment_data.getValueAs<long long int>(0, "flags") == 1 ? 1 : 0)},
                    {"width", amd.width == -1 ? 0 : amd.width},
                    {"height", amd.height == -1 ? 0 : amd.height},
                    {"quote", isquote ? 1 : 0},
                    {"data_hash", amd.hash},
                    {"upload_timestamp", results_attachment_data.value(0, "upload_timestamp")},
                    {"cdn_number", results_attachment_data.value(0, "cdn_number")}},
                   "_id", &retval))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting part-data" << std::endl;
      continue;
    }
    long long int new_part_id = std::any_cast<long long int>(retval);
    //std::cout << "Inserted part, new id: " << new_part_id << std::endl;

    std::unique_ptr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                                            "ATTACHMENTID:uint64:" + bepaald::toString(unique_id),
                                                                            "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))
    {
      new_attachment_frame->setAttachmentData(databasedir + "/attachments.noindex/" + results_attachment_data.valueAsString(0, "path"));
      d_attachments.emplace(std::make_pair(new_part_id, unique_id), new_attachment_frame.release());


      //std::cout << "APPENDED ATTACHMENT FRAME[" << new_part_id << "," << unique_id <<  "]. FILE NAME: '" << d_attachments[{new_part_id, unique_id}]->filename() << "'" << std::endl;

    }
    else
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create AttachmentFrame for data" << std::endl;
      return false; // we did already insert into part... cant just continue probably
    }
  }
  return true;
}
