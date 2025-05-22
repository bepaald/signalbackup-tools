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

#ifndef HEADERFRAME_H_
#define HEADERFRAME_H_

#include <string_view>

#include "../backupframe/backupframe.h"
#include "../base64/base64.h"

#include "../common_be.h"
#include "../common_bytes.h"

class HeaderFrame : public BackupFrame
{
  enum FIELD : std::uint8_t
  {
   INVALID = 0,
   IV = 1,   // byte[]
   SALT = 2,  // byte[]
   VERSION = 3 // uint32
  };

  static Registrar s_registrar;
 public:
  inline explicit HeaderFrame(uint64_t count = 0);
  inline HeaderFrame(unsigned char const *data, size_t length, uint64_t count = 0);
  inline HeaderFrame(HeaderFrame const &other) = default;
  inline HeaderFrame &operator=(HeaderFrame const &other) = default;
  inline HeaderFrame(HeaderFrame &&other) noexcept = default;
  inline HeaderFrame &operator=(HeaderFrame &&other) noexcept = default;
  inline virtual ~HeaderFrame() override = default;
  inline virtual HeaderFrame *clone() const override;
  inline virtual HeaderFrame *move_clone() override;
  inline virtual FRAMETYPE frameType() const override;
  inline unsigned char *iv() const;
  inline uint64_t iv_length() const;
  inline unsigned char *salt() const;
  inline uint64_t salt_length() const;
  inline uint32_t version() const;
  inline static BackupFrame *create(unsigned char const *data, size_t length, uint64_t count = 0);
  //inline static BackupFrame *createFromHumanData(std::ifstream *datastream, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline std::string getHumanData() const override;
  inline virtual bool validate(uint64_t) const override;
  inline unsigned int getField(std::string_view str) const;
 private:
  inline uint64_t dataSize() const override;
};

inline HeaderFrame::HeaderFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline HeaderFrame *HeaderFrame::clone() const
{
  return new HeaderFrame(*this);
}

inline HeaderFrame *HeaderFrame::move_clone()
{
  return new HeaderFrame(std::move(*this));
}

inline HeaderFrame::HeaderFrame(unsigned char const *data, size_t length, uint64_t count)
  :
  BackupFrame(data, length, count)
{
  d_ok = iv_length() == 16;
}

inline BackupFrame::FRAMETYPE HeaderFrame::frameType() const // virtual override
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

inline uint32_t HeaderFrame::version() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p)  == FIELD::VERSION)
      return bytesToUint32(std::get<1>(p), std::get<2>(p));
  return 0;
}

inline BackupFrame *HeaderFrame::create(unsigned char const *data, size_t length, uint64_t count) // static
{
  return new HeaderFrame(data, length, count);
}

inline void HeaderFrame::printInfo() const
{
  //DEBUGOUT("TYPE: HEADERFRAME");
  Logger::message("Frame number: ", d_count);
  Logger::message("        Size: ", d_constructedsize);
  Logger::message("        Type: HEADER");
  Logger::message("         - IV: ", bepaald::bytesToHexString(iv(), iv_length()));
  Logger::message("         - SALT: ", bepaald::bytesToHexString(salt(), salt_length()));
  Logger::message("         - VERSION: ", version());
}

inline uint64_t HeaderFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::IV:
      case FIELD::SALT:
      {
        uint64_t blobsize = std::get<2>(fd); // length of actual data
        // length of length
        size += varIntSize(blobsize);
        size += blobsize + 1; // plus one for fieldtype + wiretype
        break;
      }
      case FIELD::VERSION:
      {
        uint64_t value = bytesToInt64(std::get<1>(fd), std::get<2>(fd));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
        break;
      }
    }
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
  {
    switch (std::get<0>(fd))
    {
      case FIELD::IV:
      case FIELD::SALT:
      {
        datapos += putLengthDelimType(fd, data + datapos);
        break;
      }
      case FIELD::VERSION:
      {
        datapos += putVarIntType(fd, data + datapos);
        break;
      }
    }
  }
  return {data, size};
}

inline std::string HeaderFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::IV)
    {
      data += "IV:bytes:";
      data += Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p)) + "\n";
    }
    else if (std::get<0>(p) == FIELD::SALT)
    {
      data += "SALT:bytes:";
      data += Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p)) + "\n";
    }
    else if (std::get<0>(p) == FIELD::VERSION)
      data += "VERSION:uint32:" + bepaald::toString(bytesToUint32(std::get<1>(p), std::get<2>(p))) + "\n";
  }
  return data;
}

inline bool HeaderFrame::validate(uint64_t) const
{
  if (d_framedata.empty())
    return false;

  int foundiv = 0;
  int foundsalt = 0;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::IV &&
        std::get<0>(p) != FIELD::SALT &&
        std::get<0>(p) != FIELD::VERSION) // all possible fields, version only in newer backups
      return false;

    // must contain salt and iv, each 1 time.
    if (std::get<0>(p) == FIELD::IV)
      ++foundiv;
    if (std::get<0>(p) == FIELD::SALT)
      ++foundsalt;
  }

  // salt length is 32, iv is 16
  return foundsalt == 1 && foundiv == 1 && salt_length() == 32 && iv_length() == 16;
}

inline unsigned int HeaderFrame::getField(std::string_view str) const
{
  if (str == "IV")
    return FIELD::IV;
  if (str == "SALT")
    return FIELD::SALT;
  if (str == "VERSION")
    return FIELD::VERSION;
  return FIELD::INVALID;
}

#endif
