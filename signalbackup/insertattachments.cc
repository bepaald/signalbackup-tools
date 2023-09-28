/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

bool SignalBackup::insertAttachments(long long int mms_id, long long int unique_id, int numattachments, long long int haspreview,
                                     long long int rowid, SqliteDB const &ddb, std::string const &where, std::string const &databasedir,
                                     bool isquote, bool issticker)
{
  bool quoted_linkpreview = false;
  if (numattachments == -1 && isquote) // quote attachments, number not known yet
  {
    SqliteDB::QueryResults res;
    if (!ddb.exec("SELECT IFNULL(json_array_length(json, '$.attachments'), 0) AS numattachments FROM messages " + where, &res) ||
        (res.rows() != 1 && res.columns() != 1))
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Failed to get number of attachments in quoted message. Skipping" << std::endl;
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
        //std::cout << "GOT QUOTED LINK PREVIEW" << std::endl;
      }
    }
  }

  if (numattachments == 0)
  {
    if (haspreview > 0)
      numattachments = 1;
    else if (issticker)
      numattachments = 1;
  }

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
      jsonpath = "$.preview[0].image";
      ddb.exec("SELECT "
               "json_extract(json, '$.preview[0].url') AS url,"
               "json_extract(json, '$.preview[0].title') AS title,"
               "json_extract(json, '$.preview[0].description') AS description,"
               "json_extract(json, '$.preview[0].image') AS image"
               " FROM messages WHERE rowid = ?",
               rowid, &linkpreview_results);
    }
    if (issticker)
      jsonpath = "$.sticker.data";

    if (quoted_linkpreview)
      jsonpath = "$.preview[0].image";

    // get the attachment info (content-type, size, path, ...)
    if (!ddb.exec("SELECT "
                  "json_extract(json, '" + jsonpath + ".path') AS path,"
                  "json_extract(json, '" + jsonpath + ".contentType') AS content_type,"
                  "json_extract(json, '" + jsonpath + ".size') AS size,"
                  //"json_extract(json, '" + jsonpath + ".cdnKey') AS cdn_key,"

                  // only in sticker
                  "json_extract(json, '" + jsonpath + ".emoji') AS sticker_emoji,"
                  "json_extract(json, '" + jsonpath + ".packId') AS sticker_packid,"
                  "json_extract(json, '" + jsonpath + ".id') AS sticker_id,"

                  // not when sticker
                  "json_extract(json, '" + jsonpath + ".fileName') AS file_name,"
                  "IFNULL(json_extract(json, '" + jsonpath + ".uploadTimestamp'), 0) AS upload_timestamp,"
                  "IFNULL(json_extract(json, '" + jsonpath + ".flags'), 0) AS flags," // currently, the only flag implemented in Signal is:  VOICE_NOTE = 1
                  "IFNULL(json_extract(json, '" + jsonpath + ".pending'), 0) AS pending,"
                  "IFNULL(json_extract(json, '" + jsonpath + ".cdnNumber'), 0) AS cdn_number"

                  " FROM messages " + where, &results_attachment_data))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to get attachment data from desktop database" << std::endl;
      continue;
    }

    // insert any attachments with missing data. (pending != 0)
    if (results_attachment_data.valueAsString(0, "path").empty())
    {
      if (results_attachment_data.getValueAs<long long int>(0, "pending") != 0)
      {
        if (!insertRow("part",
                       {{"mid", mms_id},
                        {"ct", results_attachment_data.value(0, "content_type")},
                        {"pending_push", 2},
                        {"data_size", results_attachment_data.value(0, "size")},
                        {"file_name", results_attachment_data.value(0, "file_name")},
                        {"unique_id", unique_id},
                        {"voice_note", results_attachment_data.isNull(0, "flags") ? 0 : (results_attachment_data.valueAsInt(0, "flags", 0) == 1 ? 1 : 0)},
                        {"width", 0},
                        {"height", 0},
                        {"quote", isquote ? 1 : 0},
                        {"upload_timestamp", results_attachment_data.value(0, "upload_timestamp")},
                        {"cdn_number", results_attachment_data.value(0, "cdn_number")}},
                       "_id"))
        {
          std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting part-data (pending)" << std::endl;
          continue;
        }
      }
      else
      {
        //std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
        //          << ": Attachment not found." << std::endl;
      }

      if (haspreview && linkpreview_results.rows())
      {
        // this work, but just for consistency, I'd like to escape the string as Signal does for some reason
        //d_database.exec("UPDATE " + d_mms_table + " SET link_previews = json_array(json_object('url', ?, 'title', ?, 'description', ?, 'date', 0, 'attachmentId', NULL)) WHERE _id = ?",
        //                {linkpreview_results.value(0, "url"), linkpreview_results.value(0, "title"), linkpreview_results.value(0, "description"), mms_id});

        /*
          STRING_ESCAPE():
          Quotation mark (") 	\"
          Reverse solidus (\) 	\|
          Solidus (/) 	\/
          Backspace 	\b
          Form feed 	\f
          New line 	\n
          Carriage return 	\r
          Horizontal tab 	\t

          As far as I can tell/test, only '/' '\' and '"' are escaped

          NOTE backslash needs to be done first, or the backslashes inserted by other escapes are escaped...
        */
        std::string url = linkpreview_results("url");
        bepaald::replaceAll(&url, '\\', R"(\\)");
        //bepaald::replaceAll(&url, "'", R"(\')");  // not done in db
        bepaald::replaceAll(&url, '/', R"(\/)");
        bepaald::replaceAll(&url, '\"', R"(\")");
        bepaald::replaceAll(&url, '\'', R"('')");
        bepaald::replaceAll(&url, '\n', R"(\n)");
        bepaald::replaceAll(&url, '\t', R"(\t)");
        bepaald::replaceAll(&url, '\b', R"(\b)");
        bepaald::replaceAll(&url, '\f', R"(\f)");
        bepaald::replaceAll(&url, '\r', R"(\r)");
        std::string title = linkpreview_results("title");
        bepaald::replaceAll(&title, '\\', R"(\\)");
        bepaald::replaceAll(&title, '/', R"(\/)");
        bepaald::replaceAll(&title, '\"', R"(\")");
        bepaald::replaceAll(&title, '\'', R"('')");
        bepaald::replaceAll(&title, '\n', R"(\n)");
        bepaald::replaceAll(&title, '\t', R"(\t)");
        bepaald::replaceAll(&title, '\b', R"(\b)");
        bepaald::replaceAll(&title, '\f', R"(\f)");
        bepaald::replaceAll(&title, '\r', R"(\r)");
        std::string description = linkpreview_results("description");
        bepaald::replaceAll(&description, '\\', R"(\\)");
        bepaald::replaceAll(&description, '/', R"(\/)");
        bepaald::replaceAll(&description, '\"', R"(\")");
        bepaald::replaceAll(&description, '\'', R"('')");
        bepaald::replaceAll(&description, '\n', R"(\n)");
        bepaald::replaceAll(&description, '\t', R"(\t)");
        bepaald::replaceAll(&description, '\b', R"(\b)");
        bepaald::replaceAll(&description, '\f', R"(\f)");
        bepaald::replaceAll(&description, '\r', R"(\r)");

        SqliteDB::QueryResults jsonstring;
        ddb.exec("SELECT json_array(json_object('url', json('\"" + url + "\"'), 'title', json('\"" + title + "\"'), 'description', json('\"" + description + "\"'), 'date', 0, 'attachmentId', NULL)) AS link_previews",
                 &jsonstring);
        std::string linkpreview_as_string = jsonstring("link_previews");

        bepaald::replaceAll(&linkpreview_as_string, '\'', R"('')");

        d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_previews + " = '" + linkpreview_as_string + "' WHERE _id = ?", mms_id);

        //d_database.print("SELECT _id,link_previews FROM message WHERE _id = ?", mms_id);
      }

      // std::cout << "Here is the message full data:" << std::endl;
      // SqliteDB::QueryResults res;
      // ddb.exec("SELECT *,DATETIME(ROUND(IFNULL(received_at, 0) / 1000), 'unixepoch', 'localtime') AS HUMAN_READABLE_TIME FROM messages " + where, &res);
      // res.printLineMode();
      // std::string convuuid = res.valueAsString(0, "conversationId");
      // ddb.printLineMode("SELECT profileFullName FROM conversations where id = '" + convuuid + "'");
      continue;
    } // if (path.empty())

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

    if (amd.filename.empty() || (amd.filesize == 0 && results_attachment_data.valueAsInt(0, "size", 0) != 0))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Trying to set attachment data. Skipping." << std::endl;
      std::cout << "Pending: " << results_attachment_data.valueAsInt(0, "pending") << std::endl;
      //results_attachment_data.prettyPrint();
      //std::cout << amd.filesize << std::endl;

      //std::cout << "Corresponding message:" << std::endl;
      //ddb.prettyPrint("SELECT DATETIME(ROUND(messages.sent_at/1000),'unixepoch','localtime'),messages.body,COALESCE(conversations.profileFullName,conversations.name) AS correspondent FROM messages LEFT JOIN conversations ON json_extract(messages.json, '$.conversationId') == conversations.id " + where);
      continue;
    }

    if (amd.filesize == 0 || results_attachment_data.valueAsInt(0, "size", 0) == 0)
    {
      std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off << ": Skipping 0 byte attachment. Not supported in Signal Android." << std::endl;
      continue;
    }

    //insert into part
    std::any retval;
    if (!insertRow("part",
                   {{"mid", mms_id},
                    {"ct", results_attachment_data.value(0, "content_type")},
                    {"pending_push", 0},
                    {"data_size", results_attachment_data.value(0, "size")},
                    {"unique_id", unique_id},
                    {"voice_note", results_attachment_data.isNull(0, "flags") ? 0 : (results_attachment_data.valueAsInt(0, "flags", 0) == 1 ? 1 : 0)},
                    {"width", amd.width == -1 ? 0 : amd.width},
                    {"height", amd.height == -1 ? 0 : amd.height},
                    {"quote", isquote ? 1 : 0},
                    {"data_hash", amd.hash},
                    {"upload_timestamp", results_attachment_data.value(0, "upload_timestamp")},      // will be 0 on sticker
                    {"cdn_number", results_attachment_data.value(0, "cdn_number")}, // will be 0 on sticker, usually 0 or 2, but I dont know what it means
                    {"file_name", results_attachment_data.value(0, "file_name")}},
                   "_id", &retval))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Inserting part-data" << std::endl;
      continue;
    }
    long long int new_part_id = std::any_cast<long long int>(retval);
    //std::cout << "Inserted part, new id: " << new_part_id << std::endl;

    if (issticker)
    {
      // get the data from $.sticker (instead of $.sticker.data)
      SqliteDB::QueryResults stickerdata;
      if (ddb.exec("SELECT "
                   "json_extract(json, '$.sticker.packKey') AS 'packkey',"
                   "json_extract(json, '$.sticker.packId') AS 'packid',"
                   "json_extract(json, '$.sticker.stickerId') AS 'stickerid',"
                   "IFNULL(json_extract(json, '$.sticker.emoji'), '') AS 'emoji' "
                   "FROM messages " + where, &stickerdata) &&
          stickerdata.rows() == 1)
      {
        // gather data
        std::string sticker_emoji = stickerdata("emoji");
        std::string sticker_packid = stickerdata("packid");
        long long int sticker_id = -1;
        if (stickerdata.valueHasType<long long int>(0, "stickerid"))
          sticker_id = stickerdata.getValueAs<long long int>(0, "stickerid");
        std::string sticker_packkey = stickerdata("packkey");
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
          stickerdata.printLineMode();
        else
        {
          if (d_database.exec("UPDATE part SET "
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
              d_database.exec("UPDATE part SET sticker_emoji = ? WHERE _id = ?", {sticker_emoji, new_part_id});
        }
      }
    }

    if (haspreview && linkpreview_results.rows())
    {
      // this works, but I want to escape the string like Signal does
      //d_database.exec("UPDATE " + d_mms_table + " SET d_mms_previews = json_array(json_object('url', ?, 'title', ?, 'description', ?, 'date', 0, 'attachmentId', json_object('rowId', ?, 'uniqueId', ?, 'valid', true))) "
      //"WHERE _id = ?", {linkpreview_results.value(0, "url"), linkpreview_results.value(0, "title"), linkpreview_results.value(0, "description"), new_part_id, unique_id, mms_id});

      std::string url = linkpreview_results("url");
      bepaald::replaceAll(&url, '\\', R"(\\)");
      bepaald::replaceAll(&url, '/', R"(\/)");
      bepaald::replaceAll(&url, '\"', R"(\")");
      bepaald::replaceAll(&url, '\'', R"('')");
      bepaald::replaceAll(&url, '\n', R"(\n)");
      bepaald::replaceAll(&url, '\t', R"(\t)");
      bepaald::replaceAll(&url, '\b', R"(\b)");
      bepaald::replaceAll(&url, '\f', R"(\f)");
      bepaald::replaceAll(&url, '\r', R"(\r)");
      std::string title = linkpreview_results("title");
      bepaald::replaceAll(&title, '\\', R"(\\)");
      bepaald::replaceAll(&title, '/', R"(\/)");
      bepaald::replaceAll(&title, '\"', R"(\")");
      bepaald::replaceAll(&title, '\'', R"('')");
      bepaald::replaceAll(&title, '\n', R"(\n)");
      bepaald::replaceAll(&title, '\t', R"(\t)");
      bepaald::replaceAll(&title, '\b', R"(\b)");
      bepaald::replaceAll(&title, '\f', R"(\f)");
      bepaald::replaceAll(&title, '\r', R"(\r)");
      std::string description = linkpreview_results("description");
      bepaald::replaceAll(&description, '\\', R"(\\)");
      bepaald::replaceAll(&description, '/', R"(\/)");
      bepaald::replaceAll(&description, '\"', R"(\")");
      bepaald::replaceAll(&description, '\'', R"('')");
      bepaald::replaceAll(&description, '\n', R"(\n)");
      bepaald::replaceAll(&description, '\t', R"(\t)");
      bepaald::replaceAll(&description, '\b', R"(\b)");
      bepaald::replaceAll(&description, '\f', R"(\f)");
      bepaald::replaceAll(&description, '\r', R"(\r)");

      SqliteDB::QueryResults jsonstring;
      ddb.exec("SELECT json_array(json_object("
               "'url', json('\"" + url + "\"'), "
               "'title', json('\"" + title + "\"'), "
               "'description', json('\"" + description + "\"'), "
               "'date', 0, "
               "'attachmentId', json_object('rowId', ?, 'uniqueId', ?, 'valid', json('true')))) AS link_previews",
               {new_part_id, unique_id}, &jsonstring);
      std::string linkpreview_as_string = jsonstring("link_previews");

      bepaald::replaceAll(&linkpreview_as_string, '\'', R"('')");

      d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_previews + " = '" + linkpreview_as_string + "' WHERE _id = ?", mms_id);
      //d_database.print("SELECT _id,d_mms_previews FROM message WHERE _id = ?", mms_id);
    }
    /*
    // 1 link with preview image
    // DESKTOP
    // json_extract(json, '$.preview') = [{"url":"https://www.reddit.com/r/StableDiffusionInfo/comments/10h30h6/tutorial_on_installing_sd_to_run_locally_on/","title":"r/StableDiffusionInfo on Reddit","image":{"contentType":"image/jpeg","size":69266,"flags":0,"width":1200,"height":630,"blurHash":"LASX=n.:zATd_2rpkoy?:4K*BX+Z","uploadTimestamp":1674935886235,"cdnNumber":2,"cdnKey":"AnPqh3-Ujsx9ZGeW1_J1","path":"d6/d6e6cad87d22f14500024701a34aa2d76abfce1f5b2ce0e619c0c1dd6d235be1","thumbnail":{"path":"2d/2d3f2e7e9d782fd33f2b20e2a6ead088decacc5d549b1451d2d43dddae99db96","contentType":"image/png","width":150,"height":150}},"description":"Tutorial on installing SD to run locally on Windows?"}]
    //   -->
    // ANDROID
    // link_previews = [{"url":"https:\/\/www.reddit.com\/r\/StableDiffusionInfo\/comments\/10h30h6\/tutorial_on_installing_sd_to_run_locally_on\/","title":"r\/StableDiffusionInfo on Reddit","description":"Tutorial on installing SD to run locally on Windows?","date":0,"attachmentId":{"rowId":28,"uniqueId":1675171736355,"valid":true}}]

    //
    // 1 link, no preview image:
    // DESKTOP
    // preview":[{"description":"Posted by u/calilaser - 65 votes and 7 comments","title":"r/esp32 on Reddit: 10 Steps To Building a Light Up IoT Button from Scratch","url":"https://www.reddit.com/r/esp32/comments/12b5258/10_steps_to_building_a_light_up_iot_button_from/","domain":"www.reddit.com","isStickerPack":false}]
    //  --> ANDROID
    // link_previews = [{"url":"https:\/\/www.reddit.com\/r\/esp32\/comments\/12b5258\/10_steps_to_building_a_light_up_iot_button_from\/","title":"r\/esp32 on Reddit: 10 Steps To Building a Light Up IoT Button from Scratch","description":"Posted by u\/calilaser - 65 votes and 7 comments","date":0,"attachmentId":null}]
    */

    std::unique_ptr<AttachmentFrame> new_attachment_frame;
    if (setFrameFromStrings(&new_attachment_frame, std::vector<std::string>{"ROWID:uint64:" + bepaald::toString(new_part_id),
                                                                            "ATTACHMENTID:uint64:" + bepaald::toString(unique_id),
                                                                            "LENGTH:uint32:" + bepaald::toString(amd.filesize)}))/* &&
      new_attachment_frame->setAttachmentData(databasedir + "/attachments.noindex/" + results_attachment_data.valueAsString(0, "path")))*/
    {
      new_attachment_frame->setLazyDataRAW(amd.filesize, databasedir + "/attachments.noindex/" + results_attachment_data.valueAsString(0, "path"));
      d_attachments.emplace(std::make_pair(new_part_id, unique_id), new_attachment_frame.release());
    }
    else
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off << ": Failed to create AttachmentFrame for data" << std::endl;
      std::cout << "       rowid       : " << new_part_id << std::endl;
      std::cout << "       attachmentid: " << unique_id << std::endl;
      std::cout << "       length      : " << amd.filesize << std::endl;
      std::cout << "       path        : " << databasedir << "/attachments.noindex/" << results_attachment_data.valueAsString(0, "path") << std::endl;

      // try to remove the inserted part entry:
      d_database.exec("DELETE FROM part WHERE _id = ?", new_part_id);
      continue;
    }

    //std::cout << "APPENDED ATTACHMENT FRAME[" << new_part_id << "," << unique_id <<  "]. FILE NAME: '" << d_attachments[{new_part_id, unique_id}]->filename() << "'" << std::endl;
  }
  return true;
}
