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

#include "jsondatabase.ih"

#include "../common_filesystem.h"

JsonDatabase::JsonDatabase(std::string const &jsonfile, bool verbose, bool truncate)
  :
  d_ok(false),
  d_verbose(verbose),
  d_truncate(truncate)
{
  // open file, get size and read data
  std::ifstream sourcefile(jsonfile, std::ios_base::binary | std::ios_base::in);
  if (!sourcefile.is_open())
  {
    Logger::error("Failed to open file for reading: ", jsonfile);
    return;
  }

  //sourcefile.seekg(0, std::ios_base::end);
  //long long int datasize = sourcefile.tellg();
  //sourcefile.seekg(0, std::ios_base::beg);
  uint64_t datasize = bepaald::fileSize(jsonfile);
  if (datasize == 0 || datasize == static_cast<std::uintmax_t>(-1)) [[unlikely]]
  {
    Logger::error("Bad filesize (", datasize, ")");
    return;
  }

  std::unique_ptr<char[]> data(new char[datasize]);

  if (!sourcefile.read(data.get(), datasize))
  {
    Logger::error("Failed to read json data");
    return;
  }

  // create tables
  if (!d_database.exec("CREATE TABLE chats(idx INT, id TEXT, name TEXT, type TEXT)") ||
      !d_database.exec("CREATE TABLE tmp_json_tree (value TEXT, path TEXT)") ||
      !d_database.exec("CREATE TABLE messages(chatidx INT, id INT, type TEXT, date INT, "
                       "from_name TEXT, from_id TEXT, body TEXT, "
                       "reply_to_id INT, forwarded_from TEXT, "
                       "saved_from TEXT, photo TEXT, width INT, height INT, "
                       "file TEXT, media_type TEXT, mime_type TEXT, "
                       "contact_vcard TEXT, reactions TEXT, delivery_receipts TEXT, poll)"))
  {
    Logger::error("Failed to set up sql tables");
    return;
  }

  // INSERT DATA INTO CHATS TABLE
  if (d_verbose) [[unlikely]]
    Logger::message_start("Inserting chats from json...");
  if (!d_database.exec("INSERT INTO chats SELECT "
                       "key, "
                       "json_extract(value, '$.id') AS id, "
                       "json_extract(value, '$.name') AS name, "
                       "json_extract(value, '$.type') AS type "
                       "FROM json_each(?, '$.chats.list')",
                       std::string_view(data.get(), datasize)))
  {
    Logger::error("Failed to fill sql table");
    return;
  }

  if (d_database.changed() == 0) // maybe single-chat-json ?
  {
    if (d_verbose) [[unlikely]]
    {
      Logger::message("No chats-list found, trying single chat list");
      Logger::message_start("Inserting chats from json...");
    }

    if (!d_database.exec("INSERT INTO chats SELECT "
                         "0, "
                         "json_extract(?1, '$.id') AS id, "
                         "json_extract(?1, '$.name') AS name, "
                         "json_extract(?1, '$.type') AS type",
                         std::string_view(data.get(), datasize)))
    {
      Logger::error("Failed to fill sql table");
      return;
    }
  }
  if (d_verbose) [[unlikely]]
    Logger::message_end("done! (", d_database.changed(), ")");

  // std::cout << std::endl << "CHATS: " << std::endl;
  // d_database.prettyPrint(d_truncate, "SELECT COUNT(*) FROM chats");
  // d_database.prettyPrint(d_truncate, "SELECT * FROM chats LIMIT 10");

  // INSERT DATA INTO MESSAGES TABLE

  // note: to glob-match '$.chats.list[0].messages' for any number, we create a character class
  //       for the first '[' -> '[[]', since GLOB has no escape characters
  if (d_verbose) [[unlikely]]
    Logger::message_start("Inserting messages from json...");
  if (!d_database.exec("INSERT INTO tmp_json_tree SELECT value, path "
                       "FROM json_tree(?) WHERE path GLOB '$.chats.list[[][0-9]*].messages'", std::string_view(data.get(), datasize)))
    return;
  if (d_database.changed() == 0)
  {
    if (d_verbose) [[unlikely]]
    {
      Logger::message("Json tree appears empty, trying to interpret json as single-chat-export");
      Logger::message_start("Inserting messages from json...");
    }

    if (!d_database.exec("INSERT INTO tmp_json_tree SELECT value, '$.chats.list[0].messages' AS path "
                         "FROM json_tree(?) WHERE path = '$.messages'", std::string_view(data.get(), datasize)))
      return;
  }

  if (!d_database.exec("INSERT INTO messages SELECT "
                       "REPLACE(REPLACE(path, '$.chats.list[', ''), '].messages', '') AS chatidx, "
                       "json_extract(value, '$.id') AS id, "
                       "json_extract(value, '$.type') AS type, "
                       "json_extract(value, '$.date_unixtime') AS date, "
                       "json_extract(value, '$.from') AS from_name, "
                       "json_extract(value, '$.from_id') AS from_id, "
                       "json_extract(value, '$.text_entities') AS body, "
                       "json_extract(value, '$.reply_to_message_id') AS reply_to_id, "
                       "json_extract(value, '$.forwarded_from') AS forwarded_from, "
                       "json_extract(value, '$.saved_from') AS saved_from, "
                       "json_extract(value, '$.photo') AS photo, "
                       "json_extract(value, '$.width') AS width, "
                       "json_extract(value, '$.height') AS height, "
                       "json_extract(value, '$.file') AS file, "
                       "json_extract(value, '$.media_type') AS media_type, "
                       "json_extract(value, '$.mime_type') AS mime_type, "
                       "json_extract(value, '$.contact_vcard') AS contact_vcard, "
                       "json_extract(value, '$.custom_reactions') AS reactions, "
                       "json_extract(value, '$.custom_delivery_reports') AS delivery_receipts, "
                       "json_extract(value, '$.poll') AS poll "
                       "FROM tmp_json_tree"))
    return;

  // the 'saved_messages' chat has no 'name' field. Since this is note-to-self, the name should be the name of the
  // 'from' field of all messages in that chat.
  SqliteDB::QueryResults saved_messages_name;
  if (d_database.exec("SELECT DISTINCT from_name FROM messages WHERE chatidx IN "
                      "(SELECT DISTINCT idx FROM chats WHERE type = 'saved_messages')", &saved_messages_name) &&
      saved_messages_name.rows() == 1)
    d_database.exec("UPDATE chats SET name = ? WHERE name IS NULL AND type = 'saved_messages'", saved_messages_name.value(0, 0));

  if (d_verbose) [[unlikely]]
    Logger::message_end("done! (", d_database.changed(), ")");

  d_database.exec("DROP TABLE tmp_json_tree");

  // std::cout << std::endl << "MESSAGES: " << std::endl;
  // d_database.prettyPrint(d_truncate, "SELECT COUNT(*) FROM messages");
  // d_database.prettyPrint(d_truncate, "SELECT * FROM messages");// WHERE chatidx = 42 LIMIT 10");

  d_ok = true;
}
