/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

bool SignalBackup::deleteAttachments(std::vector<long long int> const &threadids,
                                     std::string const &before, std::string const &after,
                                     long long int filesize, std::vector<std::string> const &mimetypes,
                                     std::string const &append, std::string const &prepend,
                                     std::vector<std::pair<std::string, std::string>> replace)
{
  using namespace std::string_literals;

  std::string query_delete("DELETE FROM part");
  std::string query_list("SELECT _id,mid,unique_id,ct,quote FROM part");

  std::string specification;

  if (!threadids.empty() && threadids[0] != -1) // -1 indicates 'ALL', easier not to specify
  {
    if (specification.empty())
      specification += " WHERE mid IN (SELECT _id FROM " + d_mms_table + " WHERE ";
    else
      specification += " AND ";

    for (uint i = 0; i < threadids.size(); ++i)
      specification += ((i == 0) ? "("s : ""s) + "thread_id IS " + bepaald::toString(threadids[i]) + ((i == threadids.size() - 1) ? ")" : " OR ");
  }

  if (!before.empty())
  {
    long long int date = dateToMSecsSinceEpoch(before);
    if (date == -1)
      std::cout << "Error: Ignoring before-date: '" << before << "'. Failed to parse or invalid." << std::endl;
    else
    {
      if (specification.empty())
        specification += " WHERE mid IN (SELECT _id FROM " + d_mms_table + " WHERE ";
      else
        specification += " AND ";
      specification += "date_received < " + bepaald::toString(date);
    }
  }

  if (!after.empty())
  {
    long long int date = dateToMSecsSinceEpoch(after);
    if (date == -1)
      std::cout << "Error: Ignoring after-date: '" << after << "'. Failed to parse or invalid." << std::endl;
    else
    {
      if (specification.empty())
        specification += " WHERE mid IN (SELECT _id FROM " + d_mms_table + " WHERE ";
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
    for (uint i = 0; i < mimetypes.size(); ++i)
      specification += ((i == 0) ? "("s : ""s) + "ct LIKE \"" + mimetypes[i] + "%\"" + ((i == mimetypes.size() - 1) ? ")" : " OR ");
  }

  query_delete += specification;
  query_list += specification;
  //std::cout << query_list << std::endl;

  SqliteDB::QueryResults res;
  if (!d_database.exec(query_list, &res))
  {
    std::cout << "Failed to execute query." << std::endl;
    return false;
  }

  //res.prettyPrint();

  // just delete the attachments
  if (replace.empty())
  {

    if (!d_database.exec(query_delete))
    {
      std::cout << "Failed to execute query" << std::endl;
      return false;
    }
    std::cout << "Deleted: " << d_database.changed() << " 'part'-entries." << std::endl;

    // find all mms entries to which the deleted attachments belonged
    // if no attachments remain and body is empty -> delete mms -> cleanDatabaseByMessages()
    SqliteDB::QueryResults res2;
    for (uint i = 0; i < res.rows(); ++i)
    {
      if (!append.empty() || !prepend.empty())
      {
        // update existing message bodies
        if (!append.empty())
        {
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = body || ? WHERE _id = ? AND (body IS NOT NULL AND body != '')", {"\n\n" + append, res.getValueAs<long long int>(i, "mid")}))
            return false;
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')", {append, res.getValueAs<long long int>(i, "mid")}))
            return false;
        }
        if (!prepend.empty())
        {
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? || body WHERE _id = ? AND (body IS NOT NULL AND body != '')", {prepend + "\n\n", res.getValueAs<long long int>(i, "mid")}))
            return false;
          if (!d_database.exec("UPDATE " + d_mms_table + " SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')", {prepend, res.getValueAs<long long int>(i, "mid")}))
            return false;
        }
      }
      else // delete message if body empty
      {
        if (!d_database.exec("SELECT _id FROM part WHERE mid = ?", res.getValueAs<long long int>(i, "mid"), &res2))
          return false;

        if (res2.empty()) // no other attachments for this message
        {
          // delete message if body empty
          if (!d_database.exec("DELETE FROM " + d_mms_table + " WHERE _id = ? AND (body IS NULL OR body == '')", res.getValueAs<long long int>(i, "mid")))
            return false;
          // else, if body is not empty, make sure the 'mms.previews' column does not refeerence non-existing attachment
          else
          {
            SqliteDB::QueryResults res3;
            // rewrite this with sqlites json_extract
            if (!d_database.exec("SELECT " + d_mms_previews + " FROM " + d_mms_table + " WHERE _id = ? AND (" + d_mms_previews + " LIKE '%attachmentId\":{%')", res.getValueAs<long long int>(i, "mid"), &res3))
              return false;
            if (!res3.empty())
            {
              std::string previews = res3.valueAsString(0, d_mms_previews);
              //std::cout << " OLD: " << previews<< std::endl;
              std::regex attid_in_json(".*\"attachmentId\":(\\{.*?\\}).*");
              std::smatch sm;
              if (std::regex_match(previews, sm, attid_in_json))
              {
                if (sm.size() == 2) // 0 is full match, 1 is first submatch (which is what we want)
                {
                  //std::cout << sm.size() << std::endl;
                  previews.replace(sm.position(1), sm.length(1), "null");
                  //std::cout << " NEW: " << previews << std::endl;
                  d_database.exec("UPDATE " + d_mms_table + " SET " + d_mms_previews + " = ? WHERE _id = ?", {previews, res.getValueAs<long long int>(i, "mid")});
                }
              }
            }
          }
        }
      }
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

  for (uint i = 0; i < res.rows(); ++i)
  {
    // get ct (mimetype), if it matches replace[i].first -> replace with replace[i].second
    std::string mimetype = res.valueAsString(i, "ct");

    std::cout << "Checking to replace attachment: "  << mimetype << std::endl;

    for (uint j = 0; j < replace.size(); ++j)
    {
      if (res.valueAsString(i, "ct").find(replace[j].first) == 0 || replace[j].first == "default")
      {
        // replace with replace[j].second

        auto attachment = d_attachments.find({res.getValueAs<long long int>(i, "_id"), res.getValueAs<long long int>(i, "unique_id")});
        if (attachment == d_attachments.end())
        {
          std::cout << "WARNING: Failed to find attachment with this part entry" << std::endl;
          continue;
        }

        AttachmentMetadata amd = getAttachmentMetaData(replace[j].second);
        if (!amd)
        {
          std::cout << "Failed to get metadata on new attachment: \"" << replace[j].second << "\", skipping..." << std::endl;
          continue;
        }

        if (!updatePartTableForReplace(amd, res.getValueAs<long long int>(i, "_id")))
          //if (!d_database.exec("UPDATE part SET ct = ?, data_size = ?, width = ?, height = ?, data_hash = ? WHERE _id = ?",
          //                  {newmimetype, newdatasize, newwidth, newheight, newhash, res.getValueAs<long long int>(i, "_id")}) ||
          //  d_database.changed() != 1)
          return false;

        attachment->second->setLength(amd.filesize);
        attachment->second->setLazyDataRAW(amd.filesize, replace[j].second);

        std::cout << "Replaced attachment at " << i + 1 << "/" << res.rows() << " with file \"" << replace[j].second << "\"" << std::endl;
        break;
      }
    }
    // replacing -> delete where _id = res[i][_id]
    //              insert into (_id, mid, unique_id, ct, data_size, quote, ...) values(..., ... ,)
  }

  return true;
}
