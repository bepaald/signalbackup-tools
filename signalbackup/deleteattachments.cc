/*
    Copyright (C) 2021  Selwin van Dijk

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
                       long long int filesize, std::vector<std::string> const &mimetypes) const
{
  using namespace std::string_literals;

  std::string query_delete("DELETE FROM part");
  std::string query_list("SELECT _id,unique_id FROM part");

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

  std::cout << query_list << std::endl;
  d_database.print(query_list);

  // REPLACE INTO part(_id, column1,column2,...) VALUES(_id, val1,val2) ?

  return false;
}
