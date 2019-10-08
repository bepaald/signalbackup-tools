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

void SignalBackup::makeIdsUnique(long long int minthread, long long int minsms, long long int minmms, long long int minpart, long long int minrecipient_preferences, long long int mingroups, long long int minidentities, long long int mingroup_receipts, long long int mindrafts)
{
  std::cout << "Adjusting indexes in tables..." << std::endl;

  setMinimumId("thread", minthread);
  d_database.exec("UPDATE sms SET thread_id = thread_id + ?", minthread);    // update sms.thread_id to new thread._id's
  d_database.exec("UPDATE mms SET thread_id = thread_id + ?", minthread);    // ""
  d_database.exec("UPDATE drafts SET thread_id = thread_id + ?", minthread); // ""

  setMinimumId("sms",  minsms);
  compactIds("sms");

  // UPDATE t SET id = (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1) WHERE id = (SELECT MIN(id) FROM t WHERE id > (SELECT t1.id+1 FROM t t1 LEFT OUTER JOIN t t2 ON t2.id=t1.id+1 WHERE t2.id IS NULL AND t1.id > 0 ORDER BY t1.id LIMIT 1));

  setMinimumId("mms",  minmms);
  d_database.exec("UPDATE part SET mid = mid + ?", minmms); // update part.mid to new mms._id's
  d_database.exec("UPDATE group_receipts SET mms_id = mms_id + ?", minmms); // "
  compactIds("mms");

  setMinimumId("part", minpart);
  // update rowid's in attachments
  std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> newattdb;
  for (auto &att : d_attachments)
  {
    AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(att.second.release());
    a->setRowId(a->rowId() + minpart);
    newattdb.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
  }
  d_attachments.clear();
  d_attachments = std::move(newattdb);
  compactIds("part");

  setMinimumId("recipient_preferences", minrecipient_preferences);
  compactIds("recipient_preferences");

  setMinimumId("groups", mingroups);
  compactIds("groups");

  setMinimumId("identities", minidentities);
  compactIds("identities");

  setMinimumId("group_receipts", mingroup_receipts);
  compactIds("group_receipts");

  setMinimumId("drafts", mindrafts);
  compactIds("drafts");
}
