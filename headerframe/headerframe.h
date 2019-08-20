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

#ifndef HEADERFRAME_H_
#define HEADERFRAME_H_

#include "../backupframe/backupframe.h"
#include "../base64/base64.h"

class HeaderFrame : public BackupFrame
{
  enum FIELD : unsigned int
  {
   INVALID = 0,
   IV = 1,   // byte[]
   SALT = 2  // byte[]
  };

  static Registrar s_registrar;

 public:
  inline explicit HeaderFrame(uint64_t count = 0);
  inline HeaderFrame(unsigned char *data, size_t length, uint64_t count = 0);
  inline virtual ~HeaderFrame() = default;
  inline virtual FRAMETYPE frameType() const override;
  inline unsigned char *iv() const;
  inline uint64_t iv_length() const;
  inline unsigned char *salt() const;
  inline uint64_t salt_length() const;
  inline static BackupFrame *create(unsigned char *data, size_t length, uint64_t count = 0);
  //inline static BackupFrame *createFromHumanData(std::ifstream *datastream, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline std::string getHumanData() const override;
  inline virtual bool validate() const override;
  inline unsigned int getField(std::string const &str) const;
 private:
  inline uint64_t dataSize() const;
};

inline HeaderFrame::HeaderFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline HeaderFrame::HeaderFrame(unsigned char *data, size_t length, uint64_t count)
  :
  BackupFrame(data, length, count)
{
  d_ok = iv_length() == 16;
}

inline FRAMETYPE HeaderFrame::frameType() const // virtual override
{
  return FRAMETYPE::HEADER;
}

inline unsigned char *HeaderFrame::iv() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::IV)
      return std::get<1>(p);
  return nullptr;
}

inline uint64_t HeaderFrame::iv_length() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::IV)
      return std::get<2>(p);
  return 0;
}

inline unsigned char *HeaderFrame::salt() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p)  == FIELD::SALT)
      return std::get<1>(p);
  return nullptr;
}

inline uint64_t HeaderFrame::salt_length() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::SALT)
      return std::get<2>(p);
  return 0;
}

inline BackupFrame *HeaderFrame::create(unsigned char *data, size_t length, uint64_t count) // static
{
  return new HeaderFrame(data, length, count);
}

inline void HeaderFrame::printInfo() const
{
  //DEBUGOUT("TYPE: HEADERFRAME");
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Type: HEADER" << std::endl;
  std::cout << "         - IV  : " << bepaald::bytesToHexString(iv(), iv_length()) << std::endl;
  std::cout << "         - SALT: " << bepaald::bytesToHexString(salt(), salt_length()) << std::endl;
}

inline uint64_t HeaderFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
      uint64_t blobsize = std::get<2>(fd); // length of actual data
      // length of length
      size += varIntSize(blobsize);
      size += blobsize + 1; // plus one for fieldtype + wiretype
  }
  // for size of this entire frame.
  size += varIntSize(size);
  return ++size;
}

inline std::pair<unsigned char *, uint64_t> HeaderFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::HEADER, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
    datapos += putLengthDelimType(fd, data + datapos);
  return {data, size};
}

inline std::string HeaderFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::IV)
      data += "IV:bytes:";
    else if (std::get<0>(p) == FIELD::SALT)
      data += "SALT:bytes:";
    data += Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p));
  }
  return data;
}

inline bool HeaderFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::IV &&
        std::get<0>(p) != FIELD::SALT)
      return false;
  }
  return true;
}

inline unsigned int HeaderFrame::getField(std::string const &str) const
{
  if (str == "IV")
    return FIELD::IV;
  if (str == "SALT")
    return FIELD::SALT;
  return FIELD::INVALID;
}

#endif
