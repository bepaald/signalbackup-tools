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

SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, std::string const &outputdir)
  :
  d_database(outputdir.empty() ? ":memory:" : outputdir + "/database.sqlite"),
  d_fd(new FileDecryptor(filename, passphrase, false)),
  d_passphrase(passphrase),
  d_ok(false)
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

    if (d_fd->badMac())
    {
      std::cout << std::endl << "WARNING: Bad MAC in frame, trying to print frame info:" << std::endl;
      frame->printInfo();

      if (frame->frameType() == FRAMETYPE::ATTACHMENT)
      {
        std::unique_ptr<AttachmentFrame> a = std::make_unique<AttachmentFrame>(*reinterpret_cast<AttachmentFrame *>(frame.get()));
        //std::unique_ptr<AttachmentFrame> a(reinterpret_cast<AttachmentFrame *>(frame.release()));

        uint32_t rowid = a->rowId();
        uint64_t uniqueid = a->attachmentId();

        d_badattachments.emplace_back(std::make_pair(rowid, uniqueid));

        std::cout << "Frame is attachment, it belongs to entry in the 'part' table of the database:" << std::endl;
        std::vector<std::vector<std::pair<std::string, std::any>>> results;
        std::string query = "SELECT * FROM part WHERE _id = " + bepaald::toString(rowid) + " AND unique_id = " + bepaald::toString(uniqueid);
        long long int mid = -1;
        d_database.exec(query, &results);
        for (uint i = 0; i < results.size(); ++i)
        {
          for (uint j = 0; j < results[i].size(); ++j)
          {
            std::cout << " - " << std::any_cast<std::string>(results[i][j].first) << " : ";
            if (results[i][j].second.type() == typeid(nullptr))
              std::cout << "(NULL)" << std::endl;
            else if (results[i][j].second.type() == typeid(std::string))
              std::cout << std::any_cast<std::string>(results[i][j].second) << std::endl;
            else if (results[i][j].second.type() == typeid(double))
              std::cout << std::any_cast<double>(results[i][j].second) << std::endl;
            else if (results[i][j].second.type() == typeid(long long int))
            {
              if (std::any_cast<std::string>(results[i][j].first) == "mid")
                mid = std::any_cast<long long int>(results[i][j].second);
              std::cout << std::any_cast<long long int>(results[i][j].second) << std::endl;
            }
            else if (results[i][j].second.type() == typeid(std::pair<std::shared_ptr<unsigned char []>, size_t>))
              std::cout << bepaald::bytesToHexString(std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(results[i][j].second).first.get(),
                                                     std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(results[i][j].second).second) << std::endl;
            else
              std::cout << "(unhandled result type)" << std::endl;
          }
        }

        std::cout << std::endl << "Which belongs to entry in 'mms' table:" << std::endl;
        query = "SELECT * FROM mms WHERE _id = " + bepaald::toString(mid);
        d_database.exec(query, &results);

        for (uint i = 0; i < results.size(); ++i)
          for (uint j = 0; j < results[i].size(); ++j)
          {
            std::cout << " - " << std::any_cast<std::string>(results[i][j].first) << " : ";
            if (results[i][j].second.type() == typeid(nullptr))
              std::cout << "(NULL)" << std::endl;
            else if (results[i][j].second.type() == typeid(std::string))
              std::cout << std::any_cast<std::string>(results[i][j].second) << std::endl;
            else if (results[i][j].second.type() == typeid(double))
              std::cout << std::any_cast<double>(results[i][j].second) << std::endl;
            else if (results[i][j].second.type() == typeid(long long int))
            {

              if (std::any_cast<std::string>(results[i][j].first) == "date" ||
                  std::any_cast<std::string>(results[i][j].first) == "date_received")
              {
                long long int datum = std::any_cast<long long int>(results[i][j].second);
                std::time_t epoch = datum / 1000;
                std::cout << std::put_time(std::localtime(&epoch), "%F %T %z") << " (" << std::any_cast<long long int>(results[i][j].second) << ")" << std::endl;
              }
              else
                std::cout << std::any_cast<long long int>(results[i][j].second) << std::endl;
            }
            else if (results[i][j].second.type() == typeid(std::pair<std::shared_ptr<unsigned char []>, size_t>))
              std::cout << bepaald::bytesToHexString(std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(results[i][j].second).first.get(),
                                                     std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(results[i][j].second).second) << std::endl;
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
        frame->printInfo();
      }
    }

    std::cout << "\33[2K\rFRAME " << frame->frameNumber() << " ("
              << std::fixed << std::setprecision(1) << std::setw(5) << std::setfill('0')
              << (static_cast<float>(d_fd->curFilePos()) / totalsize) * 100 << "%)" << std::defaultfloat
              << "... " << std::flush;

    if (frame->frameType() == FRAMETYPE::HEADER)
    {
      if (!outputdir.empty())
        writeRawFrameDataToFile(outputdir + "/Header.sbf", frame);
      d_headerframe.reset(reinterpret_cast<HeaderFrame *>(frame.release()));
      //d_headerframe->printInfo();
    }
    else if (frame->frameType() == FRAMETYPE::DATABASEVERSION)
    {
      if (!outputdir.empty())
        writeRawFrameDataToFile(outputdir + "/DatabaseVersion.sbf", frame);
      d_databaseversionframe.reset(reinterpret_cast<DatabaseVersionFrame *>(frame.release()));
      //d_databaseversionframe->printInfo();
    }
    else if (frame->frameType() == FRAMETYPE::SQLSTATEMENT)
    {
      SqlStatementFrame *s = reinterpret_cast<SqlStatementFrame *>(frame.get());
      if (s->statement().find("CREATE TABLE sqlite_") == std::string::npos)
        d_database.exec(s->bindStatement(), s->parameters());
    }
    else if (frame->frameType() == FRAMETYPE::ATTACHMENT)
    {
      AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(frame.release());
      if (!outputdir.empty())
      {
        writeRawFrameDataToFile(outputdir + "/Attachment_" + bepaald::toString(a->rowId()) + "_" + bepaald::toString(a->attachmentId()) + ".sbf", a);
        // write actual attachment:
        std::ofstream attachmentstream(outputdir + "/Attachment_" + bepaald::toString(a->rowId()) + "_" + bepaald::toString(a->attachmentId()) + ".bin", std::ios_base::binary);
        if (!attachmentstream.is_open())
          std::cout << "Failed to open file for writing: " << outputdir
                    << "/Attachment_" << bepaald::toString(a->rowId()) << "_" << bepaald::toString(a->attachmentId()) << ".bin" << std::endl;
        else
          attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
      }
      d_attachments.emplace(std::make_pair(a->rowId(), a->attachmentId()), a);
    }
    else if (frame->frameType() == FRAMETYPE::AVATAR)
    {
      AvatarFrame *a = reinterpret_cast<AvatarFrame *>(frame.release());
      if (!outputdir.empty())
      {
        writeRawFrameDataToFile(outputdir + "/Avatar_" + a->name() + ".sbf", a);
        // write actual attachment:
        std::ofstream attachmentstream(outputdir + "/Avatar_" + a->name() + ".bin", std::ios_base::binary);
        if (!attachmentstream.is_open())
          std::cout << "Failed to open file for writing: " << outputdir
                    << "/Avatar_" << a->name() << ".bin" << std::endl;
        else
          attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
      }
      d_avatars.emplace(std::move(std::string(a->name())), a);
    }
    else if (frame->frameType() == FRAMETYPE::SHAREDPREFERENCE)
    {
      if (!outputdir.empty())
        writeRawFrameDataToFile(outputdir + "/SharedPreference_" + bepaald::toString(d_sharedpreferenceframes.size()) + ".sbf", frame);
      d_sharedpreferenceframes.emplace_back(reinterpret_cast<SharedPrefFrame *>(frame.release()));
    }
    else if (frame->frameType() == FRAMETYPE::END)
    {
      if (!outputdir.empty())
        writeRawFrameDataToFile(outputdir + "/End.sbf", frame);
      d_endframe.reset(reinterpret_cast<EndFrame *>(frame.release()));
    }
  }

  d_database.exec("COMMIT");

  std::cout << "done!" << std::endl;

  d_ok = true;
}

SignalBackup::SignalBackup(std::string const &inputdir)
  :
  d_database(inputdir + "/database.sqlite"),
  d_fe(),
  d_ok(false)
{
  if (!setFrameFromFile(&d_headerframe, inputdir + "/Header.sbf"))
    return;

  //d_headerframe->printInfo();

  if (!setFrameFromFile(&d_databaseversionframe, inputdir + "/DatabaseVersion.sbf"))
    return;

  //d_databaseversionframe->printInfo();

  int idx = 0;
  while (true)
  {
    d_sharedpreferenceframes.resize(d_sharedpreferenceframes.size() + 1);
    if (!setFrameFromFile(&d_sharedpreferenceframes.back(), inputdir + "/SharedPreference_" + bepaald::toString(idx) + ".sbf", true))
    {
      d_sharedpreferenceframes.pop_back();
      break;
    }
    //d_sharedpreferenceframes.back()->printInfo();
    ++idx;
  }

  if (!setFrameFromFile(&d_endframe, inputdir + "/End.sbf"))
    return;

  //d_endframe->printInfo();

  // avatars
  std::error_code ec;
  std::filesystem::directory_iterator dirit(inputdir, ec);
  if (ec)
  {
    std::cout << "Error iterating directory `" << inputdir << "' : " << ec.message() << std::endl;
    return;
  }
  for (auto const &avatar : dirit)
  {
    if (avatar.path().filename().string().substr(0, STRLEN("Avatar_")) != "Avatar_" || avatar.path().extension() != ".sbf")
      continue;

    std::filesystem::path avatarframe = avatar.path();
    std::filesystem::path avatarbin = avatar.path();
    avatarbin.replace_extension(".bin");

    std::unique_ptr<AvatarFrame> temp;
    if (!setFrameFromFile(&temp, avatarframe.string()))
      return;
    temp->setAttachmentData(avatarbin.string());

    //temp->printInfo();

    std::string name = temp->name();

    d_avatars.emplace(name, temp.release());
  }

  //attachments
  dirit = std::filesystem::directory_iterator(inputdir, ec);
  if (ec)
  {
    std::cout << "Error iterating directory `" << inputdir << "' : " << ec.message() << std::endl;
    return;
  }
  for (auto const &att : dirit)
  {
    if (att.path().extension() != ".sbf" || att.path().filename().string().substr(0, STRLEN("Attachment_")) != "Attachment_")
      continue;

    std::filesystem::path attframe = att.path();
    std::filesystem::path attbin = att.path();
    attbin.replace_extension(".bin");

    std::unique_ptr<AttachmentFrame> temp;
    if (!setFrameFromFile(&temp, attframe.string()))
      return;
    temp->setAttachmentData(attbin.string());

    uint64_t rowid = temp->rowId();
    uint64_t attachmentid = temp->attachmentId();
    d_attachments.emplace(std::make_pair(rowid, attachmentid), temp.release());
  }


  d_ok = true;
}

void SignalBackup::exportBackup(std::string const &filename, std::string const &passphrase)
{
  std::cout << "Exporting backup to '" << filename << "'" << std::endl;

  std::string newpw = passphrase;
  if (newpw == std::string())
    newpw = d_passphrase;

  if (checkFileExists(filename))
  {
    std::cout << "File " << filename << " exists. Refusing to overwrite" << std::endl;
    return;
  }

  std::ofstream outputfile(filename, std::ios_base::binary);

  if (!d_fe.init(newpw, d_headerframe->salt(), d_headerframe->salt_length(), d_headerframe->iv(), d_headerframe->iv_length()))
  {
    std::cout << "Error initializing FileEncryptor" << std::endl;
    return;
  }

  // HEADER // Note: HeaderFrame is not encrypted.
  std::cout << "Writing HeaderFrame..." << std::endl;
  if (!d_headerframe)
  {
    std::cout << "Error: HeaderFrame not found" << std::endl;
    return;
  }
  std::pair<unsigned char *, uint64_t> framedata = d_headerframe->getData();
  if (!framedata.first)
  {
    std::cout << "Error getting HeaderFrame data" << std::endl;
    return;
  }
  writeFrameDataToFile(outputfile, framedata);
  delete[] framedata.first;

  // VERSION
  std::cout << "Writing DatabaseVersionFrame..." << std::endl;
  if (!d_databaseversionframe)
  {
    std::cout << "Error: DataBaseVersionFrame not found" << std::endl;
    return;
  }
  writeEncryptedFrame(outputfile, d_databaseversionframe.get());

  // SQL DATABASE + ATTACHMENTS
  std::cout << "Writing SqlStatementFrame(s)..." << std::endl;

  // get and write schema
  std::string q("SELECT sql, name, type FROM sqlite_master");
  std::vector<std::vector<std::pair<std::string, std::any>>> results;
  d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.size(); ++i)
  {
    if (results[i][0].second.type() != typeid(nullptr))
    {
      if (results[i][1].second.type() !=  typeid(nullptr) &&
          (std::any_cast<std::string>(results[i][1].second) != "sms_fts" &&
           std::any_cast<std::string>(results[i][1].second).find("sms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else if (results[i][1].second.type() !=  typeid(nullptr) &&
               (std::any_cast<std::string>(results[i][1].second) != "mms_fts" &&
                std::any_cast<std::string>(results[i][1].second).find("mms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else
      {
        if (std::any_cast<std::string>(results[i][2].second) == "table")
          tables.emplace_back(std::move(std::any_cast<std::string>(results[i][1].second)));

        SqlStatementFrame NEWFRAME;
        NEWFRAME.setStatementField(std::any_cast<std::string>(results[i][0].second));

        //std::cout << "Writing SqlStatementFrame..." << std::endl;
        writeEncryptedFrame(outputfile, &NEWFRAME);
      }
    }
  }

  // write contents of tables
  for (std::string const &table : tables)
  {
    if (table == "signed_prekeys" ||
        table == "one_time_prekeys" ||
        table == "sessions" ||
        table.substr(0, STRLEN("sms_fts")) == "sms_fts" ||
        table.substr(0, STRLEN("mms_fts")) == "mms_fts" ||
        table.substr(0, STRLEN("sqlite_")) == "sqlite_")
      continue;

    std::cout << "  Dealing with table '" << table << "'...";

    d_database.exec("SELECT * FROM " + table, &results);

    std::cout << results.size() << " entries..." << std::endl;

    for (uint i = 0; i < results.size(); ++i)
    {
      SqlStatementFrame NEWFRAME = buildSqlStatementFrame(table, results[i]);

      //std::cout << "Writing SqlStatementFrame..." << std::endl;
      writeEncryptedFrame(outputfile, &NEWFRAME);

      if (table == "part") // find corresponding attachment
      {
        uint64_t rowid = 0, uniqueid = 0;
        for (uint j = 0; j < results[i].size(); ++j)
        {
          if (results[i][j].first == "_id" && results[i][j].second.type() == typeid(long long int))
          {
            rowid = std::any_cast<long long int>(results[i][j].second);
            if (rowid && uniqueid)
              break;
          }
          else if (results[i][j].first == "unique_id" && results[i][j].second.type() == typeid(long long int))
          {
            //std::cout << "UNIQUEID: " << std::any_cast<long long int>(results[i][j].second) << std::endl;

            uniqueid = std::any_cast<long long int>(results[i][j].second);
            if (rowid && uniqueid)
              break;
          }
        }
        auto attachment = d_attachments.find({rowid, uniqueid});
        if (attachment != d_attachments.end())
          writeEncryptedFrame(outputfile, attachment->second.get());
        else
          std::cout << "Warning: attachment data not found" << std::endl;
      }
    }
  }

  std::cout << "Writing SharedPrefFrame(s)..." << std::endl;
  // SHAREDPREFS
  for (uint i = 0; i < d_sharedpreferenceframes.size(); ++i)
  {
    //std::cout << "Writing SharedPreferenceFrame..." << std::endl;
    writeEncryptedFrame(outputfile, d_sharedpreferenceframes[i].get());
  }

  // AVATAR + ATTACHMENTS
  for (auto const &a : d_avatars)
  {
    //std::cout << "Writing AvatarFrame" << std::endl;
    writeEncryptedFrame(outputfile, a.second.get());
  }

  // END
  std::cout << "Writing EndFrame..." << std::endl;
  if (!d_endframe)
  {
    std::cout << "Error: EndFrame not found" << std::endl;
    return;
  }
  writeEncryptedFrame(outputfile, d_endframe.get());

  std::cout << "Done!" << std::endl;
}

bool SignalBackup::dropBadFrames()
{
  if (d_badattachments.empty())
    return true;

  std::cout << "Removing " << d_badattachments.size() << " bad frames from database..." << std::endl;
  for (auto it = d_badattachments.begin(); it != d_badattachments.end(); )
  {
    uint32_t rowid = it->first;
    uint64_t uniqueid = it->second;


    std::vector<std::vector<std::pair<std::string, std::any>>> results;
    std::string query = "SELECT mid FROM part WHERE _id = " + bepaald::toString(rowid) + " AND unique_id = " + bepaald::toString(uniqueid);
    long long int mid = -1;
    d_database.exec(query, &results);
    for (uint i = 0; i < results.size(); ++i)
      for (uint j = 0; j < results[i].size(); ++j)
        if (results[i][j].second.type() == typeid(long long int))
          if (std::any_cast<std::string>(results[i][j].first) == "mid")
          {
            mid = std::any_cast<long long int>(results[i][j].second);
            break;
          }

    if (mid == -1)
    {
      std::cout << "Failed to remove frame :( Could not find matching 'part' entry" << std::endl;
      return false;
    }

    d_database.exec("DELETE FROM part WHERE mid = " + bepaald::toString(mid));
    d_badattachments.erase(it);
  }

  return true;
}
