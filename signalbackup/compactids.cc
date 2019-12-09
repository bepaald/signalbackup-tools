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

void SignalBackup::compactIds(std::string const &table)
{
  std::cout << __FUNCTION__ << std::endl;

  std::cout << "  Compacting table: " << table << std::endl;

  SqliteDB::QueryResults results;
  // d_database.exec("SELECT _id FROM " + table, &results);
  // results.prettyPrint();

  d_database.exec("SELECT t1._id+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2._id=t1._id+1 WHERE t2._id IS NULL AND t1._id > 0 ORDER BY t1._id LIMIT 1", &results);
  while (results.rows() > 0 && results.valueHasType<long long int>(0, 0))
  {
    long long int nid = results.getValueAs<long long int>(0, 0);

    d_database.exec("SELECT MIN(_id) FROM " + table + " WHERE _id > ?", nid, &results);
    if (results.rows() == 0 || !results.valueHasType<long long int>(0, 0))
      break;
    long long int valuetochange = results.getValueAs<long long int>(0, 0);

    //std::cout << "Changing _id : " << valuetochange << " -> " << nid << std::endl;

    d_database.exec("UPDATE " + table + " SET _id = ? WHERE _id = ?", {nid, valuetochange});


    if (table == "mms")
    {
      d_database.exec("UPDATE part SET mid = ? WHERE mid = ?", {nid, valuetochange}); // update part.mid to new mms._id's
      d_database.exec("UPDATE group_receipts SET mms_id = ? WHERE mms_id = ?", {nid, valuetochange}); // "
    }
    else if (table == "part")
    {
      for (auto att = d_attachments.begin(); att != d_attachments.end(); )
      {
        if (reinterpret_cast<AttachmentFrame *>(att->second.get())->rowId() == static_cast<uint64_t>(valuetochange))
        {
          AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(att->second.release());
          att = d_attachments.erase(att);
          a->setRowId(nid);
          d_attachments.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
        }
        else
          ++att;
      }
    }

    d_database.exec("SELECT t1._id+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2._id=t1._id+1 WHERE t2._id IS NULL AND t1._id > 0 ORDER BY t1._id LIMIT 1", &results);
  }

  // d_database.exec("SELECT _id FROM " + table, &results);
  // results.prettyPrint();
}
