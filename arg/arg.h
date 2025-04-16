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

#ifndef ARGS_H_
#define ARGS_H_

#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <cstdlib>
#include <array>
#include <type_traits>
#include <algorithm>

class Arg
{
  std::array<std::string, 195> const d_alloptions{"--onlyolderthan", "--desktopdir", "--desktopdirs", "--rawdesktopdb", "--desktopkey", "--dumpmedia", "--dumpavatars", "--generatedummyfordesktop", "--importcsv", "--setselfid", "--generatedummy", "--setcountrycode", "--mapxmladdressesfromfile", "--mapxmlcontactnamesfromfile", "--onlynewerthan", "--appendbody", "--prependbody", "-l", "--logfile", "--exporthtml", "--exportdesktophtml", "--listjsonchats", "--importtelegram", "--split-by", "--exportdesktoptxt", "--exporttxt", "--dumpdesktopdb", "-s", "--source", "-op", "--opassphrase", "-o", "--output", "--exportxml", "--querymode", "-sp", "--sourcepassphrase", "-i", "--input", "-p", "--passphrase", "--replaceattachments", "--croptothreadsbyname", "--listxmlcontacts", "--croptothreads", "--mapxmlcontacts", "--onlytype", "--limittodates", "--selectxmlchats", "--exportplaintextbackuphtml", "--importplaintextbackup", "--preventjsonmapping", "--mapjsoncontacts", "--selectjsonchats", "--limittothreadsbyname", "--limittothreads", "--importthreadsbyname", "--importthreads", "--exportcsv", "--editattachmentsize", "--limitcontacts", "--rundtprettysqlquery", "--htmlignoremediatypes", "--rundtsqlquery", "--runprettysqlquery", "--runsqlquery", "--mapcsvfields", "--croptodates", "--mergegroups", "--mergerecipients", "--onlyinthreads", "--mapxmladdresses", "--setchatcolors", "--mapxmlcontactnames", "--onlylargerthan", "--desktopdbversion", "--split", "--findrecipient", "--hiperfall", "--removedoubles", "--htmlpagemenu", "--no-htmlpagemenu", "--append", "--no-append", "--migratedb", "--no-migratedb", "--linkify", "--no-linkify", "--importstickers", "--no-importstickers", "--targetisdummy", "--no-targetisdummy", "--skipmessagereorder", "--no-skipmessagereorder", "--jsonprependforward", "--no-jsonprependforward", "--jsonmarkdelivered", "--no-jsonmarkdelivered", "--jsonmarkread", "--no-jsonmarkread", "--xmlmarkdelivered", "--no-xmlmarkdelivered", "--compactfilenames", "--no-compactfilenames", "--migrate_to_191", "--no-migrate_to_191", "--xmlmarkread", "--no-xmlmarkread", "--fulldecode", "--no-fulldecode", "--xmlautogroupnames", "--no-xmlautogroupnames", "--custom_hugogithubs", "--no-custom_hugogithubs", "--truncate", "--no-truncate", "--reordermmssmsids", "--no-reordermmssmsids", "--importfromdesktop", "--no-importfromdesktop", "--scramble", "--no-scramble", "--showdbinfo", "--no-showdbinfo", "--scanmissingattachments", "--no-scanmissingattachments", "-h", "--help", "--no-help", "--deleteattachments", "--no-deleteattachments", "--dbusverbose", "--no-dbusverbose", "-v", "--verbose", "--no-verbose", "--stoponerror", "--no-stoponerror", "--autolimitdates", "--no-autolimitdates", "--showprogress", "--no-showprogress", "--listrecipients", "--no-listrecipients", "--listthreads", "--no-listthreads", "--overwrite", "--no-overwrite", "--onlydb", "--no-onlydb", "--devcustom", "--no-devcustom", "--excludestickers", "--no-excludestickers", "--showdesktopkey", "--no-showdesktopkey", "--assumebadframesizeonbadmac", "--no-assumebadframesizeonbadmac", "--includefullcontactlist", "--no-includefullcontactlist", "--importdesktopcontacts", "--no-importdesktopcontacts", "--addincompletedataforhtmlexport", "--no-addincompletedataforhtmlexport", "--originalfilenames", "--no-originalfilenames", "--chatfolders", "--no-chatfolders", "--includereceipts", "--no-includereceipts", "--stickerpacks", "--no-stickerpacks", "--searchpage", "--no-searchpage", "--themeswitching", "--no-themeswitching", "--light", "--no-light", "--includesettings", "--no-includesettings", "--includeblockedlist", "--no-includeblockedlist", "--includecalllog", "--no-includecalllog", "--addexportdetails", "--no-addexportdetails", "--interactive", "--no-interactive", "--checkdbintegrity", "--no-checkdbintegrity", "--includemms", "--no-includemms", "--ignorewal", "--no-ignorewal", "--allhtmlpages"};
  size_t d_positionals;
  size_t d_maxpositional;
  std::string d_progname;
  std::string d_onlyolderthan;
  std::string d_desktopdir;
  std::string d_desktopdirs_1;
  std::string d_desktopdirs_2;
  std::string d_rawdesktopdb;
  std::string d_desktopkey;
  std::string d_dumpmedia;
  std::string d_dumpavatars;
  std::string d_generatedummyfordesktop;
  std::string d_importcsv;
  std::string d_setselfid;
  std::string d_generatedummy;
  std::string d_setcountrycode;
  std::string d_mapxmladdressesfromfile;
  std::string d_mapxmlcontactnamesfromfile;
  std::string d_onlynewerthan;
  std::string d_appendbody;
  std::string d_prependbody;
  std::string d_logfile;
  std::string d_exporthtml;
  std::string d_exportdesktophtml;
  std::string d_listjsonchats;
  std::string d_importtelegram;
  std::string d_split_by;
  std::string d_exportdesktoptxt;
  std::string d_exporttxt;
  std::string d_dumpdesktopdb;
  std::string d_source;
  std::string d_opassphrase;
  std::string d_output;
  std::string d_exportxml;
  std::string d_querymode;
  std::string d_sourcepassphrase;
  std::string d_input;
  std::string d_passphrase;
  std::vector<std::pair<std::string,std::string>> d_replaceattachments;
  std::vector<std::string> d_croptothreadsbyname;
  std::vector<std::string> d_listxmlcontacts;
  std::vector<long long int> d_croptothreads;
  std::vector<std::pair<std::string,long long int>> d_mapxmlcontacts;
  std::vector<std::string> d_onlytype;
  std::vector<std::string> d_limittodates;
  std::vector<std::string> d_selectxmlchats;
  std::vector<std::string> d_exportplaintextbackuphtml;
  std::vector<std::string> d_importplaintextbackup;
  std::vector<std::string> d_preventjsonmapping;
  std::vector<std::pair<std::string, long long int>> d_mapjsoncontacts;
  std::vector<long long int> d_selectjsonchats;
  std::vector<std::string> d_limittothreadsbyname;
  std::vector<long long int> d_limittothreads;
  std::vector<std::string> d_importthreadsbyname;
  std::vector<long long int> d_importthreads;
  std::vector<std::pair<std::string,std::string>> d_exportcsv;
  std::vector<long long int> d_editattachmentsize;
  std::vector<std::string> d_limitcontacts;
  std::vector<std::string> d_rundtprettysqlquery;
  std::vector<std::string> d_htmlignoremediatypes;
  std::vector<std::string> d_rundtsqlquery;
  std::vector<std::string> d_runprettysqlquery;
  std::vector<std::string> d_runsqlquery;
  std::vector<std::pair<std::string,std::string>> d_mapcsvfields;
  std::vector<std::string> d_croptodates;
  std::vector<std::string> d_mergegroups;
  std::vector<std::string> d_mergerecipients;
  std::vector<long long int> d_onlyinthreads;
  std::vector<std::pair<std::string, std::string>> d_mapxmladdresses;
  std::vector<std::pair<long long int, std::string>> d_setchatcolors;
  std::vector<std::pair<std::string, std::string>> d_mapxmlcontactnames;
  long long int d_onlylargerthan;
  long long int d_desktopdbversion;
  long long int d_split;
  long long int d_findrecipient;
  long long int d_hiperfall;
  int d_removedoubles;
  bool d_htmlpagemenu;
  bool d_append;
  bool d_migratedb;
  bool d_linkify;
  bool d_importstickers;
  bool d_targetisdummy;
  bool d_skipmessagereorder;
  bool d_jsonprependforward;
  bool d_jsonmarkdelivered;
  bool d_jsonmarkread;
  bool d_xmlmarkdelivered;
  bool d_compactfilenames;
  bool d_migrate_to_191;
  bool d_xmlmarkread;
  bool d_fulldecode;
  bool d_xmlautogroupnames;
  bool d_custom_hugogithubs;
  bool d_truncate;
  bool d_reordermmssmsids;
  bool d_importfromdesktop;
  bool d_scramble;
  bool d_showdbinfo;
  bool d_scanmissingattachments;
  bool d_help;
  bool d_deleteattachments;
  bool d_dbusverbose;
  bool d_verbose;
  bool d_stoponerror;
  bool d_autolimitdates;
  bool d_showprogress;
  bool d_listrecipients;
  bool d_listthreads;
  bool d_overwrite;
  bool d_onlydb;
  bool d_devcustom;
  bool d_excludestickers;
  bool d_showdesktopkey;
  bool d_assumebadframesizeonbadmac;
  bool d_includefullcontactlist;
  bool d_importdesktopcontacts;
  bool d_addincompletedataforhtmlexport;
  bool d_originalfilenames;
  bool d_chatfolders;
  bool d_includereceipts;
  bool d_stickerpacks;
  bool d_searchpage;
  bool d_themeswitching;
  bool d_light;
  bool d_includesettings;
  bool d_includeblockedlist;
  bool d_includecalllog;
  bool d_addexportdetails;
  bool d_interactive;
  bool d_checkdbintegrity;
  bool d_includemms;
  bool d_ignorewal;
  bool d_input_required;
  bool d_replaceattachments_bool;
  bool d_split_bool;
  bool d_removedoubles_bool;
  bool d_ok;
 public:
  Arg(int argc, char *argv[]);
  inline Arg(Arg const &other) = delete;
  inline Arg &operator=(Arg const &other) = delete;
  inline bool ok() const;
  void usage() const;
  inline std::string const &onlyolderthan() const;
  inline std::string const &desktopdir() const;
  inline std::string const &desktopdirs_1() const;
  inline std::string const &desktopdirs_2() const;
  inline std::string const &rawdesktopdb() const;
  inline std::string const &desktopkey() const;
  inline std::string const &dumpmedia() const;
  inline std::string const &dumpavatars() const;
  inline std::string const &generatedummyfordesktop() const;
  inline std::string const &importcsv() const;
  inline std::string const &setselfid() const;
  inline std::string const &generatedummy() const;
  inline std::string const &setcountrycode() const;
  inline std::string const &mapxmladdressesfromfile() const;
  inline std::string const &mapxmlcontactnamesfromfile() const;
  inline std::string const &onlynewerthan() const;
  inline std::string const &appendbody() const;
  inline std::string const &prependbody() const;
  inline std::string const &logfile() const;
  inline std::string const &exporthtml() const;
  inline std::string const &exportdesktophtml() const;
  inline std::string const &listjsonchats() const;
  inline std::string const &importtelegram() const;
  inline std::string const &split_by() const;
  inline std::string const &exportdesktoptxt() const;
  inline std::string const &exporttxt() const;
  inline std::string const &dumpdesktopdb() const;
  inline std::string const &source() const;
  inline std::string const &opassphrase() const;
  inline void setopassphrase(std::string const &val);
  inline std::string const &output() const;
  inline std::string const &exportxml() const;
  inline std::string const &querymode() const;
  inline std::string const &sourcepassphrase() const;
  inline void setsourcepassphrase(std::string const &val);
  inline std::string const &input() const;
  inline std::string const &passphrase() const;
  inline void setpassphrase(std::string const &val);
  inline std::vector<std::pair<std::string,std::string>> const &replaceattachments() const;
  inline bool replaceattachments_bool() const;
  inline std::vector<std::string> const &croptothreadsbyname() const;
  inline std::vector<std::string> const &listxmlcontacts() const;
  inline std::vector<long long int> const &croptothreads() const;
  inline std::vector<std::pair<std::string,long long int>> const &mapxmlcontacts() const;
  inline std::vector<std::string> const &onlytype() const;
  inline std::vector<std::string> const &limittodates() const;
  inline std::vector<std::string> const &selectxmlchats() const;
  inline std::vector<std::string> const &exportplaintextbackuphtml() const;
  inline std::vector<std::string> const &importplaintextbackup() const;
  inline std::vector<std::string> const &preventjsonmapping() const;
  inline std::vector<std::pair<std::string, long long int>> const &mapjsoncontacts() const;
  inline std::vector<long long int> const &selectjsonchats() const;
  inline std::vector<std::string> const &limittothreadsbyname() const;
  inline std::vector<long long int> const &limittothreads() const;
  inline std::vector<std::string> const &importthreadsbyname() const;
  inline std::vector<long long int> const &importthreads() const;
  inline std::vector<std::pair<std::string,std::string>> const &exportcsv() const;
  inline std::vector<long long int> const &editattachmentsize() const;
  inline std::vector<std::string> const &limitcontacts() const;
  inline std::vector<std::string> const &rundtprettysqlquery() const;
  inline std::vector<std::string> const &htmlignoremediatypes() const;
  inline std::vector<std::string> const &rundtsqlquery() const;
  inline std::vector<std::string> const &runprettysqlquery() const;
  inline std::vector<std::string> const &runsqlquery() const;
  inline std::vector<std::pair<std::string,std::string>> const &mapcsvfields() const;
  inline std::vector<std::string> const &croptodates() const;
  inline std::vector<std::string> const &mergegroups() const;
  inline std::vector<std::string> const &mergerecipients() const;
  inline std::vector<long long int> const &onlyinthreads() const;
  inline std::vector<std::pair<std::string, std::string>> const &mapxmladdresses() const;
  inline std::vector<std::pair<long long int, std::string>> const &setchatcolors() const;
  inline std::vector<std::pair<std::string, std::string>> const &mapxmlcontactnames() const;
  inline long long int onlylargerthan() const;
  inline long long int desktopdbversion() const;
  inline long long int split() const;
  inline bool split_bool() const;
  inline long long int findrecipient() const;
  inline long long int hiperfall() const;
  inline int removedoubles() const;
  inline bool removedoubles_bool() const;
  inline bool htmlpagemenu() const;
  inline bool append() const;
  inline bool migratedb() const;
  inline bool linkify() const;
  inline bool importstickers() const;
  inline bool targetisdummy() const;
  inline bool skipmessagereorder() const;
  inline bool jsonprependforward() const;
  inline bool jsonmarkdelivered() const;
  inline bool jsonmarkread() const;
  inline bool xmlmarkdelivered() const;
  inline bool compactfilenames() const;
  inline bool migrate_to_191() const;
  inline bool xmlmarkread() const;
  inline bool fulldecode() const;
  inline bool xmlautogroupnames() const;
  inline bool custom_hugogithubs() const;
  inline bool truncate() const;
  inline bool reordermmssmsids() const;
  inline bool importfromdesktop() const;
  inline bool scramble() const;
  inline bool showdbinfo() const;
  inline bool scanmissingattachments() const;
  inline bool help() const;
  inline bool deleteattachments() const;
  inline bool dbusverbose() const;
  inline bool verbose() const;
  inline bool stoponerror() const;
  inline bool autolimitdates() const;
  inline bool showprogress() const;
  inline bool listrecipients() const;
  inline bool listthreads() const;
  inline bool overwrite() const;
  inline bool onlydb() const;
  inline bool devcustom() const;
  inline bool excludestickers() const;
  inline bool showdesktopkey() const;
  inline bool assumebadframesizeonbadmac() const;
  inline bool includefullcontactlist() const;
  inline bool importdesktopcontacts() const;
  inline bool addincompletedataforhtmlexport() const;
  inline bool originalfilenames() const;
  inline bool chatfolders() const;
  inline bool includereceipts() const;
  inline bool stickerpacks() const;
  inline bool searchpage() const;
  inline bool themeswitching() const;
  inline bool light() const;
  inline bool includesettings() const;
  inline bool includeblockedlist() const;
  inline bool includecalllog() const;
  inline bool addexportdetails() const;
  inline bool interactive() const;
  inline bool checkdbintegrity() const;
  inline bool includemms() const;
  inline bool ignorewal() const;
  inline bool input_required() const;
 private:
  template <typename T>
  bool ston(T *t, std::string const &str) const;
  bool parseArgs(std::vector<std::string> const &args);
  inline void parseStringList(std::string const &strlist, std::vector<std::string> *list) const;
  template <typename T, typename U>
  inline bool parsePairList(std::string const &pairlist, std::string const &delim, std::vector<std::pair<T, U>> *list, std::string *error) const;
  template <typename T>
  bool parseNumberList(std::string const &strlist, std::vector<T> *list, bool sort, std::vector<std::string> *faillist = nullptr) const;
  template <typename T, typename U>
  bool parsePair(std::string const &token, std::string const &delim, std::pair<T, U> *pair, std::string *error) const;
  template <typename T>
  bool parseNumberListToken(std::string const &token, std::vector<T> *list) const;
  inline bool isOption(std::string const &str) const;
};

inline std::string const &Arg::onlyolderthan() const
{
  return d_onlyolderthan;
}

inline std::string const &Arg::desktopdir() const
{
  return d_desktopdir;
}

inline std::string const &Arg::desktopdirs_1() const
{
  return d_desktopdirs_1;
}

inline std::string const &Arg::desktopdirs_2() const
{
  return d_desktopdirs_2;
}

inline std::string const &Arg::rawdesktopdb() const
{
  return d_rawdesktopdb;
}

inline std::string const &Arg::desktopkey() const
{
  return d_desktopkey;
}

inline std::string const &Arg::dumpmedia() const
{
  return d_dumpmedia;
}

inline std::string const &Arg::dumpavatars() const
{
  return d_dumpavatars;
}

inline std::string const &Arg::generatedummyfordesktop() const
{
  return d_generatedummyfordesktop;
}

inline std::string const &Arg::importcsv() const
{
  return d_importcsv;
}

inline std::string const &Arg::setselfid() const
{
  return d_setselfid;
}

inline std::string const &Arg::generatedummy() const
{
  return d_generatedummy;
}

inline std::string const &Arg::setcountrycode() const
{
  return d_setcountrycode;
}

inline std::string const &Arg::mapxmladdressesfromfile() const
{
  return d_mapxmladdressesfromfile;
}

inline std::string const &Arg::mapxmlcontactnamesfromfile() const
{
  return d_mapxmlcontactnamesfromfile;
}

inline std::string const &Arg::onlynewerthan() const
{
  return d_onlynewerthan;
}

inline std::string const &Arg::appendbody() const
{
  return d_appendbody;
}

inline std::string const &Arg::prependbody() const
{
  return d_prependbody;
}

inline std::string const &Arg::logfile() const
{
  return d_logfile;
}

inline std::string const &Arg::exporthtml() const
{
  return d_exporthtml;
}

inline std::string const &Arg::exportdesktophtml() const
{
  return d_exportdesktophtml;
}

inline std::string const &Arg::listjsonchats() const
{
  return d_listjsonchats;
}

inline std::string const &Arg::importtelegram() const
{
  return d_importtelegram;
}

inline std::string const &Arg::split_by() const
{
  return d_split_by;
}

inline std::string const &Arg::exportdesktoptxt() const
{
  return d_exportdesktoptxt;
}

inline std::string const &Arg::exporttxt() const
{
  return d_exporttxt;
}

inline std::string const &Arg::dumpdesktopdb() const
{
  return d_dumpdesktopdb;
}

inline std::string const &Arg::source() const
{
  return d_source;
}

inline std::string const &Arg::opassphrase() const
{
  return d_opassphrase;
}

inline void Arg::setopassphrase(std::string const &val)
{
  d_opassphrase = val;
}

inline std::string const &Arg::output() const
{
  return d_output;
}

inline std::string const &Arg::exportxml() const
{
  return d_exportxml;
}

inline std::string const &Arg::querymode() const
{
  return d_querymode;
}

inline std::string const &Arg::sourcepassphrase() const
{
  return d_sourcepassphrase;
}

inline void Arg::setsourcepassphrase(std::string const &val)
{
  d_sourcepassphrase = val;
}

inline std::string const &Arg::input() const
{
  return d_input;
}

inline std::string const &Arg::passphrase() const
{
  return d_passphrase;
}

inline void Arg::setpassphrase(std::string const &val)
{
  d_passphrase = val;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::replaceattachments() const
{
  return d_replaceattachments;
}

inline bool Arg::replaceattachments_bool() const
{
  return d_replaceattachments_bool;
}

inline std::vector<std::string> const &Arg::croptothreadsbyname() const
{
  return d_croptothreadsbyname;
}

inline std::vector<std::string> const &Arg::listxmlcontacts() const
{
  return d_listxmlcontacts;
}

inline std::vector<long long int> const &Arg::croptothreads() const
{
  return d_croptothreads;
}

inline std::vector<std::pair<std::string,long long int>> const &Arg::mapxmlcontacts() const
{
  return d_mapxmlcontacts;
}

inline std::vector<std::string> const &Arg::onlytype() const
{
  return d_onlytype;
}

inline std::vector<std::string> const &Arg::limittodates() const
{
  return d_limittodates;
}

inline std::vector<std::string> const &Arg::selectxmlchats() const
{
  return d_selectxmlchats;
}

inline std::vector<std::string> const &Arg::exportplaintextbackuphtml() const
{
  return d_exportplaintextbackuphtml;
}

inline std::vector<std::string> const &Arg::importplaintextbackup() const
{
  return d_importplaintextbackup;
}

inline std::vector<std::string> const &Arg::preventjsonmapping() const
{
  return d_preventjsonmapping;
}

inline std::vector<std::pair<std::string, long long int>> const &Arg::mapjsoncontacts() const
{
  return d_mapjsoncontacts;
}

inline std::vector<long long int> const &Arg::selectjsonchats() const
{
  return d_selectjsonchats;
}

inline std::vector<std::string> const &Arg::limittothreadsbyname() const
{
  return d_limittothreadsbyname;
}

inline std::vector<long long int> const &Arg::limittothreads() const
{
  return d_limittothreads;
}

inline std::vector<std::string> const &Arg::importthreadsbyname() const
{
  return d_importthreadsbyname;
}

inline std::vector<long long int> const &Arg::importthreads() const
{
  return d_importthreads;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::exportcsv() const
{
  return d_exportcsv;
}

inline std::vector<long long int> const &Arg::editattachmentsize() const
{
  return d_editattachmentsize;
}

inline std::vector<std::string> const &Arg::limitcontacts() const
{
  return d_limitcontacts;
}

inline std::vector<std::string> const &Arg::rundtprettysqlquery() const
{
  return d_rundtprettysqlquery;
}

inline std::vector<std::string> const &Arg::htmlignoremediatypes() const
{
  return d_htmlignoremediatypes;
}

inline std::vector<std::string> const &Arg::rundtsqlquery() const
{
  return d_rundtsqlquery;
}

inline std::vector<std::string> const &Arg::runprettysqlquery() const
{
  return d_runprettysqlquery;
}

inline std::vector<std::string> const &Arg::runsqlquery() const
{
  return d_runsqlquery;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::mapcsvfields() const
{
  return d_mapcsvfields;
}

inline std::vector<std::string> const &Arg::croptodates() const
{
  return d_croptodates;
}

inline std::vector<std::string> const &Arg::mergegroups() const
{
  return d_mergegroups;
}

inline std::vector<std::string> const &Arg::mergerecipients() const
{
  return d_mergerecipients;
}

inline std::vector<long long int> const &Arg::onlyinthreads() const
{
  return d_onlyinthreads;
}

inline std::vector<std::pair<std::string, std::string>> const &Arg::mapxmladdresses() const
{
  return d_mapxmladdresses;
}

inline std::vector<std::pair<long long int, std::string>> const &Arg::setchatcolors() const
{
  return d_setchatcolors;
}

inline std::vector<std::pair<std::string, std::string>> const &Arg::mapxmlcontactnames() const
{
  return d_mapxmlcontactnames;
}

inline long long int Arg::onlylargerthan() const
{
  return d_onlylargerthan;
}

inline long long int Arg::desktopdbversion() const
{
  return d_desktopdbversion;
}

inline long long int Arg::split() const
{
  return d_split;
}

inline bool Arg::split_bool() const
{
  return d_split_bool;
}

inline long long int Arg::findrecipient() const
{
  return d_findrecipient;
}

inline long long int Arg::hiperfall() const
{
  return d_hiperfall;
}

inline int Arg::removedoubles() const
{
  return d_removedoubles;
}

inline bool Arg::removedoubles_bool() const
{
  return d_removedoubles_bool;
}

inline bool Arg::htmlpagemenu() const
{
  return d_htmlpagemenu;
}

inline bool Arg::append() const
{
  return d_append;
}

inline bool Arg::migratedb() const
{
  return d_migratedb;
}

inline bool Arg::linkify() const
{
  return d_linkify;
}

inline bool Arg::importstickers() const
{
  return d_importstickers;
}

inline bool Arg::targetisdummy() const
{
  return d_targetisdummy;
}

inline bool Arg::skipmessagereorder() const
{
  return d_skipmessagereorder;
}

inline bool Arg::jsonprependforward() const
{
  return d_jsonprependforward;
}

inline bool Arg::jsonmarkdelivered() const
{
  return d_jsonmarkdelivered;
}

inline bool Arg::jsonmarkread() const
{
  return d_jsonmarkread;
}

inline bool Arg::xmlmarkdelivered() const
{
  return d_xmlmarkdelivered;
}

inline bool Arg::compactfilenames() const
{
  return d_compactfilenames;
}

inline bool Arg::migrate_to_191() const
{
  return d_migrate_to_191;
}

inline bool Arg::xmlmarkread() const
{
  return d_xmlmarkread;
}

inline bool Arg::fulldecode() const
{
  return d_fulldecode;
}

inline bool Arg::xmlautogroupnames() const
{
  return d_xmlautogroupnames;
}

inline bool Arg::custom_hugogithubs() const
{
  return d_custom_hugogithubs;
}

inline bool Arg::truncate() const
{
  return d_truncate;
}

inline bool Arg::reordermmssmsids() const
{
  return d_reordermmssmsids;
}

inline bool Arg::importfromdesktop() const
{
  return d_importfromdesktop;
}

inline bool Arg::scramble() const
{
  return d_scramble;
}

inline bool Arg::showdbinfo() const
{
  return d_showdbinfo;
}

inline bool Arg::scanmissingattachments() const
{
  return d_scanmissingattachments;
}

inline bool Arg::help() const
{
  return d_help;
}

inline bool Arg::deleteattachments() const
{
  return d_deleteattachments;
}

inline bool Arg::dbusverbose() const
{
  return d_dbusverbose;
}

inline bool Arg::verbose() const
{
  return d_verbose;
}

inline bool Arg::stoponerror() const
{
  return d_stoponerror;
}

inline bool Arg::autolimitdates() const
{
  return d_autolimitdates;
}

inline bool Arg::showprogress() const
{
  return d_showprogress;
}

inline bool Arg::listrecipients() const
{
  return d_listrecipients;
}

inline bool Arg::listthreads() const
{
  return d_listthreads;
}

inline bool Arg::overwrite() const
{
  return d_overwrite;
}

inline bool Arg::onlydb() const
{
  return d_onlydb;
}

inline bool Arg::devcustom() const
{
  return d_devcustom;
}

inline bool Arg::excludestickers() const
{
  return d_excludestickers;
}

inline bool Arg::showdesktopkey() const
{
  return d_showdesktopkey;
}

inline bool Arg::assumebadframesizeonbadmac() const
{
  return d_assumebadframesizeonbadmac;
}

inline bool Arg::includefullcontactlist() const
{
  return d_includefullcontactlist;
}

inline bool Arg::importdesktopcontacts() const
{
  return d_importdesktopcontacts;
}

inline bool Arg::addincompletedataforhtmlexport() const
{
  return d_addincompletedataforhtmlexport;
}

inline bool Arg::originalfilenames() const
{
  return d_originalfilenames;
}

inline bool Arg::chatfolders() const
{
  return d_chatfolders;
}

inline bool Arg::includereceipts() const
{
  return d_includereceipts;
}

inline bool Arg::stickerpacks() const
{
  return d_stickerpacks;
}

inline bool Arg::searchpage() const
{
  return d_searchpage;
}

inline bool Arg::themeswitching() const
{
  return d_themeswitching;
}

inline bool Arg::light() const
{
  return d_light;
}

inline bool Arg::includesettings() const
{
  return d_includesettings;
}

inline bool Arg::includeblockedlist() const
{
  return d_includeblockedlist;
}

inline bool Arg::includecalllog() const
{
  return d_includecalllog;
}

inline bool Arg::addexportdetails() const
{
  return d_addexportdetails;
}

inline bool Arg::interactive() const
{
  return d_interactive;
}

inline bool Arg::checkdbintegrity() const
{
  return d_checkdbintegrity;
}

inline bool Arg::includemms() const
{
  return d_includemms;
}

inline bool Arg::ignorewal() const
{
  return d_ignorewal;
}

inline bool Arg::input_required() const
{
  return d_input_required;
}

inline bool Arg::ok() const
{
  return d_ok;
}

template <typename T>
bool Arg::ston(T *t, std::string const &str) const
{
  std::istringstream iss(str);
  return !(iss >> *t).fail();
}

inline void Arg::parseStringList(std::string const &strlist, std::vector<std::string> *list) const
{
  size_t start = 0;
  size_t pos = 0;
  while ((pos = strlist.find(',', start)) != std::string::npos)
  {
    list->emplace_back(strlist.substr(start, pos - start));
    start = pos + 1;
  }
  list->emplace_back(strlist.substr(start));
}

template <typename T, typename U>
inline bool Arg::parsePairList(std::string const &pairlist, std::string const &delim, std::vector<std::pair<T, U>> *list, std::string *error) const
{
  size_t start = 0;
  size_t pos = 0;
  while ((pos = pairlist.find(',', start)) != std::string::npos)
  {
    std::pair<T, U> tmp;
    if (!parsePair(pairlist.substr(start, pos - start), delim, &tmp, error))
      return false;
    list->emplace_back(std::move(tmp));
    start = pos + 1;
  }
  std::pair<T, U> tmp;
  if (!parsePair(pairlist.substr(start), delim, &tmp, error))
    return false;
  list->emplace_back(std::move(tmp));
  return true;
}

template <typename T>
bool Arg::parseNumberList(std::string const &strlist, std::vector<T> *list, bool sort, std::vector<std::string> *faillist) const
{
  size_t start = 0;
  size_t pos = 0;
  while ((pos = strlist.find(',', start)) != std::string::npos)
  {
    if (!parseNumberListToken(strlist.substr(start, pos - start), list))  // get&parse token
    {
      if (faillist)
        faillist->emplace_back(strlist.substr(start, pos - start));
      else
        return false;
    }
    start = pos + 1;
  }
  if (!parseNumberListToken(strlist.substr(start), list)) // get last bit
  {
    if (faillist)
      faillist->emplace_back(strlist.substr(start));
    else
      return false;
  }

  if (sort)
    std::sort(list->begin(), list->end());

  return true;
}

template <typename T, typename U>
bool Arg::parsePair(std::string const &token, std::string const &delim, std::pair<T, U> *pair, std::string *error) const
{
  std::string::size_type pos = token.find(delim);
  if (pos == std::string::npos)
  {
    *error = "Delimiter not found.";
    return false;
  }

  std::string first(token, 0, pos);
  std::string second(token, pos + 1);
  if (first.empty() || second.empty())
  {
    *error = "Empty field in pair.";
    return false;
  }

  if constexpr (std::is_same<T, std::string>::value)
    pair->first = first;
  else
  {
    if (!ston(&(pair->first), first))
    {
      *error = "Bad argument.";
      return false;
    }
  }

  if constexpr (std::is_same<U, std::string>::value)
    pair->second = second;
  else
  {
    if (!ston(&(pair->second), second))
    {
      *error = "Bad argument.";
      return false;
    }
  }

  return true;
}

template <typename T>
bool Arg::parseNumberListToken(std::string const &token, std::vector<T> *list) const
{
  size_t pos = 0;
  T beg = -1;

  // try and get first number
  if ((pos = token.find('-')) != std::string::npos)
  {
    if (!ston<T>(&beg, token.substr(0, pos)))
      return false;

    // then there must be a second number
    T end = -1;
    if (!ston<T>(&end, token.substr(pos + 1)))
      return false;

    if (beg > end)
      return false;
    for (int i = beg; i <= end ; ++i)
      list->push_back(i);
  }
  else
  {
    if (!ston<T>(&beg, token))
      return false;
    else
      list->push_back(beg);
  }

  return true;
}

inline bool Arg::isOption(std::string const &str) const
{
  return std::find(d_alloptions.begin(), d_alloptions.end(), str) != d_alloptions.end();
}

#endif
