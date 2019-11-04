/*
    Copyright (C) 2019  Selwin van Dijk

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
  std::cout << "Deleting attachment entries from 'part' not belonging to remaining mms entries" << std::endl;
  d_database.exec("DELETE FROM part WHERE mid NOT IN (SELECT DISTINCT _id FROM mms)");

  std::cout << "Deleting other threads from 'thread'..." << std::endl;
  d_database.exec("DELETE FROM thread where _id NOT IN (SELECT DISTINCT thread_id FROM sms) AND _id NOT IN (SELECT DISTINCT thread_id FROM mms)");
  updateThreadsEntries();

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  std::cout << "Deleting removed groups..." << std::endl;
  d_database.exec("DELETE FROM groups WHERE group_id NOT IN (SELECT DISTINCT recipient_ids FROM thread)");

  //std::cout << "Groups left:" << std::endl;
  //runSimpleQuery("SELECT group_id,title,members FROM groups");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

  std::cout << "Deleting unreferenced recipient_preferences..." << std::endl;
  // this gets all recipient_ids/addresses ('+31612345678') from still existing groups and sms/mms
  d_database.exec("DELETE FROM recipient_preferences WHERE recipient_ids NOT IN (WITH RECURSIVE split(word, str) AS (SELECT '', members||',' FROM groups UNION ALL SELECT substr(str, 0, instr(str, ',')), substr(str, instr(str, ',')+1) FROM split WHERE str!='') SELECT DISTINCT split.word FROM split WHERE word!='' UNION SELECT DISTINCT address FROM sms UNION SELECT DISTINCT address FROM mms)");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

  // remove avatars not belonging to exisiting recipients
  SqliteDB::QueryResults results;
  d_database.exec("SELECT recipient_ids FROM recipient_preferences", &results); // THIS NEEDS TO CHANGE FOR DATABASE >= 33
  bool erased = true;
  while (erased)
  {
    erased = false;
    for (std::vector<std::pair<std::string, std::unique_ptr<AvatarFrame>>>::iterator avit = d_avatars.begin(); avit != d_avatars.end();)
      if (!results.contains(avit->first))
      {
        avit = d_avatars.erase(avit);
        erased = true;
        break;
      }
      else
        ++avit;
  }
  std::cout << "Delete others from 'identities'" << std::endl;
  d_database.exec("DELETE FROM identities WHERE address NOT IN (SELECT DISTINCT recipient_ids FROM recipient_preferences)");

  std::cout << "Deleting group receipts entries from deleted messages..." << std::endl;
  d_database.exec("DELETE FROM group_receipts WHERE mms_id NOT IN (SELECT DISTINCT _id FROM mms)");

  std::cout << "Deleting drafts from deleted threads..." << std::endl;
  d_database.exec("DELETE FROM drafts WHERE thread_id NOT IN (SELECT DISTINCT thread_id FROM sms) AND thread_id NOT IN (SELECT DISTINCT thread_id FROM mms)");

  //runSimpleQuery("SELECT _id, recipient_ids, system_display_name FROM recipient_preferences");

}
