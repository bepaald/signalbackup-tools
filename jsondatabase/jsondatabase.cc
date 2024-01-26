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

#include "jsondatabase.ih"

JsonDatabase::JsonDatabase(std::string const &jsonfile, bool verbose)
  :
  d_ok(false),
  d_verbose(verbose)
{
  // open file, get size and read data
  std::ifstream sourcefile(jsonfile, std::ios_base::binary | std::ios_base::in);
  if (!sourcefile.is_open())
  {
    Logger::error("Failed to open file for reading: ", jsonfile);
    return;
  }

  sourcefile.seekg(0, std::ios_base::end);
  long long int datasize = sourcefile.tellg();
  if (datasize <= 0)
  {
    Logger::error("Bad filesize (", datasize, ")");
    return;
  }

  sourcefile.seekg(0, std::ios_base::beg);
  std::unique_ptr<char []> data(new char[datasize]);

  if (!sourcefile.read(data.get(), datasize))
  {
    Logger::error("Failed to read json data");
    return;
  }

  // create tables
  if (!d_database.exec("CREATE TABLE chats(idx INT, name TEXT, type TEXT)") ||
      !d_database.exec("CREATE TABLE tmp_json_tree (value TEXT, path TEXT)") ||
      !d_database.exec("CREATE TABLE messages(chatidx INT, id INT, type TEXT, date INT, from_name TEXT, body TEXT, "
                        "reply_to_id INT, forwarded_from TEXT, "
                        "photo TEXT, width INT, height INT, "
                        "file TEXT, media_type TEXT, mime_type TEXT, "
                        "poll)"))
  {
    Logger::error("Failed to set up sql tables");
    return;
  }

  // INSERT DATA INTO CHATS TABLE
  if (d_verbose) [[unlikely]]
    Logger::message_start("Inserting chats from json...");
  if (!d_database.exec("INSERT INTO chats SELECT key,json_extract(value, '$.name') AS name, json_extract(value, '$.type') AS type FROM json_each(?, '$.chats.list')",
                       SqliteDB::StaticTextParam(data.get(), datasize)))
  {
    Logger::error("Failed to fill sql table");
    return;
  }

  if (d_database.changed() == 0) // maybe single-chat-json ?
  {
    if (d_verbose) [[unlikely]]
      Logger::message("No chats-list found, trying single chat list");

    if (!d_database.exec("INSERT INTO chats SELECT "
                         "0, "
                         "json_extract(?, '$.name') AS name, "
                         "json_extract(?, '$.type') AS type",
                         {SqliteDB::StaticTextParam(data.get(), datasize), SqliteDB::StaticTextParam(data.get(), datasize)}))
    {
      Logger::error("Failed to fill sql table");
      return;
    }
  }
  if (d_verbose) [[unlikely]]
    Logger::message("done! (", d_database.changed(), ")");
  // std::cout << std::endl << "CHATS: " << std::endl;
  // d_database.prettyPrint("SELECT COUNT(*) FROM chats");
  // d_database.prettyPrint("SELECT * FROM chats LIMIT 10");

  // INSERT DATA INTO MESSAGES TABLE

  // note: to glob-match '$.chats.list[0].messages' for any number, we create a character class for the first '[' -> [[], since GLOB has no escape characters
  if (d_verbose) [[unlikely]]
    Logger::message_start("Inserting messages from json...");
  if (!d_database.exec("INSERT INTO tmp_json_tree SELECT value, path "
                       "FROM json_tree(?) WHERE path GLOB '$.chats.list[[][0-9]*].messages'", SqliteDB::StaticTextParam(data.get(), datasize)))
    return;
  if (d_database.changed() == 0)
  {
    if (d_verbose) [[unlikely]]
      Logger::message("Json tree appears empty, trying empty list");

    if (!d_database.exec("INSERT INTO tmp_json_tree SELECT value, '$.chats.list[0].messages' AS path "
                         "FROM json_tree(?) WHERE path = '$.messages'", SqliteDB::StaticTextParam(data.get(), datasize)))
      return;
  }

  if (!d_database.exec("INSERT INTO messages SELECT "
                       "REPLACE(REPLACE(path, '$.chats.list[', ''), '].messages', '') AS chatidx, "
                       "json_extract(value, '$.id') AS id, "
                       "json_extract(value, '$.type') AS type, "
                       "json_extract(value, '$.date_unixtime') AS date, "
                       "json_extract(value, '$.from') AS from_name, "
                       "json_extract(value, '$.text_entities') AS body, "
                       "json_extract(value, '$.reply_to_message_id') AS reply_to_id, "
                       "json_extract(value, '$.forwarded_from') AS forwarded_from, "
                       "json_extract(value, '$.photo') AS photo, "
                       "json_extract(value, '$.width') AS width, "
                       "json_extract(value, '$.height') AS height, "
                       "json_extract(value, '$.file') AS file, "
                       "json_extract(value, '$.media_type') AS media_type, "
                       "json_extract(value, '$.mime_type') AS mime_type, "
                       "json_extract(value, '$.poll') AS poll FROM tmp_json_tree"))
    return;

  if (d_verbose) [[unlikely]]
    Logger::message("done! (", d_database.changed(), ")");

  d_database.exec("DROP TABLE tmp_json_tree");

  // std::cout << std::endl << "MESSAGES: " << std::endl;
  // d_database.prettyPrint("SELECT COUNT(*) FROM messages");
  // d_database.prettyPrint("SELECT * FROM messages");// WHERE chatidx = 42 LIMIT 10");
  // // d_database.saveToFile("NEW_METHOD");

  d_ok = true;
}
