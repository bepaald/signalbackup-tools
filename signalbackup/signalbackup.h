/*
  Copyright (C) 2019-2023  Selwin van Dijk

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
#include <set>
#include <unordered_set>
#include <string>
#include <iostream>
#include <algorithm>

struct HTMLMessageInfo;

class SignalBackup
{
 public:
  static bool constexpr DROPATTACHMENTDATA = false;

 private:
  SqliteDB d_database;
  std::unique_ptr<FileDecryptor> d_fd;
  FileEncryptor d_fe;
  std::string d_passphrase;

  // only used in testing
  bool d_found_sqlite_sequence_in_backup;

  // table/column names
  std::string d_mms_table;
  std::string d_thread_recipient_id;
  std::string d_thread_message_count;
  std::string d_sms_date_received;
  std::string d_sms_recipient_id;
  std::string d_sms_recipient_device_id;
  std::string d_mms_date_sent;
  std::string d_mms_recipient_id;
  std::string d_mms_recipient_device_id;
  std::string d_mms_type;
  std::string d_mms_previews;

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
  bool d_stoponerror;
  bool d_verbose;

  enum DBLinkFlag : int
  {
    NO_COMPACT = 0b01, // don't run compactids on this table
    SKIP = 0b10,       // ignore this table, but don't warn about it not being handled
    WARN = 0b100,      // this is a somewhat unknown table, warn about it
  };

  enum LinkFlag : int
  {
    SET_UNIQUELY,
  };

  struct RecipientIdentification
  {
    std::string uuid;
    std::string phone;
    std::string group_id;
  };

  struct TableConnection
  {
    std::string table;
    std::string column;
    std::string whereclause = std::string();
    std::string json_path = std::string();
    int flags = 0;
    unsigned int mindbvversion = 0;
    unsigned int maxdbvversion = std::numeric_limits<unsigned int>::max();
  };

  struct DatabaseLink
  {
    std::string table;
    std::string column;
    std::vector<TableConnection> const connections;
    int flags;
  };

  static std::vector<DatabaseLink> const d_databaselinks;

  struct AttachmentMetadata
  {
    int width;
    int height;
    std::string filetype;
    unsigned long filesize;
    std::string hash;
    std::string filename;
    operator bool() const { return (width != -1 && height != -1 && !filetype.empty() && filesize != 0); }
  };

  struct RecipientInfo
  {
    std::string display_name;
    std::string initial;
    std::string uuid;
    std::string phone;
    std::string username;
    std::string color; // "RRGGBB"
    bool hasavatar;
  };

  // data that defines ranges in a message body to be replaced for html output
  struct Range
  {
    long long int start;
    long long int length;
    std::string pre;
    std::string replacement;
    std::string post;
    bool operator<(Range const &other) const
    {
      return (start < other.start) ||
        (start == other.start && start + length < other.start + other.length) ||
        (start == other.start && start + length == other.start + other.length && replacement < other.replacement);

      //return std::tie(start, start + length, replacement) <
      //  std::tie(other.start, other.start + other.length, other.replacement);

    };
  };

  static char const *const s_emoji_unicode_list[3655];
  static std::unordered_set<char> const s_emoji_first_bytes;
  static unsigned int constexpr s_emoji_min_size = 2; // smallest emoji_unicode_size - 1
  static std::map<std::string, std::string> const s_html_colormap;

 public:
  inline SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                      bool showprogress, bool replaceattachments);
  inline SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                      bool showprogress, bool replaceattachment, bool assumebadframesizeonbadmac,
                      std::vector<long long int> editattachments, bool stoponerror);
  [[nodiscard]] inline bool exportBackup(std::string const &filename, std::string const &passphrase,
                                         bool overwrite, bool keepattachmentdatainmemory, bool onlydb = false);
  bool exportXml(std::string const &filename, bool overwrite, std::string self, bool includemms = false, bool keepattachmentdatainmemory = true) const;
  void exportCsv(std::string const &filename, std::string const &table) const;
  void listThreads() const;
  void cropToThread(long long int threadid);
  void cropToThread(std::vector<long long int> const &threadid);
  void cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges);
  inline void addSMSMessage(std::string const &body, std::string const &address, std::string const &timestamp,
                            long long int thread, bool incoming);
  void addSMSMessage(std::string const &body, std::string const &address, long long int timestamp,
                     long long int thread, bool incoming);
  bool importThread(SignalBackup *source, long long int thread);
  //bool importThread(SignalBackup *source, std::vector<long long int> const &threads);
  inline bool ok() const;
  bool dropBadFrames();
  void fillThreadTableFromMessages();
  inline void addEndFrame();
  void mergeRecipients(std::vector<std::string> const &addresses, bool editmembers);
  void mergeGroups(std::vector<std::string> const &groups);
  inline void runQuery(std::string const &q, bool pretty = true) const;
  void removeDoubles();
  inline std::vector<long long int> threadIds() const;
  bool importCSV(std::string const &file, std::map<std::string, std::string> const &fieldmap);
  bool importWAChat(std::string const &file, std::string const &fmt, std::string const &self = std::string());
  bool summarize() const;
  bool reorderMmsSmsIds() const;
  bool dumpMedia(std::string const &dir, std::vector<long long int> const &threads, bool overwrite) const;
  bool dumpAvatars(std::string const &dir, std::vector<std::string> const &contacts, bool overwrite) const;
  bool deleteAttachments(std::vector<long long int> const &threadids, std::string const &before,
                         std::string const &after, long long int filesize,
                         std::vector<std::string> const &mimetypes, std::string const &append,
                         std::string const &prepend, std::vector<std::pair<std::string, std::string>> replace);
  inline void showDBInfo() const;
  bool scramble() const;
  std::pair<std::string, std::string> getDesktopDir() const;
  bool importFromDesktop(std::string configdir, std::string appdir, long long int dbversion,
                         std::vector<std::string> const &dateranges, bool createmissingcontacts,
                         bool autodates, bool ignorewal);
  bool checkDbIntegrity(bool warn = false) const;
  bool exportHtml(std::string const &directory, std::vector<long long int> const &threads,
                  std::vector<std::string> const &dateranges, long long int split, std::string const &selfid,
                  bool migrate, bool overwrite, bool append) const;

  /* CUSTOMS */
  bool hhenkel(std::string const &);
  bool sleepyh34d(std::string const &truncatedbackup, std::string const &pwd);
  bool hiperfall(uint64_t t_id, std::string const &selfid);
  void scanMissingAttachments() const;
  void devCustom() const;
  bool carowit(std::string const &sourcefile, std::string const &sourcepw) const;
  /* CUSTOMS */

 private:
  [[nodiscard]] bool exportBackupToFile(std::string const &filename, std::string const &passphrase,
                                        bool overwrite, bool keepattachmentdatainmemory);
  [[nodiscard]] bool exportBackupToDir(std::string const &directory, bool overwrite, bool keepattachmentdatainmemory, bool onlydb);
  void initFromFile();
  void initFromDir(std::string const &inputdir, bool replaceattachments);
  void updateThreadsEntries(long long int thread = -1);
  long long int getMaxUsedId(std::string const &table, std::string const &col = "_id") const;
  long long int getMinUsedId(std::string const &table, std::string const &col = "_id") const;
  template <typename T>
  [[nodiscard]] inline bool writeRawFrameDataToFile(std::string const &outputfile, T *frame) const;
  template <typename T>
  [[nodiscard]] inline bool writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const;
  [[nodiscard]] inline bool writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const;
  [[nodiscard]] bool writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame);
  [[nodiscard]] bool writeEncryptedFrameWithoutAttachment(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> framedata);
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::string> const &headers,
                                           std::vector<std::any> const &result) const;
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::any> const &result) const;
  template <typename T>
  inline bool setFrameFromFile(std::unique_ptr<T> *frame, std::string const &file, bool quiet = false) const;
  template <typename T>
  inline bool setFrameFromStrings(std::unique_ptr<T> *frame, std::vector<std::string> const &lines) const;
  template <typename T> inline std::pair<unsigned char*, size_t> numToData(T num) const;
  void setMinimumId(std::string const &table, long long int offset, std::string const &col = "_id") const;
  void cleanDatabaseByMessages();
  void getGroupV1MigrationRecipients(std::set<long long int> *referenced_recipients, long long int = -1) const;
  void remapRecipients();
  void compactIds(std::string const &table, std::string const &col = "_id");
  // void makeIdsUnique(long long int minthread, long long int minsms, long long int minmms,
  //                    long long int minpart, long long int minrecipient, long long int mingroups,
  //                    long long int minidentities, long long int mingroup_receipts, long long int mindrafts,
  //                    long long int minsticker, long long int minmegaphone, long long int minremapped_recipients,
  //                    long long int minremapped_threads, long long int minmention,
  //                    long long int minmsl_payload, long long int minmsl_message, long long int minmsl_recipient,
  //                    long long int minreaction, long long int mingroup_call_ring,
  //                    long long int minnotification_profile, long long int minnotification_profile_allowed_members,
  //                    long long int minnotification_profile_schedule);
  void makeIdsUnique(SignalBackup *source);
  void updateRecipientId(long long int targetid, RecipientIdentification const &ident);
  //void updateRecipientId(long long int targetid, std::string const &ident);
  void updateRecipientId(long long int targetid, long long int sourceid);
  void updateGroupMembers(long long int id1, long long int id2 = -1) const; // id2 == -1 -> id1 = offset, else transform 1 into 2
  void updateReactionAuthors(long long int id1, long long int id2 = -1) const; // idem.
  void updateGV1MigrationMessage(long long int id1, long long int id2 = -1) const; // idem.
  long long int dateToMSecsSinceEpoch(std::string const &date, bool *fromdatestring = nullptr) const;
  long long int getThreadIdFromRecipient(std::string const &recipient) const;
  void dumpInfoOnBadFrame(std::unique_ptr<BackupFrame> *frame);
  void dumpInfoOnBadFrames() const;
  void duplicateQuotes(std::string *s) const;
  std::string decodeStatusMessage(std::string const &body, long long int expiration, long long int type,
                                  std::string const &contactname) const;
  void escapeXmlString(std::string *s) const;
  void handleSms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self [[maybe_unused]], int i) const;
  void handleMms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self, int i, bool keepattachmentdatainmemory) const;
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
  long long int scanSelf() const;
  bool cleanAttachments();
  AttachmentMetadata getAttachmentMetaData(std::string const &filename) const;
  inline bool updatePartTableForReplace(AttachmentMetadata const &data, long long int id);
  bool scrambleHelper(std::string const &table, std::vector<std::string> const &columns) const;
  std::vector<long long int> getGroupUpdateRecipients(int thread = -1) const;
  bool getGroupMembersModern(std::vector<long long int> *members, std::string const &group_id) const;
  bool getGroupMembersOld(std::vector<long long int> *members, std::string const &group_id,
                          std::string const &column = "members") const;
  bool missingAttachmentExpected(uint64_t rowid, uint64_t unique_id) const;
  template <typename T>
  inline bool setFrameFromLine(std::unique_ptr<T> *newframe, std::string const &line) const;
  bool insertRow(std::string const &table, std::vector<std::pair<std::string, std::any>> data,
                 std::string const &returnfield = std::string(), std::any *returnvalue = nullptr) const;
  bool insertAttachments(long long int mms_id, long long int unique_id, int numattachments, long long int haspreviews,
                         long long int rowid, SqliteDB const &ddb, std::string const &where,
                         std::string const &databasedir, bool isquote);
  bool handleDTCallTypeMessage(SqliteDB const &ddb, long long int rowid, long long int ttid, long long int address) const;
  void handleDTGroupChangeMessage(SqliteDB const &ddb, long long int rowid, long long int thread_id) const;
  bool handleDTExpirationChangeMessage(SqliteDB const &ddb, long long int rowid, long long int ttid, long long int sent_at, long long int address) const;
  bool handleDTGroupV1Migration(SqliteDB const &ddb, long long int rowid, long long int thread_id, long long int timestamp,
                                long long int address, std::map<std::string, long long int> *savedmap);
  void getDTReactions(SqliteDB const &ddb, long long int rowid, long long int numreactions,
                      std::vector<std::vector<std::string>> *reactions) const;
  void insertReactions(long long int message_id, std::vector<std::vector<std::string>> const &reactions, bool mms,
                       std::map<std::string, long long int> *savedmap) const;
  long long int getRecipientIdFromUuid(std::string const &uuid, std::map<std::string, long long int> *savedmap) const;
  long long int getRecipientIdFromPhone(std::string const &phone, std::map<std::string, long long int> *savedmap) const;
  inline std::string getNameFromUuid(std::string const &uuid) const;
  std::string getNameFromRecipientId(long long int id) const;
  void dtSetMessageDeliveryReceipts(SqliteDB const &ddb, long long int rowid, std::map<std::string, long long int> *savedmap,
                                    std::string const &databasedir, bool createcontacts, long long int msg_id, bool is_mms, bool isgroup);
  bool HTMLwriteStart(std::ofstream &file, long long int thread_recipient_id, std::string const &directory,
                      std::string const &threaddir, bool isgroup, bool isnotetoself, std::set<long long int> const &recipients,
                      std::map<long long int, RecipientInfo> *recipientinfo,
                      std::map<long long int, std::string> *written_avatars, bool overwrite, bool append) const;
  void HTMLwriteAttachmentDiv(std::ofstream &htmloutput, SqliteDB::QueryResults const &attachment_results, int indent,
                              std::string const &directory, std::string const &threaddir, bool is_image_preview,
                              bool overwrite, bool append) const;
  bool HTMLwriteAttachment(std::string const &directory, std::string const &threaddir, long long int rowid,
                           long long int uniqueid, bool overwrite, bool append) const;
  std::set<long long int> getAllThreadRecipients(long long int t) const;
  void setRecipientInfo(std::set<long long int> const &recipients, std::map<long long int, RecipientInfo> *recipientinfo) const;
  bool HTMLprepMsgBody(std::string *body, std::vector<std::tuple<long long int, long long int, long long int>> const &mentions,
                       std::map<long long int, RecipientInfo> *recipients_info, bool incoming,
                       std::pair<std::shared_ptr<unsigned char []>, size_t> const &brdata, bool isquote) const;
  void prepRanges(std::vector<Range> *ranges) const;
  void applyRanges(std::string *body, std::vector<Range> *ranges, std::set<int> *positions_excluded_from_escape) const;
  std::vector<std::pair<unsigned int, unsigned int>> HTMLgetEmojiPos(std::string const &line) const;
  bool makeFilenameUnique(std::string const &path, std::string *file_or_dir) const;
  std::string decodeProfileChangeMessage(std::string const &body, std::string const &name) const;
  std::string HTMLwriteAvatar(long long int recipient_id, std::string const &directory, std::string const &threaddir,
                              bool overwrite, bool append) const;
  void HTMLwriteMessage(std::ofstream &filt, HTMLMessageInfo const &msginfo, std::map<long long int, RecipientInfo> *recipientinfo) const;
  void HTMLwriteIndex(std::vector<long long int> const &threads, std::string const &directory,
                      std::map<long long int, RecipientInfo> *recipientinfo, long long int notetoself_tid, bool overwrite, bool append) const;
  void HTMLescapeString(std::string *in, std::set<int> const *const positions_excluded_from_escape = nullptr) const;
  void HTMLescapeUrl(std::string *in) const;
  inline int numBytesInUtf16Substring(std::string const &text, unsigned int idx, int length) const;
  inline int utf16CharSize(std::string const &body, int idx) const;
  inline int bytesToUtf8CharSize(std::string const &body, int idx, int length = 1) const;
  inline int utf8CharsToByteSize() const;
  inline std::string utf8BytesToHexString(unsigned char const *const data, size_t data_size) const;
  inline std::string utf8BytesToHexString(std::shared_ptr<unsigned char[]> const &data, size_t data_size) const;
  inline std::string utf8BytesToHexString(std::string const &data) const;
  RecipientInfo const &getRecipientInfoFromMap(std::map<long long int, RecipientInfo> *recipient_info, long long int rid) const;
  bool migrateDatabase(int from, int to) const;
  long long int dtCreateRecipient(SqliteDB const &ddb, std::string const &id, std::string const &phone, std::string const &databasedir,
                                  std::map<std::string, long long int> *recipient_info);
};

inline SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase,
                                  bool verbose, bool showprogress, bool replaceattachments)
  :
  SignalBackup(filename, passphrase, verbose, showprogress, replaceattachments, false, std::vector<long long int>(), false)
{}

inline SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                                  bool showprogress, bool replaceattachments, bool assumebadframesizeonbadmac,
                                  std::vector<long long int> editattachments, bool stoponerror)
  :
  d_database(":memory:"),
  d_passphrase(passphrase),
  d_found_sqlite_sequence_in_backup(false),
  d_ok(false),
  d_databaseversion(-1),
  d_showprogress(showprogress),
  d_stoponerror(stoponerror),
  d_verbose(verbose)
{
  if (bepaald::isDir(filename))
    initFromDir(filename, replaceattachments);
  else // not directory
  {
    d_fd.reset(new FileDecryptor(filename, passphrase, d_verbose, stoponerror, assumebadframesizeonbadmac, editattachments));
    initFromFile();
  }

  if (d_ok) // set by initfrom()
    d_ok = setColumnNames();

  checkDbIntegrity(true);
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

template <typename T>
inline bool SignalBackup::setFrameFromLine(std::unique_ptr<T> *newframe, std::string const &line) const
{
  std::string::size_type pos = line.find(":", 0);
  if (pos == std::string::npos)
  {
    std::cout << "Failed to read frame data line '" << line << "'" << std::endl;
    return false;
  }
  unsigned int field = (*newframe)->getField(line.substr(0, pos));
  if (!field)
  {
    std::cout << "Failed to get field number" << std::endl;
    return false;
  }

  ++pos;
  std::string::size_type pos2 = line.find(":", pos);
  if (pos2 == std::string::npos)
  {
    std::cout << "Failed to read frame data from line '" << line << "'" << std::endl;
    return false;
  }
  std::string type = line.substr(pos, pos2 - pos);
  std::string datastr = line.substr(pos2 + 1);

  if (type == "bytes")
  {
    std::pair<unsigned char *, size_t> decdata = Base64::base64StringToBytes(datastr);
    if (!decdata.first)
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "uint64" || type == "uint32") // Note stoul and stoull are the same on linux. Internally 8 byte int are needed anyway.
  {                                              // (on windows stoul would be four bytes and the above if-clause would cause bad data
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(datastr)));
    if (!decdata.first)
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "int64" || type == "int32") // Note stol and stoll are the same on linux. Internally 8 byte int are needed anyway.
  {                                            // (on windows stol would be four bytes and the above if-clause would cause bad data
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoll(datastr)));
    if (!decdata.first)
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "float") // due to possible precision problems, the 4 bytes of float are saved in binary format (base64 encoded)
  {                         // WARNING untested
    std::pair<unsigned char *, size_t> decfloat = Base64::base64StringToBytes(datastr);
    if (!decfloat.first || decfloat.second != 4)
      return false;
    (*newframe)->setNewData(field, decfloat.first, decfloat.second);
  }
  else if (type == "bool") // since booleans are stored as varints, this is identical to uint64/32 code
  {
    std::string val = (datastr == "true") ? "1" : "0";
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(val)));
    if (!decdata.first)
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "string")
  {
    unsigned char *data = new unsigned char[datastr.size()];
    std::memcpy(data, datastr.data(), datastr.size());
    (*newframe)->setNewData(field, data, datastr.size());
  }
  else
    return false;
  return true;
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
    if (!setFrameFromLine(&newframe, line))
      return false;
  frame->reset(newframe.release());

  return true;
}

template <typename T>
inline bool SignalBackup::setFrameFromStrings(std::unique_ptr<T> *frame, std::vector<std::string> const &lines) const
{
  std::unique_ptr<T> newframe(new T);
  for (auto const &l : lines)
    if (!setFrameFromLine(&newframe, l))
      return false;
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

inline void SignalBackup::addSMSMessage(std::string const &body, std::string const &address, std::string const &timestamp, long long int thread, bool incoming)
{
  addSMSMessage(body, address, dateToMSecsSinceEpoch(timestamp), thread, incoming);
}

inline std::vector<long long int> SignalBackup::threadIds() const
{
  std::vector<long long int> res;
  SqliteDB::QueryResults results;
  d_database.exec("SELECT DISTINCT _id FROM thread ORDER BY _id ASC", &results);
  if (results.columns() == 1)
    for (uint i = 0; i < results.rows(); ++i)
      if (results.valueHasType<long long int>(i, 0))
        res.push_back(results.getValueAs<long long int>(i, 0));
  return res;
}

inline void SignalBackup::showDBInfo() const
{
  std::cout << "Database version: " << d_databaseversion << std::endl;
  d_database.print("SELECT m.name as TABLE_NAME, p.name as COLUMN_NAME FROM sqlite_master m LEFT OUTER JOIN pragma_table_info((m.name)) p ON m.name <> p.name ORDER BY TABLE_NAME, COLUMN_NAME");
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

inline bool SignalBackup::updatePartTableForReplace(AttachmentMetadata const &data, long long int id)
{
  if (!d_database.exec("UPDATE part SET ct = ?, data_size = ?, width = ?, height = ?, data_hash = ?, "
                       "video_gif = 0, transform_properties = NULL, voice_note = 0, blur_hash = NULL WHERE _id = ?",
                       {data.filetype, data.filesize, data.width, data.height, data.hash, id}) ||
      d_database.changed() != 1)
    return false;
  return true;
}

inline std::string SignalBackup::getNameFromUuid(std::string const &uuid) const
{
  return getNameFromRecipientId(getRecipientIdFromUuid(uuid, nullptr));
}

inline int SignalBackup::utf16CharSize(std::string const &body, int idx) const
{
  // get code point
  uint32_t codepoint = 0;
  if ((static_cast<uint8_t>(body[idx]) & 0b11111000) == 0b11110000) // 4 byte char
    /*
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00000111) << 18 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111) << 12 |
      (static_cast<uint8_t>(body[idx + 2]) & 0b00111111) << 6 |
      (static_cast<uint8_t>(body[idx + 3]) & 0b00111111);
    */
    return 2; // all 4 byte utf8 chars are 2 bytes in utf16
  else if ((static_cast<uint8_t>(body[idx]) & 0b11110000) == 0b11100000) // 3 byte char
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00001111) << 12 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111) << 6 |
      (static_cast<uint8_t>(body[idx + 2]) & 0b00111111);
  /*
  else if ((static_cast<uint8_t>(body[idx]) & 0b11100000) == 0b11000000) // 2 byte char
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00011111) << 6 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111);
  else
    codepoint = static_cast<uint8_t>(body[idx]);
  */
  else // all 1 and two byte utf-8 chars are 1 utf-16 char (max is 0b11111111111 which < 0x10000)
    return 1;

  return codepoint >= 0x10000 ? 2 : 1;
}

inline int SignalBackup::numBytesInUtf16Substring(std::string const &text, unsigned int idx, int length) const
{
  int utf16count = 0;
  int bytecount = 0;

  while (utf16count < length && idx < text.size())
  {
    utf16count += utf16CharSize(text, idx);
    int utf8size = bytesToUtf8CharSize(text, idx);
    bytecount += utf8size;
    idx += utf8size;
  }
  return bytecount;
}

inline int SignalBackup::bytesToUtf8CharSize(std::string const &body, int idx, int length) const
{
  int ret = 0;
  for (int i = idx; i < idx + length; ++i)
  {
    if ((static_cast<uint8_t>(body[i]) & 0b11111000) == 0b11110000) // 4 byte char
      ret += 4;
    else if ((static_cast<uint8_t>(body[i]) & 0b11110000) == 0b11100000) // 3 byte char
      ret += 3;
    else if ((static_cast<uint8_t>(body[i]) & 0b11100000) == 0b11000000) // 2 byte char
      ret += 2;
    else
      ret += 1;
  }
  return ret;
}

inline int SignalBackup::utf8CharsToByteSize() const
{
  return 0;
}

inline std::string SignalBackup::utf8BytesToHexString(unsigned char const *const data, size_t data_size) const
{
  // NOTE THIS IS NOT GENERIC UTF-8 CONVERSION, THIS
  // DATA IS GUARANTEED TO HAVE ONLY SINGLE- AND TWO-BYTE
  // CHARS (NO 3 OR 4-BYTE). THE TWO-BYTE CHARS NEVER
  // CONTAIN MORE THAN TWO BITS OF DATA
  unsigned char output[16]{0};
  uint outputpos = 0;
  for (uint i = 0; i < data_size; ++i)
  {
    if (outputpos >= 16) [[unlikely]]
      return std::string();

    if ((data[i] & 0b10000000) == 0) // single byte char
      output[outputpos++] += data[i];
    else // 2 byte char
      output[outputpos++] = ((data[i] & 0b00000011) << 6) | (data[i + 1] & 0b00111111), ++i;
  }
  return bepaald::bytesToHexString(output, 16, true);
}

inline std::string SignalBackup::utf8BytesToHexString(std::shared_ptr<unsigned char[]> const &data, size_t data_size) const
{
  return utf8BytesToHexString(data.get(), data_size);
}

inline std::string SignalBackup::utf8BytesToHexString(std::string const &data) const
{
  return utf8BytesToHexString(reinterpret_cast<unsigned char const *>(data.data()), data.size());
}

#endif
