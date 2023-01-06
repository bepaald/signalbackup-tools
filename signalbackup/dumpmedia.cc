/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

bool SignalBackup::dumpMedia(std::string const &dir, std::vector<int> const &threads, bool overwrite) const
{
  std::cout << "Dumping media to dir '" << dir << "'" << std::endl;

  if (!d_database.containsTable("part") || !d_database.tableContainsColumn("part", "display_order"))
  {
    std::cout << "Database too badly damaged or too old, dumping media is not (yet) supported, consider a full decrypt by just passing a directory as output" << std::endl;
    return false;
  }

  if (!bepaald::isDir(dir))
  {
    std::cout << "Output directory '" << dir << "' does not exist or is not a directory" << std::endl;
    return false;
  }

  if (!bepaald::isEmpty(dir))
  {
    if (!overwrite)
    {
      std::cout << "Directory '" << dir << "' is not empty. Use --overwrite to clear directory before dump" << std::endl;
      return false;
    }
    std::cout << "Clearing contents of directory '" << dir << "'..." << std::endl;
    if (!bepaald::clearDirectory(dir))
    {
      std::cout << "Failed to empty directory '" << dir << "'" << std::endl;
      return false;
    }
  }

  MimeTypes mimetypes;
  std::pair<std::vector<int>, std::vector<std::string>> conversations; // links thread_id to thread title, if the
                                                                       // folder already exists, but from another _id,
                                                                       // it is a different thread with the same name

#if __cplusplus > 201703L
  for (int count = 0; auto const &aframe : d_attachments)
#else
    int count = 0;
  for (auto const &aframe : d_attachments)
#endif
  {

    ++count;
    std::cout << "\33[2K\rSaving attachments...  " << count << "/" << d_attachments.size() << std::flush;

    AttachmentFrame *a = aframe.second.get();

    //std::cout << "Looking for attachment: " << std::endl;
    //std::cout << "rid: " << a->rowId() << std::endl;
    //std::cout << "uid: " << a->attachmentId() << std::endl;

    SqliteDB::QueryResults results;

    // minimal query, for incomplete database
    bool fullbackup = false;
    std::string query = "SELECT part.mid, part.ct, part.file_name, part.display_order FROM part WHERE part._id == ? AND part.unique_id == ?";
    // if all tables for detailed info are present...
    if (d_database.containsTable("mms") && d_database.containsTable("thread") &&
        d_database.containsTable("groups") && d_database.containsTable("recipient"))
    {
      fullbackup = true;
      query = "SELECT part.mid, part.ct, part.file_name, part.display_order, mms.date_received, mms." + d_mms_type + ", mms.thread_id, thread." + d_thread_recipient_id +
        ", COALESCE(groups.title,recipient.system_display_name, recipient.profile_joined_name, recipient.signal_profile_name) AS 'chatpartner' FROM part "
        "LEFT JOIN mms ON part.mid == mms._id "
        "LEFT JOIN thread ON mms.thread_id == thread._id "
        "LEFT JOIN recipient ON thread." + d_thread_recipient_id + " == recipient._id "
        "LEFT JOIN groups ON recipient.group_id == groups.group_id "
        "WHERE part._id == ? AND part.unique_id == ?";
    }

    if (!threads.empty())
    {
      query += " AND thread._id IN (";
      for (uint i = 0; i < threads.size(); ++i)
        query += bepaald::toString(threads[i]) + ((i == threads.size() - 1) ? ")" : ",");
    }

    if (!d_database.exec(query, {static_cast<long long int>(a->rowId()), static_cast<long long int>(a->attachmentId())},  &results))
      return false;

    //results.prettyPrint();

    if (results.rows() == 0 && !threads.empty()) // probably an attachment for a de-selected thread
      continue;

    if (results.rows() != 1)
    {
      std::cout << " ERROR Unexpected number of results: " << results.rows()
                << " (rowid: " << a->rowId() << ", uniqueid: " << a->attachmentId() << ")" << std::endl;
      continue;
    }

    std::string filename;
    long long int datum = a->attachmentId();

    if (fullbackup && !results.isNull(0, "date_received"))
      datum = results.getValueAs<long long int>(0, "date_received");
    long long int order = results.getValueAs<long long int>(0, "display_order");

    if (!results.isNull(0, "file_name")) // file name IS SET in database
      filename = sanitizeFilename(results.valueAsString(0, "file_name"));

    if (filename.empty()) // filename was not set in database or was not impossible
    {                     // to sanitize (eg reserved name in windows 'COM1')
      // get datestring
      std::time_t epoch = datum / 1000;
      std::ostringstream tmp;
      tmp << std::put_time(std::localtime(&epoch), "signal-%Y-%m-%d-%H%M%S");
      //tmp << "." << datum % 1000;

      // get file ext
      std::string mime = results.valueAsString(0, "ct");
      std::string ext = std::string(mimetypes.getExtension(mime));
      if (ext.empty())
      {
        ext = "attach";
        std::cout << " WARNING: mimetype not found in database (" << mime
                  << ") -> saving as '" << tmp.str() << "." << ext << "'" << std::endl;
      }

      //build filename
      filename = tmp.str() + ((order) ? ("_" + bepaald::toString(order)) : "") + "." + ext;
    }

    // std::cout << "FILENAME: " << filename << std::endl;
    std::string targetdir = dir;
    if (fullbackup && !results.isNull(0, "thread_id") && !results.isNull(0, "chatpartner")
        && !results.isNull(0, d_mms_type))
    {
      long long int tid = results.getValueAs<long long int>(0, "thread_id");
      std::string chatpartner = sanitizeFilename(results.valueAsString(0, "chatpartner"));
      if (chatpartner.empty())
        chatpartner = "Contact " + bepaald::toString(tid);

      int idx_of_thread = -1;
      if ((idx_of_thread = bepaald::findIdxOf(conversations.first, tid)) == -1) // idx not found
      {
        if (std::find(conversations.second.begin(), conversations.second.end(), chatpartner) == conversations.second.end()) // chatpartner not used yet
        {
          // add it
          conversations.first.push_back(tid);
          conversations.second.push_back(chatpartner);
          idx_of_thread = conversations.second.size() - 1;
        }
        else // new conversation, but another conversation with same name already exists!
        {
          // get unique conversation name
          chatpartner += "(2)";
          while (std::find(conversations.second.begin(), conversations.second.end(), chatpartner) != conversations.second.end())
            chatpartner += "(2)";
          conversations.first.push_back(tid);
          conversations.second.push_back(chatpartner);
          idx_of_thread = conversations.second.size() - 1;
        }
      } // else, thread was found, use the name that was used before

      // create dir if not exists
      if (!bepaald::isDir(dir + "/" + conversations.second[idx_of_thread]))
      {
        // std::cout << " Creating subdirectory '" << conversations.second[idx_of_thread] << "' for conversation..." << std::endl;
        if (!bepaald::createDir(dir + "/" + conversations.second[idx_of_thread]))
        {
          std::cout << " ERROR creating directory '" << dir << "/" << conversations.second[idx_of_thread] << "'" << std::endl;
          continue;
        }
      }

      long long int msg_box = results.getValueAs<long long int>(0, d_mms_type);
      targetdir = dir + "/" + conversations.second[idx_of_thread] + "/" + (Types::isOutgoing(msg_box) ? "sent" : "received");

      // create dir if not exists
      if (!bepaald::isDir(targetdir))
      {
        //std::cout << " Creating subdirectory '" << targetdir << "' for conversation..." << std::endl;
        if (!bepaald::createDir(targetdir))
        {
          std::cout << " ERROR creating directory '" << targetdir << "'" << std::endl;
          continue;
        }
      }
    }

    // make filename unique
    while (bepaald::fileOrDirExists(targetdir + "/" + filename))
    {
      //std::cout << std::endl << "File exists: " << targetdir << "/" << filename << " -> ";

      std::filesystem::path p(filename);
      std::regex numberedfile(".*( \\(([0-9]*)\\))$");
      std::smatch sm;
      std::string filestem(p.stem().string());
      std::string ext(p.extension().string());
      int counter = 2;
      if (regex_match(filestem, sm, numberedfile) && sm.size() >= 3 && sm[2].matched)
      {
        // increase the counter
        counter = bepaald::toNumber<int>(sm[2]) + 1;
        // remove " (xx)" part from stem
        filestem.erase(sm[1].first, sm[1].second);
      }
      filename = filestem + " (" + bepaald::toString(counter) + ")" + p.extension().string();

      //std::cout << filename << std::endl;
    }

    std::ofstream attachmentstream(targetdir + "/" + filename, std::ios_base::binary);

    if (!attachmentstream.is_open())
    {
      std::cout << " ERROR Failed to open file for writing: " << targetdir << "/" << filename << std::endl;
      continue;
    }
    else
      if (!attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize()))
      {
        std::cout << " ERROR Failed to write data to file: " << targetdir << "/" << filename << std::endl;
        a->clearData();
        continue;
      }
    attachmentstream.close(); // need to close, or the auto-close will change files mtime again.
    a->clearData();

    setFileTimeStamp(targetdir + "/" + filename, datum); // ignoring return for now...

    // !! ifdef c++20
    //std::error_code ec;
    //std::filesystem::last_write_time(dir + "/" + chatpartner + "/" + filename, std::chrono::clock_cast<std::filesystem::file_time_type>(datum / 1000), ec);
  }
  std::cout << std::endl << "done." << std::endl;
  return true;
}
