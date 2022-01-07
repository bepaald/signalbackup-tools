/*
    Copyright (C) 2021-2022  Selwin van Dijk

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
                                     std::vector<std::pair<std::string, std::string>> const &replace)
{
  using namespace std::string_literals;

  std::string query_delete("DELETE FROM part");
  std::string query_list("SELECT _id,mid,unique_id,ct FROM part");

  std::string specification;

  if (!threadids.empty() && threadids[0] != -1) // -1 indicates 'ALL', easier not to specify
  {
    if (specification.empty())
      specification += " WHERE mid IN (SELECT _id FROM mms WHERE ";
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
        specification += " WHERE mid IN (SELECT _id FROM mms WHERE ";
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
        specification += " WHERE mid IN (SELECT _id FROM mms WHERE ";
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
          if (!d_database.exec("UPDATE mms SET body = body || ? WHERE _id = ? AND (body IS NOT NULL AND body != '')", {"\n\n" + append, res.getValueAs<long long int>(i, "mid")}))
            return false;
          if (!d_database.exec("UPDATE mms SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')", {append, res.getValueAs<long long int>(i, "mid")}))
            return false;
        }
        if (!prepend.empty())
        {
          if (!d_database.exec("UPDATE mms SET body = ? || body WHERE _id = ? AND (body IS NOT NULL AND body != '')", {prepend + "\n\n", res.getValueAs<long long int>(i, "mid")}))
            return false;
          if (!d_database.exec("UPDATE mms SET body = ? WHERE _id = ? AND (body IS NULL OR body == '')", {prepend, res.getValueAs<long long int>(i, "mid")}))
            return false;
        }
      }
      else // delete message if body empty
      {
        if (!d_database.exec("SELECT _id FROM part WHERE mid = ?", res.getValueAs<long long int>(i, "mid"), &res2))
          return false;

        if (res2.empty()) // no other attachments for this message
          if (!d_database.exec("DELETE FROM mms WHERE _id = ? AND (body IS NULL OR body == '')", res.getValueAs<long long int>(i, "mid")))
            return false;
      }
    }
    cleanAttachments();
    cleanDatabaseByMessages();
    return true;
  }

  // else replace attachments

  for (uint i = 0; i < res.rows(); ++i)
  {
    // get ct (mimetype), if it matches replace[i].first -> replace with replace[i].second

    // replacing -> delete where _id = res[i][_id]
    //              insert into (_id, mid, unique_id, ct, data_size, ...) values(..., ... ,)
  }

  return false;
}
