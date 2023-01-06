/*
  Copyright (C) 2021-2023  Selwin van Dijk

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

#ifndef CSVREADER_H_
#define CSVREADER_H_

#include <fstream>
#include <string>
#include <vector>

#include "../common_be.h"

class CSVReader
{
 private:
  enum class CSVState
  {
    UNQUOTEDFIELD,
    QUOTEDFIELD,
    QUOTEDQUOTE
  };

  std::ifstream d_csvfile;
  std::vector<std::vector<std::string>> d_results;
  bool d_ok;
  unsigned int d_fields; // an extra check
 public:
  inline CSVReader(std::string const &filename);
  inline bool ok() const;
  inline size_t fields() const;
  inline size_t rows() const;
  inline std::string const &get(int field, int row) const;
  inline std::string const &getFieldName(int row) const;
 private:
  bool read();
  CSVState readRow(std::string const &row, CSVState laststate);
};

inline CSVReader::CSVReader(std::string const &filename)
  :
  d_csvfile(filename),
  d_ok(false),
  d_fields(0)
{
  if (d_csvfile.is_open())
    d_ok = read();
  else
    std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off
              << " opening file '" << filename << "' for reading." << std::endl;
}

inline bool CSVReader::ok() const
{
  return d_ok;
}

inline size_t CSVReader::fields() const
{
  return d_results.size() ? d_results[0].size() : 0;
}

inline size_t CSVReader::rows() const
{
  return d_results.size() - 1;
}

inline std::string const &CSVReader::get(int field, int row) const
{
  return d_results[row + 1][field];
}

inline std::string const &CSVReader::getFieldName(int field) const
{
  return d_results[0][field];
}

#endif
