/*
  Copyright (C) 2023-2024  Selwin van Dijk

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
    Logger::warning("Failed to get quote data.");
    return false;
  }

  long long int quote_id = quote_res.valueAsInt(0, d_mms_date_sent, -1);
  long long int quote_author = quote_res.valueAsInt(0, d_mms_recipient_id, -1);
  std::string quote_body = quote_res.valueAsString(0, "body");
  if (quote_id == -1 || quote_author == -1)
  {
    Logger::warning("Failed to get quote data.");
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
    Logger::warning("Failed to set quote data.");
    return false;
  }

  // set quoted-attachment
  SqliteDB::QueryResults quote_att_res;
  std::string query = "SELECT _id, " + d_part_mid + ", " + d_part_ct + ", " + d_part_pending + ", data_size, " +
    (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : "-1 AS unique_id") +
    ", voice_note, width, height, quote";
  if (d_database.tableContainsColumn(d_part_table, "data_hash"))
    query += ", data_hash";
  else if (d_database.tableContainsColumn(d_part_table, "data_hash_start") &&
           d_database.tableContainsColumn(d_part_table, "data_hash_end"))
    query += ", data_hash_start, data_hash_end";
  query += " FROM " + d_part_table + " WHERE " + d_part_mid + " = ?";
  if (d_database.exec(query, quoted_message_id, &quote_att_res) &&
      quote_att_res.rows() >= 1)
  {
    //quote_att_res.prettyPrint();

    // get unique id for new attachments
    long long int unique_id = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_date_sent + " FROM " + d_mms_table +
                                                                          " WHERE _id = ?", new_msg_id, -1);
    if (d_database.tableContainsColumn(d_part_table, "unique_id") && unique_id == -1)
    {
      Logger::warning("Failed to get unique_id for attachment in quote. Skipping.");
      return false;
    }

    // get unique id for existing attachments (to retrieve attachment from d_attachments)...
    long long int quoted_unique_id = quote_att_res.valueAsInt(0, "unique_id", -1);
    if (d_database.tableContainsColumn(d_part_table, "unique_id") && unique_id == -1)
    {
      Logger::warning("Failed to get unique_id for quoted attachment. Skipping.");
      return false;
    }

    for (unsigned int i = 0; i < quote_att_res.rows(); ++i)
    {
      // set sql data
      std::any retval;
      if (!insertRow(d_part_table,
                     {{d_part_mid, new_msg_id},
                      {d_part_ct, quote_att_res.value(i, d_part_ct)},
                      {d_part_pending, quote_att_res.value(i, d_part_pending)},
                      {"data_size", quote_att_res.value(i, "data_size")},
                      {(d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : ""), unique_id},
                      {"voice_note", quote_att_res.value(i, "voice_note")},
                      {"width", quote_att_res.value(i, "width")},
                      {"height", quote_att_res.value(i, "height")},
                      {"quote", 1},
                      {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""),
                       (d_database.tableContainsColumn(d_part_table, "data_hash") ? quote_att_res.value(i, "data_hash") : std::any())},
                      {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""),
                       (d_database.tableContainsColumn(d_part_table, "data_hash_start") ? quote_att_res.value(i, "data_hash_start") : std::any())},
                      {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""),
                       (d_database.tableContainsColumn(d_part_table, "data_hash_end") ? quote_att_res.value(i, "data_hash_end") : std::any())}},
                     "_id", &retval))
      {
        Logger::error("Inserting part-data");
        continue;
      }
      long long int new_part_id = std::any_cast<long long int>(retval);

      // add attachment
      if (!bepaald::contains(d_attachments, std::pair{quote_att_res.valueAsInt(i, "_id", -1), quoted_unique_id}))
      {
        Logger::warning("Failed to find original of quoted attachment. Skipping.");
        d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
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
