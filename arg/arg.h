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

#ifndef ARGS_H_
#define ARGS_H_

#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>

class Arg
{
  bool d_ok;
  size_t d_positionals;
  std::vector<int> d_importthreads;
  std::string d_input;
  std::string d_password;
  std::string d_output;
  std::string d_opassword;
  std::string d_source;
  std::string d_sourcepassword;
  bool d_listthreads;
  bool d_generatefromtruncated;
  std::vector<long long int> d_croptothreads;
  std::vector<std::string> d_croptodates;
  bool d_elbrutalo;
  std::vector<std::string> d_mergerecipients;
  bool d_editgroupmembers;
 public:
  Arg(int argc, char *argv[]);
  inline Arg(Arg const &other) = delete;
  inline Arg &operator=(Arg const &other) = delete;
  inline bool ok() const;
  inline std::vector<int> const &importthreads() const;
  inline std::string const &input() const;
  inline std::string const &password() const;
  inline std::string const &output() const;
  inline std::string const &opassword() const;
  inline std::string const &source() const;
  inline std::string const &sourcepassword() const;
  inline bool listthreads() const;
  inline bool generatefromtruncated() const;
  inline std::vector<long long int> const &croptothreads() const;
  inline std::vector<std::string> const &croptodates() const;
  inline bool elbrutalo() const;
  inline std::vector<std::string> const &mergerecipients() const;
  inline bool editgroupmembers() const;
 private:
  template <typename T>
  bool ston(T *t, std::string const &str) const;
  bool parseArgs(std::vector<std::string> const &args);
  inline bool parseStringList(std::string const &strlist, std::vector<std::string> *list) const;
  template <typename T>
  bool parseNumberList(std::string const &strlist, std::vector<T> *list) const;
  template <typename T>
  bool parseNumberListToken(std::string const &token, std::vector<T> *list) const;
  void usage() const;
};

inline std::vector<int> const &Arg::importthreads() const
{
  return d_importthreads;
}

inline std::string const &Arg::input() const
{
  return d_input;
}

inline std::string const &Arg::password() const
{
  return d_password;
}

inline std::string const &Arg::output() const
{
  return d_output;
}

inline std::string const &Arg::opassword() const
{
  return d_opassword;
}

inline std::string const &Arg::source() const
{
  return d_source;
}

inline std::string const &Arg::sourcepassword() const
{
  return d_sourcepassword;
}

inline bool Arg::listthreads() const
{
  return d_listthreads;
}

inline bool Arg::generatefromtruncated() const
{
  return d_generatefromtruncated;
}

inline std::vector<long long int> const &Arg::croptothreads() const
{
  return d_croptothreads;
}

inline std::vector<std::string> const &Arg::croptodates() const
{
  return d_croptodates;
}

inline bool Arg::elbrutalo() const
{
  return d_elbrutalo;
}

inline std::vector<std::string> const &Arg::mergerecipients() const
{
  return d_mergerecipients;
}

inline bool Arg::editgroupmembers() const
{
  return d_editgroupmembers;
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

#endif
