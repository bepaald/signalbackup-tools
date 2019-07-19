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

#ifndef SHAREDPREFFRAME_H_
#define SHAREDPREFFRAME_H_

#include "../backupframe/backupframe.h"

class SharedPrefFrame : public BackupFrame
{
  enum FIELD
  {
    FILE = 1, // string
    KEY = 2,  // string
    VALUE = 3 // string
  };

  static Registrar s_registrar;
 public:
  inline SharedPrefFrame(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual ~SharedPrefFrame() = default;
  inline static BackupFrame *create(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const;
  inline virtual bool validate() const override;
 private:
  inline uint64_t dataSize() const;
};

inline SharedPrefFrame::SharedPrefFrame(unsigned char *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count)
{}

inline BackupFrame *SharedPrefFrame::create(unsigned char *bytes, size_t length, uint64_t count)
{
  return new SharedPrefFrame(bytes, length, count);
}

inline void SharedPrefFrame::printInfo() const // virtual
{
  //DEBUGOUT("TYPE: SHAREDPREFERENCEFRAME");
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Type: SHAREDPREFERENCEFRAME" << std::endl;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::FILE)
      std::cout << "         - (file  : \"";
    else if (std::get<0>(p) == FIELD::KEY)
      std::cout << "         - (key   : \"";
    else if (std::get<0>(p) == FIELD::VALUE)
      std::cout << "         - (value : \"";
    std::cout << bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) << "\" (" << std::get<2>(p) << " bytes)" << std::endl;
  }
}

inline FRAMETYPE SharedPrefFrame::frameType() const // virtual override
{
  return FRAMETYPE::SHAREDPREFERENCE;
}

inline uint64_t SharedPrefFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
      uint64_t statementsize = std::get<2>(fd); // length of actual data
      // length of length
      size += varIntSize(statementsize);
      size += statementsize + 1; // plus one for fieldtype + wiretype
  }
  // for size of this entire frame.
  size += varIntSize(size);
  return ++size;
}

inline std::pair<unsigned char *, uint64_t> SharedPrefFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::SHAREDPREFERENCE, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
    datapos += putLengthDelimType(fd, data + datapos);

  return {data, size};

}

inline bool SharedPrefFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::FILE &&
        std::get<0>(p) != FIELD::KEY &&
        std::get<0>(p) != FIELD::VALUE)
      return false;
  }
  return true;
}

#endif
