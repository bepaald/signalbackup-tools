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

#include "signalbackup.ih"
#include "msgrange.h"

void SignalBackup::updateThreadsEntries(long long int thread)
{
  Logger::message(__FUNCTION__);

  SqliteDB::QueryResults results;
  std::string query = "SELECT DISTINCT _id, " + d_thread_recipient_id + " FROM thread"; // gets all threads
  if (thread > -1)
    query += " WHERE _id = " + bepaald::toString(thread);
  d_database.exec(query, &results);
  for (unsigned int i = 0; i < results.rows(); ++i)
  {
    if (results.valueHasType<long long int>(i, "_id"))
    {
      // set message count
      std::string threadid = bepaald::toString(results.getValueAs<long long int>(i, "_id"));

      if (i == 0)
        Logger::message_start("  Dealing with thread id: ", threadid);
      else
        Logger::message_continue(", ", threadid);

      long long int thread_recipient = -1;
      if (results.valueHasType<long long int>(i, d_thread_recipient_id))
        thread_recipient = results.getValueAs<long long int>(i, d_thread_recipient_id);

      //std::cout << "    Updating msgcount" << std::endl;

      /*
ThreadTable::
  private fun isSilentType(type: Long): Boolean {
    return MessageTypes.isProfileChange(type) ||
      MessageTypes.isGroupV1MigrationEvent(type) ||
      MessageTypes.isChangeNumber(type) ||
      MessageTypes.isBoostRequest(type) ||
      MessageTypes.isGroupV2LeaveOnly(type) ||
      MessageTypes.isThreadMergeType(type)
      }
*/
      SqliteDB::QueryResults results2;
      if (d_database.containsTable("sms"))
      {
        d_database.exec("UPDATE thread SET " + d_thread_message_count + " = "
                        "(SELECT (SELECT count(*) FROM sms WHERE thread_id = " + threadid +
                        ") + (SELECT count(*) FROM " + d_mms_table + " WHERE thread_id = " + threadid + ")) WHERE _id = " + threadid);

        d_database.exec("SELECT sms.date_sent AS union_date, sms.type AS union_type, sms.body AS union_body, sms._id AS [sms._id], '' AS [mms._id] FROM 'sms' WHERE sms.thread_id = " +
                        threadid + " UNION SELECT " + d_mms_table + "." + d_mms_date_sent + " AS union_date, " + d_mms_table + "." + d_mms_type + " AS union_type, " +
                        d_mms_table + ".body AS union_body, '' AS [sms._id], " + d_mms_table + "._id AS [mms._id] FROM " + d_mms_table +
                        " WHERE " + d_mms_table + ".thread_id = " + threadid +
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " ORDER BY union_date DESC LIMIT 1",
                        {Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                         Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                         Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                         Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                         Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                         Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE}, &results2);
      }
      else // dbv >= 168
      {
        d_database.exec("UPDATE thread SET " + d_thread_message_count + " = "
                        "(SELECT count(*) FROM " + d_mms_table + " WHERE thread_id = " + threadid + ") WHERE _id = " + threadid);

        // at dbv199, an active column was added to thread. When deleted, only a thread contents are actually deleted,
        // but the thread itself is simply marked inactive (preventing it from showing up in the thread list).
        // Since this only happens to deleted threads, inactive implies 0 messages (meaningful or otherwise) in the thread,
        // we set to active if _anything_ is there
        if (d_database.tableContainsColumn("thread", "active"))
          d_database.exec("UPDATE thread SET active = "
                          "((SELECT count(*) FROM " + d_mms_table + " WHERE thread_id = " + threadid + ") > 0) WHERE _id = " + threadid + " AND active = 0");

        d_database.exec("SELECT " + d_mms_table + "." + d_mms_date_sent + " AS union_date, " + d_mms_table + "." + d_mms_type + " AS union_type, " +
                        d_mms_table + ".body AS union_body, '' AS [sms._id], " + d_mms_table + "._id AS [mms._id] FROM " + d_mms_table +
                        " WHERE " + d_mms_table + ".thread_id = " + threadid +
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " AND (union_type & ?) IS NOT ?"
                        " ORDER BY union_date DESC LIMIT 1",
                        {Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                         Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                         Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                         Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                         Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                         Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE},
                        &results2);
      }

      if (results2.rows() == 0)
        continue;

      std::any mid = results2.value(0, "mms._id"); // not d_mms_table, we used an alias in query

      std::any date = results2.value(0, "union_date");
      if (date.type() == typeid(long long int))
      {
        long long int roundeddate = std::any_cast<long long int>(date) - (std::any_cast<long long int>(date) % 1000);
        //std::cout << "    Setting last msg date (" << roundeddate << ")" << std::endl;
        d_database.exec("UPDATE thread SET date = ? WHERE _id = ?", {roundeddate, threadid});
      }

      std::any body = results2.value(0, "union_body");
      std::string newsnippet;
      if (body.type() == typeid(std::string))
      {
        newsnippet = std::any_cast<std::string>(body);
        if (d_database.containsTable("mention"))
        {
          SqliteDB::QueryResults snippet_mentions;
          if (mid.type() == typeid(long long int))
          {
            if (d_database.exec("SELECT * FROM mention WHERE message_id = ?", mid, &snippet_mentions))
            {
              std::vector<Range> ranges;
              for (unsigned int m = 0; m < snippet_mentions.rows(); ++m)
              {
                std::string displayname = getNameFromRecipientId(snippet_mentions.getValueAs<long long int>(m, "recipient_id"));
                if (displayname.empty())
                  continue;
                ranges.emplace_back(Range{snippet_mentions.getValueAs<long long int>(m, "range_start"),
                                          snippet_mentions.getValueAs<long long int>(m, "range_length"),
                                          "",
                                          "@" + displayname,
                                          "",
                                          false});
              }
              applyRanges(&newsnippet, &ranges, nullptr);
            }
          }
        }

        //std::cout << "    Updating snippet (" << newsnippet << ")" << std::endl;
        d_database.exec("UPDATE thread SET snippet = ? WHERE _id = ?", {newsnippet, threadid});
      }
      else
      {
        //std::cout << "    Updating snippet (NULL)" << std::endl;
        d_database.exec("UPDATE thread SET snippet = NULL WHERE _id = ?", threadid);
      }

      std::any type = results2.value(0, "union_type");
      if (type.type() == typeid(long long int))
      {
        //std::cout << "    Updating snippet type (" << std::any_cast<long long int>(type) << ")" << std::endl;
        d_database.exec("UPDATE thread SET snippet_type = ? WHERE _id = ?", {std::any_cast<long long int>(type), threadid});
      }

      if (mid.type() == typeid(long long int))
      {
        //std::cout << "Checking mms" << std::endl;

        SqliteDB::QueryResults results3;
        if (d_database.tableContainsColumn(d_part_table, "sticker_pack_id") &&
            d_database.tableContainsColumn(d_part_table, "sticker_emoji"))
          d_database.exec("SELECT " +
                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) +
                          ", _id, " + d_part_ct + ", sticker_pack_id, IFNULL(sticker_emoji, '') AS sticker_emoji "
                          "FROM " + d_part_table + " WHERE " + d_part_mid + " = ?", {mid}, &results3);
        else
          d_database.exec("SELECT " +
                          (d_database.tableContainsColumn(d_part_table, "unique_id") ? "unique_id"s : "-1 AS unique_id"s) +
                          ", _id, " + d_part_ct + ", NULL AS sticker_pack_id, NULL AS sticker_emoji "
                          "FROM " + d_part_table + " WHERE " + d_part_mid + " = ?", {mid}, &results3);

        if (results3.rows())
        {
          std::any uniqueid = results3.value(0, "unique_id");
          std::any id = results3.value(0, "_id");
          std::any filetype = results3.value(0, d_part_ct);

          // snippet_uri = content://org.thoughtcrime.securesms/part/ + part.unique_id + '/' + part._id
          if (id.type() == typeid(long long int) && uniqueid.type() == typeid(long long int))
          {
            //std::cout << "    Updating snippet_uri" << std::endl;
            d_database.exec("UPDATE thread SET snippet_uri = 'content://org.thoughtcrime.securesms/part/" +
                            bepaald::toString(std::any_cast<long long int>(uniqueid)) + "/" +
                            bepaald::toString(std::any_cast<long long int>(id)) + "' WHERE _id = " + threadid);
          }

          // update body to show photo/movie/file
          if (!results3.isNull(0, "sticker_pack_id") &&
              !results3("sticker_pack_id").empty())
          {
            std::string snippet = results3("sticker_emoji");
            snippet += (snippet.empty() ? "" : " ") + "Sticker"s;
            d_database.exec("UPDATE thread SET snippet = ? WHERE _id = ?", {snippet, threadid});
          }
          else if (filetype.type() == typeid(std::string))
          {
            std::string t = std::any_cast<std::string>(filetype);

            //std::cout << "FILE TYPE: " << t << std::endl;

            std::string snippet;
            if (STRING_STARTS_WITH(t, "image/gif"))
            {
              snippet = "\xF0\x9F\x8E\xA1 "; // ferris wheel emoji for some reason
              snippet += (newsnippet.empty()) ? "GIF" : newsnippet;
            }
            else if (STRING_STARTS_WITH(t, "image"))
            {
              snippet = "\xF0\x9F\x93\xB7 "; // (still) camera emoji
              snippet += (newsnippet.empty()) ? "Photo" : newsnippet;
            }
            else if (STRING_STARTS_WITH(t, "audio"))
            {
              snippet = "\xF0\x9F\x8E\xA4 "; // microphone emoji
              snippet += (newsnippet.empty()) ? "Voice message" : newsnippet;
            }
            else if (STRING_STARTS_WITH(t, "video"))
            {
              snippet = "\xF0\x9F\x8E\xA5 "; //  (movie) camera emoji
              snippet += (newsnippet.empty()) ? "Video" : newsnippet;
            }
            else // if binary file
            {
              snippet = "\xF0\x9F\x93\x8E "; // paperclip
              snippet += (newsnippet.empty()) ? "File" : newsnippet;
            }
            //std::cout << "    Updating snippet (" << snippet << ")" << std::endl;
            d_database.exec("UPDATE thread SET snippet = ? WHERE _id = ?", {snippet, threadid});
          }
        }
        else // was mms, but no part -> maybe contact sharing?
        {    // -> '[{"name":{"displayName":"Basje Timmer",...}}]'
          SqliteDB::QueryResults results4;
          d_database.exec("SELECT json_extract(" + d_mms_table + ".shared_contacts, '$[0].name.displayName') AS shared_contact_name from " + d_mms_table + " WHERE _id = ? AND shared_contacts IS NOT NULL", mid, &results4);
          if (results4.rows() != 0 && results4.valueHasType<std::string>(0, "shared_contact_name"))
          {
            std::string snippet = "\xF0\x9F\x91\xA4 " + results4.getValueAs<std::string>(0, "shared_contact_name"); // bust in silouette emoji
            //std::cout << "    Updating snippet (" << snippet << ")" << std::endl;
            d_database.exec("UPDATE thread SET snippet = ? WHERE _id = ?", {snippet, threadid});
          }
        }
      }
      else
      {
        //std::cout << "    Updating snippet (NULL)" << std::endl;
        d_database.exec("UPDATE thread SET snippet_uri = NULL");
      }

      // if isgroup && database has snippet_extras
      // set snippet_extras = {"individualRecipientId":"8"};
      if (!d_database.containsTable("sms") && d_database.tableContainsColumn("thread", "snippet_extras"))
      {
        long long int isgroup = d_database.getSingleResultAs<long long int>("SELECT group_id IS NOT NULL FROM recipient WHERE _id = ?", thread_recipient, 0);
        if  (isgroup)
        {
          long long int sender = -1;
          if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id")) // old style -> incoming: sender = message.recipient_id
          {                                                                    //              outgoing: sender = self
            SqliteDB::QueryResults snippet_extras;
            if (d_database.exec("SELECT " + d_mms_type + "," + d_mms_recipient_id + " FROM " + d_mms_table +
                                " WHERE " + d_mms_table + ".thread_id = " + threadid +
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " AND (" + d_mms_type + " & ?) IS NOT ?"
                                " ORDER BY " + d_mms_date_sent + " DESC LIMIT 1",
                                {Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                                 Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                                 Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                                 Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                                 Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                                 Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE}, &snippet_extras) &&
                snippet_extras.rows() == 1 && snippet_extras.valueHasType<long long int>(0, d_mms_type) && snippet_extras.valueHasType<long long int>(0, d_mms_recipient_id))
            {
              long long int mmstype = snippet_extras.getValueAs<long long int>(0, d_mms_type);
              if (Types::isOutgoing(mmstype))
              {
                if (d_selfid == -1)
                  d_selfid = scanSelf();
                sender = d_selfid;
              }
              else
                sender = snippet_extras.getValueAs<long long int>(0, d_mms_recipient_id);
            }
          }
          else // new style
            sender = d_database.getSingleResultAs<long long int>("SELECT " + d_mms_recipient_id + " FROM " + d_mms_table +
                                                                 " WHERE " + d_mms_table + ".thread_id = " + threadid +
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " AND (" + d_mms_type + " & ?) IS NOT ?"
                                                                 " ORDER BY " + d_mms_date_sent + " DESC LIMIT 1",
                                                                 {Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                                                                  Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                                                                  Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                                                                  Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                                                                  Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                                                                  Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE}, -1);
          if (sender > -1) // got sender, set snippet_extras
          {
            d_database.exec("UPDATE thread SET snippet_extras = json_object('individualRecipientId', '" + bepaald::toString(sender) + "') WHERE _id = ?", threadid);
            //d_database.prettyPrint("SELECT snippet_extras FROM thread WHERE _id = ?", threadid);
          }
          else
          {
            // could not set 'individualRecipientId' for some reason, should probably clear it (the currently present id might not exist)?
            Logger::message_end();
            Logger::warning("Not updating thread[", threadid, "].snippet_extras: failed to get sender (", sender, ")");
            Logger::warning_indent("Query: ",
                                   "SELECT " + d_mms_recipient_id + " FROM " + d_mms_table +
                                   " WHERE " + d_mms_table + ".thread_id = " + threadid +
                                   " AND (" + d_mms_type + " & ", Types::BASE_TYPE_MASK, ") IS NOT ", Types::PROFILE_CHANGE_TYPE,
                                   " AND (" + d_mms_type + " & ", Types::BASE_TYPE_MASK, ") IS NOT ", Types::GV1_MIGRATION_TYPE,
                                   " AND (" + d_mms_type + " & ", Types::BASE_TYPE_MASK, ") IS NOT ", Types::CHANGE_NUMBER_TYPE,
                                   " AND (" + d_mms_type + " & ", Types::BASE_TYPE_MASK, ") IS NOT ", Types::BOOST_REQUEST_TYPE,
                                   " AND (" + d_mms_type + " & ", Types::GROUP_V2_LEAVE_BITS, ") IS NOT ", Types::GROUP_V2_LEAVE_BITS,
                                   " AND (" + d_mms_type + " & ", Types::BASE_TYPE_MASK, ") IS NOT ", Types::THREAD_MERGE_TYPE,
                                   " ORDER BY " + d_mms_date_sent + " DESC LIMIT 1");
            d_database.prettyPrint(d_truncate,
                                   "SELECT " + d_mms_recipient_id + " FROM " + d_mms_table +
                                   " WHERE " + d_mms_table + ".thread_id = " + threadid +
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " AND (" + d_mms_type + " & ?) IS NOT ?"
                                   " ORDER BY " + d_mms_date_sent + " DESC LIMIT 1",
                                   {Types::BASE_TYPE_MASK, Types::PROFILE_CHANGE_TYPE,
                                    Types::BASE_TYPE_MASK, Types::GV1_MIGRATION_TYPE,
                                    Types::BASE_TYPE_MASK, Types::CHANGE_NUMBER_TYPE,
                                    Types::BASE_TYPE_MASK, Types::BOOST_REQUEST_TYPE,
                                    Types::GROUP_V2_LEAVE_BITS, Types::GROUP_V2_LEAVE_BITS,
                                    Types::BASE_TYPE_MASK, Types::THREAD_MERGE_TYPE});
          }
        }
      }

    }
    else
      Logger::warning("Unexpected type in database");
  }
  Logger::message_end();
}

/*
 meaningful messages

        NOT $TYPE & ${MessageTypes.IGNORABLE_TYPESMASK_WHEN_COUNTING} AND
        $TYPE != ${MessageTypes.PROFILE_CHANGE_TYPE} AND
        $TYPE != ${MessageTypes.CHANGE_NUMBER_TYPE} AND
        $TYPE != ${MessageTypes.SMS_EXPORT_TYPE} AND
        $TYPE != ${MessageTypes.BOOST_REQUEST_TYPE} AND
        $TYPE & ${MessageTypes.GROUP_V2_LEAVE_BITS} != ${MessageTypes.GROUP_V2_LEAVE_BITS}

long IGNORABLE_TYPESMASK_WHEN_COUNTING = END_SESSION_BIT | KEY_EXCHANGE_IDENTITY_UPDATE_BIT | KEY_EXCHANGE_IDENTITY_VERIFIED_BIT;

*/
