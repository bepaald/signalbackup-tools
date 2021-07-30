/*
    Copyright (C) 2020-2021  Selwin van Dijk

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

void SignalBackup::updateRecipientId(long long int targetid, long long int sourceid, bool verbose)
{
  std::cout << "  Mapping " << sourceid << " -> " << targetid << std::endl;

  d_database.exec("UPDATE sms SET address = ? WHERE address = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update sms, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE mms SET address = ? WHERE address = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update mms, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE mms SET quote_author = ? WHERE quote_author = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update mms.quote_author, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE identities SET address = ? WHERE address = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update identities, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE group_receipts SET address = ? WHERE address = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update group_receipts, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE thread SET " + d_thread_recipient_id + " = ? WHERE " + d_thread_recipient_id + " = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update thread, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE groups SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update groups, changed: " << d_database.changed() << std::endl;
  d_database.exec("UPDATE sessions SET address = ? WHERE address = ?", {targetid, sourceid});
  if (verbose) std::cout << "    update sessions, changed: " << d_database.changed() << std::endl;
  if (d_database.containsTable("remapped_recipients"))
  {
    d_database.exec("UPDATE remapped_recipients SET old_id = ? WHERE old_id = ?", {targetid, sourceid});
    if (verbose) std::cout << "    update remapped_recipients.old_id, changed: " << d_database.changed() << std::endl;
    d_database.exec("UPDATE remapped_recipients SET new_id = ? WHERE new_id = ?", {targetid, sourceid});
    if (verbose) std::cout << "    update remapped_recipient.new_id, changed: " << d_database.changed() << std::endl;
  }
  if (d_database.containsTable("mention"))
  {
    d_database.exec("UPDATE mention SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
    if (verbose) std::cout << "    update mention, changed: " << d_database.changed() << std::endl;
  }

  // recipient_id can also mentioned in the body of group v1 -> v2 migration message, when recipient
  // was thrown out of group.
  SqliteDB::QueryResults results;
  if (d_database.exec("SELECT _id,body FROM sms WHERE type == ?", bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
  {
    //results.prettyPrint();
    for (uint i = 0; i < results.rows(); ++i)
    {
      if (results.valueHasType<std::string>(i, "body"))
      {
        //std::cout << " ** FROM TO **" << std::endl;
        //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;
        std::string body = results.getValueAs<std::string>(i, "body");
        std::string output;
        std::string tmp; // to hold part of number while reading
        unsigned int body_idx = 0;
        while (true)
        {
          if (!std::isdigit(body[body_idx]) || i >= body.length())
          {
            // deal with any number we have
            if (tmp.size())
            {
              int id = bepaald::toNumber<int>(tmp);
              if (id == sourceid)
              {
                id = targetid;
                if (verbose) std::cout << "    updated gv1_migration message" << std::endl;
              }
              output += bepaald::toString(id);
              tmp.clear();
            }
            // add non-digit-char
            if (body_idx < body.length())
              output += body[body_idx];
          }
          else
            tmp += body[body_idx];
          ++body_idx;
          if (body_idx > body.length())
            break;
        }
        //std::cout << output << std::endl;
        long long int sms_id = results.getValueAs<long long int>(i, "_id");
        d_database.exec("UPDATE sms SET body = ? WHERE _id == ?", {output, sms_id});
        if (verbose) std::cout << "    update sms.body (GV1_MAGRATION), changed: " << d_database.changed() << std::endl;
      }
    }
  }


  // get group members:
  SqliteDB::QueryResults results2;
  d_database.exec("SELECT _id, members FROM groups", &results2);
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
    if (verbose) std::cout << "    update groups.members, changed: " << membersstr << " -> " << newmembers << std::endl;
  }

  //d_database.prettyPrint("SELECT _id,members FROM groups");

  // UPDATE 'reactions' field in sms and mms tables....
  if (d_database.tableContainsColumn("sms", "reactions"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT _id, reactions FROM sms WHERE reactions IS NOT NULL", &res);
    for (uint i = 0; i < res.rows(); ++i)
    {
      bool changed = false;
      ReactionList reactions(res.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
      for (uint j = 0; j < reactions.numReactions(); ++j)
      {
        //std::cout << "Dealing with SMS reaction author: " << reactions.getAuthor(j) << std::endl;
        if (reactions.getAuthor(j) == static_cast<uint64_t>(sourceid))
        {
          reactions.setAuthor(j, targetid);
          changed = true;
          if (verbose) std::cout << "    updated sms reaction" << std::endl;
        }
      }
      if (changed)
        d_database.exec("UPDATE sms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
                                                                       res.getValueAs<long long int>(i, "_id")});
    }
  }
  if (d_database.tableContainsColumn("mms", "reactions"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT _id, reactions FROM mms WHERE reactions IS NOT NULL", &res);
    for (uint i = 0; i < res.rows(); ++i)
    {
      bool changed = false;
      ReactionList reactions(res.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
      for (uint j = 0; j < reactions.numReactions(); ++j)
      {
        //std::cout << "Dealing with MMS reaction author: " << reactions.getAuthor(j) << std::endl;
        if (reactions.getAuthor(j) == static_cast<uint64_t>(sourceid))
        {
          reactions.setAuthor(j, targetid);
          changed = true;
          if (verbose) std::cout << "    updated mms reaction" << std::endl;
        }
      }
      if (changed)
        d_database.exec("UPDATE mms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
                                                                       res.getValueAs<long long int>(i, "_id")});
    }
  }
}

void SignalBackup::updateRecipientId(long long int targetid, std::string ident)
{
  //std::cout << __FUNCTION__ << std::endl;

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

  // the target recipient was not found in this source db, nothing to do.
  if (results.rows() == 0)
    return;

  long long int sourceid = results.getValueAs<long long int>(0, "_id");

  //std::cout << "  Mapping " << sourceid << " -> " << targetid << " (" << ident << ")" << std::endl;

  updateRecipientId(targetid, sourceid, false);

}
