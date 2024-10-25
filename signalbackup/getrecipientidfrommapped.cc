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

long long int SignalBackup::getRecipientIdFromUuidMapped(std::string const &uuid, std::map<std::string, long long int> *savedmap,
                                                         bool suppresswarning) const
{
  if (uuid.empty())
  {
    Logger::error("Asked to find recipient._id for empty uuid. Refusing");
    return -1;
  }

  if (!savedmap || savedmap->find(uuid) == savedmap->end())
  {
    std::string printable_uuid(uuid);
    unsigned int offset = (STRING_STARTS_WITH(uuid, "__signal_group__v2__!") ? STRLEN("__signal_group__v2__!") + 4 :
                           (STRING_STARTS_WITH(uuid, "__textsecure_group__!") ? STRLEN("__textsecure_group__!") + 4 : 4));
    if (offset < uuid.size()) [[likely]]
      std::replace_if(printable_uuid.begin() + offset, printable_uuid.end(), [](char c){ return c != '-'; }, 'x');
    else
      printable_uuid = "xxx";

    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT recipient._id FROM recipient WHERE " + d_recipient_aci + " = ?1 COLLATE NOCASE OR group_id = ?1 COLLATE NOCASE", uuid, &res) ||
        res.rows() != 1 ||
        !res.valueHasType<long long int>(0, 0))
    {
      if (!suppresswarning) // we can suppress this warning, if we are creating the contact after not finding it...
        Logger::warning("Failed to find recipient for uuid: ", printable_uuid);
      return -1;
    }
    //res.prettyPrint();
    if (savedmap)
      (*savedmap)[uuid] = res.getValueAs<long long int>(0, 0);

    return res.getValueAs<long long int>(0, 0);
  }
  return (*savedmap)[uuid];
}

long long int SignalBackup::getRecipientIdFromPhoneMapped(std::string const &phone, std::map<std::string, long long int> *savedmap,
                                                          bool suppresswarning) const
{
  if (phone.empty())
  {
    Logger::error("Asked to find recipient._id for empty e164. Refusing");
    return -1;
  }

  if (!savedmap || savedmap->find(phone) == savedmap->end())
  {
    std::string printable_phone(phone);
    unsigned int offset = 3;
    if (offset < phone.size()) [[likely]]
      std::replace_if(printable_phone.begin() + offset, printable_phone.end(), [](char c){ return std::isdigit(c); }, 'x');
    else
      printable_phone = "xxx";

    SqliteDB::QueryResults res;
    if (!d_database.exec("SELECT recipient._id FROM recipient WHERE " + d_recipient_e164 + " = ? COLLATE NOCASE", phone, &res) ||
        res.rows() != 1 ||
        !res.valueHasType<long long int>(0, 0))
    {
      if (!suppresswarning)
        Logger::warning("Failed to find recipient for phone: ", printable_phone);
      return -1;
    }
    //res.prettyPrint();
    if (savedmap)
      (*savedmap)[phone] = res.getValueAs<long long int>(0, 0);

    return res.getValueAs<long long int>(0, 0);
  }
  return (*savedmap)[phone];
}
