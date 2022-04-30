/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

/*

OLD VERSION

// CALLED ON SOURCE

void SignalBackup::makeIdsUnique(long long int minthread, long long int minsms, long long int minmms,
                                 long long int minpart, long long int minrecipient, long long int mingroups,
                                 long long int minidentities, long long int mingroup_receipts, long long int mindrafts,
                                 long long int minsticker, long long int minmegaphone, long long int minremapped_recipients,
                                 long long int minremapped_threads, long long int minmention,
                                 long long int minmsl_payload, long long int minmsl_message, long long int minmsl_recipient,
                                 long long int minreaction, long long int mingroup_call_ring,
                                 long long int minnotification_profile, long long int minnotification_profile_allowed_members,
                                 long long int minnotification_profile_schedule
                                 )
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
  if (d_database.containsTable("msl_message"))
    d_database.exec("UPDATE msl_message SET message_id = message_id + ? WHERE is_mms IS NOT 1", minsms);
  if (d_database.containsTable("reaction")) // dbv >= 121
    d_database.exec("UPDATE reaction SET message_id = message_id + ? WHERE is_mms IS NOT 1", minsms);
  compactIds("sms");

  // UPDATE t SET id = (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1) WHERE id = (SELECT MIN(id) FROM t WHERE id > (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1));

  setMinimumId("mms",  minmms);
  d_database.exec("UPDATE part SET mid = mid + ?", minmms); // update part.mid to new mms._id's
  d_database.exec("UPDATE group_receipts SET mms_id = mms_id + ?", minmms); // "
  if (d_database.containsTable("mention"))
    d_database.exec("UPDATE mention SET message_id = message_id + ?", minmms);
  if (d_database.containsTable("msl_message"))
    d_database.exec("UPDATE msl_message SET message_id = message_id + ? WHERE is_mms IS 1", minmms);
  if (d_database.containsTable("reaction")) // dbv >= 121
    d_database.exec("UPDATE reaction SET message_id = message_id + ? WHERE is_mms IS 1", minmms);
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

  // update rowid in previews (mms.previews contains a json string referencing the 'rowId' == part._id)
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id,previews FROM mms WHERE previews IS NOT NULL", &results);
  std::regex rowid_in_json(".*\"rowId\":([0-9]*)[,}].*");
  std::smatch sm;
  for (uint i = 0; i < results.rows(); ++i)
  {
    std::string line = results.valueAsString(i, "previews");
    //std::cout << " OLD: " << line << std::endl;
    if (std::regex_match(line, sm, rowid_in_json))
      if (sm.size() == 2) // 0 is full match, 1 is first submatch (which is what we want)
      {
        //std::cout << "MATCHED:" << std::endl;
        //std::cout << sm.size() << std::endl;
        line.replace(sm.position(1), sm.length(1), bepaald::toString(bepaald::toNumber<unsigned long>(sm[1]) + minpart));
        //std::cout << " NEW: " << line << std::endl;

        d_database.exec("UPDATE mms SET previews = ? WHERE _id = ?", {line, results.getValueAs<long long int>(i, "_id")});

      }
  }
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
    d_database.exec("UPDATE thread SET " + d_thread_recipient_id + " = " + d_thread_recipient_id + " + ?", minrecipient);
    d_database.exec("UPDATE groups SET recipient_id = recipient_id + ?", minrecipient);
    // as of writing, remapped_recipients is a new (and currently unused?) table
    if (d_database.containsTable("remapped_recipients"))
    {
      d_database.exec("UPDATE remapped_recipients SET old_id = old_id + ?", minrecipient);
      d_database.exec("UPDATE remapped_recipients SET new_id = new_id + ?", minrecipient);
    }
    if (d_database.containsTable("mention"))
      d_database.exec("UPDATE mention SET recipient_id = recipient_id + ?", minrecipient);

    if (d_database.containsTable("msl_recipient"))
      d_database.exec("UPDATE msl_recipient SET recipient_id = recipient_id + ?", minrecipient);

    if (d_database.containsTable("reaction")) // dbv >= 121
      d_database.exec("UPDATE reaction SET author_id = author_id + ?", minrecipient);

    if (d_database.containsTable("notfication_profile_allowed_members")) // dbv >= 121
      d_database.exec("UPDATE notfication_profile_allowed_members SET recipient_id = recipient_id + ?", minrecipient);

    // address is UNIQUE in identities, so we can not simply do the following:
    // d_database.exec("UPDATE identities SET address = address + ?", minrecipient);
    // instead we do the complicated way:
    setMinimumId("identities", minrecipient, "address");

    // get group members:
    //SqliteDB::QueryResults results;
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
            if (!std::isdigit(body[body_idx]) || body_idx >= body.length())
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

  if (minmsl_payload >= 0 && d_database.containsTable("msl_payload"))
  {
    setMinimumId("msl_payload", minmsl_payload);
    d_database.exec("UPDATE msl_recipient SET payload_id = payload_id + ?", minmsl_payload);
    d_database.exec("UPDATE msl_message SET payload_id = payload_id + ?", minmsl_payload);

    setMinimumId("msl_recipient", minmsl_recipient);
    compactIds("msl_recipient");

    setMinimumId("msl_message", minmsl_message);
    compactIds("msl_message");
  }

  if (mingroup_call_ring >= 0 && d_database.containsTable("group_call_ring"))
  {
    setMinimumId("group_call_ring", mingroup_call_ring);
    compactIds("group_call_ring");
  }

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

  if (minreaction >= 0 && d_database.containsTable("reaction")) // dbv >= 121
  {
    setMinimumId("reaction", minreaction);
    compactIds("reaction");
  }

  if (minnotification_profile >= 0 && d_database.containsTable("notification_profile"))
  {
    setMinimumId("notification_profile", minnotification_profile);
    d_database.exec("UPDATE notification_profile_allowed_members SET notification_profile_id = notification_profile_id + ?", minnotification_profile);
    d_database.exec("UPDATE notification_profile_schedule SET notification_profile_id = notification_profile_id + ?", minnotification_profile);

    setMinimumId("notification_profile_allowed_members", minnotification_profile_allowed_members);
    compactIds("notification_profile_allowed_members");

    setMinimumId("notification_profile_schedule", minnotification_profile_schedule);
    compactIds("notification_profile_schedule");
  }
}

*/

// (NEW VERSION) NOT CALLED ON SOURCE

void SignalBackup::makeIdsUnique(SignalBackup *source)
{
  std::cout << __FUNCTION__ << std::endl;

  std::cout << "  Adjusting indexes in tables..." << std::endl;

  for (auto const &dbl : d_databaselinks)
  {
    // skip if table/column does not exist, or if skip is set
    if ((dbl.flags & SKIP) ||
        !d_database.containsTable(dbl.table) ||
        !source->d_database.containsTable(dbl.table) ||
        !source->d_database.tableContainsColumn(dbl.table, dbl.column))
      continue;

    if (dbl.flags & WARN)
    {
      SqliteDB::QueryResults results;
      if (source->d_database.exec("SELECT * FROM " + dbl.table, &results) &&
          results.rows() > 0)
        std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " : Found entries in a usually empty table. Trying to deal with it, but problems may occur." << std::endl;
    }

    long long int offsetvalue = getMaxUsedId(dbl.table, dbl.column) + 1 - source->getMinUsedId(dbl.table, dbl.column);
    source->setMinimumId(dbl.table, offsetvalue, dbl.column);

    for (auto const &c : dbl.connections)
    {
      if (!source->d_database.containsTable(c.table) || !source->d_database.tableContainsColumn(c.table, c.column))
        continue;

      std::cout << "  Adjusting '" << c.table << "." << c.column << "' to match changes in '" << dbl.table << "'" << std::endl;

      if (!c.json_path.empty())
      {
        source->d_database.exec("UPDATE " + c.table + " SET " + c.column +
                                " = json_replace(" + c.column + ", " + c.json_path + ", json_extract(" + c.column + ", " + c.json_path + ") + ?) "
                                "WHERE json_extract(" + c.column + ", " + c.json_path + ") IS NOT NULL", offsetvalue);
      }
      else if ((c.flags & SET_UNIQUELY))
      {
        // set all values negative
        source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " * -1" +
                                (c.whereclause.empty() ? "" : " WHERE " + c.whereclause));
        // set to wanted value
        source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " * -1 + ?"
                                + (c.whereclause.empty() ? "" : " WHERE " + c.whereclause), offsetvalue);
      }
      else
        source->d_database.exec("UPDATE " + c.table + " SET " + c.column + " = " + c.column + " + ? "
                                + (c.whereclause.empty() ? "" : " WHERE " + c.whereclause), offsetvalue);
    }


    if (dbl.table == "part")
    {
      // update rowid's in attachments
      std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> newattdb;
      for (auto &att : source->d_attachments)
      {
        AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(att.second.release());
        a->setRowId(a->rowId() + offsetvalue);
        newattdb.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
      }
      source->d_attachments.clear();
      source->d_attachments = std::move(newattdb);

      /*
        REPLACED WITH JSON OPTION IN DatabaseConnections

        // update rowid in previews (mms.previews contains a json string referencing the 'rowId' == part._id)
      SqliteDB::QueryResults results;
      source->d_database.exec("SELECT _id,previews FROM mms WHERE previews IS NOT NULL", &results);
      std::regex rowid_in_json(".*\"rowId\":([0-9]*)[,}].*");
      std::smatch sm;
      for (uint i = 0; i < results.rows(); ++i)
      {
        std::string line = results.valueAsString(i, "previews");
        if (std::regex_match(line, sm, rowid_in_json))
          if (sm.size() == 2) // 0 is full match, 1 is first submatch (which is what we want)
          {
            line.replace(sm.position(1), sm.length(1), bepaald::toString(bepaald::toNumber<unsigned long>(sm[1]) + offsetvalue));
            source->d_database.exec("UPDATE mms SET previews = ? WHERE _id = ?", {line, results.getValueAs<long long int>(i, "_id")});
          }
      }
      */
    }

    if (dbl.table == "recipient")
    {
      // using namespace std::string_literals;

      // // update groups.members & groups.former_v1_members
      // for (auto const &members : {"members", "former_v1_members"})
      // {
      //   if (!source->d_database.tableContainsColumn("recipient", members))
      //     continue;

      //   // get group members:
      //   SqliteDB::QueryResults results;
      //   source->d_database.exec("SELECT _id,"s + members + " FROM groups WHERE " + members + " IS NOT NULL", &results);
      //   //source->d_database.prettyPrint("SELECT _id,members FROM groups");
      //   for (uint i = 0; i < results.rows(); ++i)
      //   {
      //     long long int gid = results.getValueAs<long long int>(i, "_id");
      //     std::string membersstr = results.getValueAs<std::string>(i, members);
      //     std::vector<int> membersvec;
      //     std::stringstream ss(membersstr);
      //     while (ss.good())
      //     {
      //       std::string substr;
      //       std::getline(ss, substr, ',');
      //       membersvec.emplace_back(bepaald::toNumber<int>(substr) + offsetvalue);
      //     }

      //     std::string newmembers;
      //     for (uint m = 0; m < membersvec.size(); ++m)
      //       newmembers += (m == 0) ? bepaald::toString(membersvec[m]) : ("," + bepaald::toString(membersvec[m]));

      //     source->d_database.exec("UPDATE groups SET "s + members + " = ? WHERE _id == ?", {newmembers, gid});
      //     //std::cout << source->d_database.changed() << std::endl;
      //   }
      // }
      source->updateGroupMembers(offsetvalue);

      // in groups, during the v1 -> v2 update, members may have been removed from the group, these messages
      // are of type "GV1_MIGRATION_TYPE" and have a body that looks like '_id,_id,...|_id,_id,_id,...' (I think, I have
      // not seen one with more than 1 id). These id_s must also be updated.
      // SqliteDB::QueryResults results;
      // if (source->d_database.exec("SELECT _id,body FROM sms WHERE type == ? AND body IS NOT NULL",
      //                             bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
      // {
      //   //results.prettyPrint();
      //   for (uint i = 0; i < results.rows(); ++i)
      //   {
      //     if (results.valueHasType<std::string>(i, "body"))
      //     {
      //       //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;
      //       std::string body = results.getValueAs<std::string>(i, "body");
      //       std::string output;
      //       std::string tmp; // to hold part of number while reading
      //       unsigned int body_idx = 0;
      //       while (true)
      //       {
      //         if (!std::isdigit(body[body_idx]) || body_idx >= body.length())
      //         {
      //           // deal with any number we have
      //           if (tmp.size())
      //           {
      //             int id = bepaald::toNumber<int>(tmp) + offsetvalue;
      //             output += bepaald::toString(id);
      //             tmp.clear();
      //           }
      //           // add non-digit-char
      //           if (body_idx < body.length())
      //             output += body[body_idx];
      //         }
      //         else
      //           tmp += body[body_idx];
      //         ++body_idx;
      //         if (body_idx > body.length())
      //           break;
      //       }
      //       //std::cout << output << std::endl;
      //       long long int sms_id = results.getValueAs<long long int>(i, "_id");
      //       source->d_database.exec("UPDATE sms SET body = ? WHERE _id == ?", {output, sms_id});
      //     }
      //   }
      // }
      source->updateGV1MigrationMessage(offsetvalue);

      //update (old-style)reaction authors
      // for (auto const &msgtable : {"sms", "mms"})
      // {
      //   if (source->d_database.tableContainsColumn(msgtable, "reactions"))
      //   {
      //     source->d_database.exec("SELECT _id, reactions FROM "s + msgtable + " WHERE reactions IS NOT NULL", &results);
      //     for (uint i = 0; i < results.rows(); ++i)
      //     {
      //       ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
      //       for (uint j = 0; j < reactions.numReactions(); ++j)
      //       {
      //         //std::cout << "Updating reaction author (" << msgtable << ") : " << reactions.getAuthor(j) << "..." << std::endl;
      //         reactions.setAuthor(j, reactions.getAuthor(j) + offsetvalue);
      //       }
      //       source->d_database.exec("UPDATE "s + msgtable + " SET reactions = ? WHERE _id = ?",
      //                               {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
      //                                results.getValueAs<long long int>(i, "_id")});
      //     }
      //   }
      // }
      source->updateReactionAuthors(offsetvalue);
      /*
      if (source->d_database.tableContainsColumn("mms", "reactions"))
      {
        source->d_database.exec("SELECT _id, reactions FROM mms WHERE reactions IS NOT NULL", &results);
        for (uint i = 0; i < results.rows(); ++i)
        {
          ReactionList reactions(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
          for (uint j = 0; j < reactions.numReactions(); ++j)
          {
            //std::cout << "Updating reaction author (mms) : " << reactions.getAuthor(j) << "..." << std::endl;
            reactions.setAuthor(j, reactions.getAuthor(j) + offsetvalue);
          }
          source->d_database.exec("UPDATE mms SET reactions = ? WHERE _id = ?",
                                  {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
                                   results.getValueAs<long long int>(i, "_id")});
        }
      }
      */
    }


    // compact table if requested
    if (!(dbl.flags & NO_COMPACT))
      source->compactIds(dbl.table, dbl.column);
  }

  /*

    CHECK!

    These are the tables that are imported by importThread(),
    check if they are all handled properly

  */

  // get tables
  std::string q("SELECT sql, name, type FROM sqlite_master");
  SqliteDB::QueryResults results;
  source->d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.valueHasType<std::nullptr_t>(i, 0))
    {
      //std::cout << "Dealing with: " << results.getValueAs<std::string>(i, 1) << std::endl;
      if (results.valueHasType<std::string>(i, 1) &&
          (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
           results.getValueAs<std::string>(i, 1).find("sms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is sms_ftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "mms_fts" &&
                results.getValueAs<std::string>(i, 1).find("mms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is mms_ftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "emoji_search" &&
                results.getValueAs<std::string>(i, 1).find("emoji_search") == 0))
        ;//std::cout << "Skipping " << results.getValueAs<std::string>(i, 1) << " because it is emoji_search_ftssecrettable" << std::endl;
      else
        if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
          tables.emplace_back(results.getValueAs<std::string>(i, 1));
    }
  }

  for (std::string const &table : tables)
  {
    if (table == "signed_prekeys" ||
        table == "one_time_prekeys" ||
        table == "sessions" ||
        //table == "job_spec" ||           // this is in the official export. But it makes testing more difficult. it
        //table == "constraint_spec" ||    // should be ok to export these (if present in source), since we are only
        //table == "dependency_spec" ||    // dealing with exported backups (not from live installations) -> they should
        //table == "emoji_search" ||       // have been excluded + the official import should be able to deal with them
        STRING_STARTS_WITH(table, "sms_fts") ||
        STRING_STARTS_WITH(table, "mms_fts") ||
        STRING_STARTS_WITH(table, "sqlite_"))
      continue;

    if (std::find_if(d_databaselinks.begin(), d_databaselinks.end(), [table](DatabaseLink const &d){ return d.table == table; }) == d_databaselinks.end())
      std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " : Found table unhandled by " << __FUNCTION__  << " : " << table << std::endl;
  }
}
