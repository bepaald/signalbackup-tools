/*
  Copyright (C) 2025  Selwin van Dijk

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

bool SignalBackup::dtSetLinkPreviewData(long long int mms_id, long long int rowid, SqliteDB const &ddb) const
{
  SqliteDB::QueryResults linkpreview_results;
  if (!ddb.exec("SELECT "
                "json_extract(json, '$.preview[0].url') AS url,"
                "json_extract(json, '$.preview[0].title') AS title,"
                "json_extract(json, '$.preview[0].description') AS description "
                "FROM messages WHERE rowid = ?", rowid, &linkpreview_results))
    return false;

  // this works, but I want to escape the string like Signal does
  //d_database.exec("UPDATE " + d_mms_table + " SET d_mms_previews = json_array(json_object('url', ?, 'title', ?, 'description', ?, 'date', 0, 'attachmentId', json_object('rowId', ?, 'uniqueId', ?, 'valid', true))) "
  //"WHERE _id = ?", {linkpreview_results.value(0, "url"), linkpreview_results.value(0, "title"), linkpreview_results.value(0, "description"), new_part_id, unique_id, mms_id});

  /*
    STRING_ESCAPE():
    Quotation mark (") 	\"
    Reverse solidus (\) 	\|
    Solidus (/) 	\/
    Backspace 	\b
    Form feed 	\f
    New line 	\n
    Carriage return 	\r
    Horizontal tab 	\t

    As far as I can tell/test, only '/' '\' and '"' are escaped

    NOTE backslash needs to be done first, or the backslashes inserted by other escapes are escaped...
  */
  std::string url = linkpreview_results("url");
  bepaald::replaceAll(&url, '\\', R"(\\)");
  bepaald::replaceAll(&url, '/', R"(\/)");
  bepaald::replaceAll(&url, '\"', R"(\")");
  bepaald::replaceAll(&url, '\'', R"('')");
  bepaald::replaceAll(&url, '\n', R"(\n)");
  bepaald::replaceAll(&url, '\t', R"(\t)");
  bepaald::replaceAll(&url, '\b', R"(\b)");
  bepaald::replaceAll(&url, '\f', R"(\f)");
  bepaald::replaceAll(&url, '\r', R"(\r)");
  bepaald::replaceAll(&url, '\x0B', R"( )");
  bepaald::replaceAll(&url, '\v', R"(\v)");
  std::string title = linkpreview_results("title");
  bepaald::replaceAll(&title, '\\', R"(\\)");
  bepaald::replaceAll(&title, '/', R"(\/)");
  bepaald::replaceAll(&title, '\"', R"(\")");
  bepaald::replaceAll(&title, '\'', R"('')");
  bepaald::replaceAll(&title, '\n', R"(\n)");
  bepaald::replaceAll(&title, '\t', R"(\t)");
  bepaald::replaceAll(&title, '\b', R"(\b)");
  bepaald::replaceAll(&title, '\f', R"(\f)");
  bepaald::replaceAll(&title, '\r', R"(\r)");
  bepaald::replaceAll(&title, '\x0B', R"( )");
  bepaald::replaceAll(&title, '\v', R"(\v)");
  std::string description = linkpreview_results("description");
  bepaald::replaceAll(&description, '\\', R"(\\)");
  bepaald::replaceAll(&description, '/', R"(\/)");
  bepaald::replaceAll(&description, '\"', R"(\")");
  bepaald::replaceAll(&description, '\'', R"('')");
  bepaald::replaceAll(&description, '\n', R"(\n)");
  bepaald::replaceAll(&description, '\t', R"(\t)");
  bepaald::replaceAll(&description, '\b', R"(\b)");
  bepaald::replaceAll(&description, '\f', R"(\f)");
  bepaald::replaceAll(&description, '\r', R"(\r)");
  bepaald::replaceAll(&description, '\x0B', R"( )");
  bepaald::replaceAll(&description, '\v', R"(\v)");

  SqliteDB::QueryResults jsonstring;
  ddb.exec(bepaald::concat("SELECT json_array(json_object("
                           "'url', json('\"", url, "\"'), "
                           "'title', json('\"", title, "\"'), "
                           "'description', json('\"", description, "\"'), "
                           "'date', 0, "
                           "'attachmentId', NULL)) AS link_previews"), &jsonstring);
  std::string linkpreview_as_string = jsonstring("link_previews");

  bepaald::replaceAll(&linkpreview_as_string, '\'', R"('')");

  return d_database.exec(bepaald::concat("UPDATE ", d_mms_table, " SET ", d_mms_previews, " = '", linkpreview_as_string, "' WHERE _id = ?"), mms_id);
}

bool SignalBackup::dtUpdateLinkPreviewAttachment(long long int mms_id, long long int new_part_id, long long int unique_id) const
{
  return d_database.exec("UPDATE " + d_mms_table + " SET link_previews = "
                         "json_replace(link_previews, '$[0].attachmentId', json_object('rowId', ?, 'uniqueId', ?, 'valid', json('true'))) "
                         "WHERE _id = ?", {new_part_id, unique_id, mms_id});
}
