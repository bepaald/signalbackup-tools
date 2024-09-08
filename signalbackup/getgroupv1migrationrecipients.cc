/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

void SignalBackup::getGroupV1MigrationRecipients(std::set<long long int> *referenced_recipients, long long int thread) const
{
  SqliteDB::QueryResults results;
  if (d_database.exec("SELECT body FROM "s +
                      (d_database.containsTable("sms") ? "sms" : d_mms_table) +
                      " WHERE " +
                      (d_database.containsTable("sms") ? "type" : d_mms_type) + " == ?" +
                      (thread != -1 ? " AND thread_id = " + bepaald::toString(thread) : ""),
                      bepaald::toString(Types::GV1_MIGRATION_TYPE), &results))
  {
    //results.prettyPrint();
    for (uint i = 0; i < results.rows(); ++i)
    {
      if (results.valueHasType<std::string>(i, "body"))
      {
        //std::cout << results.getValueAs<std::string>(i, "body") << std::endl;

        std::string body = results.getValueAs<std::string>(i, "body");
        std::string tmp; // to hold part of number while reading
        unsigned int body_idx = 0;
        while (true)
        {
          if (body_idx >= body.length() || !std::isdigit(body[body_idx])) // we are reading '|', ',' or end of string
          {
            // deal with any number we have
            if (tmp.size())
            {
              referenced_recipients->insert(bepaald::toNumber<long long int>(tmp));
              if (d_verbose) [[unlikely]]
                Logger::message("    Got recipient from GV1_MIGRATION: ", tmp);
              tmp.clear();
            }
          }
          else // we are reading (part of) a number
            tmp += body[body_idx];
          ++body_idx;
          if (body_idx > body.length())
            break;
        }
      }
    }
  }
}
