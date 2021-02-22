/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

// CALLED ON SOURCE

#include "signalbackup.ih"

void SignalBackup::makeIdsUnique(long long int minthread, long long int minsms, long long int minmms,
                                 long long int minpart, long long int minrecipient, long long int mingroups,
                                 long long int minidentities, long long int mingroup_receipts, long long int mindrafts,
                                 long long int minsticker, long long int minmegaphone, long long int minremapped_recipients,
                                 long long int minremapped_threads, long long int minmention)
{
  std::cout << __FUNCTION__ << std::endl;

  std::cout << "  Adjusting indexes in tables..." << std::endl;

  setMinimumId("thread", minthread);
  // compactIds("thread");  WE CAN NOT COMPACT THE THREAD TABLE! IT WILL BREAK THE FOLLOWING CODE
  d_database.exec("UPDATE sms SET thread_id = thread_id + ?", minthread);    // update sms.thread_id to new thread._id's
  d_database.exec("UPDATE mms SET thread_id = thread_id + ?", minthread);    // ""
  d_database.exec("UPDATE drafts SET thread_id = thread_id + ?", minthread); // ""
  if (d_database.containsTable("mention"))
    d_database.exec("UPDATE mention SET thread_id = thread_id + ?", minthread); // ""

  setMinimumId("sms",  minsms);
  compactIds("sms");

  // UPDATE t SET id = (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1) WHERE id = (SELECT MIN(id) FROM t WHERE id > (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1));

  setMinimumId("mms",  minmms);
  d_database.exec("UPDATE part SET mid = mid + ?", minmms); // update part.mid to new mms._id's
  d_database.exec("UPDATE group_receipts SET mms_id = mms_id + ?", minmms); // "
  if (d_database.containsTable("mention"))
    d_database.exec("UPDATE mention SET message_id = message_id + ?", minmms);
  compactIds("mms");

  setMinimumId("part", minpart);
  // update rowid's in attachments
  std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> newattdb;
  for (auto &att : d_attachments)
  {
    AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(att.second.release());
    a->setRowId(a->rowId() + minpart);
    newattdb.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
  }
  d_attachments.clear();
  d_attachments = std::move(newattdb);
  compactIds("part");

  //d_database.prettyPrint("SELECT _id, phone FROM recipient");
  setMinimumId((d_databaseversion < 24) ? "recipient_preferences" : "recipient", minrecipient);
  //d_database.prettyPrint("SELECT _id, phone FROM recipient");
  // compactIds((d_databaseversion < 27) ? "recipient_preferences" : "recipient"); WE CAN NOT COMPACT THE RECIPIENT TABLE! IT WILL BREAK THE FOLLOWING CODE
  if (d_databaseversion >= 24) // the _id is referenced in other tables, update
  {                            // them to link to the right identities.
    d_database.exec("UPDATE sms SET address = address + ?", minrecipient);
    d_database.exec("UPDATE mms SET address = address + ?", minrecipient);
    d_database.exec("UPDATE mms SET quote_author = quote_author + ?", minrecipient);
    d_database.exec("UPDATE sessions SET address = address + ?", minrecipient);
    d_database.exec("UPDATE group_receipts SET address = address + ?", minrecipient);
    d_database.exec("UPDATE thread SET recipient_ids = recipient_ids + ?", minrecipient);
    d_database.exec("UPDATE groups SET recipient_id = recipient_id + ?", minrecipient);
    // as of writing, remapped_recipients is a new (and currently unused?) table
    if (d_database.containsTable("remapped_recipients"))
    {
      d_database.exec("UPDATE remapped_recipients SET old_id = old_id + ?", minrecipient);
      d_database.exec("UPDATE remapped_recipients SET new_id = new_id + ?", minrecipient);
    }
    if (d_database.containsTable("mention"))
      d_database.exec("UPDATE mention SET recipient_id = recipient_id + ?", minrecipient);

    // address is UNIQUE in identities, so we can not simply do the following:
    // d_database.exec("UPDATE identities SET address = address + ?", minrecipient);
    // instead we do the complicated way:
    setMinimumId("identities", minrecipient, "address");

    // get group members:
    SqliteDB::QueryResults results;
    //std::cout << minrecipient << std::endl;
    d_database.exec("SELECT _id,members FROM groups", &results);
    //d_database.prettyPrint("SELECT _id,members FROM groups");
    for (uint i = 0; i < results.rows(); ++i)
    {
      long long int gid = results.getValueAs<long long int>(i, "_id");
      std::string membersstr = results.getValueAs<std::string>(i, "members");
      std::vector<int> membersvec;
      std::stringstream ss(membersstr);
      while (ss.good())
      {
        std::string substr;
        std::getline(ss, substr, ',');
        membersvec.emplace_back(bepaald::toNumber<int>(substr) + minrecipient);
      }

      std::string newmembers;
      for (uint m = 0; m < membersvec.size(); ++m)
        newmembers += (m == 0) ? bepaald::toString(membersvec[m]) : ("," + bepaald::toString(membersvec[m]));

      d_database.exec("UPDATE groups SET members = ? WHERE _id == ?", {newmembers, gid});
      //std::cout << d_database.changed() << std::endl;
    }
    //d_database.prettyPrint("SELECT _id,members FROM groups");

    // in groups, during the v1 -> v2 update, members may have been removed from the group, these messages
    // are of type "GV1_MIGRATION_TYPE" and have a body that looks like '_id,_id,...|_id,_id,_id,...' (I think, I have
    // not seen one with more than 1 id). These id_s must also be updated.
    if (d_database.exec("SELECT _id,body FROM sms WHERE type == ?", bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
    {
      //results.prettyPrint();
      for (uint i = 0; i < results.rows(); ++i)
      {
        if (results.valueHasType<std::string>(i, "body"))
        {
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
                int id = bepaald::toNumber<int>(tmp) + minrecipient;
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
        }
      }
    }

    // update reaction authors
    if (d_database.tableContainsColumn("sms", "reactions"))
    {
      d_database.exec("SELECT _id, reactions FROM sms WHERE reactions IS NOT NULL", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
        {
          //std::cout << "Updating reaction author (sms) : " << reactions.getAuthor(j) << "..." << std::endl;
          reactions.setAuthor(j, reactions.getAuthor(j) + minrecipient);
        }
        d_database.exec("UPDATE sms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())), results.getValueAs<long long int>(i, "_id")});
      }
    }
    if (d_database.tableContainsColumn("mms", "reactions"))
    {
      d_database.exec("SELECT _id, reactions FROM mms WHERE reactions IS NOT NULL", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
        {
          //std::cout << "Updating reaction author (mms) : " << reactions.getAuthor(j) << "..." << std::endl;
          reactions.setAuthor(j, reactions.getAuthor(j) + minrecipient);
        }
        d_database.exec("UPDATE mms SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())), results.getValueAs<long long int>(i, "_id")});
      }
    }
  }

  setMinimumId("groups", mingroups);
  compactIds("groups");

  setMinimumId("identities", minidentities);
  compactIds("identities");

  setMinimumId("group_receipts", mingroup_receipts);
  compactIds("group_receipts");

  setMinimumId("drafts", mindrafts);
  compactIds("drafts");

  setMinimumId("sticker", minsticker);
  compactIds("sticker");

  if (minmegaphone >= 0 && d_database.containsTable("megaphone"))
  {
    setMinimumId("megaphone", minmegaphone);
    compactIds("megaphone");
  }
  if (minremapped_recipients >= 0 && d_database.containsTable("remapped_recipients"))
  {
    setMinimumId("remapped_recipients", minremapped_recipients);
    compactIds("remapped_recipients");
  }
  if (minremapped_threads >= 0 && d_database.containsTable("remapped_threads"))
  {
    setMinimumId("remapped_threads", minremapped_threads);
    compactIds("remapped_threads");
  }
  if (minmention >= 0 && d_database.containsTable("mention"))
  {
    setMinimumId("mention", minmention);
    compactIds("mention");
  }
}
