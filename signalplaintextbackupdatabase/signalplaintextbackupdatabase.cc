/*
  Copyright (C) 2024  Selwin van Dijk

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

#include "signalplaintextbackupdatabase.ih"

#include "../xmldocument/xmldocument.h"

SignalPlaintextBackupDatabase::SignalPlaintextBackupDatabase(std::string const &sptbxml, bool verbose)
  :
  d_ok(false),
  d_verbose(verbose)
{
  // open and parse XML file
  XmlDocument xmldoc(sptbxml);
  if (!xmldoc.ok())
  {
    Logger::error("Reading xml data");
    return;
  }

  // check expected rootnode
  XmlDocument::Node const &rootnode = xmldoc.root();
  if (rootnode.name() != "smses")
  {
    Logger::error("Unexpected rootnode '", rootnode.name(), "', expected 'smses'.");
    return;
  }

  /*
    columns (XML attributes) imported into SQL table

       available columns:
     <sms protocol="n" address="+[phonenumber]" contact_name="[name]" date="nnnnnnnnnnnnn" readable_date="[date-as-string]" type="2" subject="null" body="[message-body]" toa="null" sc_toa="null" service_center="null" read="1" status="-1" locked="0" />

     date = ms since epoch (same as Signal database)
     address = recipient.e164
     read = 1/0 (read/unread)
     status = -1/0/32/64 (none/complete/pending/failed)
     type = 1 = Received, 2 = Sent, 3 = Draft, 4 = Outbox, 5 = Failed, 6 = Queued
  */
  std::vector<std::pair<std::string, std::string>> requiredcolumns{{"date", "INTEGER"},
                                                                   {"type", "INTEGER"},
                                                                   {"read", "INTEGER"},
                                                                   {"body", "TEXT"},
                                                                   {"contact_name", "TEXT"},
                                                                   {"address", "TEXT"}}; // etc...

  // create table
  std::string tablecreate("CREATE TABLE smses ");
#if __cplusplus > 201703L
  for (unsigned int i = 0; auto const &rc : requiredcolumns)
#else
  unsigned int i = 0;
  for (auto const &rc : requiredcolumns)
#endif
  {
    tablecreate += (i == 0 ? " (" : ", ") + rc.first + " " + rc.second + " DEFAULT NULL" + (i == requiredcolumns.size() - 1 ? ")" : "");
    ++i;
  }
  if (!d_database.exec(tablecreate))
    return;

  // fill tables
  for (auto const &n : rootnode)
  {
    if (n.name() == "sms")
    {
      // check attribute exists
      if (!std::all_of(requiredcolumns.begin(), requiredcolumns.end(),
                       [&](std::pair<std::string, std::string> const &rc)
                       {
                         if (!n.hasAttribute(rc.first))
                         {
                           Logger::warning("Skipping message, missing required attribute '", rc.first, "'");
                           return false;
                         }
                         return true;
                       }))
        continue;

      // build statement
      std::string columns;
      std::string values;
#if __cplusplus > 201703L
      for (unsigned int i = 0; auto const &rc : requiredcolumns)
#else
      i = 0;
      for (auto const &rc : requiredcolumns)
#endif
      {
        std::string const &val = n.getAttribute(rc.first);
        if (val != "null")
        {
          columns += (i == 0 ? "" : ", ") + rc.first;
          values += (i == 0 ? ((rc.second == "TEXT") ? "'" : "") : ((rc.second == "TEXT") ? ", '" : ", ")) + val + ((rc.second == "TEXT") ? "'" : "");
        }
        ++i;
      }

      // maybe prep body? (&#apos; -> ', $#NNN; -> utf8)

      if (!d_database.exec("INSERT INTO smses (" + columns + ") VALUES (" + values + ")"))
        return;

    }
    else
      Logger::warning("Skipping unsupported element: '", n.name(), "'");
  }

  //d_database.prettyPrint(true, "SELECT min(date), max(date) FROM smses");
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31611496644'");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31645756298'");

  d_ok = true;
}
