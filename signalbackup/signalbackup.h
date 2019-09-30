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
#include "../stickerframe/stickerframe.h"
#include "../endframe/endframe.h"
#include "../sqlstatementframe/sqlstatementframe.h"

#include <map>
#include <string>
#include <iostream>

class SignalBackup
{
 public:
  static bool constexpr LOWMEM = true;
  static bool constexpr DROPATTACHMENTDATA = false;
 private:
  SqliteDB d_database;
  std::unique_ptr<FileDecryptor> d_fd;
  FileEncryptor d_fe;
  std::string d_passphrase;
  std::map<std::string, std::unique_ptr<AvatarFrame>> d_avatars;
  std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> d_attachments; //maps <rowid,uniqueid> to attachment
  std::map<uint64_t, std::unique_ptr<StickerFrame>> d_stickers; //maps <rowid> to sticker
  std::unique_ptr<HeaderFrame> d_headerframe;
  std::unique_ptr<DatabaseVersionFrame> d_databaseversionframe;
  std::vector<std::unique_ptr<SharedPrefFrame>> d_sharedpreferenceframes;
  std::unique_ptr<EndFrame> d_endframe;
  std::vector<std::pair<uint32_t, uint64_t>> d_badattachments;
  bool d_ok;
 public:
  SignalBackup(std::string const &filename, std::string const &passphrase, bool issource = false);
  explicit SignalBackup(std::string const &inputdir);
  void exportBackup(std::string const &filename, std::string const &passphrase, bool keepattachmentdatainmemory = true);
  void exportBackup(std::string const &directory);
  //void exportXml(std::string const &filename) const;
  void listThreads() const;
  void cropToThread(long long int threadid);
  void cropToThread(std::vector<long long int> const &threadid);
  void cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges);
  void addSMSMessage(std::string const &body, std::string const &address, std::string const &timestamp, long long int thread, bool incoming);
  void addSMSMessage(std::string const &body, std::string const &address, long long int timestamp, long long int thread, bool incoming);
  void importThread(SignalBackup *source, long long int thread);
  void importThread(SignalBackup *source, std::vector<long long int> const &threads);
  inline bool ok() const;
  bool dropBadFrames();
  void fillThreadTableFromMessages();
  inline void addEndFrame();
  void mergeRecipients(std::vector<std::string> const &addresses);
  inline void runSimpleQuery(std::string const &q) const;

 private:
  void updateThreadsEntries(long long int thread = -1);
  long long int getMaxUsedId(std::string const &table);
  long long int getMinUsedId(std::string const &table);
  inline bool checkFileExists(std::string const &filename) const;
  template <class T>
  inline void writeRawFrameDataToFile(std::string const &outputfile, T *frame) const;
  template <class T>
  inline void writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const;
  inline void writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const;
  inline void writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame);
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::string> const &headers, std::vector<std::any> const &result) const;
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::any> const &result) const;
  template <class T>
  inline bool setFrameFromFile(std::unique_ptr<T> *frame, std::string const &file, bool quiet = false) const;
  template <typename T>
  inline std::pair<unsigned char*, size_t> numToData(T num) const;
  void setMinimumId(std::string const &table, long long int offset) const;
  void cleanDatabaseByMessages();
  void compactIds(std::string const &table);
  void makeIdsUnique(long long int thread, long long int sms, long long int mms, long long int part, long long int recipient_preferences, long long int groups, long long int identies, long long int group_receipts, long long int drafts);
  long long int dateToMSecsSinceEpoch(std::string const &date) const;
  //void showQuery(std::string const &query) const;
  long long int getThreadIdFromRecipient(std::string const &recipient) const;
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

template <class T>
inline void SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, T *frame) const
{
  std::unique_ptr<T> temp(frame);
  writeRawFrameDataToFile(outputfile, temp);
  temp.release();
}

template <class T>
inline void SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const
{
  if (!frame)
  {
    std::cout << "Error: asked to write nullptr frame to disk" << std::endl;
    return;
  }

  std::ofstream rawframefile(outputfile, std::ios_base::binary);
  if (!rawframefile.is_open())
  {
    std::cout << "Error opening file for writing: " << outputfile << std::endl;
    return;
  }

  if (frame->frameType() == BackupFrame::FRAMETYPE::END)
    rawframefile << "END" << std::endl;
  else
  {
    T *t = reinterpret_cast<T *>(frame.get());
    std::string d = t->getHumanData();
    rawframefile << d;
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

template <typename T>
inline std::pair<unsigned char*, size_t> SignalBackup::numToData(T num) const
{
  unsigned char *data = new unsigned char[sizeof(T)];
  std::memcpy(data, reinterpret_cast<unsigned char *>(&num), sizeof(T));
  return {data, sizeof(T)};
}

template <>
inline bool SignalBackup::setFrameFromFile(std::unique_ptr<EndFrame> *frame, std::string const &file, bool quiet) const
{
  std::ifstream datastream(file, std::ios_base::binary | std::ios::in);
  if (!datastream.is_open())
  {
    if (!quiet)
      std::cout << "Failed to open '" << file << "' for reading" << std::endl;
    return false;
  }
  frame->reset(new EndFrame(nullptr, 1ull));
  return true;
}

template <class T>
inline bool SignalBackup::setFrameFromFile(std::unique_ptr<T> *frame, std::string const &file, bool quiet) const
{
  std::ifstream datastream(file, std::ios_base::binary | std::ios::in);
  if (!datastream.is_open())
  {
    if (!quiet)
      std::cout << "Failed to open '" << file << "' for reading" << std::endl;
    return false;
  }

  std::unique_ptr<T> newframe(new T);

  std::string line;
  while (std::getline(datastream, line))
  {
    std::string::size_type pos = line.find(":", 0);
    if (pos == std::string::npos)
    {
      std::cout << "Failed to read frame data from '" << file << "'" << std::endl;
      return false;
    }
    unsigned int field = newframe->getField(line.substr(0, pos));
    if (!field)
    {
      std::cout << "Failed to get field number" << std::endl;
      return false;
    }

    ++pos;
    std::string::size_type pos2 = line.find(":", pos);
    if (pos2 == std::string::npos)
    {
      std::cout << "Failed to read HeaderFrame from datafile" << std::endl;
      return false;
    }
    std::string type = line.substr(pos, pos2 - pos);
    std::string datastr = line.substr(pos2 + 1);

    if (type == "bytes")
    {
      std::pair<unsigned char *, size_t> decdata = Base64::base64StringToBytes(datastr);
      if (!decdata.first)
        return false;
      newframe->setNewData(field, decdata.first, decdata.second);
    }
    else if (type == "uint32")
    {
      std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoul(datastr)));
      if (!decdata.first)
        return false;
      newframe->setNewData(field, decdata.first, decdata.second);
    }
    else if (type == "uint64")
    {
      std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(datastr)));
      if (!decdata.first)
        return false;
      newframe->setNewData(field, decdata.first, decdata.second);
    }
    else if (type == "string")
    {
      unsigned char *data = new unsigned char[datastr.size()];
      std::memcpy(data, datastr.data(), datastr.size());
      newframe->setNewData(field, data, datastr.size());
    }
    else
      return false;
  }
  frame->reset(newframe.release());

  return true;
}

inline void SignalBackup::addEndFrame()
{
  d_endframe.reset(new EndFrame(nullptr, 1ull));
}

inline void SignalBackup::runSimpleQuery(std::string const &q) const
{
  std::cout << "Executing query: " << q << std::endl;
  SqliteDB::QueryResults res;
  d_database.exec(q, &res);
  res.prettyPrint();
}

#endif
