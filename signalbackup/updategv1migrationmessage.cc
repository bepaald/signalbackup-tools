/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

// in groups, during the v1 -> v2 update, members may have been removed from the group, these messages
// are of type "GV1_MIGRATION_TYPE" and have a body that looks like '_id,_id,...|_id,_id,_id,...' (I think, I have
// not seen one with more than 1 id). These id_s must also be updated.
void SignalBackup::updateGV1MigrationMessage(long long int id1, long long int id2) const // if id2 == -1, id1 is an offset
{                                                                                        // else, change id1 into id2
  SqliteDB::QueryResults results;
  int changed = 0;
  std::string table = d_database.containsTable("sms") ? "sms" : d_mms_table;
  if (d_database.exec("SELECT _id,body FROM " + table + " WHERE type == ? AND body IS NOT NULL",
                      bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
  {
    //results.prettyPrint();
    for (uint i = 0; i < results.rows(); ++i)
    {
      if (results.valueHasType<std::string>(i, "body"))
      {
        //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;
        std::string body = results.getValueAs<std::string>(i, "body");
        std::string output;
        std::string tmp; // to hold part of number while reading
        unsigned int body_idx = 0;
        while (true)
        {
          if (!std::isdigit(body[body_idx]) || body_idx >= body.length())
          {
            // deal with any number we have
            if (tmp.size())
            {
              long long int id = bepaald::toNumber<int>(tmp);
              //std::cout << "FOUND ID: " << id << std::endl;
              if (id2 == -1)
                id += id1;
              else if (id == id1)
                id = id2;
              output += bepaald::toString(id);
              tmp.clear();
            }
            // add non-digit-char
            //std::cout << "ADD NON-DIGIT: " << body[body_idx] << std::endl;
            if (body_idx < body.length())
              output += body[body_idx];
          }
          else
            tmp += body[body_idx];
          ++body_idx;
          if (body_idx > body.length())
            break;
        }
        //std::cout << "NEW OUTPUT: " << output << std::endl;
        if (body != output)
        {
          long long int sms_id = results.getValueAs<long long int>(i, "_id");
          d_database.exec("UPDATE " + table + " SET body = ? WHERE _id == ?", {output, sms_id});
          ++changed;
        }
      }
    }
  }
  if (d_verbose && changed > 0)
    Logger::message("    update ", table, ".body (GV1_MIGRATION), changed: ", changed);
}
