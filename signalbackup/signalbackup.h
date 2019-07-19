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

#ifndef SIGNALBACKUP_H_
#define SIGNALBACKUP_H_

#include "../sqlitedb/sqlitedb.h"
#include "../filedecryptor/filedecryptor.h"
#include "../fileencryptor/fileencryptor.h"
#include "../backupframe/backupframe.h"
#include "../headerframe/headerframe.h"
#include "../databaseversionframe/databaseversionframe.h"
#include "../attachmentframe/attachmentframe.h"
#include "../avatarframe/avatarframe.h"
#include "../sharedprefframe/sharedprefframe.h"
#include "../endframe/endframe.h"
#include "../sqlstatementframe/sqlstatementframe.h"

#include <map>
#include <string>
#include <iostream>

class SignalBackup
{
  SqliteDB d_database;
  std::unique_ptr<FileDecryptor> d_fd;
  FileEncryptor d_fe;
  std::string d_passphrase;
  std::map<std::string, std::unique_ptr<AvatarFrame>> d_avatars;
  std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> d_attachments; //maps <rowid,uniqueid> to attachment
  std::unique_ptr<HeaderFrame> d_headerframe;
  std::unique_ptr<DatabaseVersionFrame> d_databaseversionframe;
  std::vector<std::unique_ptr<SharedPrefFrame>> d_sharedpreferenceframes;
  std::unique_ptr<EndFrame> d_endframe;
  std::vector<std::pair<uint32_t, uint64_t>> d_badattachments;
  bool d_ok;
 public:
  SignalBackup(std::string const &filename, std::string const &passphrase, std::string const &outputdir = std::string());
  SignalBackup(std::string const &inputdir);
  void exportBackup(std::string const &filename, std::string const &passphrase = std::string());
  inline bool ok() const;
  bool dropBadFrames();
 private:
  inline bool checkFileExists(std::string const &filename) const;
  inline void writeRawFrameDataToFile(std::string const &outputfile, BackupFrame *frame) const;
  inline void writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<BackupFrame> const &frame) const;
  inline void writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const;
  inline void writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame);
  inline SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::pair<std::string, std::any>> const &result) const;

  template <class T>
  bool setFrameFromFile(std::unique_ptr<T> *frame, std::string const &file, bool quiet = false);
};

inline bool SignalBackup::ok() const
{
  return d_ok;
}

inline bool SignalBackup::checkFileExists(std::string const &) const
{
  // needs implementing....
  return false;
}

inline void SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, BackupFrame *frame) const
{
  std::unique_ptr<BackupFrame> temp(frame);
  writeRawFrameDataToFile(outputfile, temp);
  temp.release();
}

inline void SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<BackupFrame> const &frame) const
{
  std::ofstream rawframefile(outputfile, std::ios_base::binary);
  if (!rawframefile.is_open())
    std::cout << "Error opening file for writing: " << outputfile << std::endl;
  else
  {
    std::pair<unsigned char *, uint64_t> framedata = frame->getData();
    rawframefile.write(reinterpret_cast<char *>(framedata.first), framedata.second);
    delete[] framedata.first;
  }
}

inline void SignalBackup::writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const
{
  uint32_t besize = bepaald::swap_endian(static_cast<uint32_t>(data.second));
  outputfile.write(reinterpret_cast<char *>(&besize), sizeof(uint32_t));
  outputfile.write(reinterpret_cast<char *>(data.first), data.second);
}

inline void SignalBackup::writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame)
{
  std::pair<unsigned char *, uint64_t> framedata = frame->getData();
  if (!framedata.first)
    return;
  std::pair<unsigned char *, uint64_t> encryptedframe = d_fe.encryptFrame(framedata);
  delete[] framedata.first;

  if (!encryptedframe.first)
    return;
  writeFrameDataToFile(outputfile, encryptedframe);
  delete[] encryptedframe.first;

  uint32_t attachmentsize = 0;
  if ((attachmentsize = frame->attachmentSize()) > 0)
  {
    FrameWithAttachment *f = reinterpret_cast<FrameWithAttachment *>(frame);
    unsigned char *attachmentdata = f->attachmentData();
    std::pair<unsigned char *, uint64_t> newadata = d_fe.encryptAttachment(attachmentdata, attachmentsize);
    //std::cout << "Writing attachment data..." << std::endl;
    outputfile.write(reinterpret_cast<char *>(newadata.first), newadata.second);
    delete[] newadata.first;
  }
}

inline SqlStatementFrame SignalBackup::buildSqlStatementFrame(std::string const &table, std::vector<std::pair<std::string, std::any>> const &result) const
{
  //std::cout << "Building new frame:" << std::endl;

  SqlStatementFrame NEWFRAME;
  std::string newstatement = "INSERT INTO " + table + " VALUES (";
  for (uint j = 0; j < result.size(); ++j)
  {
    if (j < result.size() - 1)
      newstatement.append("?,");
    else
      newstatement.append("?)");
    if (result[j].second.type() == typeid(nullptr))
      NEWFRAME.addNullParameter();
    else if (result[j].second.type() == typeid(long long int))
      NEWFRAME.addIntParameter(std::any_cast<long long int>(result[j].second));
    else if (result[j].second.type() == typeid(std::string))
      NEWFRAME.addStringParameter(std::any_cast<std::string>(result[j].second));
    else if (result[j].second.type() == typeid(std::pair<std::shared_ptr<unsigned char []>, size_t>))
      NEWFRAME.addBlobParameter(std::any_cast<std::pair<std::shared_ptr<unsigned char []>, size_t>>(result[j].second));
    else if (result[j].second.type() == typeid(double))
      NEWFRAME.addDoubleParameter(std::any_cast<double>(result[j].second));
    else
      std::cout << "WARNING : UNHANDLED PARAMETER TYPE = " << result[j].second.type().name() << std::endl;
  }
  NEWFRAME.setStatementField(newstatement);

  //NEWFRAME.printInfo();

  //std::exit(0);

  return NEWFRAME;
}

#endif
