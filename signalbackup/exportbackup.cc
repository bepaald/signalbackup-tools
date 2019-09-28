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

void SignalBackup::exportBackup(std::string const &directory)
{
  std::cout << std::endl << "Exporting backup into '" << directory << "/'" << std::endl;

  // maybe check directory exists, and is empty or make it empty if overwrite was requested.

  // export headerframe:
  std::cout << "Writing HeaderFrame..." << std::endl;
  writeRawFrameDataToFile(directory + "/Header.sbf", d_headerframe);

  // export databaseversionframe
  std::cout << "Writing DatabaseVersionFrame..." << std::endl;
  writeRawFrameDataToFile(directory + "/DatabaseVersion.sbf", d_databaseversionframe);

  // export attachments
  std::cout << "Writing Attachments..." << std::endl;
  for (auto const &aframe : d_attachments)
  {
    AttachmentFrame *a = aframe.second.get();
    std::string attachment_basefilename = directory + "/Attachment_" + bepaald::toString(a->rowId()) + "_" + bepaald::toString(a->attachmentId());
    writeRawFrameDataToFile(attachment_basefilename + ".sbf", a);
    // write actual attachment:
    std::ofstream attachmentstream(attachment_basefilename + ".bin", std::ios_base::binary);
    if (!attachmentstream.is_open())
      std::cout << "Failed to open file for writing: " << directory
                << attachment_basefilename << ".bin" << std::endl;
    else
      attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
  }

  // export avatars
  std::cout << "Writing Avatars..." << std::endl;
  for (auto const &aframe : d_avatars)
  {
    AvatarFrame *a = aframe.second.get();
    std::string avatar_basefilename = directory + "/Avatar_" + a->name();
    writeRawFrameDataToFile(avatar_basefilename + ".sbf", a);
    // write actual attachment:
    std::ofstream attachmentstream(avatar_basefilename + ".bin", std::ios_base::binary);
    if (!attachmentstream.is_open())
      std::cout << "Failed to open file for writing: " << directory
                << avatar_basefilename << ".bin" << std::endl;
    else
      attachmentstream.write(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
  }

  // export sharedpreferences
  std::cout << "Writing SharedPrefFrame(s)..." << std::endl;
  for (int count = 0; auto const &spframe : d_sharedpreferenceframes)
    writeRawFrameDataToFile(directory + "/SharedPreference_" + bepaald::toString(count++) + ".sbf", spframe);

  // export stickers
  std::cout << "Writing StickerFrames..." << std::endl;
  for (int count = 0; auto const &sframe : d_stickers)
  {
    StickerFrame *s = sframe.second.get();
    writeRawFrameDataToFile(directory + "/Sticker_" + bepaald::toString(count++) + ".sbf", s);
  }

  // export endframe
  std::cout << "Writing EndFrame..." << std::endl;
  writeRawFrameDataToFile(directory + "/End.sbf", d_endframe);

  // export database
  std::cout << "Writing database..." << std::endl;
  SqliteDB database(directory + "/database.sqlite", false /*readonly*/);
  if (!SqliteDB::copyDb(d_database, database))
    std::cout << "Error exporting sqlite database" << std::endl;
}

void SignalBackup::exportBackup(std::string const &filename, std::string const &passphrase, bool keepattachmentdatainmemory)
{
  std::cout << std::endl << "Exporting backup to '" << filename << "'" << std::endl;

  std::string newpw = passphrase;
  if (newpw == std::string())
    newpw = d_passphrase;

  if (/*!overwrrite && */checkFileExists(filename))
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
  SqliteDB::QueryResults results;
  d_database.exec(q, &results);
  std::vector<std::string> tables;
  for (uint i = 0; i < results.rows(); ++i)
  {
    if (!results.valueHasType<std::nullptr_t>(i, 0))
    {
      if (results.valueHasType<std::string>(i, 1) &&
          (results.getValueAs<std::string>(i, 1) != "sms_fts" &&
           results.getValueAs<std::string>(i, 1).find("sms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else if (results.valueHasType<std::string>(i, 1) &&
               (results.getValueAs<std::string>(i, 1) != "mms_fts" &&
                results.getValueAs<std::string>(i, 1).find("mms_fts") == 0))
        ;//std::cout << "Skipping " << results[i][1].second << " because it is smsftssecrettable" << std::endl;
      else
      {
        if (results.valueHasType<std::string>(i, 2) && results.getValueAs<std::string>(i, 2) == "table")
          tables.emplace_back(results.getValueAs<std::string>(i, 1));

        SqlStatementFrame NEWFRAME;
        NEWFRAME.setStatementField(results.getValueAs<std::string>(i, 0));

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
        table.starts_with("sms_fts") ||
        table.starts_with("mms_fts") ||
        table.starts_with("sqlite_"))
      continue;

    d_database.exec("SELECT * FROM " + table, &results);

    for (uint i = 0; i < results.rows(); ++i)
    {
      std::cout << "\33[2K\r  Dealing with table '" << table << "'... " << i + 1 << "/" << results.rows() << " entries..." << std::flush;

      SqlStatementFrame NEWFRAME = buildSqlStatementFrame(table, results.row(i));

      //std::cout << "Writing SqlStatementFrame..." << std::endl;
      writeEncryptedFrame(outputfile, &NEWFRAME);

      if (table == "part") // find corresponding attachment
      {
        uint64_t rowid = 0, uniqueid = 0;
        for (uint j = 0; j < results.columns(); ++j)
        {
          if (results.header(j) == "_id" && results.valueHasType<long long int>(i, j))
          {
            rowid = results.getValueAs<long long int>(i, j);
            if (rowid && uniqueid)
              break;
          }
          else if (results.header(j) == "unique_id" && results.valueHasType<long long int>(i, j))
          {
           //std::cout << "UNIQUEID: " << std::any_cast<long long int>(results[i][j].second) << std::endl;
            uniqueid = results.getValueAs<long long int>(i, j);
            if (rowid && uniqueid)
              break;
          }
        }
        auto attachment = d_attachments.find({rowid, uniqueid});
        if (attachment != d_attachments.end())
        {
          writeEncryptedFrame(outputfile, attachment->second.get());
          if (!keepattachmentdatainmemory)
            attachment->second.get()->clearData();
        }
        else
        {
          std::cout << "Warning: attachment data not found (rowid: " << rowid << ", uniqueid: " << uniqueid << ")" << std::endl;
          std::cout << "\33[2K\r  Dealing with table '" << table << "'... " << i + 1 << "/" << results.rows() << " entries..." << std::flush;
        }
      }
    }
    if (results.rows())
      std::cout << "done" << std::endl;
    else
      std::cout << "  Dealing with table '" << table << "'... 0/0 entries..." << std::endl;
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

  // STICKER + ATTACHMENTS
  for (auto const &s : d_stickers)
  {
    //std::cout << "Writing StickerFrame" << std::endl;
    writeEncryptedFrame(outputfile, s.second.get());
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
