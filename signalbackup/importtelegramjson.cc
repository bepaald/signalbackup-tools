/*
  Copyright (C) 2023  Selwin van Dijk

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

bool SignalBackup::importTelegramJson(std::string const &file, std::vector<std::pair<std::string, long long int>> contactmap) const
{
  std::cout << "Import from Telegram json export" << std::endl;

  std::cout << "CONTACT MAP: " << std::endl;
  for (uint i = 0; i < contactmap.size(); ++i)
    std::cout << contactmap[i].first << " -> " << contactmap[i].second << std::endl;

  std::ifstream sourcefile(file, std::ios_base::binary | std::ios_base::in);
  if (!sourcefile.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to open file for reading: " << file << std::endl;
    return false;
  }

  // read data from file
  sourcefile.seekg(0, std::ios_base::end);
  long long int datasize = sourcefile.tellg();
  sourcefile.seekg(0, std::ios_base::beg);
  unsigned char *data = new unsigned char[datasize];

  if (!sourcefile.read(reinterpret_cast<char *>(data), datasize))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to read json data" << std::endl;
    return false;
  }

  // create table
  SqliteDB telegram_db(":memory:");
  if (!telegram_db.exec("CREATE TABLE chats(idx INT, name TEXT, type TEXT)") ||
      !telegram_db.exec("CREATE TABLE messages(chatidx INT, type TEXT, date INT, from_name TEXT, body TEXT, "
                        "photo TEXT, width INT, height INT, "
                        "file TEXT, media_type TEXT, mime_type TEXT)"))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to set up sql table" << std::endl;
    return false;
  }

  if (!telegram_db.exec("INSERT INTO chats SELECT key,json_extract(value, '$.name') AS name, json_extract(value, '$.type') AS type FROM json_each(?, '$.chats.list')",
                        std::string(reinterpret_cast<char *>(data), datasize)))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to fill sql table" << std::endl;
    return false;
  }

  if (!telegram_db.exec("INSERT INTO messages "
                        "SELECT "
                        "tree2.key, "

                        "json_extract(each.value, '$.type'), "
                        "json_extract(each.value, '$.date_unixtime'), "
                        "json_extract(each.value, '$.from'), "
                        "json_extract(each.value, '$.text_entities'), "

                        "json_extract(each.value, '$.photo'), "
                        "json_extract(each.value, '$.width'), "
                        "json_extract(each.value, '$.height'), "

                        "json_extract(each.value, '$.file'), "
                        "json_extract(each.value, '$.media_type'), " // could be 'voice_message
                        "json_extract(each.value, '$.mime_type') "

                        "FROM json_tree(?, '$.chats.list') AS tree, json_each(tree.value) AS each "
                        "LEFT JOIN json_tree(?, '$.chats.list') AS tree2 ON tree2.fullkey || '.messages' IS tree.fullkey "
                        //"LEFT JOIN json_tree(?, '$.chats.list') AS tree2 ON /*tree2.path IS '$.chats.list' AND */tree2.fullkey || '.messages' IS tree.fullkey "
                        "WHERE tree.key = 'messages'",
                        {std::string(reinterpret_cast<char *>(data), datasize), std::string(reinterpret_cast<char *>(data), datasize)}))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to fill sql table" << std::endl;
    return false;
  }

  bepaald::destroyPtr(&data, &datasize);

  std::cout << std::endl << "CHATS: " << std::endl;
  telegram_db.prettyPrint("SELECT * FROM chats");
  std::cout << std::endl << "MESSAGES: " << std::endl;
  telegram_db.prettyPrint("SELECT * FROM messages");

  // get all contacts in json data
  SqliteDB::QueryResults json_contacts;
  if (!telegram_db.exec("SELECT DISTINCT name FROM chats WHERE name IS NOT NULL UNION SELECT DISTINCT from_name AS name FROM messages WHERE from_name IS NOT NULL", &json_contacts))
    return false;

  std::cout << std::endl << "ALL CONTACTS: " << std::endl;
  json_contacts.prettyPrint();

  std::vector<long long int> recipientsnotfound;

  for (uint i = 0; i < json_contacts.rows(); ++i)
  {
    std::string contact = json_contacts.valueAsString(i, "name");

    if (std::find_if(contactmap.begin(), contactmap.end(),
                     [contact](auto const &it){ return it.first == contact; }) != contactmap.end()) // we have a matching recipient (possibly user-supplied)
      continue;

    // find it in android db;
    long long int rec_id = getRecipientIdFromName(contact, false);

    if (rec_id == -1)
      recipientsnotfound.push_back(i);
    else
      contactmap.push_back({contact, rec_id});
  }

  if (!recipientsnotfound.empty())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": The following contacts in the JSON input were not found in the Android backup:" << std::endl;
    for (auto const r : recipientsnotfound)
      std::cout << " - \"" << json_contacts.valueAsString(r, "name") << "\"" << std::endl;

    std::cout << "Use `--mapjsonrecipients [NAME1]=[id1],[NAME2]=[id2],...' to map these to an existing recipient id " << std::endl
              << "from the backup. The list of available recipients and their id's can be obtained by running " << std::endl
              << "with `--listrecipients'." << std::endl;

    return false;
  }

  std::cout << "CONTACT MAP: " << std::endl;
  for (uint i = 0; i < contactmap.size(); ++i)
    std::cout << contactmap[i].first << " -> " << contactmap[i].second << std::endl;

  return false;
}
