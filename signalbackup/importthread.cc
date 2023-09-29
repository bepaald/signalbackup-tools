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

bool SignalBackup::importThread(SignalBackup *source, long long int thread)
{
  std::cout << __FUNCTION__ << " (" << thread << ")" << std::endl;

  if ((d_databaseversion >= 185 && source->d_databaseversion < 185) || // from/to_recipient_id
      (d_databaseversion >= 172 && source->d_databaseversion < 172) || // group.members dropped
      (d_databaseversion >= 168 && source->d_databaseversion < 168) || // sms table dropped
      (d_databaseversion >= 33 && source->d_databaseversion < 33) ||
      (d_databaseversion < 33 && source->d_databaseversion >= 33) ||
      (d_databaseversion >= 27 && source->d_databaseversion < 27) ||
      (d_databaseversion < 27 && source->d_databaseversion >= 27))
  {
    std::cout << "Source and target database at incompatible versions" << std::endl;
    return false;
  }

  if (source->d_database.containsTable("remapped_recipients"))
  {
    SqliteDB::QueryResults r;
    source->d_database.exec("SELECT * FROM remapped_recipients", &r);
    if (r.rows())
    {
      warnOnce("Source database contains 'remapped_recipients'. This case may not yet be handled correctly by this program!");

      for (uint i = 0; i < r.rows(); ++i)
      {
        long long int id = r.getValueAs<long long int>(i, "_id");
        long long int oldid = r.getValueAs<long long int>(i, "old_id");
        long long int newid = r.getValueAs<long long int>(i, "new_id");
        std::cout << id << " : " << oldid << " -> " << newid << std::endl;
        std::cout << std::endl;

        std::cout << "Old id:" << std::endl;
        source->d_database.print("SELECT * FROM recipient WHERE _id = ?", oldid);
        std::cout << std::endl;

        std::cout << "New id: " << std::endl;
        source->d_database.print("SELECT * FROM recipient WHERE _id = ?", newid);
        std::cout << std::endl;
      }

      // apply the remapping (probably only some reactions _may_ need to be transferred?)
      source->remapRecipients();
      // now, the remapping was 'applied', old_id should not occur in database anymore, and remapped_recipients can be cleared?
      source->d_database.exec("DELETE FROM remapped_recipients");
    }
  }

  /*
  // if target contains releasechannel recipient, make sure to remove it from source
  int target_releasechannel = -1;
  int source_releasechannel = -1;
  for (auto const &kv : d_keyvalueframes)
    if (kv->key() == "releasechannel.recipient_id" && !kv->value().empty())
    {
      target_releasechannel = bepaald::toNumber<int>(kv->value());
      break;
    }
  if (target_releasechannel >= 0)
    for (auto const &skv : source->d_keyvalueframes)
      if (skv->key() == "releasechannel.recipient_id")
      {
        source_releasechannel = bepaald::toNumber<int>(skv->value());
        source->d_database.exec("DELETE FROM recipient WHERE _id = ?", source_releasechannel);
        std::cout << "Deleted double releasechannel recipient from source database (_id: " << source_releasechannel << ")" << std::endl;
        break;
      }
  */
  // do not import release_channel from source. Target will either have its own, or be too old for one
  int source_releasechannel = -1;
  for (auto const &skv : source->d_keyvalueframes)
    if (skv->key() == "releasechannel.recipient_id")
    {
      source_releasechannel = bepaald::toNumber<int>(skv->value());
      source->d_database.exec("DELETE FROM recipient WHERE _id = ?", source_releasechannel);
      std::cout << "Deleted releasechannel recipient from source database (_id: " << source_releasechannel << ")" << std::endl;
      break;
    }

  long long int targetthread = -1;
  SqliteDB::QueryResults results;
  if (d_databaseversion < 24) // old database version
  {
    // get targetthread from source thread id (source.thread_id->source.recipient_id->target.thread_id
    source->d_database.exec("SELECT " + source->d_thread_recipient_id + " FROM thread WHERE _id = ?", thread, &results);
    if (results.rows() != 1 || results.columns() != 1 ||
        !results.valueHasType<std::string>(0, 0))
    {
      std::cout << "Failed to get recipient id from source database" << std::endl;
      return false;
    }
    std::string recipient_id = results.getValueAs<std::string>(0, 0);
    targetthread = getThreadIdFromRecipient(recipient_id); // -1 if none found
  }
  else // new database version
  {
    // get targetthread from source thread id (source.thread_id->source.recipient_id->source.recipient.phone/group_id->target.thread_id
    source->d_database.exec("SELECT "
                            "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                            "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                            "IFNULL(group_id, '') AS group_id FROM recipient WHERE _id IS (SELECT " + source->d_thread_recipient_id + " FROM thread WHERE _id = ?)", thread, &results);
    if (results.rows() != 1)
    {
      // skip current thread if it is the releasechannel-thread
      // maybe I should deal with this in the future
      SqliteDB::QueryResults res2;
      source->d_database.exec("SELECT " + source->d_thread_recipient_id + " FROM thread WHERE _id = ?", thread, &res2);
      if (res2.rows() &&
          ((res2.valueHasType<long long int>(0, 0) && res2.getValueAs<long long int>(0, 0) == source_releasechannel) ||
           (res2.valueHasType<std::string>(0, 0) && bepaald::toNumber<int>(res2.getValueAs<std::string>(0, 0)) == source_releasechannel)))
      {
        std::cout << "Skipping releasechannel..." << std::endl;
        return true; // when this channel is actually active, maybe remove this return statement and
                     // manually set targetthread with the help of target_releasechannel (if != -1)
      }

      std::cout << "Failed to get uuid/phone/group_id from source database" << std::endl;
      return false;
    }

    //std::string phone_or_group = results.getValueAs<std::string>(0, 0);
    RecipientIdentification rec_id = {results(0, "uuid"), results(0, "phone"), results(0, "group_id"), -1, std::string()};

    d_database.exec("SELECT _id FROM recipient WHERE "
                    "(" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                    "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                    "(group_id IS NOT NULL AND group_id IS ?)", {rec_id.uuid, rec_id.phone, rec_id.group_id}, &results);
    if (results.rows() != 1 || results.columns() != 1 ||
        !results.valueHasType<long long int>(0, 0))
      std::cout << "Failed to find recipient._id matching uuid/phone/group_id in target database" << std::endl;
    else
    {
      long long int recipient_id = results.getValueAs<long long int>(0, 0);
      targetthread = getThreadIdFromRecipient(bepaald::toString(recipient_id));
    }
  }

  // std::cout << "RECIPIENTS BEFORE CROP:" << std::endl;
  // source->d_database.prettyPrint("SELECT _id, COALESCE(signal_profile_name, group_id) FROM recipient");

  // crop the source db to the specified thread
  source->cropToThread(thread);

  // std::cout << "RECIPIENTS AFTER CROP:" << std::endl;
  // source->d_database.prettyPrint("SELECT _id, COALESCE(signal_profile_name, group_id) FROM recipient");

  // remove any storage_key entries that are already in target...
  if (d_database.containsTable("storage_key") && source->d_database.containsTable("storage_key"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT key FROM storage_key", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM storage_key WHERE key = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing storage_keys" << std::endl;
  }

  // delete double megaphones
  if (d_database.containsTable("megaphone") && source->d_database.containsTable("megaphone"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT event FROM megaphone", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM megaphone WHERE event = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing megaphones" << std::endl;
  }

  // delete double remote_megaphones
  if (d_database.containsTable("remote_megaphone") && source->d_database.containsTable("remote_megaphone"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT uuid FROM remote_megaphone", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM remote_megaphone WHERE uuid = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing remote_megaphone's" << std::endl;
  }

  // delete double cds (contact discovery service entries)
  if (d_database.containsTable("cds") && source->d_database.containsTable("cds"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT e164 FROM cds", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM cds WHERE e164 = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing cds's" << std::endl;
  }

  // delete double kyber prekey entries
  if (d_database.containsTable("kyber_prekey") && source->d_database.containsTable("kyber_prekey"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT account_id, key_id FROM kyber_prekey", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM kyber_prekey WHERE account_id = ? AND key_id = ?", {res.value(i, "account_id"), res.value(i, "key_id")});
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing kyber_prekey's" << std::endl;

  }

  // delete double key_value entries (this table does not exist anymore currently)
  if (d_database.containsTable("key_value") && source->d_database.containsTable("key_value"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT key FROM key_value", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM key_value WHERE key = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing key values" << std::endl;
  }

  // delete double distribution lists?
  if (d_database.containsTable("distribution_list") && source->d_database.containsTable("distribution_list"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT distribution_id FROM distribution_list", &res);

    int count = 0;
    for (uint i = 0; i < res.rows(); ++i)
    {
      source->d_database.exec("DELETE FROM distribution_list WHERE distribution_id = ?", res.getValueAs<std::string>(i, 0));
      count += source->d_database.changed();
    }
    if (count)
      std::cout << "  Deleted " << count << " existing distribution lists" << std::endl;

    // clean up the member table
    source->d_database.exec("DELETE FROM distribution_list_member WHERE list_id NOT IN (SELECT DISTINCT _id FROM distribution_list)");
  }

  // the target will have its own job_spec etc...
  if (source->d_database.containsTable("job_spec"))
    source->d_database.exec("DELETE FROM job_spec");
  if (source->d_database.containsTable("push")) // dropped around dbv205
    source->d_database.exec("DELETE FROM push");
  if (source->d_database.containsTable("constraint_spec"))
    source->d_database.exec("DELETE FROM constraint_spec"); // has to do with job_spec, references it...
  if (source->d_database.containsTable("dependency_spec"))
    source->d_database.exec("DELETE FROM dependency_spec"); // has to do with job_spec, references it...

  // we will delete any notification_profile data, these are specific not to any threads,
  // but to the phone owner. The notification profiles will probably already exist on the
  // target, or can be easily recreated. They are difficult to import (they contain multiple
  // UNIQUE fields and should only be imported once, while this function will otherwise
  // do it for each thread imported....
  if (source->d_database.containsTable("notification_profile_allowed_members"))
    source->d_database.exec("DELETE FROM notification_profile_allowed_members");
  if (source->d_database.containsTable("notification_profile_schedule"))
    source->d_database.exec("DELETE FROM notification_profile_schedule");
  if (source->d_database.containsTable("notification_profile"))
    source->d_database.exec("DELETE FROM notification_profile");

  // NOT NECESSARY (these tables are skipped when merging anyway) AND CAUSES BREAKAGE
  // all emoji_search_* tables are ignored on import, delete from source here to prevent failing unique constraints
  /*
  if (source->d_database.containsTable("emoji_search_data"))
    source->d_database.exec("DELETE FROM emoji_search_data");
  if (source->d_database.containsTable("emoji_search_idx"))
    source->d_database.exec("DELETE FROM emoji_search_idx");
  if (source->d_database.containsTable("emoji_search_content"))
    source->d_database.exec("DELETE FROM emoji_search_content");
  if (source->d_database.containsTable("emoji_search_docsize"))
    source->d_database.exec("DELETE FROM emoji_search_docsize");
  if (source->d_database.containsTable("emoji_search_config"))
    source->d_database.exec("DELETE FROM emoji_search_config");
  */

  if (d_database.containsTable("group_call_ring") && source->d_database.containsTable("group_call_ring"))
  {
    // not sure what this table is for, but it has a UNIQUE ring_id field,
    // so let's just delete any double ring_id's

    SqliteDB::QueryResults res;
    d_database.exec("SELECT ring_id FROM group_call_ring", &res);

    for (uint i = 0; i < res.rows(); ++i)
      source->d_database.exec("DELETE FROM group_call_ring WHERE ring_id = ?", res.getValueAs<long long int>(i, 0));
  }

  // untested
  /*
    sqlite> SELECT * FROM sqlite_master WHERE name IS "payments";
    type|name|tbl_name|rootpage|sql
    table|payments|payments|60|CREATE TABLE payments(_id INTEGER PRIMARY KEY, uuid TEXT DEFAULT NULL, recipient INTEGER DEFAULT 0, recipient_address TEXT DEFAULT NULL, timestamp INTEGER, note TEXT DEFAULT NULL, direction INTEGER, state INTEGER, failure_reason INTEGER, amount BLOB NOT NULL, fee BLOB NOT NULL, transaction_record BLOB DEFAULT NULL, receipt BLOB DEFAULT NULL, payment_metadata BLOB DEFAULT NULL, receipt_public_key TEXT DEFAULT NULL, block_index INTEGER DEFAULT 0, block_timestamp INTEGER DEFAULT 0, seen INTEGER, UNIQUE(uuid) ON CONFLICT ABORT)
  */

  // NOTE: 'recipient' here is probably recipient._id which should be updated later in updateRecipientId()
  //       maybe delete completely if recipient is not in this bit of database?

  /*
  // delete double payments
  if (d_database.containsTable("payments") && source->d_database.containsTable("payments"))
  {
    SqliteDB::QueryResults res;
    d_database.exec("SELECT uuid FROM payments", &res);

    std::cout << "  Deleting " << res.rows() << " existing payments" << std::endl;

    for (uint i = 0; i < res.rows(); ++i)
      source->d_database.exec("DELETE FROM payments WHERE uuid = ?", res.getValueAs<std::string>(i, 0));
  }
  */

  /*
    sqlite> SELECT * FROM sqlite_master WHERE name IS "chat_colors";
    table|chat_colors|chat_colors|65|CREATE TABLE chat_colors (_id INTEGER PRIMARY KEY AUTOINCREMENT,chat_colors BLOB)
  */
  // untested: I guess chat_colors are in target, or they can be easily recreated
  if (source->d_database.containsTable("chat_colors"))
    source->d_database.exec("DELETE FROM chat_colors");

  // TODO deal with 'sender_keys'
  /*
    sqlite> SELECT * FROM sqlite_master WHERE name IS "sender_keys";
table|sender_keys|sender_keys|71|CREATE TABLE sender_keys (_id INTEGER PRIMARY KEY AUTOINCREMENT, recipient_id INTEGER NOT NULL, device INTEGER NOT NULL, distribution_id TEXT NOT NULL, record BLOB NOT NULL, created_at INTEGER NOT NULL, UNIQUE(recipient_id, device, distribution_id) ON CONFLICT REPLACE)
  */
  // notes:
  // * there is recipient_id, which probably needs to be adjusted in updateRecipientId(), but it must be unique so it must then be deleted if the adjustment can be made
  // * there is a created_at, probably a timestamp -> delete the older one?, also use this in croptodate?

  //source->d_database.exec("VACUUM");

  // id's need to be unique
  makeIdsUnique(source);

  // delete double remapped_recipients
  if (d_database.containsTable("remapped_recipients") && source->d_database.containsTable("remapped_recipients"))
  {
    SqliteDB::QueryResults res;
    source->d_database.exec("SELECT * FROM remapped_recipients", &res); // get all remapped recipients in source

    for (uint i = 0; i < res.rows(); ++i)
    {
      long long int id = res.getValueAs<long long int>(i, "_id");
      long long int oldid = res.getValueAs<long long int>(i, "old_id");
      long long int newid = res.getValueAs<long long int>(i, "new_id");
      SqliteDB::QueryResults r2;
      d_database.exec("SELECT * FROM remapped_recipients WHERE old_id = ? AND new_id = ?", {oldid, newid}, &r2);
      if (r2.rows()) // this mapping is in target already
      {
        std::cout << "Skipping import of remapped_recipient (" << oldid << " -> " << newid << "), mapping already in target database" << std::endl;
        source->d_database.exec("DELETE FROM remapped_recipients WHERE _id = ?", id);
      }
    }
  }

  // merge into existing thread, set the id on the sms, mms, and drafts
  // drop the recipient_preferences, identities and thread tables, they are already in the target db
  if (targetthread > -1)
  {
    std::cout << "  Found existing thread for this recipient in target database, merging into thread " << targetthread << std::endl;

    if (source->d_database.containsTable("sms"))
      source->d_database.exec("UPDATE sms SET thread_id = ?", targetthread);
    source->d_database.exec("UPDATE " + d_mms_table + " SET thread_id = ?", targetthread);
    source->d_database.exec("UPDATE drafts SET thread_id = ?", targetthread);
    if (source->d_database.containsTable("mention"))
      source->d_database.exec("UPDATE mention SET thread_id = ?", targetthread);

    // see below for comment explaining this function
    if (d_databaseversion >= 24)
    {
      //d_database.exec("SELECT _id, COALESCE(uuid,phone,group_id) AS identifier FROM recipient", &results);
      if (d_database.tableContainsColumn("recipient", "group_type") &&
          d_database.tableContainsColumn("recipient", d_recipient_storage_service))
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "group_type, " + d_recipient_storage_service + " FROM recipient", &results);
      else
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "-1 AS 'group_type', "
                        "NULL AS '" + d_recipient_storage_service + "' FROM recipient", &results);

      std::cout << "  updateRecipientIds" << std::endl;
      for (uint i = 0; i < results.rows(); ++i)
      {
        RecipientIdentification rec_id = {results(i, "uuid"), results(i, "phone"), results(i, "group_id"), results.getValueAs<long long int>(i, "group_type"), results(i, d_recipient_storage_service)};
        //source->updateRecipientId(results.getValueAs<long long int>(i, "_id"), results.getValueAs<std::string>(i, "identifier"));
        source->updateRecipientId(results.getValueAs<long long int>(i, "_id"), rec_id);
      }
    }

    source->d_database.exec("DROP TABLE thread");
    source->d_database.exec("DROP TABLE identities");
    // even though the thread already exists, not all recipients are guaranteed to
    // exist in target. For example: current thread is group conversation, in the source
    // a member was added, but this member did not yet exist in the target.
    //
    // the same probably goees for ancient (<24) databases, but I'll write that if someone ever
    // tries to merge those.
    //
    // delete existsing recipients (recipient table has unique constraint on phone, uuid and group_id)
    if (d_databaseversion < 24)
      source->d_database.exec("DROP TABLE recipient_preferences");
    else
    {
      // get the unique features of existing recipients
      SqliteDB::QueryResults existing_rec;
      if (d_database.tableContainsColumn("recipient", "group_type") &&
          d_database.tableContainsColumn("recipient", d_recipient_storage_service))
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "group_type, " + d_recipient_storage_service + " FROM recipient", &existing_rec);
      else
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "-1 AS 'group_type', NULL AS '" + d_recipient_storage_service + "' FROM recipient", &existing_rec);

      // for each of them, check if they are also in source, and delete
      int count = 0;
      for (uint i = 0; i < results.rows(); ++i)
      {
        RecipientIdentification rec_id = {existing_rec(i, "uuid"), existing_rec(i, "phone"), existing_rec(i, "group_id"),
                                          existing_rec.getValueAs<long long int>(i, "group_type"), existing_rec(i, d_recipient_storage_service)};

        if (d_database.tableContainsColumn("recipient", "group_type"))
        {
          source->d_database.exec("DELETE FROM recipient WHERE "
                                  // one-of uuid/phone/group_id is set and equal to existing recipient
                                  "((" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                                  "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                                  "(group_id IS NOT NULL AND group_id IS ?)) OR "
                                  // none of uuid/phone/group_id are set, but storage_service_key exists and both are distribution lists?
                                  "(" + d_recipient_aci + " IS NULL AND "
                                  + d_recipient_e164 + " IS NULL AND "
                                  "group_id IS NULL AND "
                                  "group_type IS 4  AND group_type IS ? AND "
                                  + d_recipient_storage_service + " IS ?)", {rec_id.uuid, rec_id.phone, rec_id.group_id, rec_id.group_type, rec_id.storage_service});
          count += source->d_database.changed();
        }
        else
        {
          source->d_database.exec("DELETE FROM recipient WHERE "
                                  // one-of uuid/phone/group_id is set and equal to existing recipient
                                  "((" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                                  "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                                  "(group_id IS NOT NULL AND group_id IS ?))", {rec_id.uuid, rec_id.phone, rec_id.group_id});
          count += source->d_database.changed();
        }
      }
      if (count)
        std::cout << "Dropped " << count << " existing recipients from source database" << std::endl;
    }
    source->d_database.exec("DROP TABLE groups");
    source->d_avatars.clear();
  }
  else // no matching thread in target (but recipient may still exist)
  {
    std::cout << "  No existing thread found in target database for this recipient, importing." << std::endl;

    // check identities and recipient prefs for presence of values, they may be there (even though no
    // thread was found (for example via a group chat or deleted thread))
    // get identities from target, drop all rows from source that are already present
    if (d_databaseversion < 24)
    {
      d_database.exec("SELECT address FROM identities", &results); // address == phonenumber/__text_secure_group
      for (uint i = 0; i < results.rows(); ++i)
        if (results.header(0) == "address" && results.valueHasType<std::string>(i, 0))
          source->d_database.exec("DELETE FROM identities WHERE address = '" + results.getValueAs<std::string>(i, 0) + "'");
    }
    else
    {
      // get all phonenums/groups_ids for all in identities
      d_database.exec("SELECT IFNULL(" + d_recipient_aci + ", '') AS uuid, IFNULL(" + d_recipient_e164 + ", '') AS phone, IFNULL(group_id, '') AS group_id FROM recipient WHERE _id IN (SELECT address FROM identities)", &results);
      for (uint i = 0; i < results.rows(); ++i)
      {
        RecipientIdentification rec_id = {results(i, "uuid"), results(i, "phone"), results(i, "group_id"), -1, std::string()};
        // source->d_database.exec("DELETE FROM identities WHERE address IN (SELECT _id FROM recipient WHERE COALESCE(uuid,phone,group_id) = '" + results.getValueAs<std::string>(i, 0) + "')");
        source->d_database.exec("DELETE FROM identities WHERE address IN (SELECT _id FROM recipient WHERE "
                                "(" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                                "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                                "(group_id IS NOT NULL AND group_id IS ?))", {rec_id.uuid, rec_id.phone, rec_id.group_id});
      }
    }

    // get recipient(_preferences) from target, drop all rows from source that are already present
    if (d_databaseversion < 24)
    {
      d_database.exec("SELECT recipient_ids FROM recipient_preferences", &results);
      for (uint i = 0; i < results.rows(); ++i)
        if (results.header(0) == "recipient_ids" && results.valueHasType<std::string>(i, 0))
          source->d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids = '" + results.getValueAs<std::string>(i, 0) + "'");
    }
    else
    {
      //d_database.exec("SELECT _id,COALESCE(uuid,phone,group_id) AS ident FROM recipient", &results);
      if (d_database.tableContainsColumn("recipient", "group_type") &&
          d_database.tableContainsColumn("recipient", d_recipient_storage_service))
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "group_type, " + d_recipient_storage_service + " FROM recipient", &results);
      else
        d_database.exec("SELECT _id, "
                        "IFNULL(" + d_recipient_aci + ", '') AS uuid, "
                        "IFNULL(" + d_recipient_e164 + ", '') AS phone, "
                        "IFNULL(group_id, '') AS group_id, "
                        "-1 AS 'group_type', NULL AS '" + d_recipient_storage_service + "' FROM recipient", &results);
      std::cout << "  updateRecipientIds (2)" << std::endl;
      for (uint i = 0; i < results.rows(); ++i)
      {
        // if the recipient is already in target, we are going to delete it from
        // source, to prevent doubles. However, many tables refer to the recipient._id
        // which was made unique above. If we just delete the doubles (by phone/group_id,
        // and in the future probably uuid), the fields in other tables will point
        // to random or non-existing recipients, so we need to remap them:
        RecipientIdentification rec_id = {results(i, "uuid"), results(i, "phone"), results(i, "group_id"), results.getValueAs<long long int>(i, "group_type"), results(i, d_recipient_storage_service)};
        source->updateRecipientId(results.getValueAs<long long int>(i, "_id"), rec_id);
        //source->updateRecipientId(results.getValueAs<long long int>(i, "_id"), results.getValueAs<std::string>(i, "ident"));

        // std::cout << "Testing if recipient is present:" << std::endl;
        // std::cout << "\"" << rec_id.uuid << "\" \"" << rec_id.phone << "\" \"" <<  rec_id.group_id << "\" \"" <<  rec_id.group_type << "\" \"" <<  rec_id.storage_service_key << "\"" << std::endl;

        // now drop the already present recipient from source.
        // source->d_database.exec("DELETE FROM recipient WHERE COALESCE(uuid,phone,group_id) = '" + results.getValueAs<std::string>(i, "ident") + "'");
        int count = 0;
        if (d_database.tableContainsColumn("recipient", "group_type"))
        {
          source->d_database.exec("DELETE FROM recipient WHERE "
                                  // one-of uuid/phone/group_id is set and equal to existing recipient
                                  "((" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                                  "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                                  "(group_id IS NOT NULL AND group_id IS ?)) OR "
                                  // none of uuid/phone/group_id are set, but storage_service_key exists and both are distribution lists?
                                  "(" + d_recipient_aci + " IS NULL AND "
                                  + d_recipient_e164 + " IS NULL AND "
                                  "group_id IS NULL AND "
                                  "group_type IS 4  AND group_type IS ? AND "
                                  + d_recipient_storage_service + " IS ?)", {rec_id.uuid, rec_id.phone, rec_id.group_id, rec_id.group_type, rec_id.storage_service});
          count = source->d_database.changed();
        }
        else
        {
          source->d_database.exec("DELETE FROM recipient WHERE "
                                  // one-of uuid/phone/group_id is set and equal to existing recipient
                                  "((" + d_recipient_aci + " IS NOT NULL AND " + d_recipient_aci + " IS ?) OR "
                                  "(" + d_recipient_e164 + " IS NOT NULL AND " + d_recipient_e164 + " IS ?) OR "
                                  "(group_id IS NOT NULL AND group_id IS ?))", {rec_id.uuid, rec_id.phone, rec_id.group_id});
          count = source->d_database.changed();
        }

        if (count)
          std::cout << "Dropped " << count << " existing recipients from source database" << std::endl;
      }
    }

    // even though the source was cropped to single thread, and this thread was not in target, avatar might still already be in target
    // because contact (and avatar) might be present in group in source, and only as one-on-one in target

    bool erased = true;
    while (erased)
    {
      erased = false;
      for (std::vector<std::pair<std::string, std::unique_ptr<AvatarFrame>>>::iterator sourceav = source->d_avatars.begin(); sourceav != source->d_avatars.end(); ++sourceav)
      {
        for (std::vector<std::pair<std::string, std::unique_ptr<AvatarFrame>>>::iterator targetav = d_avatars.begin(); targetav != d_avatars.end(); ++targetav)
          if (sourceav->first == targetav->first)
          {
            source->d_avatars.erase(sourceav);
            erased = true;
            break;
          }
        if (erased)
          break;
      }
    }
  }

  // delete group_membership's already present
  if (d_database.containsTable("group_membership"))
  {
    SqliteDB::QueryResults gm_results;
    d_database.exec("SELECT DISTINCT group_id,recipient_id FROM group_membership", &gm_results);
    for (uint i = 0; i < gm_results.rows(); ++i)
      source->d_database.exec("DELETE FROM group_membership WHERE group_id = ? AND recipient_id = ?",
                              {gm_results.value(i, "group_id"), gm_results.value(i, "recipient_id")});
  }

  // delete double call.call_id's (call_id is timestamp, this shouldn't naturally
  // happen, but does when merging threads that overlap in time)
  if (d_database.containsTable("call"))
  {
    SqliteDB::QueryResults call_results;
    d_database.exec("SELECT DISTINCT call_id FROM call", &call_results);
    for (uint i = 0; i < call_results.rows(); ++i)
      source->d_database.exec("DELETE FROM call WHERE call_id = ?",
                              call_results.value(i, "call_id"));
  }

  // // export database
  // std::cout << "Writing database..." << std::endl;
  // SqliteDB database("NEWSTYLE.2.sqlite", false);
  // if (!SqliteDB::copyDb(source->d_database, database))
  //   std::cout << "Error exporting sqlite database" << std::endl;
  // return;

  // now import the source tables into target,

  // get tables
  std::string q("SELECT sql, name, type FROM sqlite_master");
  source->d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.isNull(i, 0))
    {
      //std::cout << "Dealing with: " << results.getValueAs<std::string>(i, 1) << std::endl;
      if (results.valueHasType<std::string>(i, 1) &&
          (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
           STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sms_fts")))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is sms_ftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != d_mms_table + "_fts" &&
                STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), d_mms_table + "_fts")))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is mms_ftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "emoji_search" &&
                STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "emoji_search")))
        ;//std::cout << "Skipping " << results.getValueAs<std::string>(i, 1) << " because it is emoji_search_ftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               STRING_STARTS_WITH(results.getValueAs<std::string>(i, 1), "sqlite_"))
        ;
      else
        if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
        {
          tables.emplace_back(results.getValueAs<std::string>(i, 1));
          //std::cout << "Added: " << results.getValueAs<std::string>(i, 1) << std::endl;
        }
    }
  }

  // write contents of tables
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
        STRING_STARTS_WITH(table, d_mms_table + "_fts") ||
        STRING_STARTS_WITH(table, "sqlite_"))
      continue;
    std::cout << "Importing statements from source table '" << table << "'...";
    source->d_database.exec("SELECT * FROM " + table, &results);
    std::cout << results.rows() << " entries..." << std::endl;

    if (results.rows() == 0)
      continue;

    if (!d_database.containsTable(table))
    {
      std::cout << "  NOTE: Skipping table '" << table << "', as it is not present in target database. Data may be missing." << std::endl;
      continue;
    }

    // check if source contains columns not existing in target
    // even though target is newer, a fresh install would not have
    // created dropped columns that may still be present in
    // source database;
    uint idx = 0;
    while (idx < results.headers().size())
    {
      if (!d_database.tableContainsColumn(table, results.headers()[idx]))
      {
        std::cout << "  NOTE: Dropping column '" << table << "." << results.headers()[idx] << "' from source : Column not present in target database" << std::endl;
        if (results.removeColumn(idx))
          continue;
      }
      // else
      ++idx;
    }

    // if all columns were dropped, the entire table (probably) does not exist in target database, we'll just skip it
    // for instance, a (newly/currently?) created database will not have the megaphone table
    if (results.columns() == 0)
    {
      std::cout << "  NOTE: Skipping table '" << table << "', it has no columns left" << std::endl;
      continue;
    }

    for (uint i = 0; i < results.rows(); ++i)
    {
      // if (table == "identities")
      // {
      //   std::cout << "Trying to add: ";
      //   for (uint j = 0; j < results.columns(); ++j)
      //     std::cout << results.valueAsString(i, j) << " ";
      //   std::cout << std::endl;
      // }
      SqlStatementFrame newframe = buildSqlStatementFrame(table, results.headers(), results.row(i));
      d_database.exec(newframe.bindStatement(), newframe.parameters());
      //newframe.printInfo();
    }
  }

  // and copy avatars and attachments.
  for (auto &att : source->d_attachments)
    d_attachments.emplace(std::move(att));

  for (auto &av : source->d_avatars)
    d_avatars.emplace_back(std::move(av));

  /*
    THIS IS NOT TRUE, CURRENTLY THE RELEASECHANNEL
    THREAD FROM SOURCE IS SKIPPED UNCONDITIONALLY
    AND THE RELEASECHANNEL RECIPIENT FROM SOURCE
    IS REMOVED. ADDING THIS KEY WILL CAUSE TROUBLE

  // if target has no release channel-recipient, but
  // source does, it is copied over, we need the pref
  if (target_releasechannel == -1)
    for (auto &skv : source->d_keyvalueframes)
      if (skv->key() == "releasechannel.recipient_id")
      {
        d_keyvalueframes.emplace_back(std::move(skv));
        break;
      }
  */

  // update thread snippet and date and count
  updateThreadsEntries();

  d_database.exec("VACUUM");
  d_database.freeMemory();

  return checkDbIntegrity();
}
