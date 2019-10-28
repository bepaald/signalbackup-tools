/*
    Copyright (C) 2019  Selwin van Dijk

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
  std::cout << std::endl << "WARNING: Bad MAC in frame, trying to print frame info:" << std::endl;
  (*frame)->printInfo();

  if ((*frame)->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
  {
    std::unique_ptr<AttachmentFrame> a = std::make_unique<AttachmentFrame>(*reinterpret_cast<AttachmentFrame *>(frame->get()));

    uint32_t rowid = a->rowId();
    uint64_t uniqueid = a->attachmentId();

    d_badattachments.emplace_back(std::make_pair(rowid, uniqueid));

    std::cout << "Frame is attachment, it belongs to entry in the 'part' table of the database:" << std::endl;
    //std::vector<std::vector<std::pair<std::string, std::any>>> results;
    SqliteDB::QueryResults results;
    std::string query = "SELECT * FROM part WHERE _id = " + bepaald::toString(rowid) + " AND unique_id = " + bepaald::toString(uniqueid);
    long long int mid = -1;
    d_database.exec(query, &results);
    for (uint i = 0; i < results.rows(); ++i)
    {
      for (uint j = 0; j < results.columns(); ++j)
      {
        std::cout << " - " << results.header(j) << " : ";
        if (results.valueHasType<std::nullptr_t>(i, j))
          std::cout << "(NULL)" << std::endl;
        else if (results.valueHasType<std::string>(i, j))
          std::cout << results.getValueAs<std::string>(i, j) << std::endl;
        else if (results.valueHasType<double>(i, j))
          std::cout << results.getValueAs<double>(i, j) << std::endl;
        else if (results.valueHasType<long long int>(i, j))
        {
          if (results.header(j) == "mid")
            mid = results.getValueAs<long long int>(i, j);
          std::cout << results.getValueAs<long long int>(i, j) << std::endl;
        }
        else if (results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
          std::cout << bepaald::bytesToHexString(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j)) << std::endl;
        else
          std::cout << "(unhandled result type)" << std::endl;
      }
    }

    std::cout << std::endl << "Which belongs to entry in 'mms' table:" << std::endl;
    query = "SELECT * FROM mms WHERE _id = " + bepaald::toString(mid);
    d_database.exec(query, &results);

    for (uint i = 0; i < results.rows(); ++i)
      for (uint j = 0; j < results.columns(); ++j)
      {
        std::cout << " - " << results.header(j) << " : ";
        if (results.valueHasType<std::nullptr_t>(i, j))
          std::cout << "(NULL)" << std::endl;
        else if (results.valueHasType<std::string>(i, j))
          std::cout << results.getValueAs<std::string>(i, j) << std::endl;
        else if (results.valueHasType<double>(i, j))
          std::cout << results.getValueAs<double>(i, j) << std::endl;
        else if (results.valueHasType<long long int>(i, j))
        {
          if (results.header(j) == "date" ||
              results.header(j) == "date_received")
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            std::cout << std::put_time(std::localtime(&epoch), "%F %T %z") << " (" << results.getValueAs<long long int>(i, j) << ")" << std::endl;
          }
          else
            std::cout << results.getValueAs<long long int>(i, j) << std::endl;
        }
        else if (results.valueHasType<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j))
          std::cout << bepaald::bytesToHexString(results.getValueAs<std::pair<std::shared_ptr<unsigned char []>, size_t>>(i, j)) << std::endl;
        else
          std::cout << "(unhandled result type)" << std::endl;
      }

    std::string afilename = "attachment_" + bepaald::toString(mid) + ".bin";
    std::cout << "Trying to dump decoded attachment to file '" << afilename << "'" << std::endl;

    std::ofstream bindump(afilename, std::ios::binary);
    bindump.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());

  }
  else
  {
    std::cout << "Error: Bad MAC in frame other than AttachmentFrame. Not sure what to do..." << std::endl;
    (*frame)->printInfo();
  }


  /*

  std::cout << std::endl << "WARNING: Bad MAC in frame, trying to print frame info:" << std::endl;
  if ((*frame)->frameType() != BackupFrame::FRAMETYPE::ATTACHMENT)
  {
    std::cout << "Error: Bad MAC in frame other than AttachmentFrame. Not sure what to do..." << std::endl;
    (*frame)->printInfo();
    return;
  }

  std::unique_ptr<AttachmentFrame> a = std::make_unique<AttachmentFrame>(*reinterpret_cast<AttachmentFrame *>(frame->get()));
  //std::unique_ptr<AttachmentFrame> a(reinterpret_cast<AttachmentFrame *>(frame.release()));

  uint32_t rowid = a->rowId();
  uint64_t uniqueid = a->attachmentId();

  d_badattachments.emplace_back(std::make_pair(rowid, uniqueid));

  {
    std::cout << "Short info (full info follows)" << std::endl;

    SqliteDB::QueryResults results;
    std::string query = "SELECT mid FROM part WHERE _id = " + bepaald::toString(rowid) + " AND unique_id = " + bepaald::toString(uniqueid);
    long long int mid = -1;
    d_database.exec(query, &results);
    if (results.header(0) == "mid" && results.valueHasType<long long int>(0, 0))
      mid = results.getValueAs<long long int>(0, 0);
    else
    {
      std::cout << "Failed to get info 1" << std::endl;
      return;
    }
    query = "SELECT * FROM mms WHERE _id = " + bepaald::toString(mid);
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
          if (results.header(j) == "msg_box")
            type = results.getValueAs<long long int>(i, j);
          if (results.header(j) == "date")
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            std::ostringstream tmp;
            tmp << std::put_time(std::localtime(&epoch), "%F %T %z") << " (" << results.getValueAs<long long int>(i, j) << ")";
            date = tmp.str();
          }
          if (results.header(j) == "date_received")
          {
            long long int datum = results.getValueAs<long long int>(i, j);
            std::time_t epoch = datum / 1000;
            std::ostringstream tmp;
            tmp << std::put_time(std::localtime(&epoch), "%F %T %z") << " (" << results.getValueAs<long long int>(i, j) << ")";
            date_received = tmp.str();
          }
        }
        else if (results.header(j) == "body" && results.valueHasType<std::string>(i, j))
          body = results.getValueAs<std::string>(i, j);
      }
    if (thread_id == -1 || date.empty() || date_received.empty() || type == -1)
    {
      std::cout << "Failed to get info 2" << std::endl;
      return;
    }

    std::string partner;
    query = "SELECT thread._id, COALESCE(recipient.system_display_name, recipient.signal_profile_name, groups.title) AS 'convpartner' FROM thread LEFT JOIN recipient ON thread.recipient_ids = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id";// WHERE thread._id = " + bepaald::toString(thread_id);
    d_database.exec(query, &results);
    results.prettyPrint();
    if (results.header(0) == "convpartner" && results.valueHasType<std::string>(0, 0))
      partner = results.getValueAs<std::string>(0, 0);

    std::cout << "Frame is attachment, corresponding message:" << std::endl;
    std::cout << "Date          :" << date << std::endl;
    std::cout << "Date received :" << date_received << std::endl;
    std::cout << "Sent " << (Types::isInboxType(type) ? "by       :" : "to       :") << partner << std::endl;
    std::cout << "Body          :" << body << std::endl;
  }


      */
}
