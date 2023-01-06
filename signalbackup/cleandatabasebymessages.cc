/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

void SignalBackup::cleanDatabaseByMessages()
{
  std::cout << __FUNCTION__ << std::endl;

  std::cout << "  Deleting attachment entries from 'part' not belonging to remaining mms entries" << std::endl;
  d_database.exec("DELETE FROM part WHERE mid NOT IN (SELECT DISTINCT _id FROM mms)");

  std::cout << "  Deleting other threads from 'thread'..." << std::endl;
  d_database.exec("DELETE FROM thread WHERE _id NOT IN (SELECT DISTINCT thread_id FROM sms) AND _id NOT IN (SELECT DISTINCT thread_id FROM mms)");
  updateThreadsEntries();

  if (d_database.containsTable("mention"))
  {
    std::cout << "  Deleting entries from 'mention' not belonging to remaining mms entries" << std::endl;
    d_database.exec("DELETE FROM mention WHERE message_id NOT IN (SELECT DISTINCT _id FROM mms) OR thread_id NOT IN (SELECT DISTINCT _id FROM thread)");
  }

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  std::cout << "  Deleting removed groups..." << std::endl;
  if (d_databaseversion < 27)
    d_database.exec("DELETE FROM groups WHERE group_id NOT IN (SELECT DISTINCT " + d_thread_recipient_id + " FROM thread)");
  else
    d_database.exec("DELETE FROM groups WHERE recipient_id NOT IN (SELECT DISTINCT " + d_thread_recipient_id + " FROM thread)");

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

  if (d_database.containsTable("msl_message") &&
      d_database.containsTable("msl_recipient") &&
      d_database.containsTable("msl_payload"))
  {
    std::cout << "  Deleting unneeded MessageSendLog entries..." << std::endl;

    // delete from msl_message table if message does not exist anymore
    d_database.exec("DELETE FROM msl_message WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT _id FROM sms)");
    d_database.exec("DELETE FROM msl_message WHERE is_mms IS 1 AND message_id NOT IN (SELECT _id FROM mms)");

    // now delete all payloads for non existing messages
    d_database.exec("DELETE FROM msl_payload WHERE _id NOT IN (SELECT DISTINCT payload_id FROM msl_message)");

    // lastly delete recipient for non existing payloads
    d_database.exec("DELETE FROM msl_recipient WHERE payload_id NOT IN (SELECT DISTINCT _id FROM msl_payload)");
  }

  if (d_database.containsTable("reaction")) // dbv >= 121
  {
    // delete reactions for messages that do not exist
    std::cout << "  Deleting reactions to non-existing messages" << std::endl;
    d_database.exec("DELETE FROM reaction WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT _id FROM sms)");
    d_database.exec("DELETE FROM reaction WHERE is_mms IS 1 AND message_id NOT IN (SELECT _id FROM mms)");
  }

  // delete story_sends entries that no longer refer to an existing message?
  if (d_database.tableContainsColumn("story_sends", "message_id"))
  {
    d_database.exec("DELETE FROM story_sends WHERE message_id NOT IN (SELECT _id FROM mms)");
  }

  if (d_databaseversion < 24)
  {
    std::cout << "  Deleting unreferenced recipient_preferences entries..." << std::endl;
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id + " FROM mms");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms
    d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids NOT IN (WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id + " FROM mms)");
  }
  else
  {
    std::cout << "  Deleting unreferenced recipient entries..." << std::endl;

    //runSimpleQuery("SELECT group_concat(_id,',') FROM recipient");
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id FROM mms UNION SELECT DISTINCT " + d_thread_recipient_id + " FROM thread");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms

    // KEEP recipients WITH _id IN remapped_recipients.old_id!?!?

    std::set<unsigned int> referenced_recipients;
    if (d_database.tableContainsColumn("sms", "reactions"))
    {
      SqliteDB::QueryResults reactionresults;
      d_database.exec("SELECT DISTINCT reactions FROM sms WHERE reactions IS NOT NULL", &reactionresults);
      for (uint i = 0; i < reactionresults.rows(); ++i)
      {
        ReactionList reactions(reactionresults.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
          referenced_recipients.insert(reactions.getAuthor(j));
      }
    }
    if (d_database.tableContainsColumn("mms", "reactions"))
    {
      SqliteDB::QueryResults reactionresults;
      d_database.exec("SELECT DISTINCT reactions FROM mms WHERE reactions IS NOT NULL", &reactionresults);
      for (uint i = 0; i < reactionresults.rows(); ++i)
      {
        ReactionList reactions(reactionresults.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
          referenced_recipients.insert(reactions.getAuthor(j));
      }
    }

    SqliteDB::QueryResults results;
    if (d_database.exec("SELECT body FROM sms WHERE type == ?", bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
    {
      //results.prettyPrint();
      for (uint i = 0; i < results.rows(); ++i)
      {
        if (results.valueHasType<std::string>(i, "body"))
        {
          //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;

          std::string body = results.getValueAs<std::string>(i, "body");
          std::string tmp; // to hold part of number while reading
          unsigned int body_idx = 0;
          while (true)
          {
            if (!std::isdigit(body[body_idx]) || body_idx >= body.length()) // we are reading '|', ',' or end of string
            {
              // deal with any number we have
              if (tmp.size())
              {
                referenced_recipients.insert(bepaald::toNumber<int>(tmp));
                if (d_verbose) [[unlikely]]
                  std::cout << "    Got recipient from GV1_MIGRATION: " << tmp << std::endl;
                tmp.clear();
              }
            }
            else // we are reading (part of) a number
              tmp += body[body_idx];
            ++body_idx;
            if (body_idx > body.length())
              break;
          }
        }
      }
    }

    using namespace std::string_literals;

    // get (former)group members
    for (auto const &members : {"members", "former_v1_members"})
    {
      if (!d_database.tableContainsColumn("groups", members))
        continue;

      d_database.exec("SELECT "s + members + " FROM groups WHERE " + members + " IS NOT NULL", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        std::string membersstr = results.getValueAs<std::string>(i, members);
        std::stringstream ss(membersstr);
        while (ss.good())
        {
          std::string substr;
          std::getline(ss, substr, ',');
          //std::cout << "ADDING " << members << " MEMBER: " << substr << std::endl;
          referenced_recipients.insert(bepaald::toNumber<int>(substr));
        }
      }
    }

    // get recipients mentioned in group updates (by uuid)
    std::vector<long long int> mentioned_in_group_updates = getGroupUpdateRecipients();
    for (long long int id : mentioned_in_group_updates)
      referenced_recipients.insert(id);

    // get recipient_id of releasechannel
    for (auto const &kv : d_keyvalueframes)
      if (kv->key() == "releasechannel.recipient_id")
        referenced_recipients.insert(bepaald::toNumber<int>(kv->value()));

    std::string referenced_recipients_query;
    if (!referenced_recipients.empty())
    {
      referenced_recipients_query = " UNION SELECT * FROM (VALUES";
      for (auto const &r : referenced_recipients)
      {
        //std::cout << "AUTHOR : " << r << std::endl;
        referenced_recipients_query += "(" + bepaald::toString(r) + "),";
      }
      referenced_recipients_query.pop_back();
      referenced_recipients_query += ")";
    }
    //std::cout << "QUERY: " << reaction_authors_query << std::endl;

    SqliteDB::QueryResults deleted_recipients;
    d_database.exec("DELETE FROM recipient WHERE _id NOT IN"
                    " (SELECT DISTINCT " + d_sms_recipient_id + " FROM sms"
                    " UNION SELECT DISTINCT " + d_mms_recipient_id + " FROM mms"s +
                    (d_database.tableContainsColumn("mms", "quote_author") ? " UNION SELECT DISTINCT quote_author FROM mms WHERE quote_author IS NOT NULL"s : ""s) +
                    (d_database.containsTable("mention") ? " UNION SELECT DISTINCT recipient_id FROM mention"s : ""s) +
                    (d_database.containsTable("reaction") ? " UNION SELECT DISTINCT author_id FROM reaction"s : ""s) +
                    (d_database.containsTable("story_sends") ? " UNION SELECT DISTINCT recipient_id FROM story_sends"s : ""s) +
                    referenced_recipients_query +
                    " UNION SELECT DISTINCT " + d_thread_recipient_id + " FROM thread) RETURNING _id"s +
                    (d_database.containsTable("distribution_list") ? ",distribution_list_id"s : ""s), &deleted_recipients);
    if (deleted_recipients.rows())
    {
      std::cout << "  Deleted " << deleted_recipients.rows() << " unreferenced recipients" << std::endl;
      if (d_database.containsTable("distribution_list"))
      {
        int count = 0;
        for (uint i = 0; i < deleted_recipients.rows(); ++i)
        {
          if (!deleted_recipients.isNull(i, "distribution_list_id"))
          {
            d_database.exec("DELETE FROM distribution_list WHERE _id = ?", deleted_recipients.getValueAs<long long int>(i, "distribution_list_id"));
            count += d_database.changed();
          }
        }
        if (count)
          std::cout << "Deleted " << count << " unneeded distribution_lists" << std::endl;

        // clean up the member table
        d_database.exec("DELETE FROM distribution_list_member WHERE list_id NOT IN (SELECT DISTINCT _id FROM distribution_list)");
      }
    }

  }

  if (d_database.containsTable("notification_profile_allowed_members"))
  {
    std::cout << "  Deleting unneeded notification profiles entries..." << std::endl;

    // delete from notification profile where recipient no longer in database
    d_database.exec("DELETE FROM notification_profile_allowed_members WHERE recipient_id NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  //runSimpleQuery((d_databaseversion < 27) ? "SELECT _id, recipient_ids, system_display_name FROM recipient_preferences" : "SELECT _id, COALESCE(system_display_name,group_id,signal_profile_name) FROM recipient");

  // remove avatars not belonging to existing recipients
  std::cout << "  Deleting unused avatars..." << std::endl;
  SqliteDB::QueryResults results;
  if (d_databaseversion < 24)
    d_database.exec("SELECT recipient_ids FROM recipient_preferences", &results);
  else if (d_databaseversion < 33)   // 'recipient_preferences' does not exist anymore, but d_avatars are still linked to "+316xxxxxxxx" strings
    d_database.exec("SELECT COALESCE(phone,group_id) FROM recipient", &results);
  else
    d_database.exec("SELECT _id FROM recipient", &results); // NOTE! _id is not a string!
  bool erased = true;
  while (erased)
  {
    erased = false;
    for (std::vector<std::pair<std::string, std::unique_ptr<AvatarFrame>>>::iterator avit = d_avatars.begin(); avit != d_avatars.end(); ++avit)
      if ((d_databaseversion < 33) ? !results.contains(avit->first) : !results.contains(bepaald::toNumber<long long int>(avit->first))) // avit first == "+316xxxxxxxx" on d_database < 33, recipient._id if > 33;
      {
        avit = d_avatars.erase(avit);
        erased = true;
        break;
      }
    //else
    //  ++avit;
  }

  /*
  // remove unused attachments
  std::cout << "  Deleting unused attachments..." << std::endl;
  d_database.exec("SELECT _id,unique_id FROM part", &results);
  for (auto it = d_attachments.begin(); it != d_attachments.end();)
  {
    bool found = false;
    for (uint i = 0; i < results.rows(); ++i)
    {
      long long int rowid = -1;
      if (results.valueHasType<long long int>(i, "_id"))
        rowid = results.getValueAs<long long int>(i, "_id");
      long long int uniqueid = -1;
      if (results.valueHasType<long long int>(i, "unique_id"))
        uniqueid = results.getValueAs<long long int>(i, "unique_id");

      if (rowid != -1 && uniqueid != -1 &&
          it->first.first == static_cast<uint64_t>(rowid) && it->first.second == static_cast<uint64_t>(uniqueid))
      {
        found = true;
        break;
      }
    }
    if (!found)
      it = d_attachments.erase(it);
    else
      ++it;
  }
  */
  cleanAttachments();

  std::cout << "  Delete others from 'identities'" << std::endl;
  if (d_databaseversion < 24)
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");
  else
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT _id FROM recipient)");

  std::cout << "  Deleting group_receipts entries from deleted messages..." << std::endl;
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM mms)");

  std::cout << "  Deleting group_receipts from non-existing recipients" << std::endl;
  if (d_databaseversion < 24)
    d_database.exec("DELETE FROM group_receipts WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");
  else
    d_database.exec("DELETE FROM group_receipts WHERE address NOT IN (SELECT DISTINCT _id FROM recipient)");

  std::cout << "  Deleting drafts from deleted threads..." << std::endl;
  d_database.exec("DELETE FROM drafts WHERE thread_id NOT IN (SELECT DISTINCT thread_id FROM sms) AND thread_id NOT IN (SELECT DISTINCT thread_id FROM mms)");

  if (d_database.containsTable("remapped_recipients"))
  {
    std::cout << "  Deleting remapped recipients for non existing recipients" << std::endl;
    //d_database.exec("DELETE FROM remapped_recipients WHERE old_id NOT IN (SELECT DISTINCT _id FROM recipient) AND new_id NOT IN (SELECT DISTINCT _id FROM recipient)");
    d_database.exec("DELETE FROM remapped_recipients WHERE new_id NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  std::cout << "  Vacuuming database" << std::endl;
  d_database.exec("VACUUM");
  d_database.freeMemory();
  // maybe remap recipients?

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

}
