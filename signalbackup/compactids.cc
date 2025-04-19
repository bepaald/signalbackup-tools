/*
  Copyright (C) 2019-2025  Selwin van Dijk

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
  //Logger::message(__FUNCTION__);

  if (d_database.getSingleResultAs<long long int>("SELECT COUNT(*) FROM " + table, -1) == 0) // table is empty
    return;

  Logger::message("  Compacting table: ", table, " (", col, ")");

  SqliteDB::QueryResults results;
  // d_database.exec("SELECT " + col + " FROM " + table, &results);
  // results.prettyPrint();

  // gets first available _id in table
  d_database.exec("SELECT t1." + col + "+1 FROM " + table + " t1 LEFT OUTER JOIN " + table + " t2 ON t2." + col + "=t1." + col + "+1 WHERE t2." + col + " IS NULL AND t1." + col + " > 0 ORDER BY t1." + col + " LIMIT 1", &results);

  while (results.rows() > 0 && results.valueHasType<long long int>(0, 0))
  {
    long long int nid = results.getValueAs<long long int>(0, 0);

    d_database.exec(bepaald::concat("SELECT MIN(", col, ") FROM ", table, " WHERE ", col, " > ?"), nid, &results);
    if (results.rows() == 0 || !results.valueHasType<long long int>(0, 0))
      break;
    long long int valuetochange = results.getValueAs<long long int>(0, 0);
    //std::cout << "Changing _id : " << valuetochange << " -> " << nid << std::endl;

    d_database.exec(bepaald::concat("UPDATE ", table, " SET ", col, " = ? WHERE ", col, " = ?"), {nid, valuetochange});

    if (col == "_id") [[likely]]
    {
      for (auto const &dbl : s_databaselinks)
      {
        if (dbl.flags & SKIP)
          continue;

        if (!d_database.containsTable(dbl.table)) [[unlikely]]
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
                  Logger::error("Compacting table '", table, "'");
              }
              else if (!d_database.exec("UPDATE " + c.table + " SET " + c.column + " = ? WHERE " + c.column + " = ?" + (c.whereclause.empty() ? "" : " AND " + c.whereclause), {nid, valuetochange}))
                Logger::error("Compacting table '", table, "'");
            }
          }
        }
      }

      if (table == d_part_table)
      {
        for (auto att = d_attachments.begin(); att != d_attachments.end(); )
        {
          if (att->second.get()->rowId() == static_cast<uint64_t>(valuetochange))
          {
            AttachmentFrame *af = att->second.release();
            att = d_attachments.erase(att);
            af->setRowId(nid);
            int64_t uniqueid = af->attachmentId();
            if (uniqueid == 0)
              uniqueid = -1;
            d_attachments.emplace(std::make_pair(af->rowId(), uniqueid), af);
          }
          else
            ++att;
        }
      }
      else if (table == "sticker")
      {
        for (auto s = d_stickers.begin(); s != d_stickers.end(); )
        {
          if (s->second.get()->rowId() == static_cast<uint64_t>(valuetochange))
          {
            StickerFrame *sf = s->second.release();
            s = d_stickers.erase(s);
            sf->setRowId(nid);
            d_stickers.emplace(std::make_pair(sf->rowId(), sf));
          }
          else
            ++s;
        }
      }

    }

    // gets first available _id in table
    d_database.exec(bepaald::concat("SELECT t1.", col, "+1 FROM ", table, " t1 LEFT OUTER JOIN ", table, " t2 ON t2.", col, "=t1.", col, "+1 WHERE t2.", col, " IS NULL AND t1.", col, " > 0 ORDER BY t1.", col, " LIMIT 1"), &results);
  }
  // d_database.exec("SELECT _id FROM " + table, &results);
  // results.prettyPrint();
}
