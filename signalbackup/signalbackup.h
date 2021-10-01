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
#include "../keyvalueframe/keyvalueframe.h"
#include "../stickerframe/stickerframe.h"
#include "../endframe/endframe.h"
#include "../sqlstatementframe/sqlstatementframe.h"

#include <map>
#include <string>
#include <iostream>
#include <algorithm>

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
  std::string d_thread_recipient_id;
  std::vector<std::pair<std::string, std::unique_ptr<AvatarFrame>>> d_avatars;
  std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<AttachmentFrame>> d_attachments; //maps <rowid,uniqueid> to attachment
  std::map<uint64_t, std::unique_ptr<StickerFrame>> d_stickers; //maps <rowid> to sticker
  std::unique_ptr<HeaderFrame> d_headerframe;
  std::unique_ptr<DatabaseVersionFrame> d_databaseversionframe;
  std::vector<std::unique_ptr<SharedPrefFrame>> d_sharedpreferenceframes;
  std::vector<std::unique_ptr<KeyValueFrame>> d_keyvalueframes;
  std::unique_ptr<EndFrame> d_endframe;
  std::vector<std::pair<uint32_t, uint64_t>> d_badattachments;
  bool d_ok;
  unsigned int d_databaseversion;
  bool d_showprogress;
  bool d_stoponbadmac;
  bool d_verbose;

 public:
  inline SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                      bool issource = false, bool showprogress = true, bool assumebadframesizeonbadmac = false,
                      std::vector<long long int> editattachments = std::vector<long long int>(),
                      bool stoponbadmac = false);
  [[nodiscard]] inline bool exportBackup(std::string const &filename, std::string const &passphrase,
                                         bool overwrite, bool keepattachmentdatainmemory, bool onlydb = false);
  bool exportXml(std::string const &filename, bool overwrite, bool includemms = false, bool keepattachmentdatainmemory = true) const;
  void exportCsv(std::string const &filename, std::string const &table) const;
  inline void listThreads() const;
  void cropToThread(long long int threadid);
  void cropToThread(std::vector<long long int> const &threadid);
  void cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges);
  inline void addSMSMessage(std::string const &body, std::string const &address, std::string const &timestamp,
                            long long int thread, bool incoming);
  void addSMSMessage(std::string const &body, std::string const &address, long long int timestamp,
                     long long int thread, bool incoming);
  void importThread(SignalBackup *source, long long int thread);
  void importThread(SignalBackup *source, std::vector<long long int> const &threads);
  inline bool ok() const;
  bool dropBadFrames();
  void fillThreadTableFromMessages();
  inline void addEndFrame();
  void mergeRecipients(std::vector<std::string> const &addresses, bool editmembers);
  void mergeGroups(std::vector<std::string> const &groups);
  inline void runQuery(std::string const &q, bool pretty = true) const;
  void removeDoubles();
  inline std::vector<int> threadIds() const;
  bool importCSV(std::string const &file, std::map<std::string, std::string> const &fieldmap);
  bool importWAChat(std::string const &file, std::string const &fmt, std::string const &self = std::string());
  bool summarize() const;
  bool reorderMmsSmsIds() const;
  bool dumpMedia(std::string const &dir, std::vector<int> const &threads, bool overwrite) const;
  bool dumpAvatars(std::string const &dir, std::vector<std::string> const &contacts, bool overwrite) const;

  /* CUSTOMS */
  bool hhenkel(std::string const &);
  bool sleepyh34d(std::string const &truncatedbackup, std::string const &pwd);
  bool hiperfall();
  /* CUSTOMS */

 private:
  [[nodiscard]] bool exportBackupToFile(std::string const &filename, std::string const &passphrase,
                                        bool overwrite, bool keepattachmentdatainmemory);
  [[nodiscard]] bool exportBackupToDir(std::string const &directory, bool overwrite, bool keepattachmentdatainmemory, bool onlydb);
  void initFromFile();
  void initFromDir(std::string const &inputdir);
  void updateThreadsEntries(long long int thread = -1);
  long long int getMaxUsedId(std::string const &table, std::string const &col = "_id");
  long long int getMinUsedId(std::string const &table, std::string const &col = "_id");
  template <typename T>
  [[nodiscard]] inline bool writeRawFrameDataToFile(std::string const &outputfile, T *frame) const;
  template <typename T>
  [[nodiscard]] inline bool writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const;
  [[nodiscard]] inline bool writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const;
  [[nodiscard]] bool writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame);
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::string> const &headers,
                                           std::vector<std::any> const &result) const;
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::any> const &result) const;
  template <class T> inline bool setFrameFromFile(std::unique_ptr<T> *frame, std::string const &file, bool quiet = false) const;
  template <typename T> inline std::pair<unsigned char*, size_t> numToData(T num) const;
  void setMinimumId(std::string const &table, long long int offset, std::string const &col = "_id") const;
  void cleanDatabaseByMessages();
  void remapRecipients();
  void compactIds(std::string const &table, std::string const &col = "_id");
  void makeIdsUnique(long long int minthread, long long int minsms, long long int minmms,
                     long long int minpart, long long int minrecipient, long long int mingroups,
                     long long int minidentities, long long int mingroup_receipts, long long int mindrafts,
                     long long int minsticker, long long int minmegaphone, long long int minremapped_recipients,
                     long long int minremapped_threads, long long int minmention);
  void updateRecipientId(long long int targetid, std::string ident);
  void updateRecipientId(long long int targetid, long long int sourceid, bool verbose = false);
  long long int dateToMSecsSinceEpoch(std::string const &date, bool *fromdatestring = nullptr) const;
  long long int getThreadIdFromRecipient(std::string const &recipient) const;
  void dumpInfoOnBadFrame(std::unique_ptr<BackupFrame> *frame);
  void dumpInfoOnBadFrames() const;
  void duplicateQuotes(std::string *s) const;
  std::string decodeStatusMessage(std::string const &body, long long int expiration, long long int type,
                                  std::string const &contactname) const;
  void escapeXmlString(std::string *s) const;
  void handleSms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, int i) const;
  void handleMms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, int i, bool keepattachmentdatainmemory) const;
  inline std::string getStringOr(SqliteDB::QueryResults const &results, int i,
                                 std::string const &columnname, std::string const &def = std::string()) const;
  inline long long int getIntOr(SqliteDB::QueryResults const &results, int i,
                                std::string const &columnname, long long int def) const;
  bool handleWAMessage(long long int thread_id, long long int time, std::string const &chatname, std::string const &author,
                       std::string const &message, std::string const &selfid, bool isgroup,
                       std::map<std::string, std::string> const &name_to_recipientid);
  bool setFileTimeStamp(std::string const &file, long long int time_usec) const;
  std::string sanitizeFilename(std::string const &filename) const;
  bool setColumnNames();
};

inline SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                                  bool issource, bool showprogress, bool assumebadframesizeonbadmac,
                                  std::vector<long long int> editattachments, bool stoponbadmac)
  :
  d_database(":memory:"),
  d_passphrase(passphrase),
  d_ok(false),
  d_databaseversion(-1),
  d_showprogress(showprogress),
  d_stoponbadmac(stoponbadmac),
  d_verbose(verbose)
{
  if (bepaald::isDir(filename))
    initFromDir(filename);
  else // not directory
  {
    d_fd.reset(new FileDecryptor(filename, passphrase, d_verbose, issource, stoponbadmac, assumebadframesizeonbadmac, editattachments));
    initFromFile();
  }

  if (d_ok) // set by initfrom()
    d_ok = setColumnNames();
}

inline bool SignalBackup::exportBackup(std::string const &filename, std::string const &passphrase, bool overwrite,
                                       bool keepattachmentdatainmemory, bool onlydb)
{
  // if output is existing directory
  if (bepaald::fileOrDirExists(filename) && bepaald::isDir(filename))
    return exportBackupToDir(filename, overwrite, keepattachmentdatainmemory, onlydb);

  // if output does not exist, but ends in directory delimiter
  if (!bepaald::fileOrDirExists(filename) &&
      (filename.back() == '/' || filename.back() == std::filesystem::path::preferred_separator))
  {
    // create dir
    std::error_code ec;
    if (!std::filesystem::create_directory(filename, ec))
    {
      std::cout << "Error: Failed to create directory \"" << filename << "\"" << std::endl;
      return false;
    }
    return exportBackupToDir(filename, overwrite, keepattachmentdatainmemory, onlydb);
  }

  // export to file
  return exportBackupToFile(filename, passphrase, overwrite, keepattachmentdatainmemory);
}

inline bool SignalBackup::ok() const
{
  return d_ok;
}

template <typename T>
inline bool SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, T *frame) const
{
  std::unique_ptr<T> temp(frame);
  bool res = writeRawFrameDataToFile(outputfile, temp);
  temp.release();
  return res;
}

template <class T>
inline bool SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const
{
  if (!frame)
  {
    std::cout << "Error: asked to write nullptr frame to disk" << std::endl;
    return false;
  }

  std::ofstream rawframefile(outputfile, std::ios_base::binary);
  if (!rawframefile.is_open())
  {
    std::cout << "Error opening file for writing: " << outputfile << std::endl;
    return false;
  }

  if (frame->frameType() == BackupFrame::FRAMETYPE::END)
    rawframefile << "END" << std::endl;
  else
  {
    T *t = reinterpret_cast<T *>(frame.get());
    std::string d = t->getHumanData();
    rawframefile << d;
  }
  return rawframefile.good();
}

inline bool SignalBackup::writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const
{
  uint32_t besize = bepaald::swap_endian(static_cast<uint32_t>(data.second));
  // write 4 byte size header
  if (!outputfile.write(reinterpret_cast<char *>(&besize), sizeof(uint32_t)))
    return false;
  // write data
  if (!outputfile.write(reinterpret_cast<char *>(data.first), data.second))
    return false;
  return true;
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

template <typename T>
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
      std::cout << "Failed to read frame data from '" << file << "'" << std::endl;
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
    else if (type == "uint64" || type == "uint32") // Note stoul and stoull are the same on linux. Internally 8 byte int are needed anyway.
    {                                              // (on windows stoul would be four bytes and the above if-clause would cause bad data
      std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(datastr)));
      if (!decdata.first)
        return false;
      newframe->setNewData(field, decdata.first, decdata.second);
    }
    else if (type == "int64" || type == "int32") // Note stol and stoll are the same on linux. Internally 8 byte int are needed anyway.
    {                                            // (on windows stol would be four bytes and the above if-clause would cause bad data
      std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoll(datastr)));
      if (!decdata.first)
        return false;
      newframe->setNewData(field, decdata.first, decdata.second);
    }
    else if (type == "float") // due to possible precision problems, the 4 bytes of float are saved in binary format (base64 encoded)
    {                         // WARNING untested
      std::pair<unsigned char *, size_t> decfloat = Base64::base64StringToBytes(datastr);
      if (!decfloat.first || decfloat.second != 4)
        return false;
      newframe->setNewData(field, decfloat.first, decfloat.second);
    }
    else if (type == "bool") // since booleans are stored as varints, this is identical to uint64/32 code
    {
      std::string val = (datastr == "true") ? "1" : "0";
      std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(val)));
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

inline void SignalBackup::runQuery(std::string const &q, bool pretty) const
{
  std::cout << " * Executing query: " << q << std::endl;
  SqliteDB::QueryResults res;
  if (!d_database.exec(q, &res))
    return;

  std::string q_comm = q.substr(0, STRLEN("DELETE")); // delete, insert and update are same length...
  std::for_each(q_comm.begin(), q_comm.end(), [] (char &ch) { ch = std::toupper(ch); });

  if (q_comm == "DELETE" || q_comm == "INSERT" || q_comm == "UPDATE")
  {
    std::cout << "Modified " << d_database.changed() << " rows" << std::endl;
    if (res.rows() == 0 && res.columns() == 0)
      return;
  }

  if (pretty)
    res.prettyPrint();
  else
    res.print();
}

inline void SignalBackup::listThreads() const
{
  std::cout << "Database version: " << d_databaseversion << std::endl;

  SqliteDB::QueryResults results;

  d_database.exec("SELECT MIN(mindate) AS 'Min Date', MAX(maxdate) AS 'Max Date' FROM (SELECT MIN(sms.date) AS mindate, MAX(sms.date) AS maxdate FROM sms UNION ALL SELECT MIN(mms.date_received) AS mindate, MAX(mms.date_received) AS maxdate FROM mms)", &results);
  results.prettyPrint();

  if (!d_database.containsTable("recipient"))
    d_database.exec("SELECT thread._id, thread." + d_thread_recipient_id + ", thread.snippet, COALESCE(recipient_preferences.system_display_name, recipient_preferences.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient_preferences ON thread." + d_thread_recipient_id + " = recipient_preferences.recipient_ids LEFT JOIN groups ON thread." + d_thread_recipient_id + " = groups.group_id ORDER BY thread._id ASC", &results);
  else // has recipient table
    if (d_database.tableContainsColumn("recipient", "profile_joined_name"))
      d_database.exec("SELECT thread._id, COALESCE(recipient.phone, recipient.group_id) AS 'recipient_ids', thread.snippet, COALESCE(recipient.system_display_name, recipient.profile_joined_name, recipient.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id ORDER BY thread._id ASC", &results);
    else
      d_database.exec("SELECT thread._id, COALESCE(recipient.phone, recipient.group_id) AS 'recipient_ids', thread.snippet, COALESCE(recipient.system_display_name, recipient.signal_profile_name, groups.title) AS 'Conversation partner' FROM thread LEFT JOIN recipient ON thread." + d_thread_recipient_id + " = recipient._id LEFT JOIN groups ON recipient.group_id = groups.group_id ORDER BY thread._id ASC", &results);
  results.prettyPrint();
}

inline void SignalBackup::addSMSMessage(std::string const &body, std::string const &address, std::string const &timestamp, long long int thread, bool incoming)
{
  addSMSMessage(body, address, dateToMSecsSinceEpoch(timestamp), thread, incoming);
}

inline std::vector<int> SignalBackup::threadIds() const
{
  std::vector<int> res;
  SqliteDB::QueryResults results;
  d_database.exec("SELECT DISTINCT _id FROM thread ORDER BY _id ASC", &results);
  if (results.columns() == 1)
    for (uint i = 0; i < results.rows(); ++i)
      if (results.valueHasType<long long int>(i, 0))
        res.push_back(results.getValueAs<long long int>(i, 0));
  return res;
}

inline std::string SignalBackup::getStringOr(SqliteDB::QueryResults const &results, int i, std::string const &columnname, std::string const &def) const
{
  std::string tmp(def);
  if (results.valueHasType<std::string>(i, columnname))
  {
    tmp = results.getValueAs<std::string>(i, columnname);
    escapeXmlString(&tmp);
  }
  return tmp;
}

inline long long int SignalBackup::getIntOr(SqliteDB::QueryResults const &results, int i,
                                     std::string const &columnname, long long int def) const
{
  long long int temp = def;
  if (results.valueHasType<long long int>(i, columnname))
    temp = results.getValueAs<long long int>(i, columnname);
  return temp;
}

#endif
