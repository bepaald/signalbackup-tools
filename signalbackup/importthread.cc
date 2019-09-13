/*
    Copyright (C)   Selwin van Dijk

    This file is part of .

     is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

     is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with .  If not, see <https://www.gnu.org/licenses/>.
*/

#include "signalbackup.ih"

void SignalBackup::importThread(SignalBackup *source, long long int thread)
{
  // crop the source db to the specified thread
  source->cropToThread(thread);

  // get targetthread
  SqliteDB::QueryResults results;
  source->d_database.exec("SELECT recipient_ids FROM thread WHERE _id = ?", thread, &results);

  if (results.rows() != 1 ||
      results.columns() != 1 ||
      !results.valueHasType<std::string>(0, 0))
  {
    std::cout << "Failed to get recipient id from source database" << std::endl;
    return;
  }
  std::string recipient_id = results.getValueAs<std::string>(0, 0);
  d_database.exec("SELECT _id FROM thread WHERE recipient_ids = ?", recipient_id, &results);
  long long int targetthread = -1;
  if (results.rows() == 1 &&
      results.columns() == 1 &&
      results.valueHasType<long long int>(0, 0))
    targetthread = results.getValueAs<long long int>(0, 0);

  // the target will have its own job_spec etc...
  source->d_database.exec("DELETE FROM job_spec");
  source->d_database.exec("DELETE FROM push");
  source->d_database.exec("DELETE FROM constraint_spec"); // has to do with job_spec, references it...
  source->d_database.exec("DELETE FROM dependency_spec"); // has to do with job_spec, references it...
  source->d_database.exec("VACUUM");

  // make sure all id's are unique
  // should rename these to offset
  long long int minthread = getMaxUsedId("thread") + 1 - source->getMinUsedId("thread");
  long long int minsms = getMaxUsedId("sms") + 1 - source->getMinUsedId("sms");
  long long int minmms = getMaxUsedId("mms") + 1 - source->getMinUsedId("mms");
  long long int minpart = getMaxUsedId("part") + 1 - source->getMinUsedId("part");
  long long int minrecipient_preferences = getMaxUsedId("recipient_preferences") + 1 - source->getMinUsedId("recipient_preferences");
  long long int mingroups = getMaxUsedId("groups") + 1 - source->getMinUsedId("groups");
  long long int minidentities = getMaxUsedId("identities") + 1 - source->getMinUsedId("identities");
  long long int mingroup_receipts = getMaxUsedId("group_receipts") + 1 - source->getMinUsedId("group_receipts");
  long long int mindrafts = getMaxUsedId("drafts") + 1 - source->getMinUsedId("drafts");
  source->makeIdsUnique(minthread, minsms, minmms, minpart, minrecipient_preferences, mingroups, minidentities, mingroup_receipts, mindrafts);

  // merge into existing thread, set the id on the sms, mms, and drafts
  // drop the recipient_preferences, identities and thread tables, they are already in the target db
  if (targetthread > -1)
  {
    std::cout << "Found existing thread for this recipient in target database, merging into thread " << targetthread << std::endl;

    source->d_database.exec("UPDATE sms SET thread_id = ?", targetthread);
    source->d_database.exec("UPDATE mms SET thread_id = ?", targetthread);
    source->d_database.exec("UPDATE drafts SET thread_id = ?", targetthread);
    source->d_database.exec("DROP TABLE thread");
    source->d_database.exec("DROP TABLE identities");
    source->d_database.exec("DROP TABLE recipient_preferences");
    source->d_database.exec("DROP TABLE groups");
    source->d_avatars.clear();
  }
  else
  {
    // check identities and recepient prefs for presence of values, they may be there (even though no thread was found (for example via a group chat or deleted thread))
    // get identities from target, drop all rows from source that are allready present
    d_database.exec("SELECT address FROM identities", &results);
    for (uint i = 0; i < results.rows(); ++i)
      if (results.header(0) == "address" && results.valueHasType<std::string>(i, 0))
        source->d_database.exec("DELETE FROM identities WHERE ADDRESS = '" + results.getValueAs<std::string>(i, 0) + "'");

    // get recipient_preferences from target, drop all rows from source that are allready present
    d_database.exec("SELECT recipient_ids FROM recipient_preferences", &results);
    for (uint i = 0; i < results.rows(); ++i)
      if (results.header(0) == "recipient_ids" && results.valueHasType<std::string>(i, 0))
        source->d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids = '" + results.getValueAs<std::string>(i, 0) + "'");
  }


  // now import the source tables into target,

  // get tables
  std::string q("SELECT sql, name, type FROM sqlite_master");
  source->d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.valueHasType<std::nullptr_t>(i, 0))
    {
      if (results.valueHasType<std::string>(i, 1) &&
          (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
           results.getValueAs<std::string>(i, 1).find("sms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "mms_fts" &&
                results.getValueAs<std::string>(i, 1).find("mms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else
        if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
          tables.emplace_back(std::move(results.getValueAs<std::string>(i, 1)));
    }
  }

  // write contents of tables
  for (std::string const &table : tables)
  {
    if (table == "signed_prekeys" ||
        table == "one_time_prekeys" ||
        table == "sessions" ||
        table.substr(0, STRLEN("sms_fts")) == "sms_fts" ||
        table.substr(0, STRLEN("mms_fts")) == "mms_fts" ||
        table.substr(0, STRLEN("sqlite_")) == "sqlite_")
      continue;
    std::cout << "Importing statements from source table '" << table << "'...";
    source->d_database.exec("SELECT * FROM " + table, &results);
    std::cout << results.rows() << " entries..." << std::endl;
    for (uint i = 0; i < results.rows(); ++i)
    {
      SqlStatementFrame NEWFRAME = buildSqlStatementFrame(table, results.headers(), results.row(i));
      d_database.exec(NEWFRAME.bindStatement(), NEWFRAME.parameters());
    }
  }

  // and copy avatars and attachments.
  d_attachments.insert(std::make_move_iterator(source->d_attachments.begin()), std::make_move_iterator(source->d_attachments.end()));
  d_avatars.insert(std::make_move_iterator(source->d_avatars.begin()), std::make_move_iterator(source->d_avatars.end()));
}
