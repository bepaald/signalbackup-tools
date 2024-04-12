/*
  Copyright (C) 2020-2024  Selwin van Dijk

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

void SignalBackup::updateRecipientId(long long int targetid, long long int sourceid)
{
  Logger::message_start("  Mapping ", sourceid, " -> ", targetid);
  if (d_database.tableContainsColumn("recipient", d_recipient_aci, d_recipient_e164, "group_id", "notification_channel", "distribution_list_id"))
  {
    SqliteDB::QueryResults r;
    if (d_database.exec("SELECT CASE WHEN NULLIF(" + d_recipient_aci + ", '') IS NULL THEN '' ELSE 'u' END || "
                        "CASE WHEN NULLIF(" + d_recipient_e164 + ", '') IS NULL THEN '' ELSE 'p' END || "
                        "CASE WHEN NULLIF(group_id, '') IS NULL THEN '' ELSE 'g' END || "
                        "CASE WHEN NULLIF(distribution_list_id, '') IS NULL THEN '' ELSE 'd' END || "
                        "CASE WHEN NULLIF(notification_channel, '') IS NULL THEN '' ELSE 'n' END "
                        "AS recipient_type FROM recipient", &r))
      Logger::message_continue(" (", r.valueAsString(0, "recipient_type"), ")");
  }
  Logger::message_end();

  for (auto const &dbl : s_databaselinks)
  {
    if (dbl.table != "recipient")
      continue;

    if (!d_database.containsTable(dbl.table)) [[unlikely]]
      continue;

    for (auto const &c : dbl.connections)
      if (d_databaseversion >= c.mindbvversion && d_databaseversion <= c.maxdbvversion &&
          d_database.containsTable(c.table) && d_database.tableContainsColumn(c.table, c.column))
      {
        d_database.exec("UPDATE " + c.table + " SET " + c.column + " = ? WHERE " + c.column + " = ?", {targetid, sourceid});
        if (d_verbose)
          Logger::message("    update table '" + c.table + "', changed: ", d_database.changed());
      }
  }
  updateGV1MigrationMessage(sourceid, targetid);
  updateGroupMembers(sourceid, targetid);
  updateReactionAuthors(sourceid, targetid);
  updateAvatars(sourceid, targetid);
}

// // OLD VERSION
// void SignalBackup::updateRecipientId(long long int targetid, long long int sourceid, bool verbose)
// {
//   using namespace std::string_literals;

//   std::cout << "  Mapping " << sourceid << " -> " << targetid << std::endl;

//   d_database.exec("UPDATE sms SET address = ? WHERE address = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update sms, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE mms SET address = ? WHERE address = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update mms, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE mms SET quote_author = ? WHERE quote_author = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update mms.quote_author, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE identities SET address = ? WHERE address = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update identities, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE group_receipts SET address = ? WHERE address = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update group_receipts, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE thread SET " + d_thread_recipient_id + " = ? WHERE " + d_thread_recipient_id + " = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update thread, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE groups SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update groups, changed: " << d_database.changed() << std::endl;
//   d_database.exec("UPDATE sessions SET address = ? WHERE address = ?", {targetid, sourceid});
//   if (verbose) std::cout << "    update sessions, changed: " << d_database.changed() << std::endl;
//   // if (d_database.containsTable("remapped_recipients"))
//   // {
//   //   d_database.exec("UPDATE remapped_recipients SET old_id = ? WHERE old_id = ?", {targetid, sourceid});
//   //   if (verbose) std::cout << "    update remapped_recipients.old_id, changed: " << d_database.changed() << std::endl;
//   //   d_database.exec("UPDATE remapped_recipients SET new_id = ? WHERE new_id = ?", {targetid, sourceid});
//   //   if (verbose) std::cout << "    update remapped_recipient.new_id, changed: " << d_database.changed() << std::endl;
//   // }
//   if (d_database.containsTable("mention"))
//   {
//     d_database.exec("UPDATE mention SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
//     if (verbose) std::cout << "    update mention, changed: " << d_database.changed() << std::endl;
//   }
//   if (d_database.containsTable("msl_recipient"))
//   {
//     d_database.exec("UPDATE msl_recipient SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
//     if (verbose) std::cout << "    update msl_recipient, changed: " << d_database.changed() << std::endl;
//   }
//   if (d_database.containsTable("reaction")) // dbv >= 121
//   {
//     d_database.exec("UPDATE reaction SET author_id = ? WHERE author_id = ?", {targetid, sourceid});
//     if (verbose) std::cout << "    update reaction, changed: " << d_database.changed() << std::endl;
//   }
//   if (d_database.containsTable("notification_profile_allowed_members")) // dbv >= 121
//   {
//     d_database.exec("UPDATE notification_profile_allowed_members SET recipient_id = ? WHERE recipient_id = ?", {targetid, sourceid});
//     if (verbose) std::cout << "    update notification_profile_allowed_members, changed: " << d_database.changed() << std::endl;
//   }

//   // recipient_id can also mentioned in the body of group v1 -> v2 migration message, when recipient
//   // was thrown out of group.
//   SqliteDB::QueryResults results;
//   bool changedsomething = false;
//   if (d_database.exec("SELECT _id,body FROM sms WHERE type == ?", bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
//   {
//     //results.prettyPrint();
//     for (uint i = 0; i < results.rows(); ++i)
//     {
//       if (results.valueHasType<std::string>(i, "body"))
//       {
//         //std::cout << " ** FROM TO **" << std::endl;
//         //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;
//         std::string body = results.getValueAs<std::string>(i, "body");
//         std::string output;
//         std::string tmp; // to hold part of number while reading
//         unsigned int body_idx = 0;
//         while (true)
//         {
//           if (!std::isdigit(body[body_idx]) || body_idx >= body.length())
//           {
//             // deal with any number we have
//             if (tmp.size())
//             {
//               int id = bepaald::toNumber<int>(tmp);
//               if (id == sourceid)
//               {
//                 id = targetid;
//                 if (verbose) std::cout << "    updated gv1_migration message" << std::endl;
//               }
//               output += bepaald::toString(id);
//               tmp.clear();
//             }
//             // add non-digit-char
//             if (body_idx < body.length())
//               output += body[body_idx];
//           }
//           else
//             tmp += body[body_idx];
//           ++body_idx;
//           if (body_idx > body.length())
//             break;
//         }
//         //std::cout << output << std::endl;
//         if (body != output)
//         {
//           long long int sms_id = results.getValueAs<long long int>(i, "_id");
//           d_database.exec("UPDATE sms SET body = ? WHERE _id == ?", {output, sms_id});
//           changedsomething = true;
//           if (verbose) std::cout << "    update sms.body (GV1_MIGRATION), changed: " << d_database.changed() << std::endl;
//         }
//       }
//     }
//     if (!changedsomething)
//       if (verbose) std::cout << "    update sms.body (GV1_MIGRATION), changed: 0" << std::endl;
//   }


//   // get group members:
//   for (auto const &members : {"members"s, d_groups_v1_members})
//   {

//     if (!d_database.tableContainsColumn("recipient", members))
//       continue;

//     SqliteDB::QueryResults results2;
//     changedsomething = false;
//     d_database.exec("SELECT _id, "s + members + " FROM groups WHERE " + members + " IS NOT NULL", &results2);
//     //std::cout << "RESULTS:" << std::endl;
//     //results2.prettyPrint();
//     for (uint i = 0; i < results2.rows(); ++i)
//     {
//       long long int gid = results2.getValueAs<long long int>(i, "_id");
//       std::string membersstr = results2.getValueAs<std::string>(i, members);
//       std::vector<int> membersvec;
//       std::stringstream ss(membersstr);
//       while (ss.good())
//       {
//         std::string substr;
//         std::getline(ss, substr, ',');
//         membersvec.emplace_back(bepaald::toNumber<int>(substr));
//       }

//       std::string newmembers;
//       for (uint m = 0; m < membersvec.size(); ++m)
//         newmembers += (m == 0) ?
//           bepaald::toString((membersvec[m] == sourceid) ? targetid : membersvec[m]) :
//           ("," + bepaald::toString((membersvec[m] == sourceid) ? targetid : membersvec[m]));

//       if (membersstr != newmembers)
//       {
//         changedsomething = true;
//         d_database.exec("UPDATE groups SET "s + members + " = ? WHERE _id == ?", {newmembers, gid});
//         if (verbose) std::cout << "    update groups." << members << ", changed: " << membersstr << " -> " << newmembers << std::endl;
//       }
//     }
//     if (!changedsomething)
//       if (verbose) std::cout << "    update groups." << members << ", changed: 0" << std::endl;
//   }

//   //d_database.prettyPrint("SELECT _id,members FROM groups");

//   // UPDATE 'reactions' field in sms and mms tables....
//   for (auto const &msgtable : {"sms", "mms"})
//   {
//     if (d_database.tableContainsColumn(msgtable, "reactions"))
//     {
//       SqliteDB::QueryResults res;
//       d_database.exec("SELECT _id, reactions FROM "s + msgtable + " WHERE reactions IS NOT NULL", &res);
//       for (uint i = 0; i < res.rows(); ++i)
//       {
//         changedsomething = false;
//         ReactionList reactions(res.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
//         for (uint j = 0; j < reactions.numReactions(); ++j)
//         {
//           //std::cout << "Dealing with " << msgtable << " reaction author: " << reactions.getAuthor(j) << std::endl;
//           if (reactions.getAuthor(j) == static_cast<uint64_t>(sourceid))
//           {
//             reactions.setAuthor(j, targetid);
//             changedsomething = true;
//             if (verbose) std::cout << "    updated " << msgtable << " reaction" << std::endl;
//           }
//         }
//         if (changedsomething)
//           d_database.exec("UPDATE "s + msgtable + " SET reactions = ? WHERE _id = ?", {std::make_pair(reactions.data(), static_cast<size_t>(reactions.size())),
//                                                                                       res.getValueAs<long long int>(i, "_id")});
//       }
//     }
//   }
// }

/*
void SignalBackup::updateRecipientId(long long int targetid, std::string const &identifier)
{
  //std::cout << __FUNCTION__ << std::endl;

  // CALLED ON SOURCE
  // the targetid should already be guaranteed to not exist in source as this is called
  // after makeIdsUnique() & friends

  if (d_databaseversion < 24) // recipient table does not exist
    return;

  // get the current (to be deleted) recipient._id for this identifier (=phone,group_id,possibly uuid)
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id FROM recipient WHERE COALESCE(uuid,phone,group_id) = '" + identifier + "'", &results);

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

  updateRecipientId(targetid, sourceid);
}
*/

void SignalBackup::updateRecipientId(long long int targetid, RecipientIdentification const &rec_id)
{
  //std::cout << __FUNCTION__ << std::endl;

  // CALLED ON SOURCE
  // the targetid should already be guaranteed to not exist in source as this is called
  // after makeIdsUnique() & friends

  if (d_databaseversion < 24) // recipient table does not exist
    return;

  // get the current (to be deleted) recipient._id for this identifier (=phone,group_id,possibly uuid)
  SqliteDB::QueryResults results;
  d_database.exec("SELECT _id FROM recipient WHERE "
                  "(" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                  "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                  "(group_id IS NOT NULL AND group_id IS ?)",
                  {rec_id.uuid, rec_id.phone, rec_id.group_id}, &results);

  if (results.rows() > 1)
  {
    Logger::error("Unexpectedly got multiple results");
    return;
  }

  // the target recipient was not found in this source db, nothing to do.
  if (results.rows() == 0)
    return;

  long long int sourceid = results.getValueAs<long long int>(0, "_id");

  //std::cout << "  Mapping " << sourceid << " -> " << targetid << " (" << ident << ")" << std::endl;

  updateRecipientId(targetid, sourceid);
}
