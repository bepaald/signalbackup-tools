/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

// #include "signalbackup.ih"

// void SignalBackup::fillThreadTableFromMessages()
// {
//   SqliteDB::QueryResults results;

//   std::cout << std::endl;
//   std::cout << " *** DEPRECATED ***" << std::endl;
//   std::cout << "THIS FUNCTION IS VERY OLD AND MAY BE BROKEN" << std::endl;
//   std::cout << "PLEASE OPEN AN ISSUE IF YOU NEED THIS" << std::endl;
//   std::cout << std::endl;

//   //d_database.exec("SELECT * FROM thread", &results);
//   //std::cout << "THREAD:" << std::endl;
//   //results.prettyPrint();

//   /*

//     NOTE, 'address' was renamed 'recipient_id' in sms and mms tables

//     for mms database:
//     - One-on-one: incoming and outgoing have address == '+31612345678' number of conversation partner
//     - Groups: outgoing have address == '__textsecure_group__!xxxxxxxxxxxxxxxxxxxx' group_id
//               incoming have address == '+31612345678' number of sender of that specific message
//     for sms database:
//     - One-one-one: incoming and outgoing have address == '+31612345678' number of conversation partner
//     - Groups: outgoing NEVER IN sms?
//               incoming have address == '+31612345678' number of sender of that specific message
//   */

//   std::cout << "Creating threads from 'mms' table data" << std::endl;
//   //std::cout << "Threadids in mms, not in thread" << std::endl;
//   d_database.exec("SELECT DISTINCT thread_id," + d_mms_recipient_id + " FROM " + d_mms_table + " WHERE (" + d_mms_type + " & " + bepaald::toString(Types::BASE_TYPE_MASK) +
//                   ") BETWEEN " + bepaald::toString(Types::BASE_OUTBOX_TYPE) + " AND " +
//                   bepaald::toString(Types::BASE_PENDING_INSECURE_SMS_FALLBACK) +
//                   " AND thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
//   //results.prettyPrint();
//   for (uint i = 0; i < results.rows(); ++i)
//     if (results.valueHasType<long long int>(i, 0) &&
//         (results.valueHasType<std::string>(i, 1) || results.valueHasType<long long int>(i, 1)))
//       d_database.exec("INSERT INTO thread (_id, " + d_thread_recipient_id + ") VALUES (?, ?)", {results.value(i, 0), results.value(i, 1)});
//   //std::cout << "Threadids in mms, not in thread" << std::endl;
//   //d_database.exec("SELECT DISTINCT thread_id,address FROM mms WHERE (msg_box&0x1f) BETWEEN 21 AND 26 AND thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
//   //results.prettyPrint();

//   if (d_database.containsTable("sms"))
//   {
//     std::cout << "Creating threads from 'sms' table data" << std::endl;
//     //std::cout << "Threadids in sms, not in thread" << std::endl;
//     d_database.exec("SELECT DISTINCT thread_id," + d_sms_recipient_id + " FROM sms WHERE (type & " + bepaald::toString(Types::BASE_TYPE_MASK) +
//                     ") BETWEEN " + bepaald::toString(Types::BASE_OUTBOX_TYPE) + " AND " +
//                     bepaald::toString(Types::BASE_PENDING_INSECURE_SMS_FALLBACK) +
//                     " AND thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
//     //results.prettyPrint();
//     for (uint i = 0; i < results.rows(); ++i)
//       if (results.valueHasType<long long int>(i, 0) &&
//           (results.valueHasType<std::string>(i, 1) || results.valueHasType<long long int>(i, 1)))
//         d_database.exec("INSERT INTO thread (_id, " + d_thread_recipient_id + ") VALUES (?, ?)", {results.value(i, 0), results.value(i, 1)});
//     // std::cout << "Threadids in sms, not in thread" << std::endl;
//     // d_database.exec("SELECT DISTINCT thread_id,address FROM sms WHERE (type&0x1f) BETWEEN 21 AND 26 AND thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
//     // results.prettyPrint();
//   }

//   // deal with threads without outgoing messages
//   // get all thread_ids not yet in thread table, if there is only one recipient in that thread AND
//   // there is no other thread with that recipient, add it (though it COULD be a 2 person group with only incoming messages)
//   d_database.exec((d_database.containsTable("sms") ? "SELECT sms.thread_id AS union_thread_id, sms." + d_sms_recipient_id + " FROM 'sms' WHERE sms.thread_id NOT IN (SELECT DISTINCT _id FROM thread) UNION " : "") +
//                   "SELECT " + d_mms_table + ".thread_id AS union_thread_id, " + d_mms_table + "." + d_mms_recipient_id + " FROM '" + d_mms_table + "' WHERE " + d_mms_table + ".thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);

//   //std::cout << "Orphan threads in db: " << std::endl;
//   //results.prettyPrint();
//   for (uint i = 0; i < results.rows(); ++i)
//   {
//     long long int thread = std::any_cast<long long int>(results.value(i, "union_thread_id"));
//     //std::cout << "Dealing with thread: " << thread << std::endl;
//     SqliteDB::QueryResults results2;
//     if (d_database.containsTable("sms"))
//       d_database.exec("SELECT DISTINCT sms." + d_sms_recipient_id + " AS union_address FROM 'sms' WHERE sms.thread_id == ? UNION "
//                       "SELECT DISTINCT " + d_mms_table + "." + d_mms_recipient_id + " AS union_address FROM '" + d_mms_table + "' WHERE " + d_mms_table + ".thread_id == ?", {thread, thread}, &results2);
//     else
//       d_database.exec("SELECT DISTINCT " + d_mms_table + "." + d_mms_recipient_id + " AS union_address FROM '" + d_mms_table + "' WHERE " + d_mms_table + ".thread_id == ?", thread, &results2);
//     if (results2.rows() == 1)
//     {
//       SqliteDB::QueryResults results3;
//       d_database.exec("SELECT DISTINCT " + d_thread_recipient_id + " FROM thread WHERE " + d_thread_recipient_id + " = ?", results2.value(0, "union_address"), &results3);
//       if (results3.rows() == 0)
//       {
//         //std::cout << "Creating thread for address " << std::any_cast<std::string>(results2.value(0, "union_address")) << "(id: " << thread << ")" << std::endl;
//         d_database.exec("INSERT INTO thread (_id, " + d_thread_recipient_id + ") VALUES (?, ?)", {thread, results2.value(0, "union_address")});
//       }
//       else
//         std::cout << "Thread for this conversation partner already exists. This may be a group with only two members and "
//                   << "only incoming messages. This case is not supported." << std::endl;
//     }
//     else
//       std::cout << "Too many addresses in orphaned thread, it appears to be group conversation without outgoing messages. This case is not supported." << std::endl;
//  }

//   d_database.exec((d_database.containsTable("sms") ? "SELECT sms.thread_id AS union_thread_id, sms." + d_sms_recipient_id + " FROM 'sms' WHERE sms.thread_id NOT IN (SELECT DISTINCT _id FROM thread) UNION " : "") +
//                   "SELECT " + d_mms_table + ".thread_id AS union_thread_id, " + d_mms_table + "." + d_mms_recipient_id + " FROM '" + d_mms_table + "' WHERE " + d_mms_table + ".thread_id NOT IN (SELECT DISTINCT _id FROM thread)", &results);
//   if (results.rows() > 0)
//   {
//     std::cout << "  !!! WARNING !!! Unable to generate thread data for messages belonging to this thread (no outgoing messages in conversation)" << std::endl;
//     results.prettyPrint();
//   }

//   updateThreadsEntries();

//   // d_database.exec("SELECT _id, date, message_count, " + d_thread_recipient_id + " , snippet, snippet_cs, type, snippet_type, snippet_uri FROM thread", &results);
//   // std::cout << "THREAD:" << std::endl;
//   // results.prettyPrint();

//   // now for each group, try to determine members:

//   SqliteDB::QueryResults threadquery;
//   std::string query = "SELECT DISTINCT _id, " + d_thread_recipient_id + " FROM thread WHERE SUBSTR(" + d_thread_recipient_id + ", 0, 22) == \"__textsecure_group__!\""; // maybe || SUBSTR == "__signal_mms_group__!"
//   d_database.exec(query, &threadquery);

//   for (uint i = 0; i < threadquery.rows(); ++i)
//   {

//     std::set<std::string> groupmembers;

//     if (threadquery.valueHasType<long long int>(i, 0))
//     {
//       std::string threadid = bepaald::toString(threadquery.getValueAs<long long int>(i, 0));
//       d_database.exec(d_database.containsTable("sms") ?
//                       "SELECT sms.date_sent AS union_date, sms.type AS union_type, sms.body AS union_body, sms." + d_sms_recipient_id + " AS union_address, sms._id AS [sms._id], '' AS [mms._id] "
//                       "FROM 'sms' WHERE sms.thread_id = " + threadid +
//                       " AND (sms.type & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + " IS NOT 0"
//                       " OR sms.type & " + bepaald::toString(Types::GROUP_QUIT_BIT) + " IS NOT 0) UNION " : ""
//                       "SELECT " + d_mms_table + "." + d_mms_date_sent + " AS union_display_date, " + d_mms_table + "." + d_mms_type + " AS union_type, " + d_mms_table + ".body AS union_body, " +
//                       d_mms_table + "." + d_mms_recipient_id + " AS union_address, '' AS [sms._id], " + d_mms_table + "._id AS [mms._id] "
//                       "FROM " + d_mms_table + " WHERE " + d_mms_table + ".thread_id = " + threadid +
//                       " AND (" + d_mms_table + "." + d_mms_type + " & " + bepaald::toString(Types::GROUP_UPDATE_BIT) + " IS NOT 0"
//                       " OR " + d_mms_table + "." + d_mms_type + " & " + bepaald::toString(Types::GROUP_QUIT_BIT) + " IS NOT 0) ORDER BY union_date", &results);

//       //std::cout << "STATUS MSGS FROM THREAD: " << threadid << std::endl;
//       //results.prettyPrint();

//       for (uint j = 0; j < results.rows(); ++j)
//       {
//         std::string body   = std::any_cast<std::string>(results.value(j, "union_body"));
//         long long int type = std::any_cast<long long int>(results.value(j, "union_type"));
//         std::string address = std::any_cast<std::string>(results.value(j, "union_address"));

//         if (Types::isGroupUpdate(type) && !Types::isGroupV2(type))
//         {
//           //std::cout << j << " GROUP UPDATE" << std::endl;
//           GroupContext statusmsg(body);

//           auto field4 = statusmsg.getField<4>();
//           for (uint k = 0; k < field4.size(); ++k)
//           {
//             //std::cout << j << " JOINED: " << field4[k] << std::endl;
//             groupmembers.insert(field4[k]);
//           }
//         }
//         else if (Types::isGroupQuit(type))
//         {
//           // std::cout << j << " GROUP QUIT!" << std::endl;
//           // std::cout << j << " LEFT: " << address << std::endl;
//           groupmembers.erase(address);
//         }
//       }
//     }

//     std::string groupid = std::any_cast<std::string>(threadquery.value(i, d_thread_recipient_id));
//     std::string members;
//     //std::cout << "GROUP MEMBERS " << groupid << " : " << std::endl;
//     for (auto it = groupmembers.begin(); it != groupmembers.end(); ++it)
//       members += ((it != groupmembers.begin()) ? "," : "") + *it;
//     //std::cout << members << std::endl;

//     std::cout << "Creating groups information" << std::endl;
//     if (d_database.tableContainsColumn("groups", "members"))
//       d_database.exec("INSERT INTO groups (group_id, members) VALUES (?, ?)", {groupid, members});
//     else
//     {
//       d_database.exec("INSERT INTO groups (group_id) VALUES (?)", groupid);
//       for (auto it = groupmembers.begin(); it != groupmembers.end(); ++it)
//         d_database.exec("INSERT INTO group_membership (group_id, recipient_id)", {groupid, it});
//     }
//   }
// }
