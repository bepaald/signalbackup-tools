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

#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <utility>
#include <array>
#include <regex>

class Arg
{
  bool d_ok;
  std::array<std::string, 201> const d_alloptions{"-i", "--input", "-p", "--passphrase", "--importthreads", "--importthreadsbyname", "--limittothreads", "--limittothreadsbyname", "-o", "--output", "-op", "--opassphrase", "-s", "--source", "-sp", "--sourcepassphrase", "--croptothreads", "--croptothreadsbyname", "--croptodates", "--mergerecipients", "--mergegroups", "--exportcsv", "--exportxml", "--querymode", "--runsqlquery", "--runprettysqlquery", "--rundtsqlquery", "--rundtprettysqlquery", "--limitcontacts", "--assumebadframesizeonbadmac", "--no-assumebadframesizeonbadmac", "--editattachmentsize", "--dumpdesktopdb", "--desktopdir", "--desktopdirs", "--rawdesktopdb", "--desktopkey", "--showdesktopkey", "--no-showdesktopkey", "--dumpmedia", "--excludestickers", "--no-excludestickers", "--dumpavatars", "--devcustom", "--no-devcustom", "--importcsv", "--mapcsvfields", "--setselfid", "--onlydb", "--no-onlydb", "--overwrite", "--no-overwrite", "--listthreads", "--no-listthreads", "--listrecipients", "--no-listrecipients", "--showprogress", "--no-showprogress", "--removedoubles", "--reordermmssmsids", "--no-reordermmssmsids", "--stoponerror", "--no-stoponerror", "-v", "--verbose", "--no-verbose", "--dbusverbose", "--no-dbusverbose", "--strugee", "--strugee3", "--ashmorgan", "--no-ashmorgan", "--strugee2", "--no-strugee2", "--hiperfall", "--arc", "--deleteattachments", "--no-deleteattachments", "--onlyinthreads", "--onlyolderthan", "--onlynewerthan", "--onlylargerthan", "--onlytype", "--appendbody", "--prependbody", "--replaceattachments", "-h", "--help", "--no-help", "--scanmissingattachments", "--no-scanmissingattachments", "--showdbinfo", "--no-showdbinfo", "--scramble", "--no-scramble", "--importfromdesktop", "--no-importfromdesktop", "--limittodates", "--autolimitdates", "--no-autolimitdates", "--ignorewal", "--no-ignorewal", "--includemms", "--no-includemms", "--checkdbintegrity", "--no-checkdbintegrity", "--interactive", "--no-interactive", "--exporthtml", "--exportdesktophtml", "--exportplaintextbackuphtml", "--importplaintextbackup", "--addexportdetails", "--no-addexportdetails", "--includecalllog", "--no-includecalllog", "--includeblockedlist", "--no-includeblockedlist", "--includesettings", "--no-includesettings", "--includefullcontactlist", "--no-includefullcontactlist", "--themeswitching", "--no-themeswitching", "--searchpage", "--no-searchpage", "--stickerpacks", "--no-stickerpacks", "--includereceipts", "--no-includereceipts", "--chatfolders", "--no-chatfolders", "--allhtmlpages", "--split", "--split-by", "--originalfilenames", "--no-originalfilenames", "--addincompletedataforhtmlexport", "--no-addincompletedataforhtmlexport", "--importdesktopcontacts", "--no-importdesktopcontacts", "--light", "--no-light", "--exporttxt", "--exportdesktoptxt", "--append", "--no-append", "--desktopdbversion", "--migratedb", "--no-migratedb", "--importstickers", "--no-importstickers", "--findrecipient", "--importtelegram", "--listjsonchats", "--selectjsonchats", "--mapjsoncontacts", "--preventjsonmapping", "--jsonprependforward", "--no-jsonprependforward", "--jsonmarkdelivered", "--no-jsonmarkdelivered", "--jsonmarkread", "--no-jsonmarkread", "--xmlmarkdelivered", "--no-xmlmarkdelivered", "--xmlmarkread", "--no-xmlmarkread", "--fulldecode", "--no-fulldecode", "-l", "--logfile", "--custom_hugogithubs", "--no-custom_hugogithubs", "--truncate", "--no-truncate", "--skipmessagereorder", "--no-skipmessagereorder", "--migrate_to_191", "--no-migrate_to_191", "--mapxmlcontacts", "--listxmlcontacts", "--selectxmlchats", "--linkify", "--no-linkify", "--setchatcolors", "--mapxmlcontactnames", "--mapxmlcontactnamesfromfile", "--mapxmladdresses", "--mapxmladdressesfromfile", "--xmlautogroupnames", "--no-xmlautogroupnames", "--setcountrycode", "--compactfilenames", "--no-compactfilenames", "--generatedummy", "--targetisdummy", "--no-targetisdummy", "--htmlignoremediatypes", "--htmlpagemenu", "--no-htmlpagemenu"};
  size_t d_positionals;
  size_t d_maxpositional;
  std::string d_progname;
  std::string d_input;
  std::string d_passphrase;
  std::vector<long long int> d_importthreads;
  std::vector<std::string> d_importthreadsbyname;
  std::vector<long long int> d_limittothreads;
  std::vector<std::string> d_limittothreadsbyname;
  std::string d_output;
  std::string d_opassphrase;
  std::string d_source;
  std::string d_sourcepassphrase;
  std::vector<long long int> d_croptothreads;
  std::vector<std::string> d_croptothreadsbyname;
  std::vector<std::string> d_croptodates;
  std::vector<std::string> d_mergerecipients;
  std::vector<std::string> d_mergegroups;
  std::vector<std::pair<std::string,std::string>> d_exportcsv;
  std::string d_exportxml;
  std::string d_querymode;
  std::vector<std::string> d_runsqlquery;
  std::vector<std::string> d_runprettysqlquery;
  std::vector<std::string> d_rundtsqlquery;
  std::vector<std::string> d_rundtprettysqlquery;
  std::vector<std::string> d_limitcontacts;
  bool d_assumebadframesizeonbadmac;
  std::vector<long long int> d_editattachmentsize;
  std::string d_dumpdesktopdb;
  std::string d_desktopdir;
  std::string d_desktopdirs_1;
  std::string d_desktopdirs_2;
  std::string d_rawdesktopdb;
  std::string d_desktopkey;
  bool d_showdesktopkey;
  std::string d_dumpmedia;
  bool d_excludestickers;
  std::string d_dumpavatars;
  bool d_devcustom;
  std::string d_importcsv;
  std::vector<std::pair<std::string,std::string>> d_mapcsvfields;
  std::string d_setselfid;
  bool d_onlydb;
  bool d_overwrite;
  bool d_listthreads;
  bool d_listrecipients;
  bool d_showprogress;
  int d_removedoubles;
  bool d_removedoubles_bool;
  bool d_reordermmssmsids;
  bool d_stoponerror;
  bool d_verbose;
  bool d_dbusverbose;
  long long int d_strugee;
  long long int d_strugee3;
  bool d_ashmorgan;
  bool d_strugee2;
  long long int d_hiperfall;
  long long int d_arc;
  bool d_deleteattachments;
  std::vector<long long int> d_onlyinthreads;
  std::string d_onlyolderthan;
  std::string d_onlynewerthan;
  long long int d_onlylargerthan;
  std::vector<std::string> d_onlytype;
  std::string d_appendbody;
  std::string d_prependbody;
  std::vector<std::pair<std::string,std::string>> d_replaceattachments;
  bool d_replaceattachments_bool;
  bool d_help;
  bool d_scanmissingattachments;
  bool d_showdbinfo;
  bool d_scramble;
  bool d_importfromdesktop;
  std::vector<std::string> d_limittodates;
  bool d_autolimitdates;
  bool d_ignorewal;
  bool d_includemms;
  bool d_checkdbintegrity;
  bool d_interactive;
  std::string d_exporthtml;
  std::string d_exportdesktophtml;
  std::vector<std::string> d_exportplaintextbackuphtml;
  std::vector<std::string> d_importplaintextbackup;
  bool d_addexportdetails;
  bool d_includecalllog;
  bool d_includeblockedlist;
  bool d_includesettings;
  bool d_includefullcontactlist;
  bool d_themeswitching;
  bool d_searchpage;
  bool d_stickerpacks;
  bool d_includereceipts;
  bool d_chatfolders;
  long long int d_split;
  bool d_split_bool;
  std::string d_split_by;
  bool d_originalfilenames;
  bool d_addincompletedataforhtmlexport;
  bool d_importdesktopcontacts;
  bool d_light;
  std::string d_exporttxt;
  std::string d_exportdesktoptxt;
  bool d_append;
  long long int d_desktopdbversion;
  bool d_migratedb;
  bool d_importstickers;
  long long int d_findrecipient;
  std::string d_importtelegram;
  std::string d_listjsonchats;
  std::vector<long long int> d_selectjsonchats;
  std::vector<std::pair<std::string, long long int>> d_mapjsoncontacts;
  std::vector<std::string> d_preventjsonmapping;
  bool d_jsonprependforward;
  bool d_jsonmarkdelivered;
  bool d_jsonmarkread;
  bool d_xmlmarkdelivered;
  bool d_xmlmarkread;
  bool d_fulldecode;
  std::string d_logfile;
  bool d_custom_hugogithubs;
  bool d_truncate;
  bool d_skipmessagereorder;
  bool d_migrate_to_191;
  std::vector<std::pair<std::string,long long int>> d_mapxmlcontacts;
  std::vector<std::string> d_listxmlcontacts;
  std::vector<std::string> d_selectxmlchats;
  bool d_linkify;
  std::vector<std::pair<long long int, std::string>> d_setchatcolors;
  std::vector<std::pair<std::string, std::string>> d_mapxmlcontactnames;
  std::string d_mapxmlcontactnamesfromfile;
  std::vector<std::pair<std::string, std::string>> d_mapxmladdresses;
  std::string d_mapxmladdressesfromfile;
  bool d_xmlautogroupnames;
  std::string d_setcountrycode;
  bool d_compactfilenames;
  std::string d_generatedummy;
  bool d_targetisdummy;
  std::vector<std::string> d_htmlignoremediatypes;
  bool d_htmlpagemenu;
  bool d_input_required;
 public:
  Arg(int argc, char *argv[]);
  inline Arg(Arg const &other) = delete;
  inline Arg &operator=(Arg const &other) = delete;
  inline bool ok() const;
  void usage() const;
  inline std::string const &input() const;
  inline std::string const &passphrase() const;
  inline void setpassphrase(std::string const &val);
  inline std::vector<long long int> const &importthreads() const;
  inline std::vector<std::string> const &importthreadsbyname() const;
  inline std::vector<long long int> const &limittothreads() const;
  inline std::vector<std::string> const &limittothreadsbyname() const;
  inline std::string const &output() const;
  inline std::string const &opassphrase() const;
  inline void setopassphrase(std::string const &val);
  inline std::string const &source() const;
  inline std::string const &sourcepassphrase() const;
  inline void setsourcepassphrase(std::string const &val);
  inline std::vector<long long int> const &croptothreads() const;
  inline std::vector<std::string> const &croptothreadsbyname() const;
  inline std::vector<std::string> const &croptodates() const;
  inline std::vector<std::string> const &mergerecipients() const;
  inline std::vector<std::string> const &mergegroups() const;
  inline std::vector<std::pair<std::string,std::string>> const &exportcsv() const;
  inline std::string const &exportxml() const;
  inline std::string const &querymode() const;
  inline std::vector<std::string> const &runsqlquery() const;
  inline std::vector<std::string> const &runprettysqlquery() const;
  inline std::vector<std::string> const &rundtsqlquery() const;
  inline std::vector<std::string> const &rundtprettysqlquery() const;
  inline std::vector<std::string> const &limitcontacts() const;
  inline bool assumebadframesizeonbadmac() const;
  inline std::vector<long long int> const &editattachmentsize() const;
  inline std::string const &dumpdesktopdb() const;
  inline std::string const &desktopdir() const;
  inline std::string const &desktopdirs_1() const;
  inline std::string const &desktopdirs_2() const;
  inline std::string const &rawdesktopdb() const;
  inline std::string const &desktopkey() const;
  inline bool showdesktopkey() const;
  inline std::string const &dumpmedia() const;
  inline bool excludestickers() const;
  inline std::string const &dumpavatars() const;
  inline bool devcustom() const;
  inline std::string const &importcsv() const;
  inline std::vector<std::pair<std::string,std::string>> const &mapcsvfields() const;
  inline std::string const &setselfid() const;
  inline bool onlydb() const;
  inline bool overwrite() const;
  inline bool listthreads() const;
  inline bool listrecipients() const;
  inline bool showprogress() const;
  inline int removedoubles() const;
  inline bool removedoubles_bool() const;
  inline bool reordermmssmsids() const;
  inline bool stoponerror() const;
  inline bool verbose() const;
  inline bool dbusverbose() const;
  inline long long int strugee() const;
  inline long long int strugee3() const;
  inline bool ashmorgan() const;
  inline bool strugee2() const;
  inline long long int hiperfall() const;
  inline long long int arc() const;
  inline bool deleteattachments() const;
  inline std::vector<long long int> const &onlyinthreads() const;
  inline std::string const &onlyolderthan() const;
  inline std::string const &onlynewerthan() const;
  inline long long int onlylargerthan() const;
  inline std::vector<std::string> const &onlytype() const;
  inline std::string const &appendbody() const;
  inline std::string const &prependbody() const;
  inline std::vector<std::pair<std::string,std::string>> const &replaceattachments() const;
  inline bool replaceattachments_bool() const;
  inline bool help() const;
  inline bool scanmissingattachments() const;
  inline bool showdbinfo() const;
  inline bool scramble() const;
  inline bool importfromdesktop() const;
  inline std::vector<std::string> const &limittodates() const;
  inline bool autolimitdates() const;
  inline bool ignorewal() const;
  inline bool includemms() const;
  inline bool checkdbintegrity() const;
  inline bool interactive() const;
  inline std::string const &exporthtml() const;
  inline std::string const &exportdesktophtml() const;
  inline std::vector<std::string> const &exportplaintextbackuphtml() const;
  inline std::vector<std::string> const &importplaintextbackup() const;
  inline bool addexportdetails() const;
  inline bool includecalllog() const;
  inline bool includeblockedlist() const;
  inline bool includesettings() const;
  inline bool includefullcontactlist() const;
  inline bool themeswitching() const;
  inline bool searchpage() const;
  inline bool stickerpacks() const;
  inline bool includereceipts() const;
  inline bool chatfolders() const;
  inline long long int split() const;
  inline bool split_bool() const;
  inline std::string const &split_by() const;
  inline bool originalfilenames() const;
  inline bool addincompletedataforhtmlexport() const;
  inline bool importdesktopcontacts() const;
  inline bool light() const;
  inline std::string const &exporttxt() const;
  inline std::string const &exportdesktoptxt() const;
  inline bool append() const;
  inline long long int desktopdbversion() const;
  inline bool migratedb() const;
  inline bool importstickers() const;
  inline long long int findrecipient() const;
  inline std::string const &importtelegram() const;
  inline std::string const &listjsonchats() const;
  inline std::vector<long long int> const &selectjsonchats() const;
  inline std::vector<std::pair<std::string, long long int>> const &mapjsoncontacts() const;
  inline std::vector<std::string> const &preventjsonmapping() const;
  inline bool jsonprependforward() const;
  inline bool jsonmarkdelivered() const;
  inline bool jsonmarkread() const;
  inline bool xmlmarkdelivered() const;
  inline bool xmlmarkread() const;
  inline bool fulldecode() const;
  inline std::string const &logfile() const;
  inline bool custom_hugogithubs() const;
  inline bool truncate() const;
  inline bool skipmessagereorder() const;
  inline bool migrate_to_191() const;
  inline std::vector<std::pair<std::string,long long int>> const &mapxmlcontacts() const;
  inline std::vector<std::string> const &listxmlcontacts() const;
  inline std::vector<std::string> const &selectxmlchats() const;
  inline bool linkify() const;
  inline std::vector<std::pair<long long int, std::string>> const &setchatcolors() const;
  inline std::vector<std::pair<std::string, std::string>> const &mapxmlcontactnames() const;
  inline std::string const &mapxmlcontactnamesfromfile() const;
  inline std::vector<std::pair<std::string, std::string>> const &mapxmladdresses() const;
  inline std::string const &mapxmladdressesfromfile() const;
  inline bool xmlautogroupnames() const;
  inline std::string const &setcountrycode() const;
  inline bool compactfilenames() const;
  inline std::string const &generatedummy() const;
  inline bool targetisdummy() const;
  inline std::vector<std::string> const &htmlignoremediatypes() const;
  inline bool htmlpagemenu() const;
  inline bool input_required() const;
 private:
  template <typename T>
  bool ston(T *t, std::string const &str) const;
  bool parseArgs(std::vector<std::string> const &args);
  inline bool parseStringList(std::string const &strlist, std::vector<std::string> *list) const;
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

inline std::vector<long long int> const &Arg::importthreads() const
{
  return d_importthreads;
}

inline std::vector<std::string> const &Arg::importthreadsbyname() const
{
  return d_importthreadsbyname;
}

inline std::vector<long long int> const &Arg::limittothreads() const
{
  return d_limittothreads;
}

inline std::vector<std::string> const &Arg::limittothreadsbyname() const
{
  return d_limittothreadsbyname;
}

inline std::string const &Arg::output() const
{
  return d_output;
}

inline std::string const &Arg::opassphrase() const
{
  return d_opassphrase;
}

inline void Arg::setopassphrase(std::string const &val)
{
  d_opassphrase = val;
}

inline std::string const &Arg::source() const
{
  return d_source;
}

inline std::string const &Arg::sourcepassphrase() const
{
  return d_sourcepassphrase;
}

inline void Arg::setsourcepassphrase(std::string const &val)
{
  d_sourcepassphrase = val;
}

inline std::vector<long long int> const &Arg::croptothreads() const
{
  return d_croptothreads;
}

inline std::vector<std::string> const &Arg::croptothreadsbyname() const
{
  return d_croptothreadsbyname;
}

inline std::vector<std::string> const &Arg::croptodates() const
{
  return d_croptodates;
}

inline std::vector<std::string> const &Arg::mergerecipients() const
{
  return d_mergerecipients;
}

inline std::vector<std::string> const &Arg::mergegroups() const
{
  return d_mergegroups;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::exportcsv() const
{
  return d_exportcsv;
}

inline std::string const &Arg::exportxml() const
{
  return d_exportxml;
}

inline std::string const &Arg::querymode() const
{
  return d_querymode;
}

inline std::vector<std::string> const &Arg::runsqlquery() const
{
  return d_runsqlquery;
}

inline std::vector<std::string> const &Arg::runprettysqlquery() const
{
  return d_runprettysqlquery;
}

inline std::vector<std::string> const &Arg::rundtsqlquery() const
{
  return d_rundtsqlquery;
}

inline std::vector<std::string> const &Arg::rundtprettysqlquery() const
{
  return d_rundtprettysqlquery;
}

inline std::vector<std::string> const &Arg::limitcontacts() const
{
  return d_limitcontacts;
}

inline bool Arg::assumebadframesizeonbadmac() const
{
  return d_assumebadframesizeonbadmac;
}

inline std::vector<long long int> const &Arg::editattachmentsize() const
{
  return d_editattachmentsize;
}

inline std::string const &Arg::dumpdesktopdb() const
{
  return d_dumpdesktopdb;
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

inline bool Arg::showdesktopkey() const
{
  return d_showdesktopkey;
}

inline std::string const &Arg::dumpmedia() const
{
  return d_dumpmedia;
}

inline bool Arg::excludestickers() const
{
  return d_excludestickers;
}

inline std::string const &Arg::dumpavatars() const
{
  return d_dumpavatars;
}

inline bool Arg::devcustom() const
{
  return d_devcustom;
}

inline std::string const &Arg::importcsv() const
{
  return d_importcsv;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::mapcsvfields() const
{
  return d_mapcsvfields;
}

inline std::string const &Arg::setselfid() const
{
  return d_setselfid;
}

inline bool Arg::onlydb() const
{
  return d_onlydb;
}

inline bool Arg::overwrite() const
{
  return d_overwrite;
}

inline bool Arg::listthreads() const
{
  return d_listthreads;
}

inline bool Arg::listrecipients() const
{
  return d_listrecipients;
}

inline bool Arg::showprogress() const
{
  return d_showprogress;
}

inline int Arg::removedoubles() const
{
  return d_removedoubles;
}

inline bool Arg::removedoubles_bool() const
{
  return d_removedoubles_bool;
}

inline bool Arg::reordermmssmsids() const
{
  return d_reordermmssmsids;
}

inline bool Arg::stoponerror() const
{
  return d_stoponerror;
}

inline bool Arg::verbose() const
{
  return d_verbose;
}

inline bool Arg::dbusverbose() const
{
  return d_dbusverbose;
}

inline long long int Arg::strugee() const
{
  return d_strugee;
}

inline long long int Arg::strugee3() const
{
  return d_strugee3;
}

inline bool Arg::ashmorgan() const
{
  return d_ashmorgan;
}

inline bool Arg::strugee2() const
{
  return d_strugee2;
}

inline long long int Arg::hiperfall() const
{
  return d_hiperfall;
}

inline long long int Arg::arc() const
{
  return d_arc;
}

inline bool Arg::deleteattachments() const
{
  return d_deleteattachments;
}

inline std::vector<long long int> const &Arg::onlyinthreads() const
{
  return d_onlyinthreads;
}

inline std::string const &Arg::onlyolderthan() const
{
  return d_onlyolderthan;
}

inline std::string const &Arg::onlynewerthan() const
{
  return d_onlynewerthan;
}

inline long long int Arg::onlylargerthan() const
{
  return d_onlylargerthan;
}

inline std::vector<std::string> const &Arg::onlytype() const
{
  return d_onlytype;
}

inline std::string const &Arg::appendbody() const
{
  return d_appendbody;
}

inline std::string const &Arg::prependbody() const
{
  return d_prependbody;
}

inline std::vector<std::pair<std::string,std::string>> const &Arg::replaceattachments() const
{
  return d_replaceattachments;
}

inline bool Arg::replaceattachments_bool() const
{
  return d_replaceattachments_bool;
}

inline bool Arg::help() const
{
  return d_help;
}

inline bool Arg::scanmissingattachments() const
{
  return d_scanmissingattachments;
}

inline bool Arg::showdbinfo() const
{
  return d_showdbinfo;
}

inline bool Arg::scramble() const
{
  return d_scramble;
}

inline bool Arg::importfromdesktop() const
{
  return d_importfromdesktop;
}

inline std::vector<std::string> const &Arg::limittodates() const
{
  return d_limittodates;
}

inline bool Arg::autolimitdates() const
{
  return d_autolimitdates;
}

inline bool Arg::ignorewal() const
{
  return d_ignorewal;
}

inline bool Arg::includemms() const
{
  return d_includemms;
}

inline bool Arg::checkdbintegrity() const
{
  return d_checkdbintegrity;
}

inline bool Arg::interactive() const
{
  return d_interactive;
}

inline std::string const &Arg::exporthtml() const
{
  return d_exporthtml;
}

inline std::string const &Arg::exportdesktophtml() const
{
  return d_exportdesktophtml;
}

inline std::vector<std::string> const &Arg::exportplaintextbackuphtml() const
{
  return d_exportplaintextbackuphtml;
}

inline std::vector<std::string> const &Arg::importplaintextbackup() const
{
  return d_importplaintextbackup;
}

inline bool Arg::addexportdetails() const
{
  return d_addexportdetails;
}

inline bool Arg::includecalllog() const
{
  return d_includecalllog;
}

inline bool Arg::includeblockedlist() const
{
  return d_includeblockedlist;
}

inline bool Arg::includesettings() const
{
  return d_includesettings;
}

inline bool Arg::includefullcontactlist() const
{
  return d_includefullcontactlist;
}

inline bool Arg::themeswitching() const
{
  return d_themeswitching;
}

inline bool Arg::searchpage() const
{
  return d_searchpage;
}

inline bool Arg::stickerpacks() const
{
  return d_stickerpacks;
}

inline bool Arg::includereceipts() const
{
  return d_includereceipts;
}

inline bool Arg::chatfolders() const
{
  return d_chatfolders;
}

inline long long int Arg::split() const
{
  return d_split;
}

inline bool Arg::split_bool() const
{
  return d_split_bool;
}

inline std::string const &Arg::split_by() const
{
  return d_split_by;
}

inline bool Arg::originalfilenames() const
{
  return d_originalfilenames;
}

inline bool Arg::addincompletedataforhtmlexport() const
{
  return d_addincompletedataforhtmlexport;
}

inline bool Arg::importdesktopcontacts() const
{
  return d_importdesktopcontacts;
}

inline bool Arg::light() const
{
  return d_light;
}

inline std::string const &Arg::exporttxt() const
{
  return d_exporttxt;
}

inline std::string const &Arg::exportdesktoptxt() const
{
  return d_exportdesktoptxt;
}

inline bool Arg::append() const
{
  return d_append;
}

inline long long int Arg::desktopdbversion() const
{
  return d_desktopdbversion;
}

inline bool Arg::migratedb() const
{
  return d_migratedb;
}

inline bool Arg::importstickers() const
{
  return d_importstickers;
}

inline long long int Arg::findrecipient() const
{
  return d_findrecipient;
}

inline std::string const &Arg::importtelegram() const
{
  return d_importtelegram;
}

inline std::string const &Arg::listjsonchats() const
{
  return d_listjsonchats;
}

inline std::vector<long long int> const &Arg::selectjsonchats() const
{
  return d_selectjsonchats;
}

inline std::vector<std::pair<std::string, long long int>> const &Arg::mapjsoncontacts() const
{
  return d_mapjsoncontacts;
}

inline std::vector<std::string> const &Arg::preventjsonmapping() const
{
  return d_preventjsonmapping;
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

inline bool Arg::xmlmarkread() const
{
  return d_xmlmarkread;
}

inline bool Arg::fulldecode() const
{
  return d_fulldecode;
}

inline std::string const &Arg::logfile() const
{
  return d_logfile;
}

inline bool Arg::custom_hugogithubs() const
{
  return d_custom_hugogithubs;
}

inline bool Arg::truncate() const
{
  return d_truncate;
}

inline bool Arg::skipmessagereorder() const
{
  return d_skipmessagereorder;
}

inline bool Arg::migrate_to_191() const
{
  return d_migrate_to_191;
}

inline std::vector<std::pair<std::string,long long int>> const &Arg::mapxmlcontacts() const
{
  return d_mapxmlcontacts;
}

inline std::vector<std::string> const &Arg::listxmlcontacts() const
{
  return d_listxmlcontacts;
}

inline std::vector<std::string> const &Arg::selectxmlchats() const
{
  return d_selectxmlchats;
}

inline bool Arg::linkify() const
{
  return d_linkify;
}

inline std::vector<std::pair<long long int, std::string>> const &Arg::setchatcolors() const
{
  return d_setchatcolors;
}

inline std::vector<std::pair<std::string, std::string>> const &Arg::mapxmlcontactnames() const
{
  return d_mapxmlcontactnames;
}

inline std::string const &Arg::mapxmlcontactnamesfromfile() const
{
  return d_mapxmlcontactnamesfromfile;
}

inline std::vector<std::pair<std::string, std::string>> const &Arg::mapxmladdresses() const
{
  return d_mapxmladdresses;
}

inline std::string const &Arg::mapxmladdressesfromfile() const
{
  return d_mapxmladdressesfromfile;
}

inline bool Arg::xmlautogroupnames() const
{
  return d_xmlautogroupnames;
}

inline std::string const &Arg::setcountrycode() const
{
  return d_setcountrycode;
}

inline bool Arg::compactfilenames() const
{
  return d_compactfilenames;
}

inline std::string const &Arg::generatedummy() const
{
  return d_generatedummy;
}

inline bool Arg::targetisdummy() const
{
  return d_targetisdummy;
}

inline std::vector<std::string> const &Arg::htmlignoremediatypes() const
{
  return d_htmlignoremediatypes;
}

inline bool Arg::htmlpagemenu() const
{
  return d_htmlpagemenu;
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

inline bool Arg::parseStringList(std::string const &strlist, std::vector<std::string> *list) const
{
  std::string tr = strlist;

  size_t start = 0;
  size_t pos = 0;
  while ((pos = tr.find(',', start)) != std::string::npos)
  {
    list->emplace_back(tr.substr(start, pos - start));
    start = pos + 1;
  }
  list->emplace_back(tr.substr(start));
  return true;
}

template <typename T, typename U>
inline bool Arg::parsePairList(std::string const &pairlist, std::string const &delim, std::vector<std::pair<T, U>> *list, std::string *error) const
{
  std::string tr = pairlist;

  size_t start = 0;
  size_t pos = 0;
  while ((pos = tr.find(',', start)) != std::string::npos)
  {
    std::pair<T, U> tmp;
    if (!parsePair(tr.substr(start, pos - start), delim, &tmp, error))
      return false;
    list->emplace_back(std::move(tmp));
    start = pos + 1;
  }
  std::pair<T, U> tmp;
  if (!parsePair(tr.substr(start), delim, &tmp, error))
    return false;
  list->emplace_back(std::move(tmp));
  return true;
}

template <typename T>
bool Arg::parseNumberList(std::string const &strlist, std::vector<T> *list, bool sort, std::vector<std::string> *faillist) const
{
  std::string tr = strlist;

  size_t start = 0;
  size_t pos = 0;
  while ((pos = tr.find(',', start)) != std::string::npos)
  {
    if (!parseNumberListToken(tr.substr(start, pos - start), list))  // get&parse token
    {
      if (faillist)
        faillist->emplace_back(tr.substr(start, pos - start));
      else
        return false;
    }
    start = pos + 1;
  }
  if (!parseNumberListToken(tr.substr(start), list)) // get last bit
  {
    if (faillist)
      faillist->emplace_back(tr.substr(start));
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
