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

#include "signalbackup.ih"

#include "../jsondatabase/jsondatabase.h"

bool SignalBackup::tgMapContacts(JsonDatabase const &jsondb, std::string const &chatlist,
                                 std::vector<std::pair<std::vector<std::string>, long long int>> *contactmap,
                                 std::vector<std::string> const &inhibitmappping) const
{
  if (d_verbose && contactmap->size()) [[unlikely]]
  {
    Logger::message("[INITIAL CONTACT MAP ]");
    for (unsigned int i = 0; i < contactmap->size(); ++i)
    {
      std::string name = getNameFromRecipientId((*contactmap)[i].second);
      Logger::message(" * ", (*contactmap)[i].first, " -> ", (*contactmap)[i].second, " (", name, ")");
    }
  }

  std::vector<std::pair<std::vector<std::string>, long long int>> realcontactmap = *contactmap;

  auto find_in_contactmap = [&realcontactmap](std::string const &identifier) -> long long int
  {
    for (unsigned int i = 0; i < realcontactmap.size(); ++i)
      for (unsigned int j = 0; j < realcontactmap[i].first.size(); ++j)
        if (realcontactmap[i].first[j] == identifier)
          return i;
    return -1;
  };

  std::vector<std::pair<long long int, std::vector<std::string>>> recipientsnotfound;
  auto move_from_not_found_to_contactmap = [&recipientsnotfound, &realcontactmap](SqliteDB::QueryResults const &contacts, std::string const &identifyer)
  {
    for (auto it = recipientsnotfound.begin(); it != recipientsnotfound.end(); ++it)
      if (contacts.valueAsString(it->first, "id") == identifyer)
      {
        for (unsigned int i = 0; i < it->second.size(); ++i)
          if (!bepaald::contains(realcontactmap.back().first, it->second[i]))
            realcontactmap.back().first.push_back(it->second[i]);
        recipientsnotfound.erase(it);
        break;
      }
  };

  // get contacts that need matching from database
  SqliteDB::QueryResults json_contacts;
  if (!jsondb.d_database.exec("SELECT DISTINCT id FROM chats " + (chatlist.empty() ? "" : "WHERE idx IN " + chatlist + " ") +
                              "UNION "
                              "SELECT DISTINCT from_id AS id FROM messages WHERE type IS 'message' " + (chatlist.empty() ? "" : "AND chatidx IN " + chatlist),
                              &json_contacts))
    return false;

  if (d_verbose) [[unlikely]]
  {
    Logger::message("ALL CONTACTS IN JSON: ");
    json_contacts.prettyPrint(d_truncate);
  }

  for (unsigned int i = 0; i < json_contacts.rows(); ++i)
  {
    std::string contact = json_contacts.valueAsString(i, "id");

    // if it's already in contactmap, we can skip it
    if (find_in_contactmap(contact) != -1)
    {
      //std::cout << "Skipping " << contact << std::endl;
      //std::cout << realcontactmap[contact].first << std::endl;
      continue;
    }

    // find it in android db by name
    long long int found_id = -1;
    SqliteDB::QueryResults aliases;
    jsondb.d_database.exec("SELECT DISTINCT from_name AS name FROM messages WHERE from_id = ?1 "
                           "UNION "
                           "SELECT DISTINCT name FROM chats WHERE id = ?1", contact, &aliases);
    for (unsigned int j = 0; j < aliases.rows(); ++j)
    {
      if (bepaald::contains(inhibitmappping, aliases(j, "name")))
        continue;

      // if the contact is already in contactmap (by alias), we are done again
      long long int contactidx = -1;
      if ((contactidx = find_in_contactmap(aliases(j, "name"))) != -1)
        found_id = realcontactmap[contactidx].second;
      else
        found_id = getRecipientIdFromName(aliases(j, "name"), false);
      if (found_id != -1)
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Found json contact by name: ", contact, " (", aliases(j, "name"), ") -> ", found_id);
        break;
      }
    }
    if (found_id != -1)
    {
      // we found this contact, add it (and all names) to map
      realcontactmap.push_back({{contact}, found_id});
      for (unsigned int j = 0; j < aliases.rows(); ++j)
        realcontactmap.back().first.push_back(aliases(j, "name"));
      continue;
    }
    else
    {
      recipientsnotfound.emplace_back(i, std::vector<std::string>());
      for (unsigned int j = 0; j < aliases.rows(); ++j)
        recipientsnotfound.back().second.emplace_back(aliases.isNull(j, "name") ? "null" : aliases(j, "name"));
    }
  }

  // now let's try to find self
  std::vector<std::string> self_json_ids;
  // check if we have already (maybe passed in map, maybe matched by name)
  for (unsigned int i = 0; i < realcontactmap.size(); ++i)
    if (realcontactmap[i].second == d_selfid)
      for (auto const &sid : realcontactmap[i].first)
        if (!bepaald::contains(self_json_ids, sid))
          self_json_ids.emplace_back(sid);

  // try to determine: 1. If the database has a 'saved_messages' type chat, it should only contain
  // messages "from": self (messages.from_id => self) + the chat itself should be self (chat.id => self)

  //if (self_json_ids.empty())
  {
    SqliteDB::QueryResults ids_in_saved_messages;
    if (jsondb.d_database.exec("SELECT DISTINCT from_id FROM messages WHERE chatidx IN (SELECT DISTINCT idx FROM chats WHERE type = 'saved_messages')", &ids_in_saved_messages) &&
        ids_in_saved_messages.rows() == 1)
    {
      if (!find_in_contactmap(ids_in_saved_messages("from_id")))
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Found json contact from saved_messages (self-msg-from-id): ", ids_in_saved_messages("from_id"), " -> ", d_selfid);

        realcontactmap.push_back({{ids_in_saved_messages("from_id")}, d_selfid});
        // copy aliases and erase from not found
        move_from_not_found_to_contactmap(json_contacts, ids_in_saved_messages("from_id"));

        for (auto const &sid : realcontactmap.back().first)
          if (!bepaald::contains(self_json_ids, sid))
            self_json_ids.emplace_back(sid);
      }
    }

    SqliteDB::QueryResults saved_messages_id;
    if (jsondb.d_database.exec("SELECT DISTINCT id FROM chats WHERE type = 'saved_messages'", &saved_messages_id) &&
        saved_messages_id.rows() == 1)
    {
      if (!find_in_contactmap(saved_messages_id("id")))
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Found json contact from saved_messages (self-chat-id): ", saved_messages_id("id"), " -> ", d_selfid);

        realcontactmap.push_back({{saved_messages_id("id")}, d_selfid});
        // copy aliases and erase from not found
        move_from_not_found_to_contactmap(json_contacts, saved_messages_id("id"));

        for (auto const &sid : realcontactmap.back().first)
          if (!bepaald::contains(self_json_ids, sid))
            self_json_ids.emplace_back(sid);
      }
    }
  }

  // try to determine: 2. only one from_id will be present in multiple 1-on-1 chats
  SqliteDB::QueryResults ids_in_personal_chats;
  jsondb.d_database.exec("SELECT from_id, COUNT(from_id) AS idcount FROM (SELECT DISTINCT from_id, chatidx FROM messages WHERE type = 'message' AND chatidx IN (SELECT idx FROM chats WHERE type = 'personal_chat')) GROUP BY from_id HAVING idcount > 1", &ids_in_personal_chats);

  //ids_in_personal_chats.prettyPrint(d_truncate);

  // if all ids except one occur only once, the exception surely is self
  if (ids_in_personal_chats.rows() == 1)
  {
    if (!find_in_contactmap(ids_in_personal_chats("from_id")))
    {
      if (d_verbose) [[unlikely]]
        Logger::message("Found json contact for self: ", ids_in_personal_chats(0, "from_id"), " -> ", d_selfid);

      realcontactmap.push_back({{ids_in_personal_chats("from_id")}, d_selfid});
      // copy aliases and erase from not found
      move_from_not_found_to_contactmap(json_contacts, ids_in_personal_chats("from_id"));

      for (auto const &sid : realcontactmap.back().first)
        if (!bepaald::contains(self_json_ids, sid))
          self_json_ids.emplace_back(sid);
    }
  }

  // if we have self, we can probably merge some chatids and userids (somehow)...
  if (!self_json_ids.empty())
  {
    // for each chat id that is personal_chat
    SqliteDB::QueryResults personal_chat_contacts;
    if (!jsondb.d_database.exec("SELECT DISTINCT from_id, chatidx FROM messages "
                                "WHERE type = 'message' "
                                //"AND chatidx IN (SELECT idx FROM chats WHERE type = 'personal_chat')",
                                "AND chatidx IN (SELECT idx FROM chats WHERE type = 'personal_chat'" + (chatlist.empty() ? "" : " AND idx IN " + chatlist) + ")",
                                &personal_chat_contacts))
      return false;

    //Logger::message("json self: ", Logger::VECTOR(self_json_ids, ", "));
    //personal_chat_contacts.prettyPrint(d_truncate);
    /*
      results, for example:

      ----------------------------
      | from_id        | chatidx |
      ----------------------------
      | user6805890839 | 2       | <- self
      | user1234567890 | 2       | <- other in chat 2
      | user6805890839 | 6       | <- self
      | user9876543210 | 6       | <- other in chat 6
      ----------------------------
    */


    for (unsigned int i = 0; i < personal_chat_contacts.rows(); ++i)
    {
      // each personal chat has at most two participants, one is 'self', skip that one...
      if (bepaald::contains(self_json_ids, personal_chat_contacts(i, "from_id")))
        continue;

      std::string linkchat = jsondb.d_database.getSingleResultAs<std::string>("SELECT id FROM chats WHERE idx = ?", personal_chat_contacts.value(i, "chatidx"), std::string());
      if (linkchat.empty()) // we failed apparently
        continue;

      //std::cout << "SHOULD LINK " << personal_chat_contacts(i, "from_id") << " WITH " << linkchat << std::endl;
      long long int contactidx = find_in_contactmap(personal_chat_contacts(i, "from_id"));
      long long int chatidx = find_in_contactmap(linkchat);

      // if both are already present
      if (contactidx != -1 && chatidx != -1)
        continue;

      // if one of the two is already in the contactmap, just make sure they link to the same Signal_id (and erase from notfound)
      if (contactidx != -1)
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Linking contacts (contactidx was found): ", personal_chat_contacts(i, "from_id"), " == ", linkchat);

        realcontactmap.push_back({{linkchat}, realcontactmap[contactidx].second});
        // copy aliases and erase from not found
        move_from_not_found_to_contactmap(json_contacts, linkchat);
        continue;
      }

      if (chatidx != -1)
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Linking contacts (chatidx was found): ", personal_chat_contacts(i, "from_id"), " == ", linkchat);

        realcontactmap.push_back({{personal_chat_contacts(i, "from_id")}, realcontactmap[chatidx].second});
        // copy aliases and erase from not found
        move_from_not_found_to_contactmap(json_contacts, personal_chat_contacts(i, "from_id"));
        continue;
      }

      // if both are not in contactmap, merge them in recipientsnotfound just for a cleaner prompt to user
      std::pair<long long int, std::vector<std::string>> tmp;

      // find the first in the recipientsnotfound, save it and delete it
      bool removed = false;
      for (auto it = recipientsnotfound.begin(); it != recipientsnotfound.end(); ++it)
      {
        if (json_contacts.valueAsString(it->first, "id") == linkchat)
        {
          tmp = *it;
          recipientsnotfound.erase(it);
          removed = true;
          break;
        }
      }
      if (!removed) [[unlikely]]
        Logger::warning("Something went wrong merging unknown json contacts");
      else // then merge its aliases into the others (if not present)
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Linking unmapped contacts: ", personal_chat_contacts(i, "from_id"), " == ", linkchat);

        for (auto it = recipientsnotfound.begin(); it != recipientsnotfound.end(); ++it)
        {
          if (json_contacts.valueAsString(it->first, "id") == personal_chat_contacts(i, "from_id"))
          {
            // the first of each of these should be guaranteed the json id?
            it->second.insert(it->second.begin(), json_contacts(tmp.first, "id"));
            for (unsigned int j = 1; j < tmp.second.size(); ++j)
              if (!bepaald::contains(it->second, tmp.second[j]))
                it->second.push_back(tmp.second[j]);
            break;
          }
        }
      }
    }
  }

  if (d_verbose) [[unlikely]]
  {
    std::sort(realcontactmap.begin(), realcontactmap.end(), [](auto const &a, auto const &b) { return a.second < b.second; });
    Logger::message("[FINAL CONTACT MAP] ");
    for (unsigned int i = 0; i < realcontactmap.size(); ++i)
    {
      std::string name = getNameFromRecipientId(realcontactmap[i].second);
      Logger::message(" * ", Logger::VECTOR(realcontactmap[i].first, ", "), " -> ", realcontactmap[i].second, " (", name, ")");
    }
  }

  *contactmap = std::move(realcontactmap);

  if (!recipientsnotfound.empty())
  {
    Logger::error("The following contacts in the JSON input were not found in the Android backup:");
    for (auto const &r : recipientsnotfound)
      if (r.second.size() == 0)
        Logger::error_indent(" - \"", json_contacts.valueAsString(r.first, "id"), "\"");
      else
        Logger::error_indent(" - \"", json_contacts.valueAsString(r.first, "id"), "\"/\"", Logger::VECTOR(r.second, "\"/\""), "\"");

    Logger::message("Use `--mapjsoncontacts [NAME1]=[id1],[NAME2]=[id2],...' to map these to an existing recipient id \n"
                    "from the backup. The list of available recipients and their id's can be obtained by running \n"
                    "with `--listrecipients'.");

    return false;
  }

  return recipientsnotfound.empty();
}
