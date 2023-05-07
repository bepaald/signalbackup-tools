/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

long long int SignalBackup::scanSelf() const
{
  // only 'works' on 'newer' versions
  if (!d_database.containsTable("recipient") ||
      !d_database.tableContainsColumn("thread", d_thread_recipient_id) ||
      !d_database.tableContainsColumn(d_mms_table, "quote_author") ||
      (!d_database.tableContainsColumn(d_mms_table, "reactions") && !d_database.containsTable("reaction")))
    return -1;

  ///// FIRST TRY BY GETTING KEY 'account.pni_identity_public_key' FROM KeyValues, and matching it to uuid from identites-table
  std::string identity_public_key;
  for (auto const &kv : d_keyvalueframes)
    if (kv->key() == "account.pni_identity_public_key" && !kv->value().empty())
    {
      identity_public_key = kv->value();
      break;
    }
  if (!identity_public_key.empty())
  {
    long long int selfid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE uuid IN "
                                                                       "(SELECT address FROM identities WHERE identity_key IS ?)",
                                                                       identity_public_key, -1);
    if (selfid != -1)
      return selfid;
  }

  ///// NEXT TRY BY GETTING KEY 'account.aci_identity_public_key' FROM KeyValues, and matching it to uuid from identites-table
  identity_public_key.clear();
  for (auto const &kv : d_keyvalueframes)
    if (kv->key() == "account.aci_identity_public_key" && !kv->value().empty())
    {
      identity_public_key = kv->value();
      break;
    }
  if (!identity_public_key.empty())
  {
    long long int selfid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE uuid IN "
                                                                       "(SELECT address FROM identities WHERE identity_key IS ?)",
                                                                       identity_public_key, -1);
    if (selfid != -1)
      return selfid;
  }

  // in newer databases (>= dbv185), message.from_recipient_id should always be set to self on outgoing messages.
  long long int selfid = d_database.getSingleResultAs<long long int>("SELECT DISTINCT " + d_mms_recipient_id + " FROM message WHERE (type & 0x1f) IN (?, ?, ?, ?, ?, ?, ?, ?)",
                                                                     {Types::BASE_OUTBOX_TYPE, Types::BASE_SENT_TYPE,
                                                                      Types::BASE_SENDING_TYPE, Types::BASE_SENT_FAILED_TYPE,
                                                                      Types::BASE_PENDING_SECURE_SMS_FALLBACK,Types:: BASE_PENDING_INSECURE_SMS_FALLBACK ,
                                                                      Types::OUTGOING_CALL_TYPE, Types::OUTGOING_VIDEO_CALL_TYPE}, -1);
  if (selfid != -1)
    return selfid;


  // get thread ids of all 1-on-1 conversations
  SqliteDB::QueryResults res;
  if (!d_database.exec("SELECT _id, " + d_thread_recipient_id + " FROM thread WHERE " + d_thread_recipient_id + " IN (SELECT _id FROM recipient WHERE group_id IS NULL)", &res))
    return -1;

  std::set<long long int> options;

  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int tid = res.getValueAs<long long int>(i, "_id");
    long long int rid = bepaald::toNumber<long long int>(res.valueAsString(i, d_thread_recipient_id));
    //std::cout << "Dealing with thread: " << tid << " (recipient: " << rid << ")" << std::endl;

    // try to find other recipient in thread
    SqliteDB::QueryResults res2;
    if (!d_database.exec("SELECT DISTINCT quote_author FROM " + d_mms_table + " "
                         "WHERE thread_id IS ? AND quote_id IS NOT 0 AND quote_id IS NOT NULL "
                         "AND quote_author IS NOT NULL AND quote_author IS NOT ?", {tid, rid}, &res2))
      continue;
    for (uint j = 0; j < res2.rows(); ++j)
    {
      //std::cout << "  From quote:" << res2.valueAsString(j, "quote_author") << std::endl;
      options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "quote_author")));
    }

    if (d_database.tableContainsColumn("sms", "reactions"))
    {
      if (!d_database.exec("SELECT reactions FROM sms WHERE thread_id IS ? AND reactions IS NOT NULL", tid, &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        ReactionList reactions(res2.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(j, "reactions"));
        for (uint k = 0; k < reactions.numReactions(); ++k)
        {
          if (reactions.getAuthor(k) != static_cast<uint64_t>(rid))
          {
            //std::cout << "  From reaction (old): " << reactions.getAuthor(k) << std::endl;
            options.insert(reactions.getAuthor(k));
          }
        }
      }
    }
    if (d_database.tableContainsColumn(d_mms_table, "reactions"))
    {
      if (!d_database.exec("SELECT reactions FROM " + d_mms_table + " WHERE thread_id IS ? AND reactions IS NOT NULL", tid, &res2))
        continue;
      for (uint j = 0; j < res2.rows(); ++j)
      {
        ReactionList reactions(res2.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(j, "reactions"));
        for (uint k = 0; k < reactions.numReactions(); ++k)
        {
          if (reactions.getAuthor(k) != static_cast<uint64_t>(rid))
          {
            //std::cout << "  From reaction (old): " << reactions.getAuthor(k) << std::endl;
            options.insert(reactions.getAuthor(k));
          }
        }
      }
    }
    if (d_database.containsTable("reaction"))
    {
      if (d_database.containsTable("sms"))
      {
        if (!d_database.exec("SELECT DISTINCT author_id FROM reaction WHERE is_mms IS 0 AND author_id IS NOT ? AND message_id IN (SELECT DISTINCT _id FROM sms WHERE thread_id = ?)", {rid, tid}, &res2))
          continue;
        for (uint j = 0; j < res2.rows(); ++j)
        {
          //std::cout << "  From reaction (new):" << res2.valueAsString(j, "author_id") << std::endl;
          options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "author_id")));
        }
      }

      if (!d_database.exec("SELECT DISTINCT author_id FROM reaction WHERE "s +
                           (d_database.tableContainsColumn("reaction", "is_mms") ? "is_mms IS 1 AND " : "") +
                           "author_id IS NOT ? AND message_id IN (SELECT DISTINCT _id FROM " + d_mms_table + " WHERE thread_id = ?)", {rid, tid} , &res2))
          continue;

      for (uint j = 0; j < res2.rows(); ++j)
      {
        //std::cout << "  From reaction (new):" << res2.valueAsString(j, "author_id") << std::endl;
        options.insert(bepaald::toNumber<long long int>(res2.valueAsString(j, "author_id")));
      }
    }
  }

  // get thread ids of all group conversations
  if (!d_database.exec("SELECT _id FROM groups WHERE active IS NOT 0", &res))
    return -1;
  //res.prettyPrint();

  // for each group-thread
  for (uint i = 0; i < res.rows(); ++i)
  {
    long long int gid = res.getValueAs<long long int>(i, "_id");

    SqliteDB::QueryResults res2;
    // skip groups without thread
    if (!d_database.exec("SELECT _id from thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id from groups WHERE _id IS ?))", gid, &res2))
      continue;
    if (res2.rows() == 0)
    {
      //std::cout << "Skipping group: " << gid << " (no thread)" << std::endl;
      continue;
    }
    long long int tid = res2.getValueAs<long long int>(0, "_id");

    //std::cout << "Dealing with group: " << gid << " (thread: " << tid << ")" << std::endl;

    SqliteDB::QueryResults res3;
    if (d_database.tableContainsColumn("groups", "members")) // old style
    {
      // this prints all group members that never appear as recipient in a message (in groups, the recipient ('address') is always the sender, except for self, who has the groups id as address)
      if (d_database.containsTable("sms"))
      {
        if (!d_database.exec("WITH split(word, str) AS (SELECT '',members||',' FROM groups WHERE _id IS ? UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT word FROM split WHERE word!='' AND word NOT IN (SELECT DISTINCT " + d_mms_recipient_id +" FROM " + d_mms_table + " WHERE thread_id IS (SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id FROM groups WHERE _id IS ?)))) AND word NOT IN (SELECT DISTINCT " + d_sms_recipient_id + " FROM sms WHERE thread_id IS (SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id FROM groups WHERE _id IS ?))))", {gid, gid, gid}, &res3))
          continue;
      }
      else
        if (!d_database.exec("WITH split(word, str) AS (SELECT '',members||',' FROM groups WHERE _id IS ? UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT word FROM split WHERE word!='' AND word NOT IN (SELECT DISTINCT " + d_mms_recipient_id +" FROM " + d_mms_table + " WHERE thread_id IS (SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS (SELECT _id FROM recipient WHERE group_id IS (SELECT group_id FROM groups WHERE _id IS ?))))", {gid, gid}, &res3))
          continue;

      for (uint j = 0; j < res3.rows(); ++j)
      {
        //std::cout << "  From group membership:" << res3.valueAsString(j, "word") << std::endl;
        options.insert(bepaald::toNumber<long long int>(res3.valueAsString(j, "word")));
      }
    }
    else if (d_database.containsTable("group_membership")) // modern style
    {
      if (!d_database.tableContainsColumn(d_mms_table, "to_recipient_id")) // < dbv185
      {
        // this prints all group members that never appear as recipient in a message (in groups, the recipient ('address') is always the sender, except for self, who has the groups id as address)
        if (!d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id IN (SELECT group_id FROM groups WHERE _id = ?) AND "
                             "recipient_id NOT IN (SELECT DISTINCT " + d_mms_recipient_id + " FROM " + d_mms_table + " WHERE thread_id IS ? AND type IS NOT ?)",
                             {gid, tid, Types::GROUP_CALL_TYPE}, &res3))
          continue;
        //res3.prettyPrint();
      }
      else
      {
        // in the newer style, ([from/to]_recipient_id), self CAN appear as recipient (to_ when incoming, from_ when outgoing).
        // for incoming messages 'self' will never appear in from_, but could (for msgs arriving since 185) appear in to_.
        // for outgoing messages 'self' will never appear in to_ (= always rec._id of group), but always (if migration succesfull) in from_.

        // // outgoing
        // if (!d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id IN (SELECT group_id FROM groups WHERE _id = ?) AND "
        //                      "recipient_id NOT IN ("
        //                      "SELECT DISTINCT to_recipient_id FROM " + d_mms_table + " WHERE thread_id IS ? AND type IN (?, ?, ?, ?, ?, ?, ?, ?)"
        //                      ")",
        //                      {gid, tid,
        //                       Types::BASE_OUTBOX_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_FAILED_TYPE,
        //                       Types::BASE_PENDING_SECURE_SMS_FALLBACK,Types:: BASE_PENDING_INSECURE_SMS_FALLBACK , Types::OUTGOING_CALL_TYPE, Types::OUTGOING_VIDEO_CALL_TYPE}, &res3))
        //   for (uint j = 0; j < res3.rows(); ++j)
        //   {
        //     std::cout << "  From group membership (NEW):" << res3(j, "recipeint_id") << std::endl;
        //     options.insert(bepaald::toNumber<long long int>(res3(j, "recipient_id")));
        //   }

        // incoming
        if (!d_database.exec("SELECT DISTINCT recipient_id FROM group_membership WHERE group_id IN (SELECT group_id FROM groups WHERE _id = ?) AND "
                             "recipient_id NOT IN ("
                             "SELECT DISTINCT from_recipient_id FROM " + d_mms_table + " WHERE thread_id IS ? AND type IS NOT ? AND type NOT IN (?, ?, ?, ?, ?, ?, ?, ?)"
                             ")",
                             {gid, tid, Types::GROUP_CALL_TYPE,
                              Types::BASE_OUTBOX_TYPE, Types::BASE_SENT_TYPE, Types::BASE_SENDING_TYPE, Types::BASE_SENT_FAILED_TYPE,
                              Types::BASE_PENDING_SECURE_SMS_FALLBACK,Types:: BASE_PENDING_INSECURE_SMS_FALLBACK , Types::OUTGOING_CALL_TYPE, Types::OUTGOING_VIDEO_CALL_TYPE}, &res3))
          for (uint j = 0; j < res3.rows(); ++j)
          {
            std::cout << "  From group membership (NEW):" << res3(j, "recipeint_id") << std::endl;
            options.insert(bepaald::toNumber<long long int>(res3(j, "recipient_id")));
          }

      }
    }
  }

  // std::cout << "OPTIONS:" << std::endl;
  // for (auto const &o: options)
  //   std::cout << "Option: " << o << std::endl;

  if (options.size() == 1)
    return *options.begin();

  return -1;
}
