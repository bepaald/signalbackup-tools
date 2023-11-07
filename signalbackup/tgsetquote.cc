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

bool SignalBackup::tgSetQuote(long long int quoted_message_id, long long int new_msg_id)
{
  SqliteDB::QueryResults quote_res;
  if (!d_database.exec("SELECT body, " + d_mms_recipient_id + ", " + d_mms_date_sent + " FROM " + d_mms_table + " "
                       "WHERE _id = ?", quoted_message_id, &quote_res) ||
      quote_res.rows() != 1)
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get quote data." << std::endl;
    return false;
  }

  long long int quote_id = quote_res.valueAsInt(0, d_mms_date_sent, -1);
  long long int quote_author = quote_res.valueAsInt(0, d_mms_recipient_id, -1);
  std::string quote_body = quote_res.valueAsString(0, "body");
  if (quote_id == -1 || quote_author == -1)
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get quote data." << std::endl;
    return false;
  }

  if (!d_database.exec("UPDATE " + d_mms_table + " SET "
                       "quote_id = ?, "
                       "quote_author = ?, "
                       "quote_body = ?, "
                       "quote_type = 0, "
                       "quote_missing = 0 "
                       "WHERE _id = ?",
                       {quote_id,
                        quote_author,
                        (quote_res.isNull(0, "body") ? std::any(nullptr) : std::any(quote_body)),
                        new_msg_id}))
  {
    std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to set quote data." << std::endl;
    return false;
  }

  // set quoted-attachment
  SqliteDB::QueryResults quote_att_res;
  if (d_database.exec("SELECT _id, mid, ct, pending_push, data_size, unique_id, voice_note, width, height, quote, data_hash "
                      "FROM part WHERE mid = ?", quoted_message_id, &quote_att_res) &&
      quote_att_res.rows() >= 1)
  {
    //quote_att_res.prettyPrint();

    // get unique id for new attachments
    long long int unique_id = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_date_sent + " FROM " + d_mms_table +
                                                                          " WHERE _id = ?", new_msg_id, -1);
    if (unique_id == -1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get unique_id for attachment in quote. Skipping." << std::endl;
      return false;
    }

    // get unique id for existing attachments...
    long long int quoted_unique_id = quote_att_res.valueAsInt(0, "unique_id", -1);
    if (unique_id == -1)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get unique_id for quoted attachment. Skipping." << std::endl;
      return false;
    }

    for (uint i = 0; i < quote_att_res.rows(); ++i)
    {
      // set sql data
      std::any retval;
      if (!insertRow("part",
                   {{"mid", new_msg_id},
                    {"ct", quote_att_res.value(i, "ct")},
                    {"pending_push", quote_att_res.value(i, "pending_push")},
                    {"data_size", quote_att_res.value(i, "data_size")},
                    {"unique_id", unique_id},
                    {"voice_note", quote_att_res.value(i, "voice_note")},
                    {"width", quote_att_res.value(i, "width")},
                    {"height", quote_att_res.value(i, "height")},
                    {"quote", 1},
                    {"data_hash", quote_att_res.value(i, "data_hash")}},
                     "_id", &retval))
      {
        std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting part-data" << std::endl;
        continue;
      }
      long long int new_part_id = std::any_cast<long long int>(retval);

      // add attachment
      if (!bepaald::contains(d_attachments, std::pair{quote_att_res.valueAsInt(i, "_id", -1), quoted_unique_id}))
      {
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to find original of quoted attachment. Skipping." << std::endl;
        d_database.exec("DELETE FROM part WHERE _id = ?", new_part_id);
        continue;
      }
      std::unique_ptr<AttachmentFrame> new_attachment_frame(new AttachmentFrame(*d_attachments.at({quote_att_res.valueAsInt(i, "_id", -1), quoted_unique_id}).get()));
      //std::cout << "Creating new attachment: " << quote_att_res.valueAsInt(i, "_id", -1) << ", "  << quoted_unique_id << " -> " << new_part_id << ", " << unique_id << std::endl;
      new_attachment_frame->setRowId(new_part_id);
      new_attachment_frame->setAttachmentId(unique_id);
      //new_attachment_frame->printInfo();
      d_attachments.emplace(std::make_pair(new_part_id, unique_id), new_attachment_frame.release());
    }
  }
  return true;
}
