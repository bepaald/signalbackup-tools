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
#include "msgrange.h"

bool SignalBackup::exportTxt(std::string const &directory, std::vector<long long int> const &limittothreads,
                             std::vector<std::string> const &daterangelist, std::string const &selfphone [[maybe_unused]],
                             bool overwrite) const
{
  // check if dir exists, create if not
  if (!bepaald::fileOrDirExists(directory))
  {
    // try to create
    if (!bepaald::createDir(directory))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed to create directory `" << directory << "'"
                << " (errno: " << std::strerror(errno) << ")" << std::endl; // note: errno is not required to be set by std
      // temporary !!
      {
        std::error_code ec;
        std::filesystem::space_info const si = std::filesystem::space(directory, ec);
        if (!ec)
        {
          std::cout << "Available  : " << static_cast<std::intmax_t>(si.available) << std::endl;
          std::cout << "Backup size: " << d_fd->total() << std::endl;
        }
      }
      return false;
    }
  }

  // directory exists, but
  // is it a dir?
  if (!bepaald::isDir(directory))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": `" << directory << "' is not a directory." << std::endl;
    return false;
  }

  // and is it empty?
  if (!bepaald::isEmpty(directory))
  {
    if (!overwrite)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Directory '" << directory << "' is not empty. Use --overwrite to clear directory before export." << std::endl;
      return false;
    }
    std::cout << "Clearing contents of directory '" << directory << "'..." << std::endl;
    if (!bepaald::clearDirectory(directory))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed to empty directory '" << directory << "'" << std::endl;
      return false;
    }
  }

  // // get note-to-self-thread
  // long long int note_to_self_thread_id = -1;
  // if (selfphone.empty())
  // {
  //   long long int selfid = scanSelf();
  //   if (selfid != -1)
  //     note_to_self_thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " = ?", selfid, -1);
  //   else
  //     std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
  //               << ": Failed to determine Note-to-self thread. Consider passing `--setselfid \"[phone]\"' to set it manually" << std::endl;
  // }
  // else
  //   note_to_self_thread_id = d_database.getSingleResultAs<long long int>("SELECT _id FROM thread WHERE " + d_thread_recipient_id + " IS "
  //                                                                        "(SELECT _id FROM recipient WHERE phone = ?)", selfphone, -1);


  std::vector<long long int> threads = ((limittothreads.empty() || (limittothreads.size() == 1 && limittothreads[0] == -1)) ?
                                        threadIds() : limittothreads);

  std::map<long long int, RecipientInfo> recipient_info;

  // set where-clause for date requested
  std::vector<std::pair<std::string, std::string>> dateranges;
  if (daterangelist.size() % 2 == 0)
    for (uint i = 0; i < daterangelist.size(); i += 2)
      dateranges.push_back({daterangelist[i], daterangelist[i + 1]});
  std::string datewhereclause;
  for (uint i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      std::cout << "Error: Skipping range: '" << dateranges[i].first << " - " << dateranges[i].second << "'. Failed to parse or invalid range." << std::endl;
      std::cout << startrange << " " << endrange << std::endl;
      continue;
    }
    std::cout << "  Using range: " << dateranges[i].first << " - " << dateranges[i].second
              << " (" << startrange << " - " << endrange << ")" << std::endl;

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    dateranges[i].first = bepaald::toString(startrange);
    dateranges[i].second = bepaald::toString(endrange);

    datewhereclause += (datewhereclause.empty() ? " AND (" : " OR ") + "date_received BETWEEN "s + dateranges[i].first + " AND " + dateranges[i].second;
    if (i == dateranges.size() - 1)
      datewhereclause += ')';
  }
  std::sort(dateranges.begin(), dateranges.end());


  // handle each thread
  for (int t : threads)
  {

    std::cout << "Dealing with thread " << t << std::endl;

    //bool is_note_to_self = false;//(t == note_to_self_thread_id);

    // get recipient_id for thread;
    SqliteDB::QueryResults recid;
    long long int thread_recipient_id = -1;
    if (!d_database.exec("SELECT _id," + d_thread_recipient_id + " FROM thread WHERE _id = ?", t, &recid) ||
        recid.rows() != 1 || (thread_recipient_id = recid.valueAsInt(0, d_thread_recipient_id)) == -1)
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed to find recipient_id for thread (" << t << ")... skipping" << std::endl;
      continue;
    }
    long long int thread_id = recid.getValueAs<long long int>(0, "_id");

    // bool isgroup = false;
    // SqliteDB::QueryResults groupcheck;
    // d_database.exec("SELECT group_id FROM recipient WHERE _id = ? AND group_id IS NOT NULL", thread_recipient_id, &groupcheck);
    // if (groupcheck.rows())
    //   isgroup = true;


    // now get all messages
    SqliteDB::QueryResults messages;
    d_database.exec("SELECT "s
                    "_id, " + d_mms_recipient_id + ", body, "
                    "date_received, quote_id, quote_author, quote_body, quote_mentions, " + d_mms_type + ", "
                    "delivery_receipt_count, read_receipt_count, IFNULL(remote_deleted, 0) AS remote_deleted, "
                    "IFNULL(view_once, 0) AS view_once, expires_in, message_ranges, "
                    + (d_database.tableContainsColumn(d_mms_table, "original_message_id") ? "original_message_id, " : "") +
                    + (d_database.tableContainsColumn(d_mms_table, "revision_number") ? "revision_number, " : "") +
                    "json_extract(link_previews, '$[0].title') AS link_preview_title, "
                    "json_extract(link_previews, '$[0].description') AS link_preview_description "
                    "FROM " + d_mms_table + " "
                    "WHERE thread_id = ?"
                    + datewhereclause +
                    + (d_database.tableContainsColumn(d_mms_table, "latest_revision_id") ? " AND latest_revision_id IS NULL" : "") +
                    " ORDER BY date_received ASC", t, &messages);
    if (messages.rows() == 0)
      continue;

    // get all recipients in thread (group member (past and present), quote/reaction authors, mentions)
    std::set<long long int> all_recipients_ids = getAllThreadRecipients(t);

    //try to set any missing info on recipients
    setRecipientInfo(all_recipients_ids, &recipient_info);

    // get conversation name, sanitize it and set outputfilename
    if (recipient_info.find(thread_recipient_id) == recipient_info.end())
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Failed set recipient info for thread (" << t << ")... skipping" << std::endl;
      continue;
    }

    std::string filename = /*(is_note_to_self ? "Note to self (_id"s + bepaald::toString(thread_id) + ")"
                             : */sanitizeFilename(recipient_info[thread_recipient_id].display_name + " (_id" + bepaald::toString(thread_id) + ").txt")/*)*/;

    if (bepaald::fileOrDirExists(directory + "/" + filename))
    {
      std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
                << ": Refusing to overwrite existing file" << std::endl;
      return false;
    }

    std::ofstream txtoutput(directory + "/" + filename);
    if (!txtoutput.is_open())
    {
      std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
                << ": Failed to open '" << directory << "/" << filename << " for writing." << std::endl;
      return false;
    }

    for (uint i = 0; i < messages.rows(); ++i)
    {
      bool is_deleted = messages.getValueAs<long long int>(i, "remote_deleted") == 1;
      bool is_viewonce = messages.getValueAs<long long int>(i, "view_once") == 1;
      if (is_deleted || is_viewonce)
        continue;

      long long int msg_id = messages.getValueAs<long long int>(i, "_id");
      //bool incoming = !Types::isOutgoing(messages.getValueAs<long long int>(i, d_mms_type));
      long long int msg_recipient_id = messages.valueAsInt(i, d_mms_recipient_id);
      if (msg_recipient_id == -1) [[unlikely]]
      {
        std::cout << bepaald::bold_on << "Warning" << bepaald::bold_off
                  << ": Failed to get message recipient id. Skipping." << std::endl;
        continue;
      }
      std::string body = messages.valueAsString(i, "body");
      long long int type = messages.getValueAs<long long int>(i, d_mms_type);
      std::string readable_date = bepaald::toDateString(messages.getValueAs<long long int>(i, "date_received") / 1000,
                                                          "%b %d, %Y %H:%M:%S");
      SqliteDB::QueryResults attachment_results;
      d_database.exec("SELECT _id,unique_id,ct,file_name,pending_push,sticker_pack_id FROM part WHERE mid IS ? AND quote IS 0", msg_id, &attachment_results);

      SqliteDB::QueryResults mention_results;
      if (d_database.containsTable("mention"))
        d_database.exec("SELECT recipient_id, range_start, range_length FROM mention WHERE message_id IS ?", msg_id, &mention_results);

      if (Types::isStatusMessage(type) || Types::isCallType(type))
      {
        std::string statusmsg = decodeStatusMessage(body, messages.getValueAs<long long int>(i, "expires_in"), type, getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name);
        txtoutput << "[" << readable_date << "] " << "***" << " " << statusmsg <<  std::endl;
      }
      else
      {
        // get originating username
        std::string user = getRecipientInfoFromMap(&recipient_info, msg_recipient_id).display_name;

        for (uint a = 0; a < attachment_results.rows(); ++a)
        {
          std::string content_type = attachment_results.valueAsString(a, "ct");
          std::string attachment_filename;
          if (!attachment_results.isNull(a, "file_name") && !attachment_results(a, "file_name").empty())
            attachment_filename = attachment_results(a, "file_name");
          else if (!content_type.empty())
            attachment_filename = "of type " + content_type;

          txtoutput << "[" << readable_date << "] *** <" << user << "> sent file"
                    << (attachment_filename.empty() ? "" : " " + attachment_filename) << std::endl;
        }
        if (!body.empty())
        {
          // prep body for mentions...
          std::vector<Range> ranges;
          for (uint m = 0; m < mention_results.rows(); ++m)
          {
            std::string displayname = getNameFromRecipientId(mention_results.getValueAs<long long int>(m, "recipient_id"));
            if (displayname.empty())
              continue;
            ranges.emplace_back(Range{mention_results.getValueAs<long long int>(m, "range_start"),
                                      mention_results.getValueAs<long long int>(m, "range_length"),
                                      "",
                                      "@" + displayname,
                                      "",
                                      false});
          }
          applyRanges(&body, &ranges, nullptr);

          txtoutput << "[" << readable_date << "] <" << user << "> " << body << std::endl;
        }
      }
    }
  }

  return false;
}
