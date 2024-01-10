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

bool SignalBackup::findRecipient(long long int id) const
{
  for (auto const &dbl : s_databaselinks)
  {
    if (dbl.table != "recipient")
      continue;

    if (!d_database.containsTable(dbl.table)) [[unlikely]]
      continue;

    for (auto const &c : dbl.connections)
    {
      if (d_database.containsTable(c.table) && d_database.tableContainsColumn(c.table, c.column))
      {
        SqliteDB::QueryResults res;
        if (!d_database.exec("SELECT COUNT(*) AS 'count' FROM " + c.table + " WHERE " + c.column + " IS ?",
                             id, &res) ||
            res.rows() != 1)
        {
          std::cout << "Error querying database." << std::endl;
          return false;
        }
        long long int count = res.getValueAs<long long int>(0, "count");
        if (count)
        {
          std::cout << "Found recipient " << id << " referenced in '"
                    << c.table << "." << c.column << "' (" << count << " times)" << std::endl;
        }
      }
    }
  }

  // get phone and uuid
  std::string uuid;
  std::string phone;
  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT " + d_recipient_e164 + ", " + d_recipient_aci + " FROM recipient WHERE _id = ?", id, &res) ||
      res.rows() != 1)
    return false;
  uuid = res(d_recipient_aci);
  phone = res(d_recipient_e164);

  // check in identities
  if (d_database.containsTable("identities") &&
      d_database.exec("SELECT COUNT(*) AS 'count' FROM identities WHERE address = ? OR address = ?", {uuid, phone}, &res) &&
      res.rows() == 1)
  {
    std::cout << "Found recipient " << id << " referenced in 'identities.address' (by uuid/phone, "
              << res.getValueAs<long long int>(0, "count") << " times)" << std::endl;
  }

  // check in quote mentions
  if (!uuid.empty())
  {
    int count = 0;
    if (d_database.exec("SELECT DISTINCT quote_mentions FROM " + d_mms_table + " WHERE quote_mentions IS NOT NULL", &res))
    {
      for (uint i = 0; i < res.rows(); ++i)
      {
        auto brdata = res.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "quote_mentions");
        BodyRanges brsproto(brdata);
        auto brs = brsproto.getField<1>();
        for (auto const &br : brs)
        {
          std::string mentionuuid = br.getField<3>().value_or(std::string());
          if (mentionuuid == uuid)
            ++count;
        }
      }
    }
    if (count)
      std::cout << "Found recipient " << id << " referenced in '" << d_mms_table << ".quote_mentions' (by uuid, "
                << count << " times)" << std::endl;
  }

  // check in group updates

  // now check the other thigns (uuid in group updates?)


  return false;
}
