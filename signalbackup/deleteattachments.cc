/*
  Copyright (C) 2021-2024  Selwin van Dijk

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

#include "../attachmentmetadata/attachmentmetadata.h"

bool SignalBackup::deleteAttachments(std::vector<long long int> const &threadids,
                                     std::string const &before, std::string const &after,
                                     long long int filesize, std::vector<std::string> const &mimetypes,
                                     std::string const &append, std::string const &prepend,
                                     std::vector<std::pair<std::string, std::string>> replace)
{
  std::string query_delete("DELETE FROM " + d_part_table);
  std::string query_list("SELECT _id," + d_part_mid + "," +
                         (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id" : "-1 AS unique_id") +
                         "," + d_part_ct + ",quote FROM " + d_part_table);

  std::string specification;

  if (!threadids.empty() && threadids[0] != -1) // -1 indicates 'ALL', easier not to specify
  {
    if (specification.empty())
      specification += " WHERE " + d_part_mid + " IN (SELECT _id FROM " + d_mms_table + " WHERE ";
    else
      specification += " AND ";

    for (unsigned int i = 0; i < threadids.size(); ++i)
      specification += ((i == 0) ? "("s : ""s) + "thread_id IS " + bepaald::toString(threadids[i]) + ((i == threadids.size() - 1) ? ")" : " OR ");
  }

  if (!before.empty())
  {
    long long int date = dateToMSecsSinceEpoch(before);
    if (date == -1)
      Logger::warning("Ignoring before-date: '", before, "'. Failed to parse or invalid.");
    else
    {
      if (specification.empty())
        specification += " WHERE " + d_part_mid + " IN (SELECT _id FROM " + d_mms_table + " WHERE ";
      else
        specification += " AND ";
      specification += "date_received < " + bepaald::toString(date);
    }
  }

  if (!after.empty())
  {
    long long int date = dateToMSecsSinceEpoch(after);
    if (date == -1)
      Logger::warning("Ignoring after-date: '", after, "'. Failed to parse or invalid.");
    else
    {
      if (specification.empty())
        specification += " WHERE " + d_part_mid + " IN (SELECT _id FROM " + d_mms_table + " WHERE ";
      else
        specification += " AND ";
      specification += "date_received > " + bepaald::toString(date);
    }
  }

  if (!specification.empty())
    specification += ")";

  if (filesize >= 0)
  {
    if (specification.empty())
      specification += " WHERE ";
    else
      specification += " AND ";
    specification += "data_size > " + bepaald::toString(filesize);
  }

  if (!mimetypes.empty())
  {
    if (specification.empty())
      specification += " WHERE ";
    else
      specification += " AND ";
    for (unsigned int i = 0; i < mimetypes.size(); ++i)
      specification += ((i == 0) ? "("s : ""s) + d_part_ct + " LIKE \"" + mimetypes[i] + "%\"" + ((i == mimetypes.size() - 1) ? ")" : " OR ");
  }

  query_delete += specification;
  query_list += specification;
  //std::cout << query_list << std::endl;

  SqliteDB::QueryResults res;
  if (!d_database.exec(query_list, &res))
  {
    Logger::error("Failed to execute query.");
    return false;
  }

  //res.prettyPrint();

  // just delete the attachments
  if (replace.empty())
  {

    if (!d_database.exec(query_delete))
    {
      Logger::error("Failed to execute query.");
      return false;
    }
    Logger::message("Deleted: ", d_database.changed(), " 'part'-entries.");

    // find all mms entries to which the deleted attachments belonged
    // if no attachments remain and body is empty -> delete mms -> cleanDatabaseByMessages()
    SqliteDB::QueryResults res2;
    long long int count = 0;
    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      if (!append.empty() || !prepend.empty())
      {
        // update existing message bodies
        if (!append.empty())
        {
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = body || ? WHERE _id = ? AND (body IS NOT NULL AND body != '')", {"\n\n" + append, res.getValueAs<long long int>(i, d_part_mid)}))
            return false;
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')", {append, res.getValueAs<long long int>(i, d_part_mid)}))
            return false;
        }
        if (!prepend.empty())
        {
          // update message ranges if present:
          std::pair<std::shared_ptr<unsigned char []>, size_t> brdata =
            d_database.getSingleResultAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>("SELECT " + d_mms_ranges + " FROM " + d_mms_table + " WHERE LENGTH(" + d_mms_ranges + ") != 0 AND _id = ?",
                                                                                               res.getValueAs<long long int>(i, d_part_mid), {nullptr, 0});
          if (brdata.second)
          {
            BodyRanges brsproto(brdata);
            //brsproto.print();

            BodyRanges new_bodyrange_vec;

            auto bodyrange_vec = brsproto.getField<1>();
            for (auto const &bodyrange : bodyrange_vec)
            {
              BodyRange new_bodyrange = bodyrange;
              int start = new_bodyrange.getField<1>().value_or(-1);
              if (start != -1) [[likely]]
              {
                new_bodyrange.deleteFields(1);
                new_bodyrange.addField<1>(start + prepend.size() + 2); // plus two for the extra new lines
                //new_bodyrange.print();
              }
              new_bodyrange_vec.addField<1>(new_bodyrange);
            }

            if (!d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_ranges + " = ? WHERE _id = ?",
                                 {std::make_pair(new_bodyrange_vec.data(), static_cast<size_t>(new_bodyrange_vec.size())), res.getValueAs<long long int>(i, d_part_mid)}))
              return false;
          }

          // update mentions if present
          d_database.exec("UPDATE mention SET range_start = range_start + ? WHERE message_id = ?", {prepend.size() + 2, res.getValueAs<long long int>(i, d_part_mid)});
          if (d_verbose) [[unlikely]]
            Logger::message("Updated ", d_database.changed(), " mention to adjust for prependbody");

          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? || body WHERE _id = ? AND (body IS NOT NULL AND body != '')",
                               {prepend + "\n\n", res.getValueAs<long long int>(i, d_part_mid)}))
            return false;
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')",
                               {prepend, res.getValueAs<long long int>(i, d_part_mid)}))
            return false;
        }
      }
      else // no append/prepend -> delete message if body empty
      {

        // check if message has other attachments
        if (!d_database.exec("SELECT _id FROM " + d_part_table
                             + " WHERE " + d_part_mid + " = ?", res.getValueAs<long long int>(i, d_part_mid), &res2))
          return false;

        if (res2.empty()) // no other attachments for this message, we can delete if body is empty
        {
          //std::cout << "no other attachments (" << i << ")" << std::endl;

          // delete message if body empty
          if (!d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id = ? AND (body IS NULL OR body == '')", res.getValueAs<long long int>(i, d_part_mid)))
            return false;
          if (d_database.changed() == 1)
          {
            ++count;
            continue;
          }
        }
      }
    }
    if (count || (append.empty() && prepend.empty()))
      Logger::message("Deleted ", count, " empty messages");

    // make sure the 'mms.previews' column does not reference non-existing attachments
    long long int maxlinkpreviews = d_database.getSingleResultAs<long long int>("SELECT MAX(json_array_length(" + d_mms_previews + ")) FROM " + d_mms_table, 0);
    for (unsigned int lp = 0; lp < maxlinkpreviews; ++lp)
    {
      //d_database.print("SELECT " + d_mms_previews + " FROM " + d_mms_table + " WHERE " + d_mms_previews + " IS NOT NULL");
      if (d_database.tableContainsColumn(d_part_table, "unique_id"))
        d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_previews + " = json_set(" + d_mms_previews + ", '$[" + bepaald::toString(lp) + "].attachmentId', json(null)) WHERE "
                        "json_extract(" + d_mms_previews + ", '$[" + bepaald::toString(lp) + "].attachmentId.rowId') NOT IN (SELECT _id FROM " + d_part_table + ") AND "
                        "json_extract(" + d_mms_previews + ", '$[" + bepaald::toString(lp) + "].attachmentId.uniqueId') NOT IN (SELECT unique_id FROM " + d_part_table +")");
      else
        d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_previews + " = json_set(" + d_mms_previews + ", '$[" + bepaald::toString(lp) + "].attachmentId', json(null)) WHERE "
                        "json_extract(" + d_mms_previews + ", '$[" + bepaald::toString(lp) + "].attachmentId.rowId') NOT IN (SELECT _id FROM " + d_part_table + ")");
      //d_database.print("SELECT " + d_mms_previews + " FROM " + d_mms_table + " WHERE " + d_mms_previews + " IS NOT NULL");
    }

    cleanAttachments();
    cleanDatabaseByMessages();
    return true;
  }

  // else replace attachments

  std::sort(replace.begin(), replace.end(), [](std::pair<std::string, std::string> const &lhs, std::pair<std::string, std::string> const &rhs)
  {
    return (lhs.first == "default" ? false : (rhs.first == "default" ? true : lhs.first.length() > rhs.first.length()));
  });

  for (unsigned int i = 0; i < res.rows(); ++i)
  {
    // get ct (mimetype), if it matches replace[i].first -> replace with replace[i].second
    std::string mimetype = res.valueAsString(i, d_part_ct);

    Logger::message("Checking to replace attachment: ", mimetype);

    for (unsigned int j = 0; j < replace.size(); ++j)
    {
      if (res.valueAsString(i, d_part_ct).find(replace[j].first) == 0 || replace[j].first == "default")
      {
        // replace with replace[j].second

        auto attachment = d_attachments.find({res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")});
        if (attachment == d_attachments.end())
        {
          Logger::warning("Failed to find attachment with this part entry");
          continue;
        }

        AttachmentMetadata amd = AttachmentMetadata::getAttachmentMetaData(replace[j].second);
        if (!amd)
        {
          Logger::message("Failed to get metadata on new attachment: \"", replace[j].second, "\", skipping...");
          continue;
        }

        if (!updatePartTableForReplace(amd, res.getValueAs<long long int>(i, "_id")))
          //if (!d_database.exec("UPDATE part SET ct = ?, data_size = ?, width = ?, height = ?, data_hash = ? WHERE _id = ?",
          //                  {newmimetype, newdatasize, newwidth, newheight, newhash, res.getValueAs<long long int>(i, "_id")}) ||
          //  d_database.changed() != 1)
          return false;

        attachment->second->setLength(amd.filesize);
        //attachment->second->setLazyDataRAW(amd.filesize, replace[j].second);
        attachment->second->setReader(new RawFileAttachmentReader(replace[j].second));

        Logger::message("Replaced attachment at ", i + 1, "/", res.rows(), " with file \"", replace[j].second, "\"");
        break;
      }
    }
    // replacing -> delete where _id = res[i][_id]
    //              insert into (_id, mid, unique_id, ct, data_size, quote, ...) values(..., ... ,)
  }

  return true;
}
