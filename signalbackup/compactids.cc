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

    if (col == "_id") [[likely]]
    {
      for (auto const &dbl : d_databaselinks)
      {
        if (dbl.flags & SKIP)
          continue;
        if (table == dbl.table)
        {
          for (auto const &c : dbl.connections)
          {
            if (d_databaseversion >= c.mindbvversion && d_databaseversion <= c.maxdbvversion &&
                d_database.containsTable(c.table) && d_database.tableContainsColumn(c.table, c.column))
            {
              if (!c.json_path.empty())
              {
                if (!d_database.exec("UPDATE " + c.table + " SET " + c.column + " = json_replace(" + c.column + ", " + c.json_path + ", ?) "
                                       "WHERE json_extract(" + c.column + ", " + c.json_path + ") = ?", {nid, valuetochange}))
                  std::cout << "ERROR: compacting table '" << table << "'" << std::endl;
              }
              else if (!d_database.exec("UPDATE " + c.table + " SET " + c.column + " = ? WHERE " + c.column + " = ?" + (c.whereclause.empty() ? "" : " AND " + c.whereclause), {nid, valuetochange}))
                std::cout << "ERROR: compacting table '" << table << "'" << std::endl;
            }
          }
        }
      }
      /*
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
        if (d_database.containsTable("story_sends"))
          d_database.exec("UPDATE story_sends SET message_id = ? WHERE message_id = ?", {nid, valuetochange});
      }
      else */if (table == "part")
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

        /*
        // update rowid in previews (mms.previews contains a json string referencing the 'rowId' == part._id)
        if (d_database.tableContainsColumn("mms", "previews"))
          d_database.exec("UPDATE mms SET previews = json_replace(previews, '$[0].attachmentId.rowId', ?) "
                          "WHERE json_extract(previews, '$[0].attachmentId.rowId') = ?", {nid, valuetochange});
        */
        /*
        //  REPLACED WITH ABOVE

        // update rowid in previews (mms.previews contains a json string referencing the 'rowId' == part._id)
        SqliteDB::QueryResults results2;
        d_database.exec("SELECT _id,previews FROM mms WHERE previews IS NOT NULL", &results2);
        std::regex rowid_in_json(".*\"rowId\":(" + bepaald::toString(valuetochange) + ")[,}].*");
        std::smatch sm;
        for (uint i = 0; i < results2.rows(); ++i)
        {
          std::string line = results2.valueAsString(i, "previews");
          //std::cout << " OLD: " << line << std::endl;
          if (std::regex_match(line, sm, rowid_in_json))
            if (sm.size() == 2) // 0 is full match, 1 is first submatch (which is what we want)
            {
              //std::cout << "MATCHED:" << std::endl;
              //std::cout << sm.size() << std::endl;
              line.replace(sm.position(1), sm.length(1), bepaald::toString(nid));
              //std::cout << " NEW: " << line << std::endl;

              d_database.exec("UPDATE mms SET previews = ? WHERE _id = ?", {line, results2.getValueAs<long long int>(i, "_id")});
            }
        }
        */
      }
      /*
      else if (table == "msl_payload")
      {
        d_database.exec("UPDATE msl_message SET payload_id = ? WHERE payload_id = ?", {nid, valuetochange});
        d_database.exec("UPDATE msl_recipient SET payload_id = ? WHERE payload_id = ?", {nid, valuetochange});
      }
      else if (table == "notification_profile") // should actually be cleared at this point...
      {
        d_database.exec("UPDATE notification_profile_allowed_members SET notification_profile_id = ? WHERE notification_profile_id = ?", {nid, valuetochange});
        d_database.exec("UPDATE notification_profile_schedule SET notification_profile_id = ? WHERE notification_profile_id = ?", {nid, valuetochange});
      }
      else if (table == "distribution_list")
      {
        d_database.exec("UPDATE recipient SET distribution_list_id = ? WHERE distribution_list_id = ?", {nid, valuetochange});
        d_database.exec("UPDATE distribution_list_member SET list_id = ? WHERE list_id = ?", {nid, valuetochange});
      }
      */
    }

    // gets first available _id in table
    d_database.exec("SELECT t1." + col + "+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2." + col + "=t1." + col + "+1 WHERE t2." + col + " IS NULL AND t1." + col + " > 0 ORDER BY t1." + col + " LIMIT 1", &results);
  }
  // d_database.exec("SELECT _id FROM " + table, &results);
  // results.prettyPrint();
}
