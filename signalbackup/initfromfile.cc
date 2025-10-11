/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

#include "../sqlstatementframe/sqlstatementframe.h"
#include <chrono>

void SignalBackup::initFromFile()
{

  using std::literals::chrono_literals::operator""ms;

  if (!d_fd->ok())
  {
    Logger::message("Failed to create filedecrypter");
    return;
  }

  // open file
  std::ifstream backupfile(d_filename, std::ios_base::binary | std::ios_base::in);
  if (!backupfile.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open file '", d_filename, "'");
    return;
  }

  auto old_time = std::chrono::steady_clock::now();
  int64_t totalsize = d_fd->total();
  if (d_verbose || !d_showprogress) [[unlikely]]
    Logger::message_overwrite("Reading backup file...");
  else
    Logger::message_overwrite("Reading backup file: 0.00%...");

  std::unique_ptr<BackupFrame> frame;

  d_database.exec("BEGIN TRANSACTION");

  // get frames and handle them until file is fully read or error is encountered
  while ((frame = d_fd->getFrame(backupfile)))
  {
    if (d_fd->badMac()) [[unlikely]]
    {
      dumpInfoOnBadFrame(&frame);
      if (d_stoponerror)
        return;
    }

    auto new_time = std::chrono::steady_clock::now();
    if (d_showprogress && (new_time - old_time > 250ms || d_verbose))
    {
      if (d_verbose) [[unlikely]]
        Logger::message_overwrite("FRAME ", frame->frameNumber(), ": ",
                                  std::fixed, std::setprecision(2), std::setw(5), std::setfill('0'),
                                  (static_cast<float>(backupfile.tellg() * 100) / totalsize), std::defaultfloat, "%...");
      else
        Logger::message_overwrite("Reading backup file: ",
                                  std::fixed, std::setprecision(2), std::setw(5), std::setfill('0'),
                                  (static_cast<float>(backupfile.tellg() * 100) / totalsize), std::defaultfloat, "%...");

      old_time = new_time;
    }

    //if (frame->frameNumber() > 73085)
    //  frame->printInfo();

    //MEMINFO("At frame ", frame->frameNumber(), " (", frame->frameTypeString(), ")");

    if (frame->frameType() == BackupFrame::FRAMETYPE::SQLSTATEMENT) [[likely]]
    {
      SqlStatementFrame *s = reinterpret_cast<SqlStatementFrame *>(frame.get());

      if (!STRING_STARTS_WITH(s->bindStatementView(), "CREATE TABLE sqlite_")) [[likely]] // skip creation of sqlite_ internal db's
      {
        // NOTE: in the official import, there are other tables that are skipped (virtual tables for search data)
        // we lazily do not check for them here, since we are dealing with official exported files which do not contain
        // these tables as they are excluded on the export-side as well. Additionally, the official import should be able
        // to properly deal with them anyway (that is: ignore them)
        if (!d_database.exec(s->bindStatementView(), s->parametersView())) [[unlikely]]
          Logger::warning("Failed to execute statement: ", s->statement());
      }
#ifdef BUILT_FOR_TESTING
      else if (s->bindStatementView().find("CREATE TABLE sqlite_sequence") != std::string::npos)
      {
        // force early creation of sqlite_sequence table,
        // this is completely unnecessary and only used
        // to get byte-identical backups during testing
        Logger::message("BUILT_FOR_TESTING : Forcing early creation of sqlite_sequence");
        d_database.exec("CREATE TABLE dummy (_id INTEGER PRIMARY KEY AUTOINCREMENT)");
        d_database.exec("DROP TABLE dummy");
        d_found_sqlite_sequence_in_backup = true;
      }
#endif
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
    {
      AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(frame.release());
      if (d_fulldecode) [[unlikely]]
      {
        a->attachmentData(d_verbose);
        a->clearData();
      }
      int64_t attachmentid = a->attachmentId();
      d_attachments.emplace(std::make_pair(a->rowId(), attachmentid ? attachmentid : -1), a);
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::AVATAR)
    {
      AvatarFrame *a = reinterpret_cast<AvatarFrame *>(frame.release());
      if (d_fulldecode) [[unlikely]]
      {
        a->attachmentData(d_verbose);
        a->clearData();
      }
      d_avatars.emplace_back(d_databaseversion < 33 ? a->name() : a->recipient(), a);
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::STICKER)
    {
      StickerFrame *s = reinterpret_cast<StickerFrame *>(frame.release());
      if (d_fulldecode) [[unlikely]]
      {
        s->attachmentData(d_verbose);
        s->clearData();
      }
      d_stickers.emplace(s->rowId(), s);
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::SHAREDPREFERENCE)
    {
      //frame->printInfo();
      d_sharedpreferenceframes.emplace_back(reinterpret_cast<SharedPrefFrame *>(frame.release()));
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::KEYVALUE)
    {
      //frame->printInfo();
      d_keyvalueframes.emplace_back(reinterpret_cast<KeyValueFrame *>(frame.release()));
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::HEADER)
    {
      d_headerframe.reset(reinterpret_cast<HeaderFrame *>(frame.release()));
      d_backupfileversion = d_headerframe->version();

      if (d_verbose) [[unlikely]]
        d_headerframe->printInfo();
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::DATABASEVERSION)
    {
      d_databaseversionframe.reset(reinterpret_cast<DatabaseVersionFrame *>(frame.release()));
      d_databaseversion = d_databaseversionframe->version();

      if (d_verbose) [[unlikely]]
        Logger::message("Database version: ", d_databaseversionframe->version());
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::END)
    {
      //frame->printInfo();
      if (d_verbose) [[unlikely]]
        Logger::message("Read EndFrame");
      d_endframe.reset(reinterpret_cast<EndFrame *>(frame.release()));
    }
    else [[unlikely]] // if (frame->frameType() == BackupFrame::FRAMETYPE::INVALID)
    {
      Logger::warning(Logger::Control::BOLD, "SKIPPING UNKNOWN OR INVALID FRAME! (", static_cast<int>(frame->frameType()), ")", Logger::Control::NORMAL);
    }
  }

  d_database.exec("COMMIT");

  if (!d_badattachments.empty()) [[unlikely]]
  {
    Logger::message("Attachment data with BAD MAC was encountered:");
    dumpInfoOnBadFrames();
  }
  //std::cout << "" << std::endl;

  if (d_fd->badMac()) [[unlikely]]
  {
    if (d_stoponerror)
      return;
  }

  if (!d_endframe) [[unlikely]]
  {
    Logger::warning("EndFrame was not read: backup is probably incomplete");
    addEndFrame();
  }

  if (backupfile.tellg() == totalsize && d_showprogress) [[likely]]
    Logger::message_overwrite("Reading backup file:", " 100.0%... done!", Logger::Control::ENDOVERWRITE);

  d_ok = setColumnNames();
}
