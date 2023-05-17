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

class Arg
{
  bool d_ok;
  std::array<std::string, 80> const d_alloptions{"-i", "--input", "-p", "--passphrase", "--importthreads", "--limittothreads", "-o", "--output", "-op", "--opassphrase", "-s", "--source", "-sp", "--sourcepassphrase", "--croptothreads", "--croptodates", "--mergerecipients", "--mergegroups", "--exportcsv", "--exportxml", "--runsqlquery", "--runprettysqlquery", "--limitcontacts", "--assumebadframesizeonbadmac", "--editattachmentsize", "--dumpdesktopdb", "--dumpmedia", "--dumpavatars", "--hhenkel", "--devcustom", "--importcsv", "--mapcsvfields", "-//", "--importwachat", "--setselfid", "--onlydb", "--overwrite", "--listthreads", "--listrecipients", "--editgroupmembers", "--showprogress", "--removedoubles", "--reordermmssmsids", "--stoponerror", "-v", "--verbose", "--strugee", "--strugee3", "--ashmorgan", "--strugee2", "--deleteattachments", "--onlyinthreads", "--onlyolderthan", "--onlynewerthan", "--onlylargerthan", "--onlytype", "--appendbody", "--prependbody", "--replaceattachments", "-h", "--help", "--scanmissingattachments", "--showdbinfo", "--scramble", "--importfromdesktop", "--limittodates", "--autolimitdates", "--ignorewal", "--includemms", "--checkdbintegrity", "--interactive", "--exporthtml", "--append", "--split", "--desktopdbversion", "--migratedb", "--addincompletedataforhtmlexport", "--light", "--themeswitching", "--findrecipient"};
  size_t d_positionals;
  size_t d_maxpositional;
  std::string d_progname;
  std::string d_input;
  std::string d_passphrase;
  std::vector<long long int> d_importthreads;
  std::vector<long long int> d_limittothreads;
  std::string d_output;
  std::string d_opassphrase;
  std::string d_source;
  std::string d_sourcepassphrase;
  std::vector<long long int> d_croptothreads;
  std::vector<std::string> d_croptodates;
  std::vector<std::string> d_mergerecipients;
  std::vector<std::string> d_mergegroups;
  std::vector<std::pair<std::string,std::string>> d_exportcsv;
  std::string d_exportxml;
  std::vector<std::string> d_runsqlquery;
  std::vector<std::string> d_runprettysqlquery;
  std::vector<std::string> d_limitcontacts;
  bool d_assumebadframesizeonbadmac;
  std::vector<long long int> d_editattachmentsize;
  std::string d_dumpdesktopdb_1;
  std::string d_dumpdesktopdb_2;
  std::string d_dumpmedia;
  std::string d_dumpavatars;
  std::string d_hhenkel;
  bool d_devcustom;
  std::string d_importcsv;
  std::vector<std::pair<std::string,std::string>> d_mapcsvfields;
  std::string d_importwachat;
  std::string d_setselfid;
  bool d_onlydb;
  bool d_overwrite;
  bool d_listthreads;
  bool d_listrecipients;
  bool d_editgroupmembers;
  bool d_showprogress;
  bool d_removedoubles;
  bool d_reordermmssmsids;
  bool d_stoponerror;
  bool d_verbose;
  long long int d_strugee;
  long long int d_strugee3;
  bool d_ashmorgan;
  bool d_strugee2;
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
  std::string d_importfromdesktop_1;
  std::string d_importfromdesktop_2;
  bool d_importfromdesktop_bool;
  std::vector<std::string> d_limittodates;
  bool d_autolimitdates;
  bool d_ignorewal;
  bool d_includemms;
  bool d_checkdbintegrity;
  bool d_interactive;
  std::string d_exporthtml;
  bool d_append;
  long long int d_split;
  bool d_split_bool;
  long long int d_desktopdbversion;
  bool d_migratedb;
  bool d_addincompletedataforhtmlexport;
  bool d_light;
  bool d_themeswitching;
  long long int d_findrecipient;
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
  inline std::vector<long long int> const &limittothreads() const;
  inline std::string const &output() const;
  inline std::string const &opassphrase() const;
  inline void setopassphrase(std::string const &val);
  inline std::string const &source() const;
  inline std::string const &sourcepassphrase() const;
  inline void setsourcepassphrase(std::string const &val);
  inline std::vector<long long int> const &croptothreads() const;
  inline std::vector<std::string> const &croptodates() const;
  inline std::vector<std::string> const &mergerecipients() const;
  inline std::vector<std::string> const &mergegroups() const;
  inline std::vector<std::pair<std::string,std::string>> const &exportcsv() const;
  inline std::string const &exportxml() const;
  inline std::vector<std::string> const &runsqlquery() const;
  inline std::vector<std::string> const &runprettysqlquery() const;
  inline std::vector<std::string> const &limitcontacts() const;
  inline bool assumebadframesizeonbadmac() const;
  inline std::vector<long long int> const &editattachmentsize() const;
  inline std::string const &dumpdesktopdb_1() const;
  inline std::string const &dumpdesktopdb_2() const;
  inline std::string const &dumpmedia() const;
  inline std::string const &dumpavatars() const;
  inline std::string const &hhenkel() const;
  inline bool devcustom() const;
  inline std::string const &importcsv() const;
  inline std::vector<std::pair<std::string,std::string>> const &mapcsvfields() const;
  inline std::string const &importwachat() const;
  inline std::string const &setselfid() const;
  inline bool onlydb() const;
  inline bool overwrite() const;
  inline bool listthreads() const;
  inline bool listrecipients() const;
  inline bool editgroupmembers() const;
  inline bool showprogress() const;
  inline bool removedoubles() const;
  inline bool reordermmssmsids() const;
  inline bool stoponerror() const;
  inline bool verbose() const;
  inline long long int strugee() const;
  inline long long int strugee3() const;
  inline bool ashmorgan() const;
  inline bool strugee2() const;
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
  inline std::string const &importfromdesktop_1() const;
  inline std::string const &importfromdesktop_2() const;
  inline bool importfromdesktop_bool() const;
  inline std::vector<std::string> const &limittodates() const;
  inline bool autolimitdates() const;
  inline bool ignorewal() const;
  inline bool includemms() const;
  inline bool checkdbintegrity() const;
  inline bool interactive() const;
  inline std::string const &exporthtml() const;
  inline bool append() const;
  inline long long int split() const;
  inline bool split_bool() const;
  inline long long int desktopdbversion() const;
  inline bool migratedb() const;
  inline bool addincompletedataforhtmlexport() const;
  inline bool light() const;
  inline bool themeswitching() const;
  inline long long int findrecipient() const;
 private:
  template <typename T>
  bool ston(T *t, std::string const &str) const;
  bool parseArgs(std::vector<std::string> const &args);
  inline bool parseStringList(std::string const &strlist, std::vector<std::string> *list) const;
  template <typename T, typename U>
  inline bool parsePairList(std::string const &pairlist, std::string const &delim, std::vector<std::pair<T, U>> *list, std::string *error) const;
  template <typename T>
  bool parseNumberList(std::string const &strlist, std::vector<T> *list) const;
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

inline std::vector<long long int> const &Arg::limittothreads() const
{
  return d_limittothreads;
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

inline std::vector<std::string> const &Arg::runsqlquery() const
{
  return d_runsqlquery;
}

inline std::vector<std::string> const &Arg::runprettysqlquery() const
{
  return d_runprettysqlquery;
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

inline std::string const &Arg::dumpdesktopdb_1() const
{
  return d_dumpdesktopdb_1;
}

inline std::string const &Arg::dumpdesktopdb_2() const
{
  return d_dumpdesktopdb_2;
}

inline std::string const &Arg::dumpmedia() const
{
  return d_dumpmedia;
}

inline std::string const &Arg::dumpavatars() const
{
  return d_dumpavatars;
}

inline std::string const &Arg::hhenkel() const
{
  return d_hhenkel;
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

inline std::string const &Arg::importwachat() const
{
  return d_importwachat;
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

inline bool Arg::editgroupmembers() const
{
  return d_editgroupmembers;
}

inline bool Arg::showprogress() const
{
  return d_showprogress;
}

inline bool Arg::removedoubles() const
{
  return d_removedoubles;
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

inline std::string const &Arg::importfromdesktop_1() const
{
  return d_importfromdesktop_1;
}

inline std::string const &Arg::importfromdesktop_2() const
{
  return d_importfromdesktop_2;
}

inline bool Arg::importfromdesktop_bool() const
{
  return d_importfromdesktop_bool;
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

inline bool Arg::append() const
{
  return d_append;
}

inline long long int Arg::split() const
{
  return d_split;
}

inline bool Arg::split_bool() const
{
  return d_split_bool;
}

inline long long int Arg::desktopdbversion() const
{
  return d_desktopdbversion;
}

inline bool Arg::migratedb() const
{
  return d_migratedb;
}

inline bool Arg::addincompletedataforhtmlexport() const
{
  return d_addincompletedataforhtmlexport;
}

inline bool Arg::light() const
{
  return d_light;
}

inline bool Arg::themeswitching() const
{
  return d_themeswitching;
}

inline long long int Arg::findrecipient() const
{
  return d_findrecipient;
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
    list->push_back(tr.substr(start, pos - start));
    start = pos + 1;
  }
  list->push_back(tr.substr(start));
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
bool Arg::parseNumberList(std::string const &strlist, std::vector<T> *list) const
{
  std::string tr = strlist;

  size_t start = 0;
  size_t pos = 0;
  while ((pos = tr.find(',', start)) != std::string::npos)
  {
    if (!parseNumberListToken(tr.substr(start, pos - start), list))  // get&parse token
      return false;
    start = pos + 1;
  }
  if (!parseNumberListToken(tr.substr(start), list)) // get last bit
    return false;

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

  std::string first = token.substr(0, pos);
  std::string second = token.substr(pos + 1);
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
