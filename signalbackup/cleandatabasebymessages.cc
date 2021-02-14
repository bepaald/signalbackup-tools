/*
    Copyright (C) 2019-2021  Selwin van Dijk

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
  d_database.exec("DELETE FROM thread where _id NOT IN (SELECT DISTINCT thread_id FROM sms) AND _id NOT IN (SELECT DISTINCT thread_id FROM mms)");
  updateThreadsEntries();

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  std::cout << "  Deleting removed groups..." << std::endl;
  if (d_databaseversion < 27)
    d_database.exec("DELETE FROM groups WHERE group_id NOT IN (SELECT DISTINCT recipient_ids FROM thread)");
  else
    d_database.exec("DELETE FROM groups WHERE recipient_id NOT IN (SELECT DISTINCT recipient_ids FROM thread)");

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

  if (d_databaseversion < 24)
  {
    std::cout << "  Deleting unreferenced recipient_preferences..." << std::endl;
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT address FROM sms UNION SELECT DISTINCT address FROM mms");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms
    d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids NOT IN (WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT address FROM sms UNION SELECT DISTINCT address FROM mms)");
  }
  else
  {
    std::cout << "  Deleting unreferenced recipient entries..." << std::endl;

    //runSimpleQuery("SELECT group_concat(_id,',') FROM recipient");
    //runSimpleQuery("WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT address FROM sms UNION SELECT DISTINCT address FROM mms UNION SELECT DISTINCT recipient_ids FROM thread");

    // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms

    // KEEP recipients WITH _id IN remapped_recipients.old_id!?!?

    d_database.exec("DELETE FROM recipient WHERE _id NOT IN (WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT address FROM sms UNION SELECT DISTINCT address FROM mms UNION SELECT DISTINCT recipient_ids FROM thread)");

  }

  //runSimpleQuery((d_databaseversion < 27) ? "SELECT _id, recipient_ids, system_display_name FROM recipient_preferences" : "SELECT _id, COALESCE(system_display_name,group_id,signal_profile_name) FROM recipient");

  // remove avatars not belonging to exisiting recipients
  std::cout << "  Deleting unused avatars..." << std::endl;
  SqliteDB::QueryResults results;
  if (d_databaseversion < 24)
    d_database.exec("SELECT recipient_ids FROM recipient_preferences", &results);
  else if (d_databaseversion < 33)
    d_database.exec("SELECT COALESCE(phone,group_id) FROM recipient", &results); // recipient_preferences does not exist anymore, but d_avatars are still linked to "+316xxxxxxxx" strings
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

  std::cout << "  Delete others from 'identities'" << std::endl;
  if (d_databaseversion < 24)
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");
  else
    d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT _id FROM recipient)");

  std::cout << "  Deleting group receipts entries from deleted messages..." << std::endl;
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM mms)");

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
