/*
  Copyright (C) 2022-2025  Selwin van Dijk

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

#include "../groupv2statusmessageproto_typedef/groupv2statusmessageproto_typedef.h"
#include "../protobufparser/protobufparser.h"

std::vector<long long int> SignalBackup::getGroupUpdateRecipients(int thread) const
{
  SqliteDB::QueryResults res;

  std::set<std::string> uuids;

  std::vector<std::string> queries{"SELECT "s +
                                   (d_database.tableContainsColumn(d_mms_table, "message_extras") ? "COALESCE(message_extras, body) AS groupctx" : "body AS groupctx") +
                                   " FROM " + d_mms_table + " WHERE (" + d_mms_type + " & ?) != 0 AND (" + d_mms_type + " & ?) != 0"s +
                                   (thread != -1 ? " AND thread_id = " + bepaald::toString(thread) : "")};
  if (d_database.containsTable("sms"))
    queries.emplace_back("SELECT body AS groupctx FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0"s +
                         (thread != -1 ? " AND thread_id = " + bepaald::toString(thread) : ""));
  for (auto const &q : queries)
  {
    //d_database.exec("SELECT body FROM sms WHERE (type & ?) != 0 AND (type & ?) != 0",
    d_database.exec(q, {Types::GROUP_UPDATE_BIT, Types::GROUP_V2_BIT}, &res);

    for (unsigned int i = 0; i < res.rows(); ++i)
    {
      // std::cout << "GROUPCTX: " << "\"" << res.valueAsString(i, "groupctx") << "\"" << std::endl;
      if (res.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "groupctx"))
      {
        //std::cout << "FROM BLOB 1" << std::endl;
        MessageExtras me(res.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, "groupctx"));
        //me.print();
        //std::cout << "---" << std::endl;
        auto field1 = me.getField<1>();
        if (field1.has_value())
        {
          auto field1_1 = field1->getField<1>();
          if (field1_1.has_value())
            getGroupUpdateRecipientsFromGV2Context(*field1_1, &uuids);
        }
        // std::cout << "FROM BLOB 2" << std::endl;
        // sts2.print();
      }
      else // valueHasType<std::string>
      {
        getGroupUpdateRecipientsFromGV2Context(DecryptedGroupV2Context{res.valueAsString(i, "groupctx")}, &uuids);
        //std::cout << "FROM BASE64STRING" << std::endl;
        //sts2.print();
      }
    }
  }

  // std::cout << "LIST OF FOUND UUIDS:" << std::endl;
  // for (auto &uuid : uuids)
  //   std::cout << uuid << " (" << uuid.length() << ")" << std::endl;

  std::vector<long long int> ids;

  if (uuids.size())
  {
    std::string q("SELECT DISTINCT _id FROM recipient WHERE LOWER(" + d_recipient_aci + ") IN (");
#if __cplusplus > 201703L
    for (int pos = 0; std::string uuid : uuids)
#else
    int pos = 0;
    for (std::string uuid : uuids)
#endif
    {
      if (uuid.length() < 32) [[unlikely]]
        continue;

      if (pos > 0)
        q += ", ";

      uuid.insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
      q += "LOWER('" + uuid + "')";
      ++pos;
    }
    q += ")";

    d_database.exec(q, &res);

    for (unsigned int i = 0; i < res.rows(); ++i)
      ids.push_back(res.getValueAs<long long int>(i, "_id"));

  }
  return ids;
}

void SignalBackup::getGroupUpdateRecipientsFromGV2Context(DecryptedGroupV2Context const &sts2, std::set<std::string> *uuids) const
{
  // NEW DATA
  auto field3 = sts2.getField<3>();
  if (field3.has_value())
  {
    auto field3_7 = field3->getField<7>();
    for (unsigned int j = 0; j < field3_7.size(); ++j)
    {
      auto field3_7_1 = field3_7[j].getField<1>();
      if (field3_7_1.has_value())
        uuids->insert(bepaald::bytesToHexString(*field3_7_1, true));
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
    for (unsigned int j = 0; j < field4_7.size(); ++j)
    {
      auto field4_7_1 = field4_7[j].getField<1>();
      if (field4_7_1.has_value())
        uuids->insert(bepaald::bytesToHexString(*field4_7_1, true));
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
