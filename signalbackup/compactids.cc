/*
    Copyright (C) 2019-2022  Selwin van Dijk

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

void SignalBackup::compactIds(std::string const &table, std::string const &col)
{
  std::cout << __FUNCTION__ << std::endl;

  std::cout << "  Compacting table: " << table << " (" << col << ")" << std::endl;

  SqliteDB::QueryResults results;
  // d_database.exec("SELECT " + col + " FROM " + table, &results);
  // results.prettyPrint();

  // gets first available _id in table
  d_database.exec("SELECT t1." + col + "+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2." + col + "=t1." + col + "+1 WHERE t2." + col + " IS NULL AND t1." + col + " > 0 ORDER BY t1." + col + " LIMIT 1", &results);
  while (results.rows() > 0 && results.valueHasType<long long int>(0, 0))
  {
    long long int nid = results.getValueAs<long long int>(0, 0);

    d_database.exec("SELECT MIN(" + col + ") FROM " + table + " WHERE " + col + " > ?", nid, &results);
    if (results.rows() == 0 || !results.valueHasType<long long int>(0, 0))
      break;
    long long int valuetochange = results.getValueAs<long long int>(0, 0);
    //std::cout << "Changing _id : " << valuetochange << " -> " << nid << std::endl;

    d_database.exec("UPDATE " + table + " SET " + col + " = ? WHERE " + col + " = ?", {nid, valuetochange});

    [[ likely ]] if (col == "_id")
    {
      if (table == "sms")
      {
        if (d_database.containsTable("msl_message"))
          d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ? AND is_mms IS NOT 1", {nid, valuetochange});

        if (d_database.containsTable("reaction")) // dbv >= 121
          d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ? AND is_mms IS NOT 1", {nid, valuetochange});
      }
      else if (table == "mms")
      {
        d_database.exec("UPDATE part SET mid = ? WHERE mid = ?", {nid, valuetochange}); // update part.mid to new mms._id's
        d_database.exec("UPDATE group_receipts SET mms_id = ? WHERE mms_id = ?", {nid, valuetochange}); //
        if (d_database.containsTable("mention"))
          d_database.exec("UPDATE mention SET message_id = ? WHERE message_id = ?", {nid, valuetochange});
        if (d_database.containsTable("msl_message"))
          d_database.exec("UPDATE msl_message SET message_id = ? WHERE message_id = ? AND is_mms IS 1", {nid, valuetochange});
        if (d_database.containsTable("reaction")) // dbv >= 121
          d_database.exec("UPDATE reaction SET message_id = ? WHERE message_id = ? AND is_mms IS 1", {nid, valuetochange});
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
    }

    // gets first available _id in table
    d_database.exec("SELECT t1." + col + "+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2." + col + "=t1." + col + "+1 WHERE t2." + col + " IS NULL AND t1." + col + " > 0 ORDER BY t1." + col + " LIMIT 1", &results);
  }
  // d_database.exec("SELECT _id FROM " + table, &results);
  // results.prettyPrint();
}
