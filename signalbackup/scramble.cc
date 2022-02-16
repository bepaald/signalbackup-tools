/*
    Copyright (C) 2022  Selwin van Dijk

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

bool SignalBackup::scrambleHelper(std::string const &table, std::vector<std::string> const &columns) const
{
  std::cout << "Scrambling " << table << std::endl;

  std::string selectquery = "SELECT _id";
  for (uint i = 0; i < columns.size(); ++i)
    selectquery += "," + columns[i];
  selectquery += " FROM " + table;

  SqliteDB::QueryResults res;
  d_database.exec(selectquery, &res);

  for (uint i = 0; i < res.rows(); ++i)
  {
    std::vector<std::string> str;
    for (uint j = 0; j < columns.size(); ++j)
    {
      str.push_back(res.valueAsString(i, columns[j]));
      std::replace_if(str.back().begin(), str.back().end(), [](char c)
      {
        return c != ' ' && std::islower(c);
      }, 'x');
      std::replace_if(str.back().begin(), str.back().end(), [](char c)
      {
        return c != ' ' && !std::islower(c);
      }, 'X');
    }

    std::string updatequery = "UPDATE " + table + " SET ";
    for (uint j = 0; j < columns.size(); ++j)
      updatequery += columns[j] + " = ?" + ((j == columns.size() - 1) ? " WHERE _id = ?" : ", ");

    std::vector<std::any> values;
    for (uint j = 0; j < str.size(); ++j)
      values.push_back(str[j]);
    values.push_back(res.getValueAs<long long int>(i, "_id"));

    if (!d_database.exec(updatequery, values))
      return false;
  }
  return true;
}

bool SignalBackup::scramble() const
{
  if (!scrambleHelper("sms", {"body"}))
    return false;

  if (!scrambleHelper("mms", {"body", "quote_body"}))
    return false;

  if (!scrambleHelper("recipient", {"system_display_name", "profile_joined_name", "signal_profile_name", "profile_family_name", "system_family_name", "system_given_name"}))
    return false;

  if (!scrambleHelper("thread", {"snippet"}))
    return false;

  if (!scrambleHelper("groups", {"title"}))
    return false;

  /*
  SqliteDB::QueryResults res;
  d_database.exec("SELECT _id,body FROM sms", &res);
  for (uint i = 0; i < res.rows(); ++i)
  {
    std::string str = res.valueAsString(i, "body");
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');
    if (!d_database.exec("UPDATE sms SET body = ? WHERE _id = ?", {str, res.getValueAs<long long int>(i, "_id")}))
      return false;
  }

  d_database.exec("SELECT _id,body FROM mms", &res);
  for (uint i = 0; i < res.rows(); ++i)
  {
    std::string str = res.valueAsString(i, "body");
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');
    if (!d_database.exec("UPDATE mms SET body = ? WHERE _id = ?", {str, res.getValueAs<long long int>(i, "_id")}))
      return false;
  }

  d_database.exec("SELECT _id,system_display_name,profile_joined_name,signal_profile_name FROM recipient", &res);
  for (uint i = 0; i < res.rows(); ++i)
  {
    std::string str1 = res.valueAsString(i, "system_display_name");
    std::replace_if(str1.begin(), str1.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str1.begin(), str1.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');

    std::string str2 = res.valueAsString(i, "profile_joined_name");
    std::replace_if(str2.begin(), str2.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str2.begin(), str2.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');

    std::string str3 = res.valueAsString(i, "signal_profile_name");
    std::replace_if(str3.begin(), str3.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str3.begin(), str3.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');
    if (!d_database.exec("UPDATE recipient SET system_display_name = ?, profile_joined_name = ?, signal_profile_name = ? WHERE _id = ?", {str1, str2, str3, res.getValueAs<long long int>(i, "_id")}))
      return false;
  }

  d_database.exec("SELECT _id,title FROM groups", &res);
  for (uint i = 0; i < res.rows(); ++i)
  {
    std::string str = res.valueAsString(i, "title");
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && std::islower(c);
    }, 'x');
    std::replace_if(str.begin(), str.end(), [](char c)
    {
      return c != ' ' && !std::islower(c);
    }, 'X');
    if (!d_database.exec("UPDATE groups SET title = ? WHERE _id = ?", {str, res.getValueAs<long long int>(i, "_id")}))
      return false;
  }
  */
  return true;
}
