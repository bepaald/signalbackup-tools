/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

std::vector<long long int> SignalBackup::getGroupUpdateRecipients() const
{
  SqliteDB::QueryResults res;
  // d_database.exec("SELECT body FROM sms where _id = 120", &res);
  // DecryptedGroupV2Context sts(res.valueAsString(0, "body"));
  // sts.print();

  std::set<std::string> uuids;

  using namespace std::string_literals;
  for (auto const &q : {"SELECT body FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0"s, "SELECT body FROM mms WHERE (" + d_mms_type + " & ?) != 0 AND (" + d_mms_type + " & ?) != 0"s})
  {
    //d_database.exec("SELECT body FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0",
    d_database.exec(q, {Types::GROUP_UPDATE_BIT, Types::GROUP_V2_BIT}, &res);

    for (uint i = 0; i < res.rows(); ++i)
    {
      DecryptedGroupV2Context sts2(res.valueAsString(i, "body"));
      //std::cout << "STATUS MSG " << i << std::endl;

      // NEW DATA
      auto field3 = sts2.getField<3>();
      if (field3.has_value())
      {
        auto field3_7 = field3->getField<7>();
        for (uint j = 0; j < field3_7.size(); ++j)
        {
          auto field3_7_1 = field3_7[j].getField<1>();
          if (field3_7_1.has_value())
            uuids.insert(bepaald::bytesToHexString(*field3_7_1, true));
          // else
          // {
          //   std::cout << "No members found in field 3" << std::endl;
          //   sts2.print();
          // }
        }
        // if (field3_7.size() == 0)
        // {
        //   std::cout << "No members found in field 3" << std::endl;
        //   sts2.print();
        // }
      }
      // else
      // {
      //   std::cout << "No members found in field 3" << std::endl;
      //   sts2.print();
      // }

      // OLD DATA?
      auto field4 = sts2.getField<4>();
      if (field4.has_value())
      {
        auto field4_7 = field4->getField<7>();
        for (uint j = 0; j < field4_7.size(); ++j)
        {
          auto field4_7_1 = field4_7[j].getField<1>();
          if (field4_7_1.has_value())
            uuids.insert(bepaald::bytesToHexString(*field4_7_1, true));
          // else
          // {
          //   std::cout << "No members found in field 4" << std::endl;
          //   sts2.print();
          // }
        }
        // if (field4_7.size() == 0)
        // {
        //   std::cout << "No members found in field 4" << std::endl;
        //   sts2.print();
        // }
      }
      // else
      // {
      //   std::cout << "No members found in field 4" << std::endl;
      //   sts2.print();
      // }
    }
  }

  // std::cout << "LIST OF FOUND UUIDS:" << std::endl;
  // for (auto &uuid : uuids)
  //   std::cout << uuid << std::endl;

  std::vector<long long int> ids;

  if (uuids.size())
  {
    std::string q = "SELECT DISTINCT _id FROM recipient WHERE LOWER(uuid) IN (";
#if __cplusplus > 201703L
    for (int pos = 0; std::string uuid : uuids)
#else
    int pos = 0;
    for (std::string uuid : uuids)
#endif
    {
      if (pos > 0)
        q += ", ";

      uuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      q += "LOWER('" + uuid + "')";
      ++pos;
    }
    q += ")";

    d_database.exec(q, &res);

    for (uint i = 0; i < res.rows(); ++i)
      ids.push_back(res.getValueAs<long long int>(i, "_id"));

  }
  return ids;
}
