/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SignalBackup::dumpInfoOnBadFrame(std::unique_ptr<BackupFrame> *frame)
{
  Logger::warning("Bad MAC in frame, trying to print frame info:");
  (*frame)->printInfo();

  if ((*frame)->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
  {
    std::unique_ptr<AttachmentFrame> a = std::make_unique<AttachmentFrame>(*reinterpret_cast<AttachmentFrame *>(frame->get()));

    uint32_t rowid = a->rowId();
    int64_t uniqueid = a->attachmentId();

    d_badattachments.emplace_back(std::make_pair(rowid, uniqueid));

    Logger::message("Frame is attachment, it belongs to entry in the 'part' table of the database:");
    //std::vector<std::vector<std::pair<std::string, std::any>>> results;
    SqliteDB::QueryResults results;
    std::string query = "SELECT * FROM " + d_part_table
      + " WHERE _id = " + bepaald::toString(rowid);
    if (d_database.tableContainsColumn(d_part_table, "unique_id"))
      query += " AND unique_id = " + bepaald::toString(uniqueid);

    long long int mid = -1;
    d_database.exec(query, &results);
    for (uint i = 0; i < results.rows(); ++i)
    {
      for (uint j = 0; j < results.columns(); ++j)
      {
        Logger::message_start(" - ", results.header(j), " : ");
        if (results.isNull(i, j))
          Logger::message("(NULL)");
        else if (results.valueHasType<std::string>(i, j))
          Logger::message(results.getValueAs<std::string>(i, j));
        else if (results.valueHasType<double>(i, j))
          Logger::message(results.getValueAs<double>(i, j));
        else if (results.valueHasType<long long int>(i, j))
        {
          if (results.header(j) == d_part_mid)
            mid = results.getValueAs<long long int>(i, j);
          Logger::message(results.getValueAs<long long int>(i, j));
        }
        else if (results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
          Logger::message(bepaald::bytesToHexString(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j)));
        else
          Logger::message("(unhandled result type)");
      }
    }

    Logger::message("Which belongs to entry in '" + d_mms_table + "' table:");
    query = "SELECT * FROM " + d_mms_table + " WHERE _id = " + bepaald::toString(mid);
    d_database.exec(query, &results);

    for (uint i = 0; i < results.rows(); ++i)
      for (uint j = 0; j < results.columns(); ++j)
      {
        Logger::message_start(" - ", results.header(j), " : ");
        if (results.isNull(i, j))
          Logger::message("(NULL)");
        else if (results.valueHasType<std::string>(i, j))
          Logger::message(results.getValueAs<std::string>(i, j));
        else if (results.valueHasType<double>(i, j))
          Logger::message(results.getValueAs<double>(i, j));
        else if (results.valueHasType<long long int>(i, j))
        {
          if (results.header(j) == d_mms_date_sent ||
              results.header(j) == "date_received")
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            //std::cout << std::put_time(std::localtime(&epoch), "%F %T %z") << " (" << results.getValueAs<long long int>(i, j) << ")"); // %F and %T do not work with mingw
            Logger::message(std::put_time(std::localtime(&epoch), "%Y-%m-%d %H:%M:%S %z"), " (", results.getValueAs<long long int>(i, j), ")");
          }
          else
            Logger::message(results.getValueAs<long long int>(i, j));
        }
        else if (results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
          Logger::message(bepaald::bytesToHexString(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j)));
        else
          Logger::message("(unhandled result type)");
      }

    std::string afilename = "attachment_" + bepaald::toString(mid) + ".bin";
    Logger::message("Trying to dump decoded attachment to file '", afilename, "'");

    std::ofstream bindump(afilename, std::ios_base::binary);
    bindump.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());

  }
  else
  {
    Logger::error("Bad MAC in frame other than AttachmentFrame. Just dropping the frame, this could cause more problems...");
    (*frame)->printInfo();
  }
}


void SignalBackup::dumpInfoOnBadFrames() const
{
  for (uint a = 0; a < d_badattachments.size(); ++a)
  {
    uint32_t rowid = d_badattachments[a].first;
    int64_t uniqueid = d_badattachments[a].second;

    Logger::message("Short info on message to which attachment with bad mac belongs (", a + 1, "/", d_badattachments.size(), "):");

    SqliteDB::QueryResults results;
    std::string query = "SELECT " + d_part_mid + " FROM " + d_part_table
      + " WHERE _id = " + bepaald::toString(rowid);
    if (d_database.tableContainsColumn(d_part_table, "unique_id"))
      query += " AND unique_id = " + bepaald::toString(uniqueid);

    long long int mid = -1;
    d_database.exec(query, &results);
    if (results.header(0) == d_part_mid && results.valueHasType<long long int>(0, 0))
      mid = results.getValueAs<long long int>(0, 0);
    else
    {
      Logger::error("Failed to get info 1");
      return;
    }
    query = "SELECT * FROM " + d_mms_table + " WHERE _id = " + bepaald::toString(mid);
    d_database.exec(query, &results);
    long long int thread_id = -1;
    std::string body;
    std::string date;
    std::string date_received;
    long long int type = -1;
    for (uint i = 0; i < results.rows(); ++i)
      for (uint j = 0; j < results.columns(); ++j)
      {
        if (results.valueHasType<long long int>(i, j))
        {
          if (results.header(j) == "thread_id")
            thread_id = results.getValueAs<long long int>(i, j);
          if (results.header(j) == d_mms_type)
            type = results.getValueAs<long long int>(i, j);
          if (results.header(j) == d_mms_date_sent)
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            std::ostringstream tmp;
            //tmp << std::put_time(std::localtime(&epoch), "%F T %z") << " (" << results.getValueAs<long long int>(i, j) << ")"; // %F and %T do not work on mingw
            tmp << std::put_time(std::localtime(&epoch), "%Y-%m-%d %H:%M:%S %z") << " (" << results.getValueAs<long long int>(i, j) << ")";
            date = tmp.str();
          }
          if (results.header(j) == "date_received")
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            std::ostringstream tmp;
            //tmp << std::put_time(std::localtime(&epoch), "%F T %z") << " (" << results.getValueAs<long long int>(i, j) << ")"; // %F and %T do not work on mingw
            tmp << std::put_time(std::localtime(&epoch), "%Y-%m-%d %H:%M:%S %z") << " (" << results.getValueAs<long long int>(i, j) << ")";
            date_received = tmp.str();
          }
        }
        else if (results.header(j) == "body" && results.valueHasType<std::string>(i, j))
          body = results.getValueAs<std::string>(i, j);
      }
    if (thread_id == -1 || date.empty() || date_received.empty() || type == -1)
    {
      Logger::error("Failed to get info 2");
      return;
    }

    std::string partner;
    if (d_databaseversion < 24) // OLD VERSION
      query = "SELECT COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name, groups.title) AS 'convpartner' FROM thread LEFT JOIN recipient_preferences ON thread." + d_thread_recipient_id + " = recipient_preferences.recipient_ids LEFT JOIN groups ON thread." + d_thread_recipient_id + " = groups.group_id WHERE thread._id = " + bepaald::toString(thread_id);
    else
      query = "SELECT COALESCE(recipient." + d_recipient_system_joined_name + ", recipient." + d_recipient_profile_given_name + ", groups.title) AS 'convpartner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id WHERE thread._id = " + bepaald::toString(thread_id);
    d_database.exec(query, &results);
    if (results.header(0) == "convpartner" && results.valueHasType<std::string>(0, 0))
      partner = results.getValueAs<std::string>(0, 0);

    Logger::message("Date sent     : ", date);
    Logger::message("Date received : ", date_received);
    Logger::message("Sent ", (Types::isInboxType(type) ? "by       : " : "to       : "), partner);
    Logger::message("Message body  : ", body);
  }
}
