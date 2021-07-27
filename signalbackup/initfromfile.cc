/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

void SignalBackup::initFromFile()
{
  if (!d_fd->ok())
  {
    std::cout << "Failed to create filedecrypter" << std::endl;
    return;
  }

  uint64_t totalsize = d_fd->total();

  std::cout << "Reading backup file..." << std::endl;
  std::unique_ptr<BackupFrame> frame(nullptr);

  d_database.exec("BEGIN TRANSACTION");

  while ((frame = d_fd->getFrame())) // deal with bad mac??
  {
    [[unlikely]] if (d_fd->badMac())
    {
      dumpInfoOnBadFrame(&frame);
      if (d_stoponbadmac)
        return;
    }

    [[likely]] if (d_showprogress)
    {
      std::cout << (d_verbose ? "" : "\33[2K\r") << "FRAME " << frame->frameNumber() << " ("
                << std::fixed << std::setprecision(1) << std::setw(5) << std::setfill('0')
                << (static_cast<float>(d_fd->curFilePos()) / totalsize) * 100 << "%)" << std::defaultfloat
                << "... " << std::flush;
      [[unlikely]] if (d_verbose)
        std::cout << "\n";
    }

    //std::cout << frame->frameTypeString() << std::endl;

    //MEMINFO("At frame ", frame->frameNumber(), " (", frame->frameTypeString(), ")");

    [[unlikely]] if (frame->frameType() == BackupFrame::FRAMETYPE::HEADER)
    {
      d_headerframe.reset(reinterpret_cast<HeaderFrame *>(frame.release()));
      //d_headerframe->printInfo();
    }
    else [[unlikely]] if (frame->frameType() == BackupFrame::FRAMETYPE::DATABASEVERSION)
    {
      d_databaseversionframe.reset(reinterpret_cast<DatabaseVersionFrame *>(frame.release()));
      d_databaseversion = d_databaseversionframe->version();

      [[unlikely]] if (d_verbose)
        std::cout << std::endl << "Database version: " << d_databaseversionframe->version() << std::endl;
    }
    else [[likely]] if (frame->frameType() == BackupFrame::FRAMETYPE::SQLSTATEMENT)
    {
      SqlStatementFrame *s = reinterpret_cast<SqlStatementFrame *>(frame.get());

      //if (frame->frameNumber() < 500)
      //  frame->printInfo();

      [[likely]] if (s->statement().find("CREATE TABLE sqlite_") == std::string::npos) // skip creation of sqlite_ internal db's
      {
        // NOTE: in the official import, there are other tables that are skipped (virtual tables for search data)
        // we lazily do not check for them here, since we are dealing with official exported fiels which do not contain
        // these tables as they are excluded on the export-side as well. Additionally, the official import should be able
        // to properly deal with them anyway (that is: ignore them)
        if (!d_database.exec(s->bindStatement(), s->parameters()))
          std::cout << "WARNING: Failed to execute statement: " << s->statement() << std::endl;
      }
      #ifdef BUILT_FOR_TESTING
      else if (s->statement().find("CREATE TABLE sqlite_sequence") != std::string::npos)
      {
        // force early creation of sqlite_sequence table, this is completely unnessecary and only used
        // to get byte-identical backups during testing
        std::cout << "BUILT_FOR_TESTING : Forcing early creation of sqlite_sequence" << std::endl;
        d_database.exec("CREATE TABLE dummy (_id INTEGER PRIMARY KEY AUTOINCREMENT)");
        d_database.exec("DROP TABLE dummy");
      }
      #endif
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
    {
      AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(frame.release());
      d_attachments.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
    }
    else if (frame->frameType() == BackupFrame::FRAMETYPE::AVATAR)
    {
      AvatarFrame *a = reinterpret_cast<AvatarFrame *>(frame.release());
      d_avatars.emplace_back(std::string((d_databaseversion < 33) ? a->name() : a->recipient()), a);
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
    else if (frame->frameType() == BackupFrame::FRAMETYPE::STICKER)
    {
      StickerFrame *s = reinterpret_cast<StickerFrame *>(frame.release());
      d_stickers.emplace(s->rowId(), s);
    }
    else [[unlikely]] if (frame->frameType() == BackupFrame::FRAMETYPE::END)
    {
      d_endframe.reset(reinterpret_cast<EndFrame *>(frame.release()));
    }
    else [[unlikely]] if (frame->frameType() == BackupFrame::FRAMETYPE::INVALID)
    {
      std::cout << std::endl << "WARNING! SKIPPING INVALID FRAME!" << std::endl;
    }
  }

  d_database.exec("COMMIT");

  if (!d_badattachments.empty())
  {
    std::cout << "Attachment data with BAD MAC was encountered:" << std::endl;
    dumpInfoOnBadFrames();
  }
  std::cout << "" << std::endl;

  [[unlikely]] if (d_fd->badMac())
  {
    if (d_stoponbadmac)
      return;
  }

  std::cout << "done!" << std::endl;

  if (!d_endframe)
  {
    std::cout << bepaald::bold_on << "WARNING " << bepaald::bold_off
              << "EndFrame was not read: backup is probably incomplete" << std::endl;
    addEndFrame();
  }

  d_ok = true;
}
