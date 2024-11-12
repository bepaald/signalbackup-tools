/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#include "../memsqlitedb/memsqlitedb.h"
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
#include "../logger/logger.h"
#include "../deepcopyinguniqueptr/deepcopyinguniqueptr.h"
#include "../groupv2statusmessageproto/groupv2statusmessageproto.h"
#include "../attachmentmetadata/attachmentmetadata.h"

#include "../common_bytes.h"

#include <map>
#include <set>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <regex>

struct HTMLMessageInfo;
struct Range;
struct GroupInfo;
enum class IconType;
class JsonDatabase;
class DesktopDatabase;
class SignalPlaintextBackupDatabase;

class SignalBackup
{
 public:
  static bool constexpr DROPATTACHMENTDATA = false;

 protected:
  MemSqliteDB d_database;
  DeepCopyingUniquePtr<FileDecryptor> d_fd;
  FileEncryptor d_fe;
  std::string d_filename;
  std::string d_passphrase;

  // only used in testing
  bool d_found_sqlite_sequence_in_backup;

  // table/column names
  std::string d_mms_table;
  std::string d_part_table;
  std::string d_thread_recipient_id;
  std::string d_thread_message_count;
  std::string d_thread_delivery_receipts;
  std::string d_thread_read_receipts;
  std::string d_sms_date_received;
  std::string d_sms_recipient_id;
  std::string d_sms_recipient_device_id;
  std::string d_mms_date_sent;
  std::string d_mms_ranges;
  std::string d_mms_recipient_id;
  std::string d_mms_recipient_device_id;
  std::string d_mms_type;
  std::string d_mms_previews;
  std::string d_mms_delivery_receipts;
  std::string d_mms_read_receipts;
  std::string d_mms_viewed_receipts;
  std::string d_recipient_aci;
  std::string d_recipient_e164;
  std::string d_recipient_avatar_color;
  std::string d_recipient_system_joined_name;
  std::string d_recipient_profile_given_name;
  std::string d_recipient_storage_service;
  std::string d_recipient_type;
  std::string d_recipient_profile_avatar;
  std::string d_recipient_sealed_sender;
  std::string d_groups_v1_members;
  std::string d_part_mid;
  std::string d_part_ct;
  std::string d_part_pending;
  std::string d_part_cd;
  std::string d_part_cl;

  // table/column names for desktop db
  std::string d_dt_c_uuid;
  std::string d_dt_m_sourceuuid;

  std::vector<std::pair<std::string, DeepCopyingUniquePtr<AvatarFrame>>> d_avatars;
  std::map<std::pair<uint64_t, int64_t>, DeepCopyingUniquePtr<AttachmentFrame>> d_attachments; //maps <rowid,uniqueid> to attachment
  std::map<uint64_t, DeepCopyingUniquePtr<StickerFrame>> d_stickers; //maps <sticker._id> to sticker
  DeepCopyingUniquePtr<HeaderFrame> d_headerframe;
  DeepCopyingUniquePtr<DatabaseVersionFrame> d_databaseversionframe;
  std::vector<DeepCopyingUniquePtr<SharedPrefFrame>> d_sharedpreferenceframes;
  std::vector<DeepCopyingUniquePtr<KeyValueFrame>> d_keyvalueframes;
  DeepCopyingUniquePtr<EndFrame> d_endframe;
  std::vector<std::pair<uint32_t, uint64_t>> d_badattachments;
  bool d_ok;
  unsigned int d_databaseversion;
  unsigned int d_backupfileversion;
  bool d_showprogress;
  bool d_stoponerror;
  bool d_verbose;
  bool d_truncate;
  bool d_fulldecode;
  std::set<std::string> d_warningsgiven;
  long long int d_selfid;
  std::string d_selfuuid;

  enum DBLinkFlag : int
  {
    NO_COMPACT = 0b01, // don't run compactids on this table
    SKIP = 0b10,       // ignore this table, but don't warn about it not being handled
    WARN = 0b100,      // this is a somewhat unknown table, warn about it
  };

  enum LinkFlag : int
  {
    SET_UNIQUELY = (1 << 0),
    //NEW_FLAG = (1 << 1),
    //ANOTHER_FLAG = (1 << 2),
  };

  struct RecipientIdentification
  {
    std::string uuid;
    std::string phone;
    std::string group_id;
    std::string distribution_id;
    //long long int group_type; //    NONE(0), MMS(1), SIGNAL_GV1(2), SIGNAL_GV2(3), DISTRIBUTION_LIST(4), CALL_LINK(5);
    std::string storage_service;
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

  struct RecipientInfo
  {
    std::string display_name;
    std::string initial;
    bool initial_is_emoji;
    std::string uuid;
    std::string phone;
    std::string username;
    long long int mute_until;           // -1 : n/a
    long long int mention_setting;      // -1 : n/a
    long long int custom_notifications; // -1 : n/a
    std::string color; // "RRGGBB"
    std::string wall_light;
    std::string wall_dark;
    bool hasavatar;
  };

  static std::vector<DatabaseLink> const s_databaselinks;
  static std::map<std::string, std::vector<std::vector<std::string>>> const s_columnaliases;
  static char const *const s_emoji_unicode_list[3781];
  static std::unordered_set<char> const s_emoji_first_bytes;
  static unsigned int constexpr s_emoji_min_size = 2; // smallest emoji_unicode_size - 1
  static std::map<std::string, std::string> const s_html_colormap;
  static std::regex const s_linkify_pattern;

 protected:
  inline SignalBackup(bool verbose, bool truncate, bool showprogress);
 public:
  inline SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                      bool truncate, bool showprogress, bool replaceattachments);
  SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
               bool truncate, bool showprogress, bool replaceattachment, bool assumebadframesizeonbadmac,
               std::vector<long long int> const &editattachments, bool stoponerror, bool fulldecode);
  inline SignalBackup(SignalBackup const &other) = default;
  inline SignalBackup &operator=(SignalBackup const &other) = default;
  inline SignalBackup(SignalBackup &&other) = default;
  inline SignalBackup &operator=(SignalBackup &&other) = default;
  [[nodiscard]] bool exportBackup(std::string const &filename, std::string const &passphrase,
                                  bool overwrite, bool keepattachmentdatainmemory, bool onlydb = false);
  bool exportXml(std::string const &filename, bool overwrite, std::string self, bool includemms = false, bool keepattachmentdatainmemory = true);
  bool exportCsv(std::string const &filename, std::string const &table, bool overwrite) const;
  void listThreads() const;
  void listRecipients() const;
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
  //void fillThreadTableFromMessages();
  inline void addEndFrame();
  bool mergeRecipients(std::vector<std::string> const &addresses);
  void mergeGroups(std::vector<std::string> const &groups);
  inline void runQuery(std::string const &q, std::string const &mode = std::string()) const;
  void removeDoubles(long long int milliseconds = 0);
  inline std::vector<long long int> threadIds() const;
  bool importCSV(std::string const &file, std::map<std::string, std::string> const &fieldmap);
  //bool importWAChat(std::string const &file, std::string const &fmt, std::string const &self = std::string());
  bool summarize() const;
  bool reorderMmsSmsIds() const;
  bool dumpMedia(std::string const &dir, std::vector<std::string> const &dateranges,
                 std::vector<long long int> const &threads, bool excludestickers, bool overwrite) const;
  bool dumpAvatars(std::string const &dir, std::vector<std::string> const &contacts, bool overwrite) const;
  bool deleteAttachments(std::vector<long long int> const &threadids, std::string const &before,
                         std::string const &after, long long int filesize,
                         std::vector<std::string> const &mimetypes, std::string const &append,
                         std::string const &prepend, std::vector<std::pair<std::string, std::string>> replace);
  inline void showDBInfo() const;
  bool scramble() const;
  //std::pair<std::string, std::string> getDesktopDir() const;
  bool importFromDesktop(std::unique_ptr<DesktopDatabase> const &dtdb, bool skipmessagereorder,
                         std::vector<std::string> const &dateranges, bool createmissingcontacts,
                         bool createcontacts_nowarn, bool autodates, bool importstickers,
                         std::string const &selfphone);
  bool importFromPlaintextBackup(std::unique_ptr<SignalPlaintextBackupDatabase> const &ptdb, bool skipmessagereorder,
                                 std::vector<std::pair<std::string, long long int>> const &initial_contactmap,
                                 std::vector<std::string> const &daterangelist, std::vector<std::string> const &chats,
                                 bool createmissingcontacts, bool markdelivered, bool markread, bool autodates,
                                 std::string const &selfphone);
  bool checkDbIntegrity(bool warn = false) const;
  bool exportHtml(std::string const &directory, std::vector<long long int> const &threads,
                  std::vector<std::string> const &dateranges, std::string const &splitby,
                  long long int split, std::string const &selfid, bool calllog, bool searchpage,
                  bool stickerpacks, bool migrate, bool overwrite, bool append, bool theme,
                  bool themeswitching, bool addexportdetails, bool blocked, bool fullcontacts,
                  bool settings, bool receipts, bool use_original_filenames, bool linkify,
                  bool chatfolders);
  bool exportTxt(std::string const &directory, std::vector<long long int> const &threads,
                 std::vector<std::string> const &dateranges, std::string const &selfid, bool migrate, bool overwrite);
  bool findRecipient(long long int id) const;
  long long int getRecipientIdFromName(std::string const &name, bool withthread) const;
  long long int getRecipientIdFromPhone(std::string const &phone, bool withthread) const;
  long long int getRecipientIdFromUsername(std::string const &phone, bool withthread) const;
  long long int getThreadIdFromRecipient(std::string const &recipient) const;
  long long int getThreadIdFromRecipient(long long int recipientid) const;
  bool importTelegramJson(std::string const &file, std::vector<long long int> const &chatselection,
                          std::vector<std::pair<std::string, long long int>> contactmap,
                          std::vector<std::string> const &inhibitmapping, bool prependforwarded,
                          bool skipmessagereorder, bool markdelivered, bool markread, std::string const &selfphone);

  /* CUSTOMS */
  //bool hhenkel(std::string const &);
  //bool sleepyh34d(std::string const &truncatedbackup, std::string const &pwd);
  //bool hiperfall(uint64_t t_id, std::string const &selfid);
  void scanMissingAttachments() const;
  //void devCustom() const;
  //bool carowit(std::string const &sourcefile, std::string const &sourcepw) const;
  bool custom_hugogithubs();

  // for bug 233/13034
  bool migrate_to_191();
  /* CUSTOMS */

 protected:
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
  [[nodiscard]] bool writeEncryptedFrameWithoutAttachment(std::ofstream &outputfile,
                                                          std::pair<std::shared_ptr<unsigned char[]>, uint64_t> framedata);
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::string> const &headers,
                                           std::vector<std::any> const &result) const;
  SqlStatementFrame buildSqlStatementFrame(std::string const &table, std::vector<std::any> const &result) const;
  template <typename T>
  inline bool setFrameFromFile(DeepCopyingUniquePtr<T> *frame, std::string const &file, bool quiet = false) const;
  template <typename T>
  inline bool setFrameFromStrings(DeepCopyingUniquePtr<T> *frame, std::vector<std::string> const &lines) const;
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
  void updateAvatars(long long int id1, long long int id2 = -1); // idem.
  void updateSnippetExtrasRecipient(long long int id1, long long int id2 = -1) const; // idem.
  long long int dateToMSecsSinceEpoch(std::string const &date, bool *fromdatestring = nullptr) const;
  void dumpInfoOnBadFrame(std::unique_ptr<BackupFrame> *frame);
  void dumpInfoOnBadFrames() const;
  void duplicateQuotes(std::string *s) const;
  std::string decodeStatusMessage(std::string const &body, long long int expiration, long long int type,
                                  std::string const &contactname, IconType *icon = nullptr) const;
  std::string decodeStatusMessage(std::pair<std::shared_ptr<unsigned char []>, size_t> const &body, long long int expiration,
                                  long long int type, std::string const &contactname, IconType *icon = nullptr) const;
  void escapeXmlString(std::string *s) const;
  bool unescapeXmlString(std::string *s) const;
  void handleSms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self [[maybe_unused]], int i) const;
  void handleMms(SqliteDB::QueryResults const &results, std::ofstream &outputfile, std::string const &self, int i, bool keepattachmentdatainmemory) const;
  inline std::string getStringOr(SqliteDB::QueryResults const &results, int i,
                          std::string const &columnname, std::string const &def = std::string()) const;
  inline long long int getIntOr(SqliteDB::QueryResults const &results, int i,
                         std::string const &columnname, long long int def) const;
  //  bool handleWAMessage(long long int thread_id, long long int time, std::string const &chatname, std::string const &author,
  //                     std::string const &message, std::string const &selfid, bool isgroup,
  //                     std::map<std::string, std::string> const &name_to_recipientid);
  bool setFileTimeStamp(std::string const &file, long long int time_usec) const;
  std::string sanitizeFilename(std::string const &filename) const;
  bool setColumnNames();
  void dtSetColumnNames(SqliteDB *ddb);
  long long int scanSelf() const;
  bool cleanAttachments();
  inline bool updatePartTableForReplace(AttachmentMetadata const &data, long long int id);
  bool scrambleHelper(std::string const &table, std::vector<std::string> const &columns) const;
  std::vector<long long int> getGroupUpdateRecipients(int thread = -1) const;
  void getGroupUpdateRecipientsFromGV2Context(DecryptedGroupV2Context const &sts2, std::set<std::string> *uuids) const;
  bool getGroupMembersModern(std::vector<long long int> *members, std::string const &group_id) const;
  bool getGroupMembersOld(std::vector<long long int> *members, std::string const &group_id,
                          std::string const &column = "members") const;
  bool missingAttachmentExpected(uint64_t rowid, int64_t unique_id) const;
  template <typename T>
  inline bool setFrameFromLine(DeepCopyingUniquePtr<T> *newframe, std::string const &line) const;
  bool insertRow(std::string const &table, std::vector<std::pair<std::string, std::any>> data,
                 std::string const &returnfield = std::string(), std::any *returnvalue = nullptr) const;
  bool updateRows(std::string const &table, std::vector<std::pair<std::string, std::any>> data,
                  std::vector<std::pair<std::string, std::any>> whereclause,
                  std::string const &returnfield = std::string(), std::any *returnvalue = nullptr) const;
  bool dtInsertAttachments(long long int mms_id, long long int unique_id, int numattachments, long long int haspreviews,
                           long long int rowid, SqliteDB const &ddb, std::string const &where,
                           std::string const &databasedir, bool isquote, bool issticker);
  bool handleDTCallTypeMessage(SqliteDB const &ddb, std::string const &callid, long long int rowid, long long int ttid, long long int address, bool insertincompletedataforexport) const;
  void handleDTGroupChangeMessage(SqliteDB const &ddb, long long int rowid, long long int thread_id, long long int address,
                                  long long int date, std::map<long long int, long long int> *adjusted_timestamps, std::map<std::string, long long int> *savedmap, bool istimermessage) const;
  bool handleDTExpirationChangeMessage(SqliteDB const &ddb, long long int rowid, long long int ttid, long long int sent_at, long long int address) const;
  bool handleDTGroupV1Migration(SqliteDB const &ddb, long long int rowid, long long int thread_id, long long int timestamp,
                                long long int address, std::map<std::string, long long int> *savedmap,
                                bool createcontacts, std::string const &databasedir, bool create_valid_contacts, bool *warn);
  void getDTReactions(SqliteDB const &ddb, long long int rowid, long long int numreactions,
                      std::vector<std::vector<std::string>> *reactions) const;
  void insertReactions(long long int message_id, std::vector<std::vector<std::string>> const &reactions, bool mms,
                       std::map<std::string, long long int> *savedmap) const;
  long long int getRecipientIdFromUuidMapped(std::string const &uuid, std::map<std::string, long long int> *savedmap,
                                             bool suppresswarning = false) const;
  long long int getRecipientIdFromPhoneMapped(std::string const &phone, std::map<std::string, long long int> *savedmap,
                                              bool suppresswarning = false) const;
  inline std::string getNameFromUuid(std::string const &uuid) const;
  std::string getNameFromRecipientId(long long int id) const;
  void dtSetMessageDeliveryReceipts(SqliteDB const &ddb, long long int rowid, std::map<std::string, long long int> *savedmap,
                                    std::string const &databasedir, bool createcontacts, long long int msg_id, bool is_mms, bool isgroup,
                                    bool create_valid_contacts, bool *warn);
  bool HTMLwriteStart(std::ofstream &file, long long int thread_recipient_id, std::string const &directory,
                      std::string const &threaddir, bool isgroup, bool isnotetoself, bool isreleasechannel,
                      std::set<long long int> const &recipients, std::map<long long int, RecipientInfo> *recipientinfo,
                      std::map<long long int, std::string> *written_avatars, bool overwrite, bool append,
                      bool light, bool themeswitching, bool searchpage, bool exportdetails) const;
  void HTMLwriteAttachmentDiv(std::ofstream &htmloutput, SqliteDB::QueryResults const &attachment_results, int indent,
                              std::string const &directory, std::string const &threaddir, bool use_original_filenames,
                              bool is_image_preview, bool overwrite, bool append) const;
  void HTMLwriteSharedContactDiv(std::ofstream &htmloutput, std::string const &shared_contact, int indent,
                                 std::string const &directory, std::string const &threaddir,
                                 bool overwrite, bool append) const;
  bool HTMLwriteAttachment(std::string const &directory, std::string const &threaddir, long long int rowid,
                           long long int uniqueid, std::string const &attachment_filename, bool overwrite, bool append) const;
  bool HTMLprepMsgBody(std::string *body, std::vector<std::tuple<long long int, long long int, long long int>> const &mentions,
                       std::map<long long int, RecipientInfo> *recipients_info, bool incoming,
                       std::pair<std::shared_ptr<unsigned char []>, size_t> const &brdata,
                       bool linkify, bool isquote) const;
  std::string HTMLwriteAvatar(long long int recipient_id, std::string const &directory, std::string const &threaddir,
                              bool overwrite, bool append) const;
  void HTMLwriteMessage(std::ofstream &filt, HTMLMessageInfo const &msginfo, std::map<long long int, RecipientInfo> *recipientinfo,
                        bool searchpage, bool writereceipts) const;
  void HTMLwriteRevision(long long int msg_id, std::ofstream &filt, HTMLMessageInfo const &parent_info,
                         std::map<long long int, RecipientInfo> *recipientinfo, bool linkify) const;
  void HTMLwriteMsgReceiptInfo(std::ofstream &htmloutput, std::map<long long int, RecipientInfo> *recipientinfo,
                               long long int message_id, bool isgroup, long long int read_count,
                               long long int delivered_count, long long int timestamp, int indent) const;

  inline bool HTMLwriteIndex(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                             std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid, bool calllog,
                             bool searchpage, bool stickerpacks, bool blocked, bool fullcontacts, bool settings,  bool overwrite,
                             bool append, bool light, bool themeswitching, std::string const &exportdetails,
                             std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders) const;
  inline bool HTMLwriteChatFolder(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                                  std::string const &basename,  std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid,
                                  bool calllog, bool searchpage, bool stickerpacks, bool blocked, bool fullcontacts, bool settings,  bool overwrite,
                                  bool append, bool light, bool themeswitching, std::string const &exportdetails, long long int chatfolderidx,
                                  std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders) const;

  bool HTMLwriteIndexImpl(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                          std::string const &basename,  std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid,
                          bool calllog, bool searchpage, bool stickerpacks, bool blocked, bool fullcontacts, bool settings,  bool overwrite,
                          bool append, bool light, bool themeswitching, std::string const &exportdetails, long long int chatfolderidx,
                          std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders) const;


  void HTMLwriteSearchpage(std::string const &dir, bool light, bool themeswitching) const;
  void HTMLwriteCallLog(std::vector<long long int> const &threads, std::string const &directory,
                        std::string const &datewhereclause, std::map<long long int, RecipientInfo> *recipientinfo,
                        long long int notetoself_tid, bool overwrite, bool append, bool light, bool themeswitching,
                        std::string const &exportdetails) const;
  bool HTMLwriteStickerpacks(std::string const &dir, bool overwrite, bool append, bool light, bool themeswitching,
                             std::string const &exportdetails) const;
  bool HTMLwriteBlockedlist(std::string const &dir, std::map<long long int, RecipientInfo> *recipientinfo,
                            bool overwrite, bool append, bool light, bool themeswitching, std::string const &exportdetails) const;
  bool HTMLwriteFullContacts(std::string const &dir, std::map<long long int, RecipientInfo> *recipientinfo,
                             bool overwrite, bool append, bool light, bool themeswitching, std::string const &exportdetails) const;
  bool HTMLwriteSettings(std::string const &dir, bool overwrite, bool append, bool light,
                         bool themeswitching, std::string const &exportdetails) const;
  void HTMLescapeString(std::string *in, std::set<int> const *const positions_excluded_from_escape = nullptr) const;
  std::string HTMLescapeString(std::string const &in) const;
  void HTMLescapeUrl(std::string *in) const;
  std::string HTMLescapeUrl(std::string const &in) const;
  void HTMLLinkify(std::string const &body, std::vector<Range> *ranges) const;
  std::set<long long int> getAllThreadRecipients(long long int t) const;
  void setRecipientInfo(std::set<long long int> const &recipients, std::map<long long int, RecipientInfo> *recipientinfo) const;
  std::string getAvatarExtension(long long int recipient_id) const;
  void prepRanges(std::vector<Range> *ranges) const;
  void applyRanges(std::string *body, std::vector<Range> *ranges, std::set<int> *positions_excluded_from_escape) const;
  std::vector<std::pair<unsigned int, unsigned int>> HTMLgetEmojiPos(std::string const &line) const;
  bool makeFilenameUnique(std::string const &path, std::string *file_or_dir) const;
  std::string decodeProfileChangeMessage(std::string const &body, std::string const &name) const;
  inline int numBytesInUtf16Substring(std::string const &text, unsigned int idx, int length) const;
  inline int utf16CharSize(std::string const &body, int idx) const;
  inline int utf8Chars(std::string const &body) const;
  inline void resizeToNUtf8Chars(std::string &body, unsigned long size) const;
  inline int bytesToUtf8CharSize(std::string const &body, int idx) const;
  std::string utf8BytesToHexString(unsigned char const *const data, size_t data_size) const;
  inline std::string utf8BytesToHexString(std::shared_ptr<unsigned char[]> const &data, size_t data_size) const;
  inline std::string utf8BytesToHexString(std::string const &data) const;
  RecipientInfo const &getRecipientInfoFromMap(std::map<long long int, RecipientInfo> *recipient_info, long long int rid) const;
  bool migrateDatabase(int from, int to) const;
  long long int dtCreateRecipient(SqliteDB const &ddb, std::string const &id, std::string const &phone, std::string const &gidb64,
                                  std::string const &databasedir, std::map<std::string, long long int> *recipient_info,
                                  bool create_valid_contacts, bool *warn);
  bool dtUpdateProfile(SqliteDB const &ddb, std::string const &dtid, long long int aid, std::string const &databasedir);
  bool dtSetAvatar(std::string const &avatarpath, std::string const &key, int64_t size, int version,
                   long long int rid, std::string const &databasedir);
  std::string dtSetSharedContactsJsonString(SqliteDB const &ddb, long long int rowid) const;
  void warnOnce(std::string const &msg, bool error = false);
  void getGroupInfo(long long int rid, GroupInfo *groupinfo) const;
  std::pair<std::string, std::string> getCustomColor(std::pair<std::shared_ptr<unsigned char []>, size_t> const &colorproto) const;
  std::string HTMLprepLinkPreviewDescription(std::string const &in) const;
  long long int getFreeDateForMessage(long long int targetdate, long long int thread_id, long long int from_recipient_id) const;
  inline void TXTaddReactions(SqliteDB::QueryResults const *const reaction_results, std::ofstream *out) const;
  void setLongMessageBody(std::string *body, SqliteDB::QueryResults *attachment_results) const;
  bool tgImportMessages(SqliteDB const &db, std::vector<std::pair<std::vector<std::string>, long long int>> const &contactmap,
                        std::string const &datapath, std::string const &threadname, long long int chat_idx,
                        bool prependforwarded, bool markdelivered, bool markread, bool isgroup);
  bool tgMapContacts(JsonDatabase const &jdb, std::string const &chatselection,
                     std::vector<std::pair<std::vector<std::string>, long long int>> *contactmap,
                     std::vector<std::string> const &inhibitmappping) const;
  std::string tgBuildBody(std::string const &bodyjson) const;
  bool tgSetBodyRanges(std::string const &bodyjson, long long int message_id);
  bool tgSetAttachment(SqliteDB::QueryResults const &message_data, std::string const &datapath,
                       long long int r, long long int new_msg_id);
  bool tgSetQuote(long long int quoted_message_id, long long int new_msg_id);
  bool dtImportStickerPacks(SqliteDB const &ddb, std::string const &databasedir);
  void dtImportLongText(std::string const &msgbody_full, long long int new_mms_id, long long int uniqueid);
  bool prepareOutputDirectory(std::string const &dir, bool overwrite, bool allowappend = false, bool append = false) const;

  std::string getTranslatedName(std::string const &table, std::string const &old_column_name) const;
  bool writeStickerToDisk(long long int id, std::string const &packid, std::string const &directory,
                          bool overwrite, bool append, std::string *extension) const;
  long long int getRecipientIdFromField(std::string const &field, std::string const &value, bool withthread) const;
  std::string unicodeToUtf8(uint32_t unicode) const;
  int utf16ToUnicodeCodepoint(uint16_t utf16, uint32_t *codepoint) const;
  std::string makePrintable(std::string const &in) const;
};

// ONLY FOR DUMMYBACKUP
inline SignalBackup::SignalBackup(bool verbose, bool truncate, bool showprogress)
  :
  d_found_sqlite_sequence_in_backup(false),
  d_ok(false),
  d_databaseversion(-1),
  d_backupfileversion(-1),
  d_showprogress(showprogress),
  d_stoponerror(false),
  d_verbose(verbose),
  d_truncate(truncate),
  d_fulldecode(false),
  d_selfid(-1)
{}

inline SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                                  bool truncate, bool showprogress, bool replaceattachments)
  :
  SignalBackup(filename, passphrase, verbose, truncate, showprogress, replaceattachments, false, std::vector<long long int>(), false, false)
{}

inline bool SignalBackup::ok() const
{
  return d_ok;
}

template <typename T>
inline bool SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, T *frame) const
{
  if (!frame)
  {
    Logger::error("Asked to write nullptr frame to disk");
    return false;
  }

  std::ofstream rawframefile(outputfile, std::ios_base::binary);
  if (!rawframefile.is_open())
  {
    Logger::error("Opening file for writing: ", outputfile);
    return false;
  }

  if (frame->frameType() == BackupFrame::FRAMETYPE::END)
    rawframefile << "END" << std::endl;
  else
  {
    std::string d = frame->getHumanData();
    rawframefile << d;
  }
  return rawframefile.good();
}

template <typename T>
inline bool SignalBackup::writeRawFrameDataToFile(std::string const &outputfile, std::unique_ptr<T> const &frame) const
{
  return writeRawFrameDataToFile(outputfile, frame.get());
}

bool SignalBackup::writeFrameDataToFile(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> const &data) const
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
inline bool SignalBackup::setFrameFromLine(DeepCopyingUniquePtr<T> *newframe, std::string const &line) const
{
  if (line.empty())
    return true;

  std::string::size_type pos = line.find(":", 0);
  if (pos == std::string::npos) [[unlikely]]
  {
    Logger::error("Failed to read frame data line '", line, "'");
    return false;
  }

  unsigned int field = (*newframe)->getField(std::string_view(line.data(), pos));
  if (!field) [[unlikely]]
  {
    Logger::error("Failed to get field number");
    return false;
  }

  ++pos;
  std::string::size_type pos2 = line.find(":", pos);
  if (pos2 == std::string::npos) [[unlikely]]
  {
    Logger::error("Failed to read frame data from line '", line, "'");
    return false;
  }

  std::string_view type(line.data() + pos, pos2 - pos);
  std::string_view datastr(line.data() + pos2 + 1);

  if (type == "uint64" || type == "uint32") // Note stoul and stoull are the same on linux. Internally 8 byte int are needed anyway.
  {
    // (on windows stoul would be four bytes and the above if-clause would cause bad data
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(bepaald::toNumber<unsigned long long>(datastr)));
    if (!decdata.first) [[unlikely]]
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "string")
  {
    unsigned char *data = new unsigned char[datastr.size()];
    std::memcpy(data, datastr.data(), datastr.size());
    (*newframe)->setNewData(field, data, datastr.size());
  }
  else if (type == "bool") // since booleans are stored as varints, this is identical to uint64/32 code
  {
    std::string val = (datastr == "true") ? "1" : "0";
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoull(val)));
    if (!decdata.first) [[unlikely]]
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "bytes")
  {
    std::pair<unsigned char *, size_t> decdata = Base64::base64StringToBytes(datastr);
    if (!decdata.first) [[unlikely]]
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "int64" || type == "int32") // Note stol and stoll are the same on linux. Internally 8 byte int are needed anyway.
  {
    // (on windows stol would be four bytes and the above if-clause would cause bad data
    std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(bepaald::toNumber<long long>(datastr)));
    if (!decdata.first) [[unlikely]]
      return false;
    (*newframe)->setNewData(field, decdata.first, decdata.second);
  }
  else if (type == "float") // due to possible precision problems, the 4 bytes of float are saved in binary format (base64 encoded)
  {                         // WARNING untested
    std::pair<unsigned char *, size_t> decfloat = Base64::base64StringToBytes(datastr);
    if (!decfloat.first) [[unlikely]]
      return false;
    if (decfloat.second != 4) [[unlikely]]
    {
      delete[] decfloat.first;
      return false;
    }
    (*newframe)->setNewData(field, decfloat.first, decfloat.second);
  }
  else
    return false;
  return true;
}

template <>
inline bool SignalBackup::setFrameFromFile(DeepCopyingUniquePtr<EndFrame> *frame, std::string const &file, bool quiet) const
{
  std::ifstream datastream(file, std::ios_base::binary | std::ios::in);
  if (!datastream.is_open())
  {
    if (!quiet)
      Logger::error("Failed to open '", file, "' for reading");
    return false;
  }
  frame->reset(new EndFrame(nullptr, 1ull));
  return true;
}

template <typename T>
inline bool SignalBackup::setFrameFromFile(DeepCopyingUniquePtr<T> *frame, std::string const &file, bool quiet) const
{
  std::ifstream datastream(file, std::ios_base::binary | std::ios::in);
  if (!datastream.is_open())
  {
    if (!quiet)
      Logger::error("Failed to open '", file, "' for reading");
    return false;
  }

  DeepCopyingUniquePtr<T> newframe(new T);
  std::string line;
  while (std::getline(datastream, line))
    if (!setFrameFromLine(&newframe, line))
      return false;
  frame->reset(newframe.release());

  return true;
}

template <typename T>
inline bool SignalBackup::setFrameFromStrings(DeepCopyingUniquePtr<T> *frame, std::vector<std::string> const &lines) const
{
  DeepCopyingUniquePtr<T> newframe(new T);
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

inline void SignalBackup::runQuery(std::string const &q, std::string const &mode) const
{
  Logger::message(" * Executing query: ", q);
  SqliteDB::QueryResults res;
  if (!d_database.exec(q, &res))
    return;

  std::string q_comm = q.substr(0, STRLEN("DELETE")); // delete, insert and update are same length...
  std::for_each(q_comm.begin(), q_comm.end(), [] (char &ch) { ch = std::toupper(ch); });

  if (q_comm == "DELETE" || q_comm == "INSERT" || q_comm == "UPDATE")
  {
    Logger::message("Modified ", d_database.changed(), " rows");
    if (res.rows() == 0 && res.columns() == 0)
      return;
  }

  if (mode == "pretty")
    res.prettyPrint(d_truncate);
  else if (mode == "line")
    res.printLineMode();
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
    for (unsigned int i = 0; i < results.rows(); ++i)
      if (results.valueHasType<long long int>(i, 0))
        res.push_back(results.getValueAs<long long int>(i, 0));
  return res;
}

inline void SignalBackup::showDBInfo() const
{
  Logger::message("Database version: ", d_databaseversion);
  d_database.print("SELECT m.name as TABLE_NAME, p.name as COLUMN_NAME FROM sqlite_master m LEFT OUTER JOIN pragma_table_info((m.name)) p ON m.name <> p.name ORDER BY TABLE_NAME, COLUMN_NAME");
}

inline std::string SignalBackup::getStringOr(SqliteDB::QueryResults const &results, int i, std::string const &columnname, std::string const &def) const
{
  std::string tmp(def);
  if (results.hasColumn(columnname))                       // to prevent warning in this case, we check the
    if (results.valueHasType<std::string>(i, columnname))  // column name. This function expect it may fail
    {
      tmp = results.getValueAs<std::string>(i, columnname);
      escapeXmlString(&tmp);
    }
  return tmp;
}

inline long long int SignalBackup::getIntOr(SqliteDB::QueryResults const &results, int i,
                                            std::string const &columnname, long long int def) const
{
  long long int tmp = def;
  if (results.hasColumn(columnname))                        // to prevent warning in this case, we check the
    if (results.valueHasType<long long int>(i, columnname)) // column name. This function expect it may fail
      tmp = results.getValueAs<long long int>(i, columnname);
  return tmp;
}

inline bool SignalBackup::updatePartTableForReplace(AttachmentMetadata const &data, long long int id)
{
  if (!updateRows(d_part_table,
                  {{d_part_ct, data.filetype},
                   {"data_size", data.filesize},
                   {"width", data.width},
                   {"height", data.height},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), data.hash},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), data.hash},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), data.hash}},
                  {{"_id", id}}) ||
      d_database.changed() != 1)
    return false;
  return true;
}

inline std::string SignalBackup::getNameFromUuid(std::string const &uuid) const
{
  return getNameFromRecipientId(getRecipientIdFromUuidMapped(uuid, nullptr));
}

inline bool SignalBackup::HTMLwriteIndex(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                                         std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid,
                                         bool calllog, bool searchpage, bool stickerpacks, bool blocked, bool fullcontacts, bool settings,  bool overwrite,
                                         bool append, bool light, bool themeswitching, std::string const &exportdetails,
                                         std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders) const
{
  return HTMLwriteIndexImpl(threads, maxtimestamp, directory, "index", recipient_info, note_to_self_tid, calllog, searchpage, stickerpacks, blocked,
                            fullcontacts, settings, overwrite, append, light, themeswitching, exportdetails, -1, chatfolders);
}

inline bool SignalBackup::HTMLwriteChatFolder(std::vector<long long int> const &threads, long long int maxtimestamp, std::string const &directory,
                                              std::string const &basename,  std::map<long long int, RecipientInfo> *recipient_info, long long int note_to_self_tid,
                                              bool calllog, bool searchpage, bool stickerpacks, bool blocked, bool fullcontacts, bool settings,  bool overwrite,
                                              bool append, bool light, bool themeswitching, std::string const &exportdetails, long long int chatfolderidx,
                                              std::vector<std::tuple<long long int, std::string, std::string>> const &chatfolders) const
{
  return HTMLwriteIndexImpl(threads, maxtimestamp, directory, basename, recipient_info, note_to_self_tid, calllog, searchpage, stickerpacks, blocked,
                            fullcontacts, settings, overwrite, append, light, themeswitching, exportdetails, chatfolderidx, chatfolders);
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

inline int SignalBackup::utf8Chars(std::string const &body) const
{
  int res = 0;
  for (unsigned int i = 0; i < body.size(); )
  {
    ++res;
    i += bytesToUtf8CharSize(body, i);
  }
  return res;
}

inline void SignalBackup::resizeToNUtf8Chars(std::string &body, unsigned long size) const
{
  unsigned long res = 0;
  unsigned int idx = 0;
  while (idx < body.size())
  {
    ++res;
    idx += bytesToUtf8CharSize(body, idx);
    if (res == size)
      break;
  }
  if (idx < body.size())
    body.resize(idx);
}

inline int SignalBackup::bytesToUtf8CharSize(std::string const &body, int idx) const
{
  if ((static_cast<uint8_t>(body[idx]) & 0b10000000) == 0b00000000)
    return 1;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11100000) == 0b11000000) // 2 byte char
    return 2;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11110000) == 0b11100000) // 3 byte char
    return 3;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11111000) == 0b11110000) // 4 byte char
    return 4;
  else
    return 1;
}

inline std::string SignalBackup::utf8BytesToHexString(std::shared_ptr<unsigned char[]> const &data, size_t data_size) const
{
  return utf8BytesToHexString(data.get(), data_size);
}

inline std::string SignalBackup::utf8BytesToHexString(std::string const &data) const
{
  return utf8BytesToHexString(reinterpret_cast<unsigned char const *>(data.data()), data.size());
}

inline void SignalBackup::TXTaddReactions(SqliteDB::QueryResults const *const reaction_results, std::ofstream *out) const
{
  if (reaction_results->rows() == 0) [[likely]]
    return;

  *out << " (";
  for (unsigned int r = 0; r < reaction_results->rows(); ++r)
  {
    std::string emojireaction = reaction_results->valueAsString(r, "emoji");
    std::string authordisplayname = getNameFromRecipientId(reaction_results->getValueAs<long long int>(r, "author_id"));

    *out << authordisplayname << ": " << emojireaction;
    if (r < reaction_results->rows() - 1)
      *out << "; ";
  }
  *out << ")";
}

#endif
