#include "signalbackup.ih"

#include <regex>

bool SignalBackup::migrate_to_191()
{
  if (d_databaseversion < 132)
  {
    std::cout << "Sorry, db version too old. Not supported (yet?)" << std::endl;
    return false;
  }

  if (d_databaseversion < 133)
  {
    std::cout << "To 133" << std::endl;
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN allows_replies INTEGER DEFAULT 1"))
      return false;
  }

  if (d_databaseversion < 134)
  {
    std::cout << "To 134" << std::endl;
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN display_as_story INTEGER DEFAULT 0"))
      return false;
  }

  if (d_databaseversion < 135)
  {
    std::cout << "To 135" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_index ON mms (thread_id, date_received, is_story, parent_story_id)"))
      return false;
  }

  if (d_databaseversion < 136)
  {
    std::cout << "To 136" << std::endl;
    if (!d_database.exec("CREATE TABLE story_sends ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, allows_replies INTEGER NOT NULL )") ||
        !d_database.exec("CREATE INDEX story_sends_recipient_id_sent_timestamp_allows_replies_index ON story_sends (recipient_id, sent_timestamp, allows_replies)"))
      return false;
  }

  if (d_databaseversion < 137)
  {
    std::cout << "To 137" << std::endl;
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN deletion_timestamp INTEGER DEFAULT 0") ||
        !d_database.exec("UPDATE recipient SET group_type = 4 WHERE distribution_list_id IS NOT NULL") ||
        !d_database.exec("UPDATE distribution_list SET name = '00000000-0000-0000-0000-000000000000', distribution_id = '00000000-0000-0000-0000-000000000000' WHERE _id = 1"))
      return false;
  }

  if (d_databaseversion < 138)
  {
    std::cout << "To 138" << std::endl;
    if (!d_database.exec("UPDATE recipient SET storage_service_key = NULL WHERE distribution_list_id IS NOT NULL AND NOT EXISTS(SELECT _id from distribution_list WHERE _id = distribution_list_id)"))
      return false;
  }

  if (d_databaseversion < 139)
  {
    std::cout << "To 139" << std::endl;
    if (!d_database.exec("DELETE FROM storage_key WHERE type <= 4"))
      return false;
  }

  if (d_databaseversion < 140)
  {
    std::cout << "To 140" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS recipient_service_id_profile_key ON recipient (uuid, profile_key) WHERE uuid NOT NULL AND profile_key NOT NULL") ||
        !d_database.exec("CREATE TABLE cds ( _id INTEGER PRIMARY KEY, e164 TEXT NOT NULL UNIQUE ON CONFLICT IGNORE, last_seen_at INTEGER DEFAULT 0)"))
      return false;
  }

  if (d_databaseversion < 141)
  {
    std::cout << "To 141" << std::endl;
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN auth_service_id TEXT DEFAULT NULL"))
      return false;
  }

  if (d_databaseversion < 142)
  {
    std::cout << "To 142" << std::endl;
    if (!d_database.exec("ALTER TABLE mms ADD COLUMN quote_type INTEGER DEFAULT 0"))
      return false;
  }

  if (d_databaseversion < 143)
  {
    std::cout << "To 143" << std::endl;
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN is_unknown INTEGER DEFAULT 0"))
      return false;

    if (!d_database.exec("CREATE TABLE story_sends_tmp ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, allows_replies INTEGER NOT NULL, distribution_id TEXT NOT NULL REFERENCES distribution_list (distribution_id) ON DELETE CASCADE )"))
      return false;

    if (!d_database.exec("INSERT INTO story_sends_tmp (_id, message_id, recipient_id, sent_timestamp, allows_replies, distribution_id) SELECT story_sends._id, story_sends.message_id, story_sends.recipient_id, story_sends.sent_timestamp, story_sends.allows_replies, distribution_list.distribution_id FROM story_sends INNER JOIN mms ON story_sends.message_id = mms._id INNER JOIN distribution_list ON distribution_list.recipient_id = mms.address"))
      return false;

    if (!d_database.exec("DROP TABLE story_sends") ||
        !d_database.exec("DROP INDEX IF EXISTS story_sends_recipient_id_sent_timestamp_allows_replies_index") ||
        !d_database.exec("ALTER TABLE story_sends_tmp RENAME TO story_sends") ||
        !d_database.exec("CREATE INDEX story_sends_recipient_id_sent_timestamp_allows_replies_index ON story_sends (recipient_id, sent_timestamp, allows_replies)"))
      return false;
  }

  if (d_databaseversion < 144)
  {
    std::cout << "To 144" << std::endl;
    if (!d_database.exec("UPDATE mms SET read = 1 WHERE parent_story_id > 0"))
      return false;
  }

  if (d_databaseversion < 145)
  {
    std::cout << "To 145" << std::endl;
    if (!d_database.exec("DELETE FROM mms WHERE parent_story_id > 0 AND parent_story_id NOT IN (SELECT _id FROM mms WHERE remote_deleted = 0)"))
      return false;
  }

  if (d_databaseversion < 146)
  {
    std::cout << "To 146" << std::endl;
    if (!d_database.exec("CREATE TABLE remote_megaphone ( _id INTEGER PRIMARY KEY, uuid TEXT UNIQUE NOT NULL, priority INTEGER NOT NULL, countries TEXT, minimum_version INTEGER NOT NULL, dont_show_before INTEGER NOT NULL, dont_show_after INTEGER NOT NULL, show_for_days INTEGER NOT NULL, conditional_id TEXT, primary_action_id TEXT, secondary_action_id TEXT, image_url TEXT, image_uri TEXT DEFAULT NULL, title TEXT NOT NULL, body TEXT NOT NULL, primary_action_text TEXT, secondary_action_text TEXT, shown_at INTEGER DEFAULT 0, finished_at INTEGER DEFAULT 0)"))
      return false;
  }

  if (d_databaseversion < 147)
  {
    std::cout << "To 147" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_quote_id_quote_author_index ON mms (quote_id, quote_author)"))
      return false;
  }

  if (d_databaseversion < 148)
  {
    std::cout << "To 148" << std::endl;
    if (!d_database.exec("ALTER TABLE distribution_list ADD COLUMN privacy_mode INTEGER DEFAULT 0") ||
        !d_database.exec("UPDATE distribution_list SET privacy_mode = 1 WHERE _id = 1"))
      return false;

    if (!d_database.exec("CREATE TABLE distribution_list_member_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT, list_id INTEGER NOT NULL REFERENCES distribution_list (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id), privacy_mode INTEGER DEFAULT 0)") ||
        !d_database.exec("INSERT INTO distribution_list_member_tmp SELECT _id, list_id, recipient_id, 0 FROM distribution_list_member") ||
        !d_database.exec("DROP TABLE distribution_list_member") ||
        !d_database.exec("ALTER TABLE distribution_list_member_tmp RENAME TO distribution_list_member") ||
        !d_database.exec("UPDATE distribution_list_member SET privacy_mode = 1 WHERE list_id = 1") ||
        !d_database.exec("CREATE UNIQUE INDEX distribution_list_member_list_id_recipient_id_privacy_mode_index ON distribution_list_member (list_id, recipient_id, privacy_mode)"))
      return false;
  }

  if (d_databaseversion < 149)
  {
    std::cout << "To 149" << std::endl;
    if (!d_database.exec("UPDATE recipient SET profile_key_credential = NULL"))
      return false;
  }

  if (d_databaseversion < 150)
  {
    std::cout << "To 150" << std::endl;
    if (!d_database.exec("ALTER TABLE msl_payload ADD COLUMN urgent INTEGER NOT NULL DEFAULT 1"))
      return false;

    // setDBV 150
  }

  // NOTE THIS IS A DUPLICATE OF 153 FOR SOME REASON
  if (d_databaseversion < 151)
  {
    std::cout << "To 151" << std::endl;
    std::string mystory_dist_id = d_database.getSingleResultAs<std::string>("SELECT distribution_id FROM distribution_list WHERE _id = 1", std::string());

    if (mystory_dist_id == "00000000-0000-0000-0000-000000000000") // ok!
      ;
    else
    {
      if (mystory_dist_id.empty()) // need to create...
      {
        // get mystoryrecipient_id
        long long int mystory_rid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE distribution_list_id = 1", -1);
        if (mystory_rid == -1) // create
        {
          std::cout << "Unable to create new recipient" << std::endl;
          return false;
        }
        if (!d_database.exec("INSERT INTO distribution_list (_id, name, distribution_id, recipient_id, privacy_mode) "
                             "VALLUES "
                             "(?, ?, ?, ?, ?)",
                             {1, "00000000-0000-0000-0000-000000000000", "00000000-0000-0000-0000-000000000000", mystory_rid, 2}))
          return false;
      }
      else // wrong dist id
      {
        if (!d_database.exec("UPDATE distribution_list SET distribution_id = ? WHERE _id = 1", "00000000-0000-0000-0000-000000000000"))
          return false;
      }
    }

    // setDBV 151
  }

  if (d_databaseversion < 152)
  {
    std::cout << "To 152" << std::endl;
    if (!d_database.exec("UPDATE recipient SET group_type = 4 WHERE  distribution_list_id IS NOT NULL"))
      return false;

    // setDBV 152
  }


  // NOTE THIS IS A DUPLICATE OF 151 FOR SOME REASON
  if (d_databaseversion < 153)
  {
    std::cout << "To 153" << std::endl;
    std::string mystory_dist_id = d_database.getSingleResultAs<std::string>("SELECT distribution_id FROM distribution_list WHERE _id = 1", std::string());

    if (mystory_dist_id == "00000000-0000-0000-0000-000000000000") // ok!
      ;
    else
    {
      if (mystory_dist_id.empty()) // need to create...
      {
        // get mystoryrecipient_id
        long long int mystory_rid = d_database.getSingleResultAs<long long int>("SELECT _id FROM recipient WHERE distribution_list_id = 1", -1);
        if (mystory_rid == -1) // create
        {
          std::cout << "Unable to create new recipient" << std::endl;
          return false;
        }
        if (!d_database.exec("INSERT INTO distribution_list (_id, name, distribution_id, recipient_id, privacy_mode) "
                             "VALLUES "
                             "(?, ?, ?, ?, ?)",
                             {1, "00000000-0000-0000-0000-000000000000", "00000000-0000-0000-0000-000000000000", mystory_rid, 2}))
          return false;
      }
      else // wrong dist id
      {
        if (!d_database.exec("UPDATE distribution_list SET distribution_id = ? WHERE _id = 1", "00000000-0000-0000-0000-000000000000"))
          return false;
      }
    }


    // setDBV 153
  }

  if (d_databaseversion < 154)
  {
    std::cout << "To 154" << std::endl;
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN needs_pni_signature") ||
        !d_database.exec("CREATE TABLE pending_pni_signature_message (_id INTEGER PRIMARY KEY, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, sent_timestamp INTEGER NOT NULL, device_id INTEGER NOT NULL)") ||
        !d_database.exec("CREATE UNIQUE INDEX pending_pni_recipient_sent_device_index ON pending_pni_signature_message (recipient_id, sent_timestamp, device_id)"))
      return false;

    // setDBV154
  }


  if (d_databaseversion < 155)
  {
    std::cout << "To 155" << std::endl;
    if (!d_database.exec("ALTER TABLE mms ADD COLUMN export_state BLOB DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE mms ADD COLUMN exported INTEGER DEFAULT 0") ||
        !d_database.exec("ALTER TABLE sms ADD COLUMN export_state BLOB DEFAULT NULL") ||
        !d_database.exec("ALTER TABLE sms ADD COLUMN exported INTEGER DEFAULT 0"))
      return false;
    // setDBV155
  }


  if (d_databaseversion < 156)
  {
    std::cout << "To 156" << std::endl;
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN unregistered_timestamp INTEGER DEFAULT 0"))
      return false;

    // 2678400000 is suppoedly 31 days in millisecond
    if (!d_database.exec("UPDATE recipient SET unregistered_timestamp = ? WHERE registered = ? AND group_type = ?", {2678400000, 2, 0}))
      return false;

    // setDBV156
  }

  if (d_databaseversion < 157)
  {
    std::cout << "To 157" << std::endl;
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN hidden INTEGER DEFAULT 0"))
      return false;

    // setDBV157
  }

  if (d_databaseversion < 158)
  {
    std::cout << "To 158" << std::endl;
    if (!d_database.exec("ALTER TABLE groups ADD COLUMN last_force_update_timestamp INTEGER DEFAULT 0"))
      return false;

    // setDBV158
  }

  if (d_databaseversion < 159)
  {
    std::cout << "To 159" << std::endl;
    if (!d_database.exec("ALTER TABLE thread ADD COLUMN unread_self_mention_count INTEGER DEFAULT 0"))
      return false;

    // setDBV159
  }

  if (d_databaseversion < 160)
  {
    std::cout << "To 160" << std::endl;

    if (!d_database.exec("CREATE INDEX IF NOT EXISTS sms_exported_index ON sms (exported)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_exported_index ON mms (exported)"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 161)
  {
    std::cout << "To 161" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS story_sends_message_id_distribution_id_index ON story_sends (message_id, distribution_id)"))
      return false;

    // setDBV161
  }

  if (d_databaseversion < 162)
  {
    std::cout << "To 162" << std::endl;
    if (!d_database.tableContainsColumn("thread", "unread_self_mention_count"))
      if (!d_database.exec("ALTER TABLE thread ADD COLUMN unread_self_mention_count INTEGER DEFAULT 0"))
        return false;

    // setDBV162
  }

  if (d_databaseversion < 163)
  {
    std::cout << "To 163" << std::endl;
    if (!d_database.tableContainsColumn("remote_megaphone", "primary_action_data"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN primary_action_data TEXT DEFAULT NULL"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "secondary_action_data"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN secondary_action_data TEXT DEFAULT NULL"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "snoozed_at"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN snoozed_at INTEGER DEFAULT 0"))
        return false;
    if (!d_database.tableContainsColumn("remote_megaphone", "seen_count"))
      if (!d_database.exec("ALTER TABLE remote_megaphone ADD COLUMN seen_count INTEGER DEFAULT 0"))
        return false;

    // setDBV163
  }

  if (d_databaseversion < 164)
  {
    std::cout << "To 164" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS thread_read ON thread (read)"))
      return false;

    // setDBV164
  }

  if (d_databaseversion < 165)
  {
    std::cout << "To 165" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS mms_id_msg_box_payment_transactions_index ON mms (_id, msg_box) WHERE msg_box & 0x300000000 != 0"))
      return false;

    // setDBV165
  }

  if (d_databaseversion < 166)
  {
    std::cout << "To 166" << std::endl;
    if (d_database.tableContainsColumn("thread", "thread_recipient_id"))
    {
      // removeDuplicateThreadEntries(db)
      SqliteDB::QueryResults res;
      if (!d_database.exec("SELECT thread_recipient_id, COUNT(*) AS thread_count FROM thread GROUP BY thread_recipient_id HAVING thread_count > 1", &res))
        return false;
      for (uint i = 0; i < res.rows(); ++i)
      {
        long long int rid = res.valueAsInt(i, "recipient_id", -1);
        if (rid == -1)
          return false;

        SqliteDB::QueryResults res2;
        if (!d_database.exec("SELECT _id, date FROM thread WHERE thread_recipient_id = ? ORDER BY date DESC", rid, &res2))
          return false;

        long long int mainthread = res2.valueAsInt(0, "_id", -1);
        if (mainthread == -1)
          return false;

        for (uint j = 1; j < res2.rows(); ++j)
        {
          // merge into mainthread
          if (!d_database.exec("UPDATE drafts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE mms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE sms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE pending_retry_receipts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
              !d_database.exec("UPDATE remapped_threads SET new_id = ? WHERE new_id = ?", {mainthread, res2.value(j, "_id")}))
            return false;

          if (!d_database.exec("DELETE FROM thread WHERE _id = ?", res2.value(j, "_id")))
            return false;
        }
      }

      // updateThreadTableSchema(db)
      if (!d_database.exec("CREATE TABLE thread_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, meaningful_messages INTEGER DEFAULT 0,recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE,read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0,snippet TEXT, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type TEXT DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, unread_count INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, unread_self_mention_count INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO thread_tmp SELECT _id,date,message_count,thread_recipient_id,read,type,error,snippet,snippet_type,snippet_uri,snippet_content_type,snippet_extras,unread_count,archived,status,delivery_receipt_count,read_receipt_count,expires_in,last_seen,has_sent,last_scrolled,pinned,unread_self_mention_count FROM thread"))
        return false;

      if (!d_database.exec("DROP TABLE thread") ||
          !d_database.exec("ALTER TABLE thread_tmp RENAME TO thread") ||
          !d_database.exec("CREATE INDEX thread_recipient_id_index ON thread (recipient_id)") ||
          !d_database.exec("CREATE INDEX archived_count_index ON thread (archived, meaningful_messages)") ||
          !d_database.exec("CREATE INDEX thread_pinned_index ON thread (pinned)") ||
          !d_database.exec("CREATE INDEX thread_read ON thread (read)"))
        return false;

      // fixDanglingSmsMessages(db)
      if (!d_database.exec("DELETE FROM sms WHERE address IS NULL OR address NOT IN (SELECT _id FROM recipient)") ||
          !d_database.exec("DELETE FROM sms WHERE thread_id IS NULL OR thread_id NOT IN (SELECT _id FROM thread)"))
        return false;

      // fixDanglingMmsMessages(db)
      if (!d_database.exec("DELETE FROM mms WHERE address IS NULL OR address NOT IN (SELECT _id FROM recipient)") ||
          !d_database.exec("DELETE FROM mms WHERE thread_id IS NULL OR thread_id NOT IN (SELECT _id FROM thread)"))
        return false;

      // updateSmsTableSchema(db)
      if (!d_database.exec("CREATE TABLE sms_tmp (_id INTEGER PRIMARY KEY AUTOINCREMENT,date_sent INTEGER NOT NULL,date_received INTEGER NOT NULL,date_server INTEGER DEFAULT -1,thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE,recipient_id NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE,recipient_device_id INTEGER DEFAULT 1,type INTEGER,body TEXT,read INTEGER DEFAULT 0,status INTEGER DEFAULT -1,delivery_receipt_count INTEGER DEFAULT 0,mismatched_identities TEXT DEFAULT NULL,subscription_id INTEGER DEFAULT -1,expires_in INTEGER DEFAULT 0,expire_started INTEGER DEFAULT 0,notified INTEGER DEFAULT 0,read_receipt_count INTEGER DEFAULT 0,unidentified INTEGER DEFAULT 0,reactions_unread INTEGER DEFAULT 0,reactions_last_seen INTEGER DEFAULT -1,remote_deleted INTEGER DEFAULT 0,notified_timestamp INTEGER DEFAULT 0,server_guid TEXT DEFAULT NULL,receipt_timestamp INTEGER DEFAULT -1,export_state BLOB DEFAULT NULL,exported INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO sms_tmp SELECT _id, date_sent, date, date_server, thread_id, address, address_device_id, type, body, read, status, delivery_receipt_count, mismatched_identities, subscription_id, expires_in, expire_started, notified, read_receipt_count, unidentified, reactions_unread, reactions_last_seen, remote_deleted, notified_timestamp, server_guid, receipt_timestamp, export_state, exported FROM sms"))
        return false;

      if (!d_database.exec("DROP TABLE sms") ||
          !d_database.exec("ALTER TABLE sms_tmp RENAME TO sms") ||
          !d_database.exec("CREATE INDEX sms_read_and_notified_and_thread_id_index ON sms(read, notified, thread_id)") ||
          !d_database.exec("CREATE INDEX sms_type_index ON sms (type)") ||
          !d_database.exec("CREATE INDEX sms_date_sent_index ON sms (date_sent, recipient_id, thread_id)") ||
          !d_database.exec("CREATE INDEX sms_date_server_index ON sms (date_server)") ||
          !d_database.exec("CREATE INDEX sms_thread_date_index ON sms (thread_id, date_received)") ||
          !d_database.exec("CREATE INDEX sms_reactions_unread_index ON sms (reactions_unread)") ||
          !d_database.exec("CREATE INDEX sms_exported_index ON sms (exported)") ||
          !d_database.exec("CREATE TRIGGER sms_ai AFTER INSERT ON sms BEGIN INSERT INTO sms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER sms_ad AFTER DELETE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER sms_au AFTER UPDATE ON sms BEGIN INSERT INTO sms_fts(sms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO sms_fts(rowid, body, thread_id) VALUES(new._id, new.body, new.thread_id); END;") ||
          !d_database.exec("CREATE TRIGGER msl_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 0); END"))
        return false;

      // updateMmsTableSchema(db)
      if (!d_database.exec("CREATE TABLE mms_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, date_server INTEGER DEFAULT -1, thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE, recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, recipient_device_id INTEGER, type INTEGER NOT NULL, body TEXT, read INTEGER DEFAULT 0, ct_l TEXT, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, subscription_id INTEGER DEFAULT -1, receipt_timestamp INTEGER DEFAULT -1, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, viewed_receipt_count INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author INTEGER DEFAULT 0, quote_body TEXT DEFAULT NULL, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, quote_type INTEGER DEFAULT 0, shared_contacts TEXT DEFAULT NULL, unidentified INTEGER DEFAULT 0, link_previews TEXT DEFAULT NULL, view_once INTEGER DEFAULT 0, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL, message_ranges BLOB DEFAULT NULL, story_type INTEGER DEFAULT 0, parent_story_id INTEGER DEFAULT 0, export_state BLOB DEFAULT NULL, exported INTEGER DEFAULT 0)"))
        return false;
      if (!d_database.exec("INSERT INTO mms_tmp SELECT _id, date, date_received, date_server, thread_id, address, address_device_id, msg_box, body, read, ct_l, exp, m_type, m_size, st, tr_id, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, viewed_receipt_count, mismatched_identities, network_failures, expires_in, expire_started, notified, quote_id, quote_author, quote_body, quote_missing, quote_mentions, quote_type, shared_contacts, unidentified, previews, reveal_duration, reactions_unread, reactions_last_seen, remote_deleted, mentions_self, notified_timestamp, server_guid, ranges, is_story, parent_story_id, export_state, exported FROM mms"))
        return false;

      if (!d_database.exec("DROP TABLE mms") ||
          !d_database.exec("ALTER TABLE mms_tmp RENAME TO mms") ||
          !d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
          !d_database.exec("CREATE INDEX mms_type_index ON mms (type)") ||
          !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (date_sent, recipient_id, thread_id)") ||
          !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
          !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
          !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_story_type_index ON mms (story_type)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_parent_story_id_index ON mms (parent_story_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_index ON mms (thread_id, date_received, story_type, parent_story_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_quote_id_quote_author_index ON mms (quote_id, quote_author)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_exported_index ON mms (exported)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS mms_id_type_payment_transactions_index ON mms (_id, type) WHERE type & 0x300000000 != 0") ||
          !d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER mms_ad AFTER DELETE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER mms_au AFTER UPDATE ON mms BEGIN INSERT INTO mms_fts(mms_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO mms_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END") ||
          !d_database.exec("CREATE TRIGGER msl_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id AND is_mms = 1); END"))
        return false;
    }
    // setDBV166
  }

  if (d_databaseversion < 167)
  {
    std::cout << "To 167" << std::endl;
    if (!d_database.exec("DELETE FROM reaction  WHERE (is_mms = 0 AND message_id NOT IN (SELECT _id FROM sms)) OR(is_mms = 1 AND message_id NOT IN (SELECT _id FROM mms))") ||
        !d_database.exec("CREATE TRIGGER IF NOT EXISTS reactions_sms_delete AFTER DELETE ON sms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 0; END") ||
        !d_database.exec("CREATE TRIGGER IF NOT EXISTS reactions_mms_delete AFTER DELETE ON mms BEGIN DELETE FROM reaction WHERE message_id = old._id AND is_mms = 1; END"))
      return false;
    // setDBV167
  }


  if (d_databaseversion < 168)
  {
    std::cout << "To 168" << std::endl;

    if (!d_database.exec("DROP TRIGGER msl_sms_delete") ||
        !d_database.exec("DROP TRIGGER reactions_sms_delete") ||
        !d_database.exec("DROP TRIGGER sms_ai") ||
        !d_database.exec("DROP TRIGGER sms_au") ||
        !d_database.exec("DROP TRIGGER sms_ad") ||
        !d_database.exec("DROP TABLE sms_fts") ||
        !d_database.exec("DROP INDEX mms_read_and_notified_and_thread_id_index") ||
        !d_database.exec("DROP INDEX mms_type_index") ||
        !d_database.exec("DROP INDEX mms_date_sent_index") ||
        !d_database.exec("DROP INDEX mms_date_server_index") ||
        !d_database.exec("DROP INDEX mms_thread_date_index") ||
        !d_database.exec("DROP INDEX mms_reactions_unread_index") ||
        !d_database.exec("DROP INDEX mms_story_type_index") ||
        !d_database.exec("DROP INDEX mms_parent_story_id_index") ||
        !d_database.exec("DROP INDEX mms_thread_story_parent_story_index") ||
        !d_database.exec("DROP INDEX mms_quote_id_quote_author_index") ||
        !d_database.exec("DROP INDEX mms_exported_index") ||
        !d_database.exec("DROP INDEX mms_id_type_payment_transactions_index") ||
        !d_database.exec("DROP TRIGGER mms_ai"))
      return false;

    // copySmsToMms(db, nextMmsId)
    SqliteDB::QueryResults minmax;
    if (!d_database.exec("SELECT MIN(_id) AS min, MAX(_id) AS max FROM sms", &minmax))
      return false;
    long long int minsms = minmax.getValueAs<long long int>(0, "min");
    if (!d_database.exec("SELECT MIN(_id) AS min, MAX(_id) AS max FROM mms", &minmax))
      return false;
    long long int maxmms = minmax.getValueAs<long long int>(0, "max");

    long long int id_offset = (maxmms - minsms) + 1;

    if (!d_database.exec("INSERT INTO mms SELECT _id + ?, date_sent, date_received, date_server, thread_id, recipient_id, recipient_device_id, type, body, read, null, 0, 0, 0, status, null, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, 0, mismatched_identities, null, expires_in, expire_started, notified, 0, 0, null, 0, null, 0, null, unidentified, null, 0, reactions_unread, reactions_last_seen, remote_deleted, 0, notified_timestamp, server_guid, null, 0, 0, export_state, exported FROM sms", id_offset))
      return false;

    if (!d_database.exec("DROP TABLE sms"))
      return false;

    if (!d_database.exec("CREATE INDEX mms_read_and_notified_and_thread_id_index ON mms(read, notified, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_type_index ON mms (type)") ||
        !d_database.exec("CREATE INDEX mms_date_sent_index ON mms (date_sent, recipient_id, thread_id)") ||
        !d_database.exec("CREATE INDEX mms_date_server_index ON mms (date_server)") ||
        !d_database.exec("CREATE INDEX mms_thread_date_index ON mms (thread_id, date_received)") ||
        !d_database.exec("CREATE INDEX mms_reactions_unread_index ON mms (reactions_unread)") ||
        !d_database.exec("CREATE INDEX mms_story_type_index ON mms (story_type)") ||
        !d_database.exec("CREATE INDEX mms_parent_story_id_index ON mms (parent_story_id)") ||
        !d_database.exec("CREATE INDEX mms_thread_story_parent_story_index ON mms (thread_id, date_received, story_type, parent_story_id)") ||
        !d_database.exec("CREATE INDEX mms_quote_id_quote_author_index ON mms (quote_id, quote_author)") ||
        !d_database.exec("CREATE INDEX mms_exported_index ON mms (exported)") ||
        !d_database.exec("CREATE INDEX mms_id_type_payment_transactions_index ON mms (_id, type) WHERE type & 0x300000000 != 0") ||
        !d_database.exec("CREATE TRIGGER mms_ai AFTER INSERT ON mms BEGIN INSERT INTO mms_fts (rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;"))
      return false;

    if (!d_database.exec("UPDATE reaction SET message_id = message_id + ? WHERE is_mms = 0", id_offset) ||
        !d_database.exec("UPDATE msl_message SET message_id = message_id + ? WHERE is_mms = 0", id_offset))
      return false;

    // setDBV168
  }

  if (d_databaseversion < 169)
  {
    std::cout << "To 169" << std::endl;
    if (!d_database.exec("CREATE TABLE emoji_search_tmp ( _id INTEGER PRIMARY KEY, label TEXT NOT NULL, emoji TEXT NOT NULL, rank INTEGER DEFAULT 2147483647)" /*int.MAX_VALUE*/) ||
        !d_database.exec("INSERT INTO emoji_search_tmp (label, emoji) SELECT label, emoji from emoji_search") ||
        !d_database.exec("DROP TABLE emoji_search") ||
        !d_database.exec("ALTER TABLE emoji_search_tmp RENAME TO emoji_search") ||
        !d_database.exec("CREATE INDEX emoji_search_rank_covering ON emoji_search (rank, label, emoji)"))
      return false;
    // setDBV169
  }

  if (d_databaseversion < 170)
  {
    std::cout << "To 170" << std::endl;
    if (!d_database.exec("CREATE TABLE call ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL UNIQUE, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, peer INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL)") ||
        !d_database.exec("CREATE INDEX call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX call_message_id_index ON call (message_id)"))
      return false;
    // setDBV170
  }

  if (d_databaseversion < 171)
  {
    std::cout << "To 171" << std::endl;

    // removeDuplicateThreadEntries // almost (no sms) duplicates 166
    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT recipient_id, COUNT(*) AS thread_count FROM thread GROUP BY recipient_id HAVING thread_count > 1", &res))
      return false;

    for (uint i = 0; i < res.rows(); ++i)
    {
      long long int rid = res.valueAsInt(i, "recipient_id", -1);
      if (rid == -1)
        return false;


      SqliteDB::QueryResults res2;
      if (!d_database.exec("SELECT _id, date FROM thread WHERE recipient_id = ? ORDER BY date DESC", rid, &res2))
        return false;

      long long int mainthread = res2.valueAsInt(0, "recipient_id", -1);
      if (mainthread == -1)
        return false;

      for (uint j = 1; j < res2.rows(); ++j)
      {
        // merge into mainthread
        if (!d_database.exec("UPDATE drafts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE mention SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE mms SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE pending_retry_receipts SET thread_id = ? WHERE thread_id = ?", {mainthread, res2.value(j, "_id")}) ||
            !d_database.exec("UPDATE remapped_threads SET new_id = ? WHERE new_id = ?", {mainthread, res2.value(j, "_id")}))
          return false;

        if (!d_database.exec("DELETE FROM thread WHERE _id = ?", res2.value(j, "_id")))
          return false;
      }
    }

    // updateThreadTableSchema(db)
    if (!d_database.exec("CREATE TABLE thread_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER DEFAULT 0, meaningful_messages INTEGER DEFAULT 0, recipient_id INTEGER NOT NULL UNIQUE REFERENCES recipient (_id) ON DELETE CASCADE, read INTEGER DEFAULT 1, type INTEGER DEFAULT 0, error INTEGER DEFAULT 0, snippet TEXT, snippet_type INTEGER DEFAULT 0, snippet_uri TEXT DEFAULT NULL, snippet_content_type TEXT DEFAULT NULL, snippet_extras TEXT DEFAULT NULL, unread_count INTEGER DEFAULT 0, archived INTEGER DEFAULT 0, status INTEGER DEFAULT 0, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, expires_in INTEGER DEFAULT 0, last_seen INTEGER DEFAULT 0, has_sent INTEGER DEFAULT 0, last_scrolled INTEGER DEFAULT 0, pinned INTEGER DEFAULT 0, unread_self_mention_count INTEGER DEFAULT 0)"))
      return false;

    if (!d_database.exec("INSERT INTO thread_tmp SELECT _id, date, meaningful_messages, recipient_id, read, type, error, snippet, snippet_type, snippet_uri, snippet_content_type, snippet_extras, unread_count, archived, status, delivery_receipt_count, read_receipt_count, expires_in, last_seen, has_sent, last_scrolled, pinned, unread_self_mention_count FROM thread"))
      return false;

    if (!d_database.exec("DROP TABLE thread") ||
        !d_database.exec("ALTER TABLE thread_tmp RENAME TO thread") ||
        !d_database.exec("CREATE INDEX thread_recipient_id_index ON thread (recipient_id)") ||
        !d_database.exec("CREATE INDEX archived_count_index ON thread (archived, meaningful_messages)") ||
        !d_database.exec("CREATE INDEX thread_pinned_index ON thread (pinned)") ||
        !d_database.exec("CREATE INDEX thread_read ON thread (read)"))
      return false;

    // setDBV171
  }

  if (d_databaseversion < 172)
  {
    std::cout << "To 172" << std::endl;
    if (!d_database.exec("CREATE TABLE group_membership (_id INTEGER PRIMARY KEY,group_id TEXT NOT NULL,recipient_id INTEGER NOT NULL,UNIQUE(group_id, recipient_id))"))
      return false;

    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT members, group_id FROM groups", &res))
      return false;

    for (uint i = 0; i < res.rows(); ++i)
    {
      std::string group_id(res(i, "group_id"));
      if (group_id.empty())
        return false;

      std::string membersstring(res.valueAsString(i, "members"));
      std::regex comma(",");
      std::sregex_token_iterator iter(membersstring.begin(), membersstring.end(), comma, -1);
      std::vector<long long int> members_list;
      std::transform(iter, std::sregex_token_iterator(), std::back_inserter(members_list),
                     [](std::string const &m) -> long long int { return bepaald::toNumber<long long int>(m); });

      for (uint j = 0; j < members_list.size(); ++j)
        if (!d_database.exec("INSERT INTO group_membership (group_id, recipient_id) VALUES (?, ?)", {group_id, members_list[j]}))
          return false;
    }

    if (!d_database.exec("ALTER TABLE groups DROP COLUMN members"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 173)
  {
    std::cout << "To 173" << std::endl;

    if (!d_database.exec("ALTER TABLE mms ADD COLUMN scheduled_date INTEGER DEFAULT -1") ||
        !d_database.exec("DROP INDEX mms_thread_story_parent_story_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS mms_thread_story_parent_story_scheduled_date_index ON mms (thread_id, date_received,story_type,parent_story_id,scheduled_date);"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 174)
  {
    std::cout << "To 174" << std::endl;
    if (!d_database.exec("CREATE TABLE reaction_tmp ( _id INTEGER PRIMARY KEY, message_id INTEGER NOT NULL REFERENCES mms (_id) ON DELETE CASCADE, author_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, emoji TEXT NOT NULL, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, UNIQUE(message_id, author_id) ON CONFLICT REPLACE )"))
      return false;
    if (!d_database.exec("INSERT INTO reaction_tmp SELECT _id, message_id, author_id, emoji, date_sent, date_received FROM reaction WHERE message_id IN (SELECT _id FROM mms)"))
      return false;

    if (!d_database.exec("DROP TABLE reaction") ||
        !d_database.exec("DROP TRIGGER IF EXISTS reactions_mms_delete") ||
        !d_database.exec("ALTER TABLE reaction_tmp RENAME TO reaction") ||
        !d_database.exec("ALTER TABLE mms RENAME TO message"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 175)
  {
    std::cout << "To 175" << std::endl;

    if (!d_database.exec("DROP TABLE mms_fts") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_ai") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_ad") ||
        !d_database.exec("DROP TRIGGER IF EXISTS mms_au") ||
        !d_database.exec("CREATE VIRTUAL TABLE message_fts USING fts5(body, thread_id UNINDEXED, content=message, content_rowid=_id)") ||
        !d_database.exec("CREATE TRIGGER message_ai AFTER INSERT ON message BEGIN INSERT INTO message_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER message_ad AFTER DELETE ON message BEGIN INSERT INTO message_fts(message_fts, rowid, body, thread_id) VALUES ('delete', old._id, old.body, old.thread_id); END;") ||
        !d_database.exec("CREATE TRIGGER message_au AFTER UPDATE ON message BEGIN INSERT INTO message_fts(message_fts, rowid, body, thread_id) VALUES('delete', old._id, old.body, old.thread_id); INSERT INTO message_fts(rowid, body, thread_id) VALUES (new._id, new.body, new.thread_id); END;"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 176)
  {
    std::cout << "To 176" << std::endl;
    if (!d_database.exec("DROP INDEX IF EXISTS mms_quote_id_quote_author_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_index ON message (quote_id, quote_author, scheduled_date);"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 177)
  {
    std::cout << "To 177" << std::endl;
    if (!d_database.exec("DROP TRIGGER IF EXISTS msl_mms_delete") ||
        !d_database.exec("DROP TRIGGER IF EXISTS msl_attachment_delete") ||
        !d_database.exec("DROP INDEX IF EXISTS msl_message_message_index"))
      return false;

    if (!d_database.exec("DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id NOT IN (SELECT _id FROM message))") ||
        !d_database.exec("CREATE TABLE msl_message_tmp (_id INTEGER PRIMARY KEY,payload_id INTEGER NOT NULL REFERENCES msl_payload (_id) ON DELETE CASCADE,message_id INTEGER NOT NULL)") ||
        !d_database.exec("INSERT INTO msl_message_tmp SELECT _id, payload_id, message_id FROM msl_message") ||
        !d_database.exec("DROP TABLE msl_message") ||
        !d_database.exec("ALTER TABLE msl_message_tmp RENAME TO msl_message") ||
        !d_database.exec("CREATE INDEX msl_message_message_index ON msl_message (message_id, payload_id)") ||
        !d_database.exec("CREATE TRIGGER msl_message_delete AFTER DELETE ON message BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old._id); END") ||
        !d_database.exec("CREATE TRIGGER msl_attachment_delete AFTER DELETE ON part BEGIN DELETE FROM msl_payload WHERE _id IN (SELECT payload_id FROM msl_message WHERE message_id = old.mid); END"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 178)
  {
    std::cout << "To 178" << std::endl;
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN reporting_token BLOB DEFAULT NULL"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 179)
  {
    std::cout << "To 179" << std::endl;
    if (!d_database.exec("DELETE FROM msl_message WHERE payload_id NOT IN (SELECT _id FROM msl_payload)") ||
        !d_database.exec("DELETE FROM msl_recipient WHERE payload_id NOT IN (SELECT _id FROM msl_payload)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 180)
  {
    std::cout << "To 180" << std::endl;
    if (!d_database.exec("ALTER TABLE recipient ADD COLUMN system_nickname TEXT DEFAULT NULL"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 181)
  {
    std::cout << "To 181" << std::endl;
    if (!d_database.exec("DELETE FROM thread WHERE recipient_id NOT IN (SELECT _id FROM recipient)"))
      return false;
    if (d_database.changed())
    {
      if (!d_database.exec("DELETE FROM message WHERE thread_id NOT IN (SELECT _id FROM thread)"))
        return false;
      if (d_database.changed())
      {
        if (!d_database.exec("DELETE FROM story_send  WHERE message_id NOT IN (SELECT _id FROM message)") ||
            !d_database.exec("DELETE FROM reaction WHERE message_id NOT IN (SELECT _id FROM message)") ||
            !d_database.exec("DELETE FROM call WHERE message_id NOT IN (SELECT _id FROM message)"))
          return false;
      }
    }
    // setDBV
  }


  if (d_databaseversion < 182)
  {
    std::cout << "To 182" << std::endl;
    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL UNIQUE, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0 )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, type, direction, event, (SELECT date_sent FROM message WHERE message._id = call.message_id) as timestamp, NULL as ringer, 0 as deletion_timestamp FROM call"))
      return false;

    if (!d_database.exec("DROP TABLE group_call_ring") ||
        !d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 183)
  {
    std::cout << "To 183" << std::endl;

    if (!d_database.exec("CREATE TABLE call_link (_id INTEGER PRIMARY KEY)"))
      return false;

    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link INTEGER DEFAULT NULL REFERENCES call_link (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)) )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;

    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call"))
      return false;
    // setDBV
  }

  if (d_databaseversion < 184)
  {
    std::cout << "To 184" << std::endl;
    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link INTEGER DEFAULT NULL REFERENCES call_link (_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (call_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)) )"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;


    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call") ||
        !d_database.exec("CREATE INDEX call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX call_message_id_index ON call (message_id)"))
      return false;

    // setDBV184
  }

  if (d_databaseversion < 185)
  {
    std::cout << "To 185" << std::endl;

    // first part gets selfid
    if (d_selfid == -1)
      d_selfid = scanSelf();
    if (d_selfid == -1)
    {
      std::cout << "Need self id, use `--setselfid'" << std::endl;
      return false;
    }

    //getdeps
    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT type, name, sql FROM sqlite_schema WHERE tbl_name = 'message' AND type != 'table'", &res))
      return false;
    for (uint i = 0; i < res.rows(); ++i)
      if (!d_database.exec("DROP " + res(i, "type") + " IF EXISTS " + res(i, "name")))
        return false;

    // redo message table
    if (!d_database.exec("CREATE TABLE message_tmp ( _id INTEGER PRIMARY KEY AUTOINCREMENT, date_sent INTEGER NOT NULL, date_received INTEGER NOT NULL, date_server INTEGER DEFAULT -1, thread_id INTEGER NOT NULL REFERENCES thread (_id) ON DELETE CASCADE, from_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, from_device_id INTEGER, to_recipient_id INTEGER NOT NULL REFERENCES recipient (_id) ON DELETE CASCADE, type INTEGER NOT NULL, body TEXT, read INTEGER DEFAULT 0, ct_l TEXT, exp INTEGER, m_type INTEGER, m_size INTEGER, st INTEGER, tr_id TEXT, subscription_id INTEGER DEFAULT -1, receipt_timestamp INTEGER DEFAULT -1, delivery_receipt_count INTEGER DEFAULT 0, read_receipt_count INTEGER DEFAULT 0, viewed_receipt_count INTEGER DEFAULT 0, mismatched_identities TEXT DEFAULT NULL, network_failures TEXT DEFAULT NULL, expires_in INTEGER DEFAULT 0, expire_started INTEGER DEFAULT 0, notified INTEGER DEFAULT 0, quote_id INTEGER DEFAULT 0, quote_author INTEGER DEFAULT 0, quote_body TEXT DEFAULT NULL, quote_missing INTEGER DEFAULT 0, quote_mentions BLOB DEFAULT NULL, quote_type INTEGER DEFAULT 0, shared_contacts TEXT DEFAULT NULL, unidentified INTEGER DEFAULT 0, link_previews TEXT DEFAULT NULL, view_once INTEGER DEFAULT 0, reactions_unread INTEGER DEFAULT 0, reactions_last_seen INTEGER DEFAULT -1, remote_deleted INTEGER DEFAULT 0, mentions_self INTEGER DEFAULT 0, notified_timestamp INTEGER DEFAULT 0, server_guid TEXT DEFAULT NULL, message_ranges BLOB DEFAULT NULL, story_type INTEGER DEFAULT 0, parent_story_id INTEGER DEFAULT 0, export_state BLOB DEFAULT NULL, exported INTEGER DEFAULT 0, scheduled_date INTEGER DEFAULT -1, latest_revision_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, original_message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE CASCADE, revision_number INTEGER DEFAULT 0 )"))
      return false;

    if (!d_database.exec("INSERT INTO message_tmp SELECT _id, date_sent, date_received, date_server, thread_id, recipient_id, recipient_device_id, recipient_id, type, body, read, ct_l, exp, m_type, m_size, st, tr_id, subscription_id, receipt_timestamp, delivery_receipt_count, read_receipt_count, viewed_receipt_count, mismatched_identities, network_failures, expires_in, expire_started, notified, quote_id, quote_author, quote_body, quote_missing, quote_mentions, quote_type, shared_contacts, unidentified, link_previews, view_once, reactions_unread, reactions_last_seen, remote_deleted, mentions_self, notified_timestamp, server_guid, message_ranges, story_type, parent_story_id, export_state, exported, scheduled_date, NULL AS latest_revision_id, NULL AS original_message_id, 0 as revision_number FROM message"))
      return false;

    if (!d_database.exec("UPDATE message_tmp SET to_recipient_id = from_recipient_id,from_recipient_id = ?,from_device_id = 1 WHERE (type & 0x1f IN (21, 23, 22, 24, 25, 26, 2, 11))", d_selfid))
      return false;

    if (!d_database.exec("DROP TABLE message") ||
        !d_database.exec("ALTER TABLE message_tmp RENAME TO message"))
      return false;

    for (uint i = 0; i < res.rows(); ++i)
    {
      if (res(i, "name") == "mms_thread_story_parent_story_scheduled_date_index")
      {
        if (!d_database.exec("CREATE INDEX message_thread_story_parent_story_scheduled_date_latest_revision_id_index ON message (thread_id, date_received, story_type, parent_story_id, scheduled_date, latest_revision_id)"))
          return false;
      }
      else if (res(i, "name") == "mms_quote_id_quote_author_scheduled_date_index")
      {
        if (!d_database.exec("CREATE INDEX message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
          return false;
      }
      else if (res(i, "name") == "mms_date_sent_index")
      {
        if (!d_database.exec("CREATE INDEX message_date_sent_from_to_thread_index ON message (date_sent, from_recipient_id, to_recipient_id, thread_id)"))
          return false;
      }
      else
      {
        std::string statement(res(i, "sql"));
        if (!statement.empty())
        {
          statement = std::regex_replace(statement, std::regex("CREATE INDEX mms_"), "CREATE INDEX message_");

          // dont actually see the altered statements being executed...
          if (!d_database.exec(statement))
            return false;
        }
      }
    }

    if (!checkDbIntegrity())
      return false;

    // setDBV
  }

  if (d_databaseversion < 186)
  {
    std::cout << "To 186" << std::endl;
    if (d_database.tableContainsColumn("message", "from_recipient_id"))
    {
      if (!d_database.exec("CREATE INDEX IF NOT EXISTS message_original_message_id_index ON message (original_message_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_latest_revision_id_index ON message (latest_revision_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_from_recipient_id_index ON message (from_recipient_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_to_recipient_id_index ON message (to_recipient_id)") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS reaction_author_id_index ON reaction (author_id)") ||
          !d_database.exec("DROP INDEX IF EXISTS message_quote_id_quote_author_scheduled_date_index") ||
          !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
        return false;

      if (!d_database.exec("ANALYZE message"))
        return false;
    }
    // setDBV
  }

  if (d_databaseversion < 187)
  {
    std::cout << "To 187" << std::endl;
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS call_call_link_index ON call (call_link)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_peer_index ON call (peer)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS distribution_list_member_recipient_id ON distribution_list_member (recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS msl_message_payload_index ON msl_message (payload_id)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 188)
  {
    std::cout << "To 188" << std::endl;
    if (!d_database.tableContainsColumn("message", "from_recipient_id"))
    {
      std::cout << "This should not happen... " << std::endl;
      return false;
    }

    // These are the indexes that should have been created in V186 -- conditionally done here in case it didn't run properly
    if (!d_database.exec("CREATE INDEX IF NOT EXISTS message_original_message_id_index ON message (original_message_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_latest_revision_id_index ON message (latest_revision_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_from_recipient_id_index ON message (from_recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_to_recipient_id_index ON message (to_recipient_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS reaction_author_id_index ON reaction (author_id)") ||
        !d_database.exec("DROP INDEX IF EXISTS message_quote_id_quote_author_scheduled_date_index") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS message_quote_id_quote_author_scheduled_date_latest_revision_id_index ON message (quote_id, quote_author, scheduled_date, latest_revision_id)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 189)
  {
    std::cout << "To 189" << std::endl;
    if (!d_database.exec("CREATE TABLE call_link_tmp ( _id INTEGER PRIMARY KEY, root_key BLOB NOT NULL, room_id TEXT NOT NULL UNIQUE, admin_key BLOB, name TEXT NOT NULL, restrictions INTEGER NOT NULL, revoked INTEGER NOT NULL, expiration INTEGER NOT NULL, avatar_color TEXT NOT NULL )"))
      return false;

    if (!d_database.exec("CREATE TABLE call_tmp ( _id INTEGER PRIMARY KEY, call_id INTEGER NOT NULL, message_id INTEGER DEFAULT NULL REFERENCES message (_id) ON DELETE SET NULL, peer INTEGER DEFAULT NULL REFERENCES recipient (_id) ON DELETE CASCADE, call_link TEXT DEFAULT NULL REFERENCES call_link (room_id) ON DELETE CASCADE, type INTEGER NOT NULL, direction INTEGER NOT NULL, event INTEGER NOT NULL, timestamp INTEGER NOT NULL, ringer INTEGER DEFAULT NULL, deletion_timestamp INTEGER DEFAULT 0, UNIQUE (call_id, peer, call_link) ON CONFLICT FAIL, CHECK ((peer IS NULL AND call_link IS NOT NULL) OR (peer IS NOT NULL AND call_link IS NULL)))"))
      return false;

    if (!d_database.exec("INSERT INTO call_tmp SELECT _id, call_id, message_id, peer, NULL as call_link, type, direction, event, timestamp, ringer, deletion_timestamp FROM call"))
      return false;


    if (!d_database.exec("DROP TABLE call") ||
        !d_database.exec("ALTER TABLE call_tmp RENAME TO call") ||
        !d_database.exec("DROP TABLE call_link") ||
        !d_database.exec("ALTER TABLE call_link_tmp RENAME TO call_link") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_call_id_index ON call (call_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_message_id_index ON call (message_id)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_call_link_index ON call (call_link)") ||
        !d_database.exec("CREATE INDEX IF NOT EXISTS call_peer_index ON call (peer)"))
      return false;

    // setDBV
  }

  if (d_databaseversion < 190)
  {
    std::cout << "To 190" << std::endl;
    // (yes this one's empty, it was buggy and replaced by 191
    // setDBV
  }

  if (d_databaseversion < 191)
  {
    std::cout << "To 191" << std::endl;

    if (!d_database.exec("DROP INDEX IF EXISTS message_unique_sent_from_thread"))
      return false;

    // change date for bad decrypt, expiration_timer_update and chat_session_refreshed duplicates
    if (!d_database.exec("WITH needs_update AS "
                         "("
                         "  SELECT _id FROM message M WHERE "
                         "  ("
                         "    type & 0x40000 != 0 OR "
                         "    type & 0x10000000 != 0 OR "
                         "    type = 13"
                         "  )"
                         "  AND "
                         "  ("
                         "    SELECT COUNT(*) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id"
                         "  ) > 1"
                         ") "
                         "UPDATE message SET date_sent = date_sent - 1 WHERE _id IN needs_update"))
      return false;

    // delete other duplicates if body is identical and type = groupupdate
    if (!d_database.exec("WITH needs_delete AS "
                         "("
                         "  SELECT _id FROM message M WHERE "
                         "  _id > "
                         "  ("
                         "    SELECT min(_id) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id AND "
                         "      ( "
                         "        COALESCE(body, '') = COALESCE(M.body, '') OR "
                         "        type & 0x10000 != 0"
                         "      )"
                         "  )"
                         ") "
                         "DELETE FROM message WHERE _id IN needs_delete"))
      return false;


    // #warning !!! REMOVE THIS !!!
    // SqliteDB::QueryResults res2;
    // if (d_database.exec("SELECT type, date_received, date_sent, from_recipient_id, to_recipient_id, thread_id, body FROM message WHERE _id IN (6851,6852,6853,6867,6868,6869,6803,6804,6805)", &res2))
    // {
    //   for (uint i = 0; i < 5; ++i)
    //     for (uint j = 0; j < res2.rows(); ++j)
    //       if (d_database.exec("INSERT INTO message (type, date_received, date_sent, from_recipient_id, to_recipient_id, thread_id, body) VALUES (?, ?, ?, ?, ?, ?, ?)",
    //                           {res2.value(j, "type"), res2.value(j, "date_received"), res2.value(0, "date_sent"), res2.value(j, "from_recipient_id"), res2.value(j, "to_recipient_id"), res2.value(j, "thread_id"), res2.valueAsString(j, "body") + " " + bepaald::toString(i + 1)}))
    //         ;//std::cout << "added some dupes" << std::endl;
    // }


    //findRemainingDuplicates(db)
    SqliteDB::QueryResults res;
    if (!d_database.exec("WITH dupes AS "
                         "("
                         "  SELECT _id, date_sent, from_recipient_id, thread_id, type FROM message M WHERE "
                         "  ("
                         "    SELECT COUNT(*) FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                         "      date_sent = M.date_sent AND "
                         "      from_recipient_id = M.from_recipient_id AND "
                         "      thread_id = M.thread_id"
                         "  ) > 1"
                         ") "
                         "SELECT _id, date_sent, from_recipient_id, thread_id, type, body FROM message WHERE "
                         "  _id IN (SELECT _id FROM dupes) ORDER BY date_sent ASC, _id ASC", &res))
      return false;

    if (!res.empty())
    {
      //std::cout << "DUPES REMAINING: " << res.rows() << std::endl;
      //res.prettyPrint(d_truncate);

      struct DupeKey
      {
        long long int date_sent;
        long long int thread_id;
        long long int from_recipient_id;
        bool operator<(DupeKey const &other) const
        {
          return date_sent < other.date_sent ||
            (date_sent == other.date_sent &&
             thread_id < other.thread_id) ||
            (date_sent == other.date_sent &&
             thread_id == other.thread_id &&
             from_recipient_id < other.from_recipient_id);
        }
      };
      std::map<DupeKey, std::vector<long long int>> dupemap;
      for (uint i = 0; i < res.rows(); ++i)
        dupemap[{res.valueAsInt(i, "date_sent"), res.valueAsInt(i, "thread_id"), res.valueAsInt(i, "from_recipient_id")}].push_back(res.valueAsInt(i, "_id"));

      auto dateTaken = [&](long long int date, long long int frid, long long int tid)
      {
        return d_database.getSingleResultAs<long long int>("SELECT EXISTS (SELECT 1 FROM message INDEXED BY message_date_sent_from_to_thread_index WHERE "
                                                           "date_sent = ? AND from_recipient_id = ? AND thread_id = ?)", {date, frid, tid}, 1) == 1;
      };

      for (auto const &p : dupemap)
      {
        // 'p' is one dupe-group, fix it...
        std::vector<long long int> dupe_ids(p.second);

        // make sure the dupes are sorted highest to lowest _id
        std::sort(dupe_ids.begin(), dupe_ids.end(), std::greater{});

        // pun intended
        long long int candiDATE = p.first.date_sent - 1;

        // drop the first (highest _id), it can stay
        dupe_ids.erase(dupe_ids.begin());

        for (auto const &i : dupe_ids)
        {
          while (dateTaken(candiDATE, p.first.from_recipient_id, p.first.thread_id))
            --candiDATE;
          if (!d_database.exec("UPDATE message SET date_sent = ? WHERE _id = ?", {candiDATE, i}))
            return false;
          --candiDATE;
        }
      }

      //d_database.prettyPrint(d_truncate, "SELECT _id, date_sent, from_recipient_id, thread_id, type, body FROM message WHERE _id = 6803 OR _id BETWEEN 73817 AND 73861 ORDER BY from_recipient_id ASC, thread_id ASC, date_sent ASC");

    }


    // setDBV
  }

  // adjust DatabaseVersionFrame
  DeepCopyingUniquePtr<DatabaseVersionFrame> d_new_dbvframe;
  if (!setFrameFromStrings(&d_new_dbvframe, std::vector<std::string>{"VERSION:uint32:191"}))
  {
    Logger::error("Failed to create new databaseversionframe");
    return false;
  }
  d_databaseversionframe.reset(d_new_dbvframe.release());
  d_databaseversion = 191;
  setColumnNames();

  return checkDbIntegrity();
}
