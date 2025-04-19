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

#if __cpp_lib_span >= 202002L && (!defined __apple_build_version__ || __apple_build_version__ >= 15000100)
SignalPlaintextBackupDatabase::SignalPlaintextBackupDatabase(std::span<std::string const> const &sptbxmls, bool truncate, bool verbose,
                                                             std::vector<std::pair<std::string, std::string>> namemap,
                                                             std::string const &namemap_filename,
                                                             std::vector<std::pair<std::string, std::string>> const &addressmap,
                                                             std::string const &addressmap_filename,
                                                             std::string const &countrycode, bool autogroupnames)
#else
SignalPlaintextBackupDatabase::SignalPlaintextBackupDatabase(std::vector<std::string> const &sptbxmls, bool truncate, bool verbose,
                                                             std::vector<std::pair<std::string, std::string>> namemap,
                                                             std::string const &namemap_filename,
                                                             std::vector<std::pair<std::string, std::string>> const &addressmap,
                                                             std::string const &addressmap_filename,
                                                             std::string const &countrycode, bool autogroupnames)
#endif
  :
  d_countrycode(countrycode),
  d_addressmap(addressmap),
  d_ok(false),
  d_truncate(truncate),
  d_verbose(verbose)
{
  // read map from file
  auto readMapFromFile = [](std::string const &mapfilename, std::vector<std::pair<std::string, std::string>> *map) STATICLAMBDA
  {
    if (mapfilename.empty())
      return;

    // do NOT open in binary mode. We are using getline, on Windows,
    // using binary mode will append '\r' at the end of each line.
    std::ifstream mapfile(mapfilename, std::ios_base::in);
    if (mapfile.is_open()) [[unlikely]]
    {
      std::string line;
      while (std::getline(mapfile, line))
      {
        if (line.empty() || line[0] == '#')
          continue;

        std::string::size_type pos;
        if ((pos = line.find('=')) == std::string::npos)
        {
          Logger::warning("Failed to find delimiter in line ('", line, "')");
          continue;
        }

        // std::cout << line.substr(0, pos) << std::endl;
        // std::cout << line.substr(pos + 1) << std::endl;
        map->emplace_back(line.substr(0, pos), line.substr(pos + 1));
      }
    }
    else
      Logger::warning("Failed to open file '", mapfilename, "'");
  };

  // read and append namemap from file
  readMapFromFile(namemap_filename, &namemap);

  // read and append addressmap from file
  readMapFromFile(addressmap_filename, &d_addressmap);

  /*
  if (!namemap_filename.empty())
  {
    // do NOT open in binary mode. We are using getline, on Windows,
    // using binary mode will append '\r' at the end of each line.
    std::ifstream namemapfile(namemap_filename, std::ios_base::in);
    if (namemapfile.is_open()) [[unlikely]]
    {
      std::string line;
      while (std::getline(namemapfile, line))
      {
        if (line.empty() || line[0] == '#')
          continue;

        std::string::size_type pos;
        if ((pos = line.find('=')) == std::string::npos)
        {
          Logger::warning("Failed to find delimiter in line ('", line, "')");
          continue;
        }

        // std::cout << line.substr(0, pos) << std::endl;
        // std::cout << line.substr(pos + 1) << std::endl;
        namemap.emplace_back(line.substr(0, pos), line.substr(pos + 1));
      }
    }
    else
      Logger::warning("Failed to open file '", namemap_filename, "'");
  }
  */

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
                                                    {"ismms", "INTEGER", "none", ""},
                                                    {"sourceaddress", "TEXT", "none", ""},
                                                    {"targetaddresses", "TEXT", "none", ""}, // the target addresses of group message (json array)
                                                    {"numaddresses", "INTEGER", "none", ""},
                                                    {"numattachments", "INTEGER", "none", ""},
                                                    {"skip", "INTEGER", "none", ""}};     // skip entries are not real messages, they are just there to set a
                                                                                          // contacts name who otherwise does not appear in the database (never
                                                                                          // sent a message, but could be member of group)

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
                       "pos INTEGER DEFAULT -1, size INTEGER DEFAULT -1, ct TEXT default NULL, cl TEXT default NULL)"))
    return;

  // fill tables
  std::vector<std::any> values;
  std::string columns;
  std::string placeholders;
  std::set<std::string> group_only_contacts; // the set of contacts that only appear in groups, and only receives messages
  std::set<std::string> group_recipients; // the same but for each message...

  for (auto const &xmlfile : sptbxmls)
  {
    Logger::message("Parsing file: ", xmlfile);

    // open and parse XML file
    XmlDocument xmldoc(xmlfile);
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

                           //if (d_verbose) [[unlikely]]
                           {
                             Logger::warning_indent("Full node data:");
                             n.print();
                           }

                           return false;
                         }
                         return true;
                       }))
        continue;

      if (n.name() == "sms" || n.name() == "mms")
      {
        // build statement
        columns.clear();
        placeholders.clear();
        values.clear();
        group_recipients.clear();

        for (auto const &rc : columninfo)
        {
          if (rc.required_in_node != n.name() && !rc.required_in_node.empty())
            continue;

          std::string val = n.getAttribute(rc.name);

          if (val != "null")
          {
            if (rc.name == "address")
              addvalue(rc.name, normalizePhoneNumber(val));
            else
              addvalue((rc.columnname.empty() ? rc.name : rc.columnname), (rc.type == "INTEGER") ? std::any(bepaald::toNumber<long long int>(val)) : std::any(val));
          }
        }

        std::vector<std::tuple<XmlDocument::Node::StringOrRef, std::string, std::string>> attachments;
        if (n.name() == "mms")
        {
          // get message body && attachments
          std::string body;
          bool hasbody = false;
          std::string sourceaddress;
          for (auto const &childnode : n)
          {
            //std::cout << childnode.name() << std::endl;
            if (childnode.name() == "parts")
            {
              for (auto const &part : childnode)
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
                  if (attachmentdata.size > 0 && attachmentdata.file.empty() && attachmentdata.value.empty()) [[unlikely]]
                  {
                    Logger::warning("Got data attribute, but no value or reference");
                    n.print();
                    continue;
                  }
                  std::string ct;
                  if (part.hasAttribute("ct"))
                    ct = part.getAttribute("ct");

                  std::string cl;
                  if (part.hasAttribute("cl"))
                    cl = part.getAttribute("cl");

                  attachments.emplace_back(std::move(attachmentdata), std::move(ct), std::move(cl));

                }
              }
            }
            else if (childnode.name() == "addrs")
            {
              int numaddresses = 0;
              for (auto const &addr : childnode)
              {
                ++numaddresses;
                //addr.print();
                if (!addr.hasAttribute("address"))
                {
                  Logger::warning("No address attribute found in <addr>");
                  continue;
                }
                std::string groupmsgaddress = normalizePhoneNumber(addr.getAttribute("address"));

                // type - The type of address, 129 = BCC, 130 = CC, 151 = To, 137 = From
                if (addr.hasAttribute("type") && addr.getAttribute("type") == "137")
                {
                  if (!sourceaddress.empty()) [[unlikely]]
                  {
                    Logger::warning("Multiple source addresses for message");
                    sourceaddress.clear();
                    continue;
                  }
                  sourceaddress = groupmsgaddress;
                  group_only_contacts.insert(std::move(groupmsgaddress));
                }
                else // likely a receiving addr
                {
                  group_recipients.insert(groupmsgaddress);
                  group_only_contacts.insert(std::move(groupmsgaddress));
                }
              }
              addvalue("numaddresses", numaddresses);
            }
          }

          if (hasbody)
            addvalue("body", std::move(body));

          if (!sourceaddress.empty())
            addvalue("sourceaddress", std::move(sourceaddress));

          if (!group_recipients.empty())
          {
            // might need to not do this manually...
            std::string json_array_recipients("[");
            for (auto it = group_recipients.begin(); it != group_recipients.end(); ++it)
              json_array_recipients += (it != group_recipients.begin() ? (", \"" + *it + "\"") : ("\"" + *it + "\""));
            json_array_recipients += ']';
            addvalue("targetaddresses", json_array_recipients);
          }
        }

        //std::cout << "attachments: " << attachments.size() << std::endl;
        addvalue("numattachments", attachments.size());

        // is sms
        addvalue("ismms", (n.name() == "mms") ? 1 : 0);

        // dont skip, this is a real message
        addvalue("skip", 0);

        if (!d_database.exec(bepaald::concat("INSERT INTO smses (", columns, ") VALUES (", placeholders, ")"), values))
          return;

        if (!attachments.empty())
        {
          long long int lastid = d_database.lastId();
          for (auto const &a : attachments)
            d_database.exec("INSERT INTO attachments (mid, data, filename, pos, size, ct, cl) "
                            "VALUES "
                            "(?, ?, ?, ?, ?, ?, ?)", {lastid, std::get<0>(a).value, std::get<0>(a).file, std::get<0>(a).pos, std::get<0>(a).size,
                                                      std::get<1>(a), std::get<2>(a)});
        }

      }
      else [[unlikely]]
        Logger::warnOnce("Skipping unsupported element: '" + n.name() + "'");
    }
  }

  // add group-only-contacts, as 'skip' messages, so they can be mapped...
  Logger::message("Marking group-only contacts");
  for (auto const &a : group_only_contacts)
    if (!d_database.exec("INSERT INTO smses (address, skip, contact_name) SELECT ?1, 1, ?1 "
                         "WHERE NOT EXISTS (SELECT 1 FROM smses WHERE address = ?1 AND skip = 0)", a))
    {
      Logger::warning("Failed to add group-only-contact ", a);
      continue;
    }
  //d_database.prettyPrint(false, "SELECT address FROM smses WHERE skip = 1");
  //d_database.prettyPrint(false, "SELECT address FROM smses WHERE skip = 1 AND address IN (SELECT DISTINCT address FROM smses WHERE skip = 0)");

  // If contact_name IS NULL, "", or "(Unknown)", set it to MAX(contact_name) for that address,
  // If still empty (all messsages from that contact were NULL, "", OR "(Unknown)", set it
  // to address
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");

  Logger::message("Setting contact names where empty");
  SqliteDB::QueryResults addresses;
  if (!d_database.exec("SELECT DISTINCT address FROM smses", &addresses))
    return;
  for (unsigned int i = 0; i < addresses.rows(); ++i)
  {
    std::string cn = d_database.getSingleResultAs<std::string>("SELECT MAX(contact_name) FROM smses "
                                                               "WHERE contact_name IS NOT '(Unknown)' AND contact_name IS NOT NULL AND contact_name IS NOT '' "
                                                               "AND address = ?", addresses.value(i, 0), std::string());
    //std::cout << addresses(i, "address") << " '" << cn << "'" << std::endl;
    if (!d_database.exec("UPDATE smses SET contact_name = ? WHERE address = ?", {cn.empty() ? addresses.value(i, 0) : cn, addresses.value(i, 0)}))
      return;
  }
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");

  Logger::message("Apply name-mapping");
  // apply name-mapping....
  for (auto const &[addr, cn] : namemap)
  {
    //std::cout << "Map " << addr << " -> " << cn << std::endl;
    if (!d_database.exec("UPDATE smses SET contact_name = ? WHERE address = ?", {cn, addr}))
    {
      Logger::warning("Failed to set contact name of ", addr, " to \"", cn, "\"");
      continue;
    }

    if (d_database.changed() == 0) // no messages from this contact... add special entry
    {
      if (!d_database.exec("INSERT INTO smses (address, contact_name, skip) VALUES (?, ?, ?)", {addr, cn, 1}))
      {
        Logger::warning("Failed to set contact name of ", addr, " to \"", cn, "\"");
        continue;
      }
    }
  }

  if (autogroupnames)
  {
    Logger::message("Auto-generating groupnames");
    SqliteDB::QueryResults allgroups;
    if (d_database.exec("SELECT DISTINCT address FROM smses WHERE address LIKE '%~%' AND SUBSTR(address, 1, 1) != '~' AND SUBSTR(address, LENGTH(address), 1) != '~'", &allgroups))
    {
      //std::cout << "allgroups size: " << allgroups.rows() << std::endl;
      for (unsigned int i = 0; i < allgroups.rows(); ++i)
      {
        std::string new_contact_name;
        std::string groupaddress = allgroups(i, "address");

        //std::cout << "Doing: " << groupaddress << std::endl;

        std::string::size_type start = 0;
        std::string::size_type end;
        while (true)
        {
          end = groupaddress.find('~', start);

          //std::cout << "Single: " << groupaddress.substr(start, (end == std::string::npos ? end : end - start)) << std::endl;
          std::string groupname_part =  d_database.getSingleResultAs<std::string>("SELECT DISTINCT contact_name FROM smses WHERE address = ? LIMIT 1",
                                                                                  groupaddress.substr(start, (end == std::string::npos ? end : end - start)),
                                                                                  std::string());
          if (groupname_part.empty()) [[unlikely]]
            groupname_part = groupaddress.substr(start, (end == std::string::npos ? end : end - start));

          new_contact_name += new_contact_name.empty() ? groupname_part : ", " + groupname_part;

          if (end == std::string::npos)
            break;
          start = end + 1;
        }

        //std::cout << "Got: '" << new_contact_name << "'" << std::endl;

        if (!d_database.exec("UPDATE smses SET contact_name = ? WHERE address = ?", {new_contact_name, groupaddress}))
          Logger::warning("Failed to set contact_name of group ('", groupaddress, "') to '", new_contact_name, "'");
        // else
        //   std::cout << "Updated " << d_database.changed() << std::endl;
      }
    }
  }

  // show if we have other sources than self as the sender of outgoing messages
  //d_database.prettyPrint(d_truncate, "SELECT DISTINCT sourceaddress FROM smses WHERE ismms = 1 AND type = 2");

  /*
  // for all distinct names, set address for that name to be the same..?
  //d_database.prettyPrint(false, "SELECT DISTINCT rowid,targetaddresses FROM smses WHERE targetaddresses IS NOT NULL");
  SqliteDB::QueryResults all_names_res;
  if (d_database.exec("SELECT contact_name, address FROM smses GROUP BY contact_name, address LIKE '%~%'", &all_names_res)) // "pick one address for each name"
  {
  // all_names_res.prettyPrint(false);

  SqliteDB::QueryResults old_addresses;
  for (unsigned int i = 0; i < all_names_res.rows(); ++i)
  {
  // get the old addresses, that we are going to change
  d_database.exec("SELECT DISTINCT address FROM smses WHERE contact_name IS ? AND address IS NOT ? AND "
  "((address LIKE '%~%' AND ? LIKE '%~%') OR (address NOT LIKE '%~%' AND ? NOT LIKE '%~%'))",
  {all_names_res.value(i, "contact_name"), all_names_res.value(i, "address"),
  all_names_res.value(i, "address"), all_names_res.value(i, "address")},
  &old_addresses);

  Logger::message(all_names_res(i, "address"), ":");
  old_addresses.prettyPrint(false);

  // change address, and sourceaddress, and targetaddress
  for (unsigned int j = 0; j < old_addresses.rows(); ++j)
  {
  d_database.exec("UPDATE smses SET address = ? WHERE address = ?",
  {all_names_res.value(i, "address"), old_addresses.value(j, "address")});
  //std::cout << "Addr change: " << d_database.changed() << std::endl;

  d_database.exec("UPDATE smses SET sourceaddress = ? WHERE sourceaddress = ?",
  {all_names_res.value(i, "address"), old_addresses.value(j, "address")});
  //std::cout << "SrcAddr change: " << d_database.changed() << std::endl;

  d_database.exec("WITH to_update AS "
  "("
  "  SELECT smses.rowid, json_set(smses.targetaddresses, fullkey, ?) AS new_array FROM smses, json_each(targetaddresses) WHERE value = ?"
  ") "
  "UPDATE smses SET targetaddresses = "
  "  ("
  "    SELECT new_array FROM to_update WHERE to_update.rowid = smses.rowid"
  "  )"
  "  WHERE smses.rowid IN (SELECT to_update.rowid FROM to_update)",
  {all_names_res.value(i, "address"), old_addresses.value(j, "address")});
  // alternative... looks better, not sure if it is better (will always change all rows?)
  // d_database.exec("UPDATE smses SET targetaddresses = "
  //                 "("
  //                 "  SELECT json_group_array("
  //                 "    CASE"
  //                 "      WHEN value = ? THEN ?"
  //                 "      ELSE value"
  //                 "    END"
  //                 "  )"
  //                 "  FROM json_each(smses.targetaddresses)"
  //                 ") "
  //                 "WHERE targetaddresses IS NOT NULL",
  // {old_addresses.value(j, "address"), all_names_res.value(i, "address")});
  //std::cout << "TgtAddr change: " << d_database.changed() << std::endl;
  }
  }
  //d_database.prettyPrint(false, "SELECT DISTINCT rowid,targetaddresses FROM smses WHERE targetaddresses IS NOT NULL");
  }
  */

  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");
  //d_database.prettyPrint(true, "SELECT DISTINCT address, contact_name FROM smses ORDER BY address ASC");
  //d_database.prettyPrint(true, "SELECT min(date), max(date) FROM smses");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31611496644'");
  //d_database.prettyPrint(true, "SELECT DISTINCT contact_name, body FROM smses WHERE address = '+31645756298'");
  //d_database.prettyPrint(true, "SELECT DISTINCT ismms, sourceaddress, numaddresses FROM smses");
  //d_database.prettyPrint(true, "SELECT DISTINCT sourceaddress, numaddresses FROM smses WHERE type = 2 AND ismms = 1");
  // this is how to scan for selfid (sourceaddress of outgoing message with at least one target recipient)
  //d_database.prettyPrint(true, "SELECT DISTINCT sourceaddress FROM smses WHERE numaddresses > 1 AND type = 2 AND ismms = 1");
  //d_database.prettyPrint(true, "SELECT * FROM smses WHERE sourceaddress IS NULL AND type = 2 AND ismms = 1");
  //d_database.prettyPrint(true, "SELECT * FROM smses LIMIT 50");
  //d_database.prettyPrint(false, "SELECT DISTINCT COUNT(DISTINCT numaddresses) FROM smses WHERE address LIKE '%~%' GROUP BY address");
  //d_database.printLineMode("SELECT body,HEX(body) FROM smses WHERE date = 1734628440524");
  //d_database.saveToFile("plaintext.sqlite");

  d_ok = true;
}
