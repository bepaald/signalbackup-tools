/*
    Copyright (C) 2020  Selwin van Dijk

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

void SignalBackup::updateRecipientId(long long int targetid, std::string ident)
{
  std::cout << __FUNCTION__ << std::endl;


  // CALLED ON SOURCE
  // the targetid should already be guaranteed to not exist in source as this is called
  // after makeIdsUnique() & friends

  if (d_databaseversion < 24) // recipient table does not exist
    return;

  // get the current (to be deleted) recipient._id for this identifier (=phone,group_id,possibly uuid)
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id FROM recipient WHERE COALESCE(phone,group_id) = '" + ident + "'", &results);

  if (results.rows() > 1)
  {
    std::cout << "ERROR! Unexpectedly got multiple results" << std::endl;
    return;
  }

  if (results.rows() == 0)
    return;

  long long int sourceid = results.getValueAs<long long int>(0, "_id");

  //std::cout << "Need to change id " << sourceid << " into " << targetid << " in all tables that reference it" << std::endl;

  d_database.exec("UPDATE sms SET address = ? WHERE address = ?", {targetid, sourceid});
  d_database.exec("UPDATE mms SET address = ? WHERE address = ?", {targetid, sourceid});
  d_database.exec("UPDATE mms SET quote_author = ? WHERE quote_author = ?", {targetid, sourceid});
  d_database.exec("UPDATE identities SET address = ? WHERE address = ?", {targetid, sourceid});
  d_database.exec("UPDATE group_receipts SET address = ? WHERE address = ?", {targetid, sourceid});
  d_database.exec("UPDATE thread SET recipient_ids = ? WHERE recipient_ids = ?", {targetid, sourceid});
  d_database.exec("UPDATE groups SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
  d_database.exec("UPDATE sessions SET address = ? WHERE address = ?", {targetid, sourceid});

  // get group members:
  SqliteDB::QueryResults results2;
  d_database.exec("SELECT _id,members FROM groups", &results2);
  //d_database.prettyPrint("SELECT _id,members FROM groups");
  for (uint i = 0; i < results2.rows(); ++i)
  {
    long long int gid = results2.getValueAs<long long int>(i, "_id");
    std::string membersstr = results2.getValueAs<std::string>(i, "members");
    std::vector<int> membersvec;
    std::stringstream ss(membersstr);
    while (ss.good())
    {
      std::string substr;
      std::getline(ss, substr, ',');
      membersvec.emplace_back(bepaald::toNumber<int>(substr));
    }

    std::string newmembers;
    for (uint m = 0; m < membersvec.size(); ++m)
      newmembers += (m == 0) ?
        bepaald::toString((membersvec[m] == sourceid) ? targetid : membersvec[m]) :
        ("," + bepaald::toString((membersvec[m] == sourceid) ? targetid : membersvec[m]));

    d_database.exec("UPDATE groups SET members = ? WHERE _id == ?", {newmembers, gid});
  }

  //d_database.prettyPrint("SELECT _id,members FROM groups");

}
