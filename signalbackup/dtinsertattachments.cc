/*
  Copyright (C) 2022-2025  Selwin van Dijk

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

#include "../common_filesystem.h"
#include "../desktopattachmentreader/desktopattachmentreader.h"
#include "../attachmentmetadata/attachmentmetadata.h"


void SignalBackup::dtInsertAttachment(long long int mms_id, long long int unique_id, long long int realsize,
                                      SqliteDB::QueryResults const &attachment_results,
                                      SqliteDB::QueryResults const &linkpreview_results,
                                      SqliteDB::QueryResults const &sticker_results,
                                      std::string const &databasedir, bool isquote, bool targetisdummy)
{
  // if the 'path' is empty, we do not have actual attachment data, maybe
  // the attachment download failed... we still insert it.
  if (attachment_results.valueAsString(0, "path").empty())
  {
    if (attachment_results.valueAsInt(0, "pending", 0) != 0)
    {
      if (!insertRow(d_part_table,
                     {{d_part_mid, mms_id},
                      {d_part_ct, attachment_results.value(0, "content_type")},
                      {d_part_pending, 2},
                      {"data_size", attachment_results.value(0, "size")},
                      {"file_name", attachment_results.value(0, "file_name")},
                      {(d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : ""), unique_id},
                      {"voice_note", attachment_results.isNull(0, "flags") ? 0 : (attachment_results.valueAsInt(0, "flags", 0) == 1 ? 1 : 0)},
                      {"width", 0},
                      {"height", 0},
                      {"quote", isquote ? 1 : 0},
                      {"upload_timestamp", attachment_results.value(0, "upload_timestamp")},
                      {"cdn_number", attachment_results.value(0, "cdn_number")},
                      {"display_order", attachment_results.value(0, "orderInMessage")}},
                     "_id"))
      {
        Logger::error("Inserting part-data (pending)");
        return;
      }
    }
    else
    {
      //std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
      //          << ": Attachment not found." << std::endl;
    }

    // std::cout << "Here is the message full data:" << std::endl;
    // SqliteDB::QueryResults res;
    // ddb.exec("SELECT *,DATETIME(ROUND(IFNULL(received_at, 0) / 1000), 'unixepoch', 'localtime') AS HUMAN_READABLE_TIME FROM messages " + where, &res);
    // res.printLineMode();
    // std::string convuuid = res.valueAsString(0, "conversationId");
    // ddb.printLineMode("SELECT profileFullName FROM conversations where id = '" + convuuid + "'");
    return;
  }

  if (attachment_results.isNull(0, "path") || attachment_results.valueAsString(0, "path").empty())
  {
    Logger::error("Attachment path not found.");
    //attachment_results.printLineMode();
    return;
  }

  int version = attachment_results.valueAsInt(0, "version", -1);
  std::string localkey(attachment_results(0, "localKey"));
  int64_t size = realsize;
  std::string fullpath(databasedir + "/attachments.noindex/" + attachment_results.valueAsString(0, "path"));

  if (version >= 2 && (localkey.empty() || size == -1))
  {
    Logger::error("Decryption info for attachment not valid. (version: ", version, ", key: ", localkey, ", size: ", size, ")");
    //attachment_results.printLineMode();
    return;
  }

  long long int filesize = realsize == -1 ? 0 : realsize;
  std::string hash;

  if (!targetisdummy || filesize == 0)
  {
    // get attachment metadata
    AttachmentMetadata amd;
    if (version >= 2) [[likely]]
    {
      DesktopAttachmentReader dar(version, fullpath, localkey, size);
#if __cpp_lib_out_ptr >= 202106L
      std::unique_ptr<unsigned char[]> att_data;
      if (dar.getAttachmentData(std::out_ptr(att_data), d_verbose) != DesktopAttachmentReader::ReturnCode::OK)
#else
      unsigned char *att_data = nullptr; // !! NOTE RAW POINTER
      if (dar.getAttachmentData(&att_data, d_verbose) != DesktopAttachmentReader::ReturnCode::OK)
#endif
      {
        Logger::error("Failed to get attachment data");
        return;
      }

#if __cpp_lib_out_ptr >= 202106L
      amd = AttachmentMetadata::getAttachmentMetaData(fullpath, att_data.get(), size); // get metadata from heap
#else
      amd = AttachmentMetadata::getAttachmentMetaData(fullpath, att_data, size);       // get metadata from heap
      if (att_data)
        delete[] att_data;
#endif
    }
    else
      amd = AttachmentMetadata::getAttachmentMetaData(fullpath);                        // get from file

    if (amd.filename.empty() || (amd.filesize == 0 && attachment_results.valueAsInt(0, "size", 0) != 0))
    {
      Logger::error("Trying to set attachment data. Skipping.");
      Logger::error_indent("Pending: ", attachment_results.valueAsInt(0, "pending"));
      //attachment_results.prettyPrint();
      //std::cout << amd.filesize << std::endl;

      //std::cout << "Corresponding message:" << std::endl;
      //ddb.prettyPrint("SELECT DATETIME(ROUND(messages.sent_at/1000),'unixepoch','localtime'),messages.body,COALESCE(conversations.profileFullName,conversations.name) AS correspondent FROM messages LEFT JOIN conversations ON json_extract(messages.json, '$.conversationId') == conversations.id " + where);
      return;
    }

    if (amd.filesize == 0)
    {
      Logger::warning("Skipping 0 byte attachment. Not supported in Signal Android.");
      return;
    }

    filesize = amd.filesize;
    hash = amd.hash;
  }
  else if (targetisdummy)
    if (!bepaald::fileOrDirExists(fullpath))
    {
      Logger::warning("Skipping attachment: '", fullpath, "'. File not found.");
      return;
    }

  //insert into part
  std::any retval;
  if (!insertRow(d_part_table,
                 {{d_part_mid, mms_id},
                  {d_part_ct, attachment_results.value(0, "content_type")},
                  {d_part_pending, 0},
                  {"data_size", filesize},
                  {(d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : ""), unique_id},
                  {"voice_note", attachment_results.isNull(0, "flags") ? 0 : (attachment_results.valueAsInt(0, "flags", 0) == 1 ? 1 : 0)},
                  {"width", attachment_results.value(0, "width")},
                  {"height", attachment_results.value(0, "height")},
                  {"quote", isquote ? 1 : 0},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), hash},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), hash},
                  {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), hash},
                  {"upload_timestamp", attachment_results.value(0, "upload_timestamp")},      // will be 0 on sticker
                  {"cdn_number", attachment_results.value(0, "cdn_number")}, // will be 0 on sticker, usually 0 or 2, but I dont know what it means
                  {"display_order", attachment_results.value(0, "orderInMessage")},
                  {"file_name", attachment_results.value(0, "file_name")}},
                 "_id", &retval))
  {
    Logger::error("Inserting part-data");
    return;
  }
  long long int new_part_id = std::any_cast<long long int>(retval);
  //std::cout << "Inserted part, new id: " << new_part_id << std::endl;

  if (sticker_results.rows() == 1)
  {
    // gather data
    std::string sticker_emoji = sticker_results("emoji");
    std::string sticker_packid = sticker_results("packid");
    long long int sticker_id = -1;
    if (sticker_results.valueHasType<long long int>(0, "stickerid"))
      sticker_id = sticker_results.getValueAs<long long int>(0, "stickerid");
    std::string sticker_packkey = sticker_results("packkey");
    if (!sticker_packkey.empty())
    {
      auto [key, keysize] = Base64::base64StringToBytes(sticker_packkey);
      if (key && keysize)
      {
        sticker_packkey = bepaald::bytesToHexString(key, keysize, true);
        bepaald::destroyPtr(&key, &keysize);
      }
    }

    // check data, emoji can be empty
    if (sticker_packid.empty() || sticker_packkey.empty() || sticker_id == -1)
    {
      Logger::warning("Sticker data unexectedly empty");
      sticker_results.printLineMode();
    }
    else
    {
      //std::cout << "UPDATING ATTACHMENT TABLE WOITH STICKER DATA (2) " << new_part_id << std::endl;
      if (d_database.exec("UPDATE " + d_part_table + " SET "
                          "sticker_pack_id = ?, "
                          "sticker_pack_key = ?, "
                          "sticker_id = ?"
                          " WHERE _id = ?",
                          {sticker_packid,
                           sticker_packkey,
                           sticker_id,
                           new_part_id}))
        // set emoji if not empty
        if (!sticker_emoji.empty())
          d_database.exec("UPDATE " + d_part_table + " SET sticker_emoji = ? WHERE _id = ?", {sticker_emoji, new_part_id});
    }
  }


  if (linkpreview_results.rows())
  {
    //std::cout << std::endl << "CALLING UPDATE LINK PREVIEW FOR MMS " << mms_id << " PARTID " << new_part_id << " UNIQUEID " << unique_id << std::endl << std::endl;
    dtUpdateLinkPreviewAttachment(mms_id, new_part_id, unique_id);
  }

  DeepCopyingUniquePtr<AttachmentFrame> new_attachment_frame;
  if (setFrameFromStrings(&new_attachment_frame,
                          std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                   (d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                                    "ATTACHMENTID:uint64:" + bepaald::toString(unique_id) : ""),
                                                   "LENGTH:uint32:" + bepaald::toString(filesize)}))
  {
    new_attachment_frame->setReader(new DesktopAttachmentReader(version, fullpath, localkey, filesize));
    d_attachments.emplace(std::make_pair(new_part_id,
                                         d_database.tableContainsColumn(d_part_table, "unique_id") ?
                                         unique_id : -1), new_attachment_frame.release());
  }
  else
  {
    Logger::error("Failed to create AttachmentFrame for data");
    Logger::error_indent("       rowid       : ", new_part_id);
    Logger::error_indent("       attachmentid: ", unique_id);
    Logger::error_indent("       length      : ", filesize);
    Logger::error_indent("       path        : ", databasedir, "/attachments.noindex/",
                         attachment_results.valueAsString(0, "path"));

    // try to remove the inserted part entry:
    d_database.exec("DELETE FROM " + d_part_table + " WHERE _id = ?", new_part_id);
    return;
  }

  //std::cout << "APPENDED ATTACHMENT FRAME[" << new_part_id << "," << unique_id <<  "]. FILE NAME: '" << d_attachments[{new_part_id, unique_id}]->filename() << "'" << std::endl;

}

bool SignalBackup::dtInsertAttachments(long long int mms_id, long long int unique_id, long long int rowid,
                                       SqliteDB const &ddb, std::string const &databasedir, bool targetisdummy,
                                       bool force_is_quote)
{
  SqliteDB::QueryResults attachment_results;
  ddb.exec("SELECT "
           "attachmentType,"  // attachmenttype = attachment, preview, sticker, long-message, quote, contact
           "IFNULL(orderInMessage, 0) AS orderInMessage,"
           "contentType AS content_type,"
           "size,"
           "path,"
           "localKey,"
           "fileName AS file_name,"
           "IFNULL(height, 0) AS height,"
           "IFNULL(width, 0) AS width,"
           "blurHash," // not used (yet)
           "IFNULL(pending, 0) AS pending,"
           "IFNULL(flags, 0) AS flags,"
           "IFNULL(transitCdnNumber, 0) AS cdn_number,"
           "IFNULL(transitCdnUploadTimestamp, 0) AS upload_timestamp,"
           "IFNULL(version, 1) AS version "
           //"thumbnailPath, thumbnailSize, thumbnailContentType, thumbnailLocalKey, thumbnailVersion "
           "FROM message_attachments " +
           (force_is_quote ?
            "WHERE messageId = (SELECT id FROM messages WHERE sent_at = (SELECT IFNULL(json_extract(json, '$.quote.id'), -1) FROM messages WHERE rowid = ?))" :
            "WHERE messageId = (SELECT id FROM messages WHERE rowid = ?) "s) +
           "AND "
           "editHistoryIndex = -1 "
           "ORDER BY orderInMessage ASC", rowid, &attachment_results);

  for (unsigned int i = 0; i < attachment_results.rows(); ++i)
  {
    std::string attachment_type = attachment_results(i, "attachmentType");

    // currently 'long-messages' have an entry in message_attachments, including a path.
    // however the path does not exist, and the full msg body is still simply in
    // messages.body. This is handled in dtImportLongText() called from importFromDesktop()
    if (attachment_type == "long-message")
      continue;

    long long int relevant_rowid = rowid;
    if (!force_is_quote)
    {
      if (attachment_type == "quote")
      {
        // skip quotes:
        // there is a strange thing in the database, where some quotes have an entry in the message_attachments table
        // for their quoted attachments, and some do not.
        // since, for the ones which do not, we already need to call this function again with the data from the
        // quoted message, we skip these quote-type attachments so the ones _with_ and entry in the message_attachments
        // table are not inserted doubled...
        continue;
      }
    }
    else
    {
      relevant_rowid = ddb.getSingleResultAs<long long int>("SELECT rowid FROM messages "
                                                            "WHERE sent_at = (SELECT json_extract(json, '$.quote.id') FROM messages WHERE rowid = ?)", rowid, rowid);
      bool is_erased = ddb.getSingleResultAs<long long int>("SELECT IFNULL(isErased, 0) FROM messages WHERE rowid = ?", relevant_rowid, 0) != 0;
      if (is_erased)
      {
        Logger::warning("Quoted message is deleted... skipping");
        //ddb.printLineMode("SELECT * FROM messages WHERE rowid = ?", rowid);
        continue;
      }
    }

    SqliteDB::QueryResults linkpreview_results;
    if (attachment_type == "preview")
      ddb.exec("SELECT "
               "json_extract(json, '$.preview[0].url') AS url,"
               "json_extract(json, '$.preview[0].title') AS title,"
               "json_extract(json, '$.preview[0].description') AS description "
               "FROM messages WHERE rowid = ?", rowid, &linkpreview_results);

    int64_t realsize = attachment_results.valueAsInt(i, "size", -1);
    SqliteDB::QueryResults sticker_results;
    if (attachment_type == "sticker")
    {
      ddb.exec("SELECT "
               "json_extract(json, '$.sticker.packKey') AS 'packkey',"
               "json_extract(json, '$.sticker.packId') AS 'packid',"
               "json_extract(json, '$.sticker.stickerId') AS 'stickerid',"
               "IFNULL(json_extract(json, '$.sticker.emoji'), '') AS 'emoji' "
               "FROM messages "
               "WHERE messages.rowid = ?", relevant_rowid, &sticker_results);

      // the size of a sticker in the messages/attachments table can be incorrect,
      // so lets get it from the stickers table...
      // in one single instance in my Desktop database, one sticker has
      // its size set to the encrypted filesize (13600) instead of the
      // real filesize (13062) in message_attachments.size,
      // this would cause a bufffer overflow when getting the data from
      // DesktopAttachmentReader. So here we get the correct size.
      realsize = ddb.getSingleResultAs<long long int>("SELECT size FROM stickers WHERE packId = ? AND id = ?", {sticker_results.value(0, "packid"), sticker_results.value(0, "stickerid")}, realsize);
    }

    dtInsertAttachment(mms_id, unique_id, realsize, attachment_results.getRow(i), linkpreview_results, sticker_results, databasedir, force_is_quote, targetisdummy);
  }

  return false;
}

bool SignalBackup::dtInsertAttachmentsOld(long long int mms_id, long long int unique_id, int numattachments, long long int haspreview,
                                          long long int rowid, SqliteDB const &ddb, std::string const &where, std::string const &databasedir,
                                          bool isquote, bool issticker, bool targetisdummy)
{
  bool quoted_linkpreview = false;
  bool quoted_sticker = false;
  if (numattachments == -1 && isquote) // quote attachments, number not known yet
  {
    SqliteDB::QueryResults res;
    if (!ddb.exec("SELECT IFNULL(json_array_length(json, '$.attachments'), 0) AS numattachments FROM messages " + where, &res) ||
        (res.rows() != 1 && res.columns() != 1))
    {
      Logger::warning("Failed to get number of attachments in quoted message. Skipping");
      return false;
    }
    numattachments = res.getValueAs<long long int>(0, "numattachments");

    if (numattachments == 0)
    {
      if (ddb.exec("SELECT json_extract(json, '$.preview[0].image.path') IS NOT NULL AS quoteispreview FROM messages " + where, &res) &&
          res.rows() == 1 && res.getValueAs<long long int>(0, "quoteispreview") != 0)
      {
        quoted_linkpreview = true;
        numattachments = 1;
        // std::cout << "              *********               GOT QUOTED LINK PREVIEW                *****************" << std::endl;
      }
    }

    if (numattachments == 0)
    {
      if (ddb.exec("SELECT json_extract(json, '$.sticker.data.path') IS NOT NULL AS quoteissticker FROM messages " + where, &res) &&
          res.rows() == 1 && res.getValueAs<long long int>(0, "quoteissticker") != 0)
      {
        quoted_sticker = true;
        numattachments = 1;
        //std::cout << "              *********               GOT QUOTED STICKER                *****************" << std::endl;
      }
    }
  }

  if (numattachments == 0)
    if (haspreview > 0 || issticker)
      numattachments = 1;

  //std::cout << "rowid: " << rowid << std::endl;
  //if (numattachments)
  //  std::cout << "  " << numattachments << " attachments" << (isquote ? " (in quote)" : "") << std::endl;


  // for each attachment:
  SqliteDB::QueryResults results_attachment_data;
  for (int k = 0; k < numattachments; ++k)
  {
    //std::cout << "  Attachment " << k + 1 << "/" << numattachments << ": " << std::flush;

    std::string jsonpath = "$.attachments[" + bepaald::toString(k) + "]";

    SqliteDB::QueryResults linkpreview_results;
    if (haspreview)
    {
      jsonpath = "$.preview[0].image"; // only in old style
      ddb.exec("SELECT "
               "json_extract(json, '$.preview[0].url') AS url,"
               "json_extract(json, '$.preview[0].title') AS title,"
               "json_extract(json, '$.preview[0].description') AS description,"
               "json_extract(json, '$.preview[0].image') AS image" // only in old style
               " FROM messages WHERE rowid = ?",
               rowid, &linkpreview_results);
    }
    if (issticker || quoted_sticker)
      jsonpath = "$.sticker.data";

    if (quoted_linkpreview)
      jsonpath = "$.preview[0].image";

    // get the attachment info (content-type, size, path, ...)

    // the size of a sticker in the messages table can be incorrect,
    // so lets get it from the stickers table...
    // in one single instance in my Desktop database, one sticker has
    // its size set to the encrypted filesize (13600) instead of the
    // real filesize (13062) in messages.json<$.sticker.data.size>,
    // this woud cause a bufffer overflow when getting the data from
    // DesktopAttachmentReader. So here, we COALESCE with stickers.size
    // on packid and stickerid, to get the correct size.
    if (!ddb.exec(bepaald::concat("SELECT "
                                  "json_extract(messages.json, '", jsonpath, ".path') AS path,"
                                  "json_extract(messages.json, '", jsonpath, ".contentType') AS content_type,"
                                  "COALESCE(stickers.size, json_extract(messages.json, '", jsonpath, ".size')) AS size,"
                                  //"json_extract(json, '", jsonpath, ".cdnKey') AS cdn_key,"
                                  "json_extract(messages.json, '", jsonpath, ".localKey') AS localKey,"
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".version'), 1) AS version,"
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".width'), 0) AS width,"
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".height'), 0) AS height,"
                                  "0 AS orderInMessage," // not present in old attachment-json?

                                  // only in sticker
                                  "json_extract(messages.json, '", jsonpath, ".emoji') AS sticker_emoji,"
                                  "json_extract(messages.json, '", jsonpath, ".packId') AS sticker_packid,"
                                  "json_extract(messages.json, '", jsonpath, ".id') AS sticker_id,"

                                  // not when sticker
                                  "json_extract(messages.json, '", jsonpath, ".fileName') AS file_name,"
                                  "IFNULL(JSONLONG(json_extract(messages.json, '", jsonpath, ".uploadTimestamp')), 0) AS upload_timestamp,"
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".flags'), 0) AS flags," // currently, the only flag implemented in Signal is:  VOICE_NOTE = 1
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".pending'), 0) AS pending,"
                                  "IFNULL(json_extract(messages.json, '", jsonpath, ".cdnNumber'), 0) AS cdn_number"

                                  " FROM messages "
                                  "LEFT JOIN stickers ON stickers.packId = sticker_packid AND stickers.id = sticker_id ", where),
                  &results_attachment_data))
    {
      Logger::error("Failed to get attachment data from desktop database");
      continue;
    }

    SqliteDB::QueryResults sticker_results;
    if (issticker || quoted_sticker)
    {
      // get the data from $.sticker (instead of $.sticker.data)
      ddb.exec("SELECT "
               "json_extract(json, '$.sticker.packKey') AS 'packkey',"
               "json_extract(json, '$.sticker.packId') AS 'packid',"
               "json_extract(json, '$.sticker.stickerId') AS 'stickerid',"
               "IFNULL(json_extract(json, '$.sticker.emoji'), '') AS 'emoji' "
               "FROM messages " + where, &sticker_results);
    }

    dtInsertAttachment(mms_id, unique_id, results_attachment_data.valueAsInt(0, "size", -1), results_attachment_data, linkpreview_results, sticker_results, databasedir, isquote, targetisdummy);
  }
  return true;
}
