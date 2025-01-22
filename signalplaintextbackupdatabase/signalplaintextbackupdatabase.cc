/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

SignalPlaintextBackupDatabase::SignalPlaintextBackupDatabase(std::string const &sptbxml, bool truncate, bool verbose)
  :
  d_ok(false),
  d_truncate(truncate),
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
  struct PlaintextColumnInfo
  {
    std::string name;
    std::string type;
    std::string required_in_node; // empty = ALL
    std::string columnname;
  };

  std::vector<PlaintextColumnInfo> const columninfo{{"date", "INTEGER", "", ""},
                                                    {"type", "INTEGER", "sms", ""},
                                                    {"msg_box", "INTEGER", "mms", "type"},
                                                    {"read", "INTEGER", "", ""},
                                                    {"body", "TEXT", "sms", ""},
                                                    {"contact_name", "TEXT", "", ""},
                                                    {"address", "TEXT", "", ""},
                                                    {"recipients", "TEXT", "none", ""},  // json? {"source":"address", "target":["address1", address2"]} ?
                                                    {"ismms", "INTEGER", "none", ""},
                                                    {"sourceaddress", "TEXT", "none", ""},
                                                    {"numaddresses", "INTEGER", "none", ""},
                                                    {"numattachments", "INTEGER", "none", ""}};

  // create message table
  std::string tablecreate;
#if __cplusplus > 201703L
  for (unsigned int i = 0; auto const &rc : columninfo)
#else
  unsigned int i = 0;
  for (auto const &rc : columninfo)
#endif
  {
    if (rc.columnname.empty()) [[likely]]
      tablecreate += (tablecreate.empty() ? "CREATE TABLE smses (" : ", ") + rc.name + " " + rc.type + " DEFAULT NULL";
    ++i;
  }
  tablecreate += ")";
  if (!d_database.exec(tablecreate))
    return;

  // create attachment table
  if (!d_database.exec("CREATE TABLE attachments (mid INTEGER, data TEXT DEFAULT NULL, filename TEXT DEFAULT NULL, "
                       "pos INTEGER DEFAULT -1, size INTEGER DEFAULT -1, ct TEXT default NULL)"))
    return;

  // fill tables
  std::vector<std::any> values;
  std::string columns;
  std::string placeholders;

  auto addvalue = [&](std::string const &column, std::any &&value)
  {
    values.emplace_back(value);
    columns += (columns.empty() ? ""s : ", "s) + column;
    placeholders += (placeholders.empty() ? ""s : ", "s) + "?"s;
  };

  for (auto const &n : rootnode)
  {

    // check required attributes exist
    if (!std::all_of(columninfo.begin(), columninfo.end(),
                     [&](PlaintextColumnInfo const &rc)
                     {
                       if ((rc.required_in_node.empty() || rc.required_in_node == n.name()) && !n.hasAttribute(rc.name))
                       {
                         Logger::warning("Skipping message, missing required attribute '", rc.name, "'");
                         return false;
                       }
                       return true;
                     }))
      continue;

    if (n.name() == "sms" || n.name() == "mms")
    {
      // if (n.name() == "mms")
      // {
      //   warnOnce("Skipping unsupported element: '" + n.name() + "'");
      //   continue;
      // }

      // build statement
      columns.clear();
      placeholders.clear();
      values.clear();
      for (auto const &rc : columninfo)
      {
        if (rc.required_in_node != n.name() && !rc.required_in_node.empty())
          continue;

        std::string val = n.getAttribute(rc.name);

        if (val != "null")
          addvalue((rc.columnname.empty() ? rc.name : rc.columnname), (rc.type == "INTEGER") ? std::any(bepaald::toNumber<long long int>(val)) : std::any(val));
      }

      std::vector<std::pair<XmlDocument::Node::StringOrRef, std::string>> attachments;
      if (n.name() == "mms")
      {
        // get message body && attachments
        std::string body;
        bool hasbody = false;
        std::string sourceaddress;
        for (auto const &sub : n)
        {
          //std::cout << sub.name() << std::endl;
          if (sub.name() == "parts")
          {
            for (auto const &part : sub)
            {
              if (part.hasAttribute("text"))
              {
                if (part.hasAttribute("ct") && part.getAttribute("ct") == "text/plain")
                {
                  body += part.getAttribute("text");
                  hasbody = true;
                }
              }
              if (part.hasAttribute("data"))
              {
                //std::cout << "HAS DATA" << std::endl;

                // do something with data...
                XmlDocument::Node::StringOrRef attachmentdata = part.getAttributeStringOrRef("data");
                if (attachmentdata.file.empty() && attachmentdata.value.empty()) [[unlikely]]
                {
                  Logger::warning("Got data attribute, but no value or reference");
                  continue;
                }
                std::string ct;
                if (part.hasAttribute("ct"))
                  ct = part.getAttribute("ct");
                attachments.emplace_back(std::make_pair(attachmentdata, ct));
              }
            }
          }
          else if (sub.name() == "addrs")
          {
            int numaddresses = 0;
            for (auto const &addr : sub)
            {
              ++numaddresses;

              // type - The type of address, 129 = BCC, 130 = CC, 151 = To, 137 = From
              //addr.print();
              if (addr.hasAttribute("type") && addr.getAttribute("type") == "137")
              {
                if (!sourceaddress.empty()) [[unlikely]]
                {
                  Logger::warning("Multiple source addresses for message");
                  sourceaddress.clear();
                  continue;
                }

                if (!addr.hasAttribute("address"))
                {
                  Logger::warning("No address attribute found in <addr>");
                  continue;
                }

                sourceaddress = addr.getAttribute("address");
              }
            }
            addvalue("numaddresses", numaddresses);
          }
        }

        if (hasbody)
          addvalue("body", std::move(body));

        if (!sourceaddress.empty())
          addvalue("sourceaddress", std::move(sourceaddress));
      }

      //std::cout << "attachments: " << attachments.size() << std::endl;
      addvalue("numattachments", attachments.size());

      // is sms
      addvalue("ismms", (n.name() == "mms") ? 1 : 0);

      if (!d_database.exec("INSERT INTO smses (" + columns + ") VALUES (" + placeholders + ")", values))
        return;

      if (!attachments.empty())
      {
        long long int lastid = d_database.lastId();
        for (auto const &a : attachments)
          d_database.exec("INSERT INTO attachments (mid, data, filename, pos, size, ct) "
                          "VALUES "
                          "(?, ?, ?, ?, ?, ?)", {lastid, a.first.value, a.first.file, a.first.pos, a.first.size, a.second});
      }

    }
    else [[unlikely]]
      warnOnce("Skipping unsupported element: '" + n.name() + "'");
  }

  // If contact_name IS NULL, "", or "(Unknown)", set it to MAX(contact_name) for that address,
  // If still empty (all messsages from that contact were NULL, "", OR "(Unknown)", set it
  // to address
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");
  SqliteDB::QueryResults addresses;
  if (!d_database.exec("SELECT DISTINCT address FROM smses", &addresses))
    return;
  for (unsigned int i = 0; i < addresses.rows(); ++i)
  {
    std::string cn = d_database.getSingleResultAs<std::string>("SELECT MAX(contact_name) FROM smses WHERE contact_name IS NOT '(Unknown)' AND contact_name IS NOT NULL AND contact_name IS NOT '' AND address = ?", addresses.value(i, 0), std::string());
    //std::cout << addresses(i, "address") << " '" << cn << "'" << std::endl;
    if (!d_database.exec("UPDATE smses SET contact_name = ? WHERE address = ?", {cn.empty() ? addresses.value(i, 0) : cn, addresses.value(i, 0)}))
      return;
  }
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");

  //d_database.prettyPrint(true, "SELECT min(date), max(date) FROM smses");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31611496644'");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31645756298'");
  //d_database.prettyPrint(true, "SELECT DISTINCT ismms, sourceaddress, numaddresses FROM smses");
  //d_database.prettyPrint(true, "SELECT DISTINCT sourceaddress, numaddresses FROM smses WHERE type = 2 AND ismms = 1");

  // this is how to scan for selfid (sourceaddress of outgoing message with at least one target recipient)
  //d_database.prettyPrint(true, "SELECT DISTINCT sourceaddress FROM smses WHERE numaddresses > 1 AND type = 2 AND ismms = 1");
  //d_database.prettyPrint(true, "SELECT * FROM smses WHERE sourceaddress IS NULL AND type = 2 AND ismms = 1");

  d_ok = true;
}
