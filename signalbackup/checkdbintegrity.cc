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

bool SignalBackup::checkDbIntegrity(bool warn) const
{
  SqliteDB::QueryResults results;

  // CHECKING FOREIGN KEY CONSTRAINTS
  if (!warn)
    Logger::message_start("Checking foreign key constraints...");
  d_database.exec("SELECT DISTINCT [table],[parent],[fkid] FROM pragma_foreign_key_check", &results);
  if (results.rows())
  {
    if (!warn)
      Logger::error("Foreign key constraint violated. This will not end well, aborting."
                    "\n\n"
                    "Please report this error to the program author.");
    else
      Logger::warning("Foreign key constraint violated.");
    results.prettyPrint(d_truncate);
    return false;
  }
  if (!warn)
    Logger::message_end(" ok");

  // std::cout << "Checking database integrity (quick)..." << std::flush;
  // d_database.exec("SELECT * FROM pragma_quick_check", &results);
  // if (results.rows() && results.valueAsString(0, "quick_check") != "ok")
  // {
  //   std::cout << std::endl << bepaald::bold_on << "ERROR" << bepaald::bold_off << " Database integrity check failed. This will not end well, aborting." << std::endl
  //             <<                                  "     "                         " Please report this error to the program author." << std::endl;
  //   results.prettyPrint();
  //   return false;
  // }
  // std::cout << " ok" << std::endl;

  // CHECKING DATABASE
  if (!warn)
    Logger::message_start("Checking database integrity (full)...");
  d_database.exec("SELECT * FROM pragma_integrity_check", &results);
  if (results.rows() && results.valueAsString(0, "integrity_check") != "ok")
  {
    if (!warn)
      Logger::error("Database integrity check failed. This will not end well, aborting."
                    "\n\n"
                    "Please report this error to the program author.");
    else
      Logger::warning("Foreign key constraint violated.");
    results.prettyPrint(d_truncate);
    return false;
  }
  if (!warn)
    Logger::message_end(" ok");

  return true;
}
