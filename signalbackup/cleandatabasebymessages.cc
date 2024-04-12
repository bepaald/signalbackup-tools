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

void SignalBackup::cleanDatabaseByMessages()
{
  Logger::message(__FUNCTION__);

  Logger::message("  Deleting attachment entries from '", d_part_table, "' not belonging to remaining ", d_mms_table, " entries");
  d_database.exec("DELETE FROM " + d_part_table + " WHERE " + d_part_mid + " NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");

  Logger::message("  Deleting other threads from 'thread'...");
  d_database.exec("DELETE FROM thread WHERE _id NOT IN (SELECT DISTINCT thread_id FROM " + d_mms_table + ")" + (d_database.containsTable("sms") ? " AND _id NOT IN (SELECT DISTINCT thread_id FROM sms)" : ""));
  updateThreadsEntries();

  if (d_database.containsTable("mention"))
  {
    Logger::message("  Deleting entries from 'mention' not belonging to remaining mms entries");
    d_database.exec("DELETE FROM mention WHERE message_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ") OR thread_id NOT IN (SELECT DISTINCT _id FROM thread)");
  }

  //Logger::message("Groups left:");
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  Logger::message_start("  Deleting removed groups...");
  if (d_databaseversion < 27)
  {
    d_database.exec("DELETE FROM groups WHERE group_id NOT IN (SELECT DISTINCT " + d_thread_recipient_id + " FROM thread)");
    Logger::message(" (", d_database.changed(), ")");
  }
  else
  {
    d_database.exec("DELETE FROM groups WHERE recipient_id NOT IN (SELECT DISTINCT " + d_thread_recipient_id + " FROM thread) RETURNING group_id");
    Logger::message(" (", d_database.changed(), ")");
    if (d_database.containsTable("group_membership"))
      d_database.exec("DELETE FROM group_membership WHERE group_id NOT IN (SELECT DISTINCT group_id FROM groups)");
  }

  //Logger::message("Groups left:");
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

  if (d_database.containsTable("msl_message") &&
      d_database.containsTable("msl_recipient") &&
      d_database.containsTable("msl_payload"))
  {
    Logger::message_start("  Deleting unneeded MessageSendLog entries...");

    long long int count_msg = 0, count_payl = 0, count_rec = 0;

    // note this function is generally called because messages (and/or attachments) have been deleted
    // the msl_payload table has triggers that delete its entries:
    // (delete from msl_payload where _id in (select payload_id from message where message_id = (message.deleted_id/part.deletedmid)))
    // apparently these triggers even when editing within this program, even though the 'ON DELETE CASCADE' stuff does not and
    // foreign key constraints are not enforced... This causes a sort of circular thing here but I think we can just clean up the
    // msl_message table according to still-existing msl_payloads first
    d_database.exec("DELETE FROM msl_message WHERE payload_id NOT IN (SELECT DISTINCT _id FROM msl_payload)");
    count_msg += d_database.changed();

    // delete from msl_message table if message does not exist anymore
    if (d_database.containsTable("sms"))
    {
      d_database.exec("DELETE FROM msl_message WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT _id FROM sms)");
      count_msg += d_database.changed();
      d_database.exec("DELETE FROM msl_message WHERE is_mms IS 1 AND message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
      count_msg += d_database.changed();
    }
    else
    {
      d_database.exec("DELETE FROM msl_message WHERE message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
      count_msg += d_database.changed();
    }

    // now delete all msl_payloads for non existing msl_messages
    d_database.exec("DELETE FROM msl_payload WHERE _id NOT IN (SELECT DISTINCT payload_id FROM msl_message)");
    count_payl = d_database.changed();

    // lastly delete recipient for non existing payloads
    d_database.exec("DELETE FROM msl_recipient WHERE payload_id NOT IN (SELECT DISTINCT _id FROM msl_payload)");
    count_rec = d_database.changed();

    Logger::message(" (", count_msg, ", ", count_payl, ", ", count_rec, ")");
  }

  if (d_database.containsTable("reaction")) // dbv >= 121
  {
    // delete reactions for messages that do not exist
    Logger::message_start("  Deleting reactions to non-existing messages...");

    long long int count = 0;

    if (d_database.containsTable("sms"))
    {
      d_database.exec("DELETE FROM reaction WHERE is_mms IS NOT 1 AND message_id NOT IN (SELECT _id FROM sms)");
      count += d_database.changed();
      d_database.exec("DELETE FROM reaction WHERE is_mms IS 1 AND message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
      count += d_database.changed();
    }
    else
    {
      d_database.exec("DELETE FROM reaction WHERE message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
      count += d_database.changed();
    }
    Logger::message(" (", count, ")");
  }

  if (d_database.containsTable("call")) // dbv >= ~170?
  {
    Logger::message_start("  Deleting call details from non-existing messages...");
    d_database.exec("DELETE FROM call WHERE message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
    Logger::message(" (", d_database.changed(), ")");
  }

  // delete story_sends entries that no longer refer to an existing message?
  if (d_database.tableContainsColumn("story_sends", "message_id"))
  {
    d_database.exec("DELETE FROM story_sends WHERE message_id NOT IN (SELECT _id FROM " + d_mms_table + ")");
  }

  if (d_databaseversion < 24)
  {
    Logger::message("  Deleting unreferenced recipient_preferences entries...");
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id + " FROM mms");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms
    d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids NOT IN (WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id + " FROM " + d_mms_table + ")");
  }
  else
  {
    Logger::message("  Deleting unreferenced recipient entries...");

    //runSimpleQuery("SELECT group_concat(_id,',') FROM recipient");
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms UNION SELECT DISTINCT " + d_mms_recipient_id FROM mms UNION SELECT DISTINCT " + d_thread_recipient_id + " FROM thread");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms

    // KEEP recipients WITH _id IN remapped_recipients.old_id!?!?

    std::set<long long int> referenced_recipients;
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
    if (d_database.tableContainsColumn(d_mms_table, "reactions"))
    {
      SqliteDB::QueryResults reactionresults;
      d_database.exec("SELECT DISTINCT reactions FROM " + d_mms_table + " WHERE reactions IS NOT NULL", &reactionresults);
      for (uint i = 0; i < reactionresults.rows(); ++i)
      {
        ReactionList reactions(reactionresults.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "reactions"));
        for (uint j = 0; j < reactions.numReactions(); ++j)
          referenced_recipients.insert(reactions.getAuthor(j));
      }
    }
    if (d_verbose) [[unlikely]]
      if (d_database.tableContainsColumn("sms", "reactions") || d_database.tableContainsColumn(d_mms_table, "reactions"))
        Logger::message("Got recipients from reactions. List now: ", std::vector<long long int>(referenced_recipients.begin(), referenced_recipients.end()));

    getGroupV1MigrationRecipients(&referenced_recipients);
    if (d_verbose) [[unlikely]]
      Logger::message("Got recipients from gv1migration. List now: ", std::vector<long long int>(referenced_recipients.begin(), referenced_recipients.end()));

    // get (former)group members
    SqliteDB::QueryResults results;
    for (auto const &members : {"members"s, d_groups_v1_members})
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
          //Logger::message("ADDING ", members, " MEMBER: ", substr);
          referenced_recipients.insert(bepaald::toNumber<int>(substr));
        }
      }
    }
    if (d_database.containsTable("group_membership"))
      if (d_database.exec("SELECT DISTINCT recipient_id FROM group_membership", &results))
        for (uint i = 0; i < results.rows(); ++i)
          referenced_recipients.insert(results.getValueAs<long long int>(i, "recipient_id"));
    if (d_verbose) [[unlikely]]
      Logger::message("Got recipients from groupmemberships. List now: ", std::vector<long long int>(referenced_recipients.begin(), referenced_recipients.end()));

    // get recipients mentioned in group updates (by uuid)
    std::vector<long long int> mentioned_in_group_updates(getGroupUpdateRecipients());
    for (long long int id : mentioned_in_group_updates)
      referenced_recipients.insert(id);
    if (d_verbose) [[unlikely]]
      Logger::message("Got recipients from mentions. List now: ", std::vector<long long int>(referenced_recipients.begin(), referenced_recipients.end()));

    // get recipient_id of releasechannel
    for (auto const &kv : d_keyvalueframes)
      if (kv->key() == "releasechannel.recipient_id")
        referenced_recipients.insert(bepaald::toNumber<int>(kv->value()));

    if (d_verbose) [[unlikely]]
      Logger::message("Got recipients from releasechannel. List now: ", std::vector<long long int>(referenced_recipients.begin(), referenced_recipients.end()));

    std::string referenced_recipients_query;
    if (!referenced_recipients.empty())
    {
      referenced_recipients_query = " UNION SELECT * FROM (VALUES";
      for (auto const &r : referenced_recipients)
      {
        //Logger::message("AUTHOR : ", r);
        referenced_recipients_query += "(" + bepaald::toString(r) + "),";
      }
      referenced_recipients_query.pop_back();
      referenced_recipients_query += ")";
    }
    //Logger::message("QUERY: ", reaction_authors_query);

    SqliteDB::QueryResults deleted_recipients;
    d_database.exec("DELETE FROM recipient WHERE _id NOT IN"
                    " (SELECT DISTINCT " + d_mms_recipient_id + " FROM " + d_mms_table +
                    (d_database.containsTable("sms") ? " UNION SELECT DISTINCT " + d_sms_recipient_id + " FROM sms"s : "") +
                    (d_database.tableContainsColumn(d_mms_table, "quote_author") ? " UNION SELECT DISTINCT quote_author FROM " +
                     d_mms_table + " WHERE quote_author IS NOT NULL"s : ""s) +
                    (d_database.tableContainsColumn(d_mms_table, "to_recipient_id") ? " UNION SELECT DISTINCT to_recipient_id FROM " +
                     d_mms_table : ""s) +
                    (d_database.containsTable("mention") ? " UNION SELECT DISTINCT recipient_id FROM mention"s : ""s) +
                    (d_database.containsTable("reaction") ? " UNION SELECT DISTINCT author_id FROM reaction"s : ""s) +
                    (d_database.containsTable("story_sends") ? " UNION SELECT DISTINCT recipient_id FROM story_sends"s : ""s) +
                    referenced_recipients_query +
                    " UNION SELECT DISTINCT " + d_thread_recipient_id + " FROM thread) RETURNING _id"s +
                    //",COALESCE(NULLIF(" + d_recipient_system_joined_name + ", ''), NULLIF(profile_joined_name, ''), NULLIF(" + d_recipient_profile_given_name + ", ''), NULLIF(recipient." + d_recipient_e164 + ", ''), NULLIF(recipient." + d_recipient_aci + ", ''), recipient._id) AS 'display_name'," + d_recipient_e164 +
                    (d_database.containsTable("distribution_list") ? ",distribution_list_id"s : ""s), &deleted_recipients, d_verbose);
    if (deleted_recipients.rows())
    {
      //deleted_recipients.prettyPrint();
      Logger::message("  Deleted ", deleted_recipients.rows(), " unreferenced recipients");
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
          Logger::message("  Deleted ", count, " unneeded distribution_lists");

        // clean up the member table
        d_database.exec("DELETE FROM distribution_list_member WHERE list_id NOT IN (SELECT DISTINCT _id FROM distribution_list)");
      }
    }
  }

  if (d_database.containsTable("notification_profile_allowed_members"))
  {
    Logger::message("  Deleting unneeded notification profiles entries...");

    // delete from notification profile where recipient no longer in database
    d_database.exec("DELETE FROM notification_profile_allowed_members WHERE recipient_id NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  if (d_database.containsTable("pending_pni_signature_message"))
  {
    Logger::message("  Deleting pending_pni_signature_messages not belonging to existing recipients...");

    d_database.exec("DELETE FROM pending_pni_signature_message WHERE recipient_id NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  //runSimpleQuery((d_databaseversion < 27) ? "SELECT _id, recipient_ids, system_display_name FROM recipient_preferences" : "SELECT _id, COALESCE(system_display_name,group_id,signal_profile_name) FROM recipient");

  // remove avatars not belonging to existing recipients
  Logger::message("  Deleting unused avatars...");
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
    for (std::vector<std::pair<std::string, DeepCopyingUniquePtr<AvatarFrame>>>::iterator avit = d_avatars.begin(); avit != d_avatars.end(); ++avit)
      if ((d_databaseversion < 33) ? !results.contains(avit->first) : !results.contains(bepaald::toNumber<long long int>(avit->first))) // avit first == "+316xxxxxxxx" on d_database < 33, recipient._id if > 33;
      {
        avit = d_avatars.erase(avit);
        erased = true;
        break;
      }
    //else
    //  ++avit;
  }

  cleanAttachments();

  Logger::message("  Delete others from 'identities'");
  if (d_databaseversion < 24)
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");
  else
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT _id FROM recipient)");

  Logger::message("  Deleting group_receipts entries from deleted messages...");
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM " + d_mms_table + ")");

  Logger::message("  Deleting group_receipts from non-existing recipients");
  if (d_databaseversion < 24)
    d_database.exec("DELETE FROM group_receipts WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");
  else
    d_database.exec("DELETE FROM group_receipts WHERE address NOT IN (SELECT DISTINCT _id FROM recipient)");

  Logger::message("  Deleting drafts from deleted threads...");
  d_database.exec("DELETE FROM drafts WHERE "s +
                  (d_database.containsTable("sms") ? "thread_id NOT IN (SELECT DISTINCT thread_id FROM sms) AND " : "")
                  + "thread_id NOT IN (SELECT DISTINCT thread_id FROM " + d_mms_table + ")");

  if (d_database.containsTable("remapped_recipients"))
  {
    Logger::message("  Deleting remapped recipients for non existing recipients");
    //d_database.exec("DELETE FROM remapped_recipients WHERE old_id NOT IN (SELECT DISTINCT _id FROM recipient) AND new_id NOT IN (SELECT DISTINCT _id FROM recipient)");
    d_database.exec("DELETE FROM remapped_recipients WHERE new_id NOT IN (SELECT DISTINCT _id FROM recipient)");
  }

  Logger::message("  Vacuuming database");
  d_database.exec("VACUUM");
  d_database.freeMemory();
  // maybe remap recipients?

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

}
