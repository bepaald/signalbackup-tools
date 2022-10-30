/*
  Copyright (C) 2019-2022  Selwin van Dijk

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

#ifndef DATABASEVERSIONFRAME_H_
#define DATABASEVERSIONFRAME_H_

#include "../backupframe/backupframe.h"

class DatabaseVersionFrame : public BackupFrame
{
  enum FIELD: unsigned int
  {
    INVALID = 0,
    VERSION = 1 // uint32
  };

  static Registrar s_registrar;
 public:
  inline explicit DatabaseVersionFrame(uint64_t count = 0);
  inline DatabaseVersionFrame(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual ~DatabaseVersionFrame() = default;
  inline static BackupFrame *create(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual FRAMETYPE frameType() const override;
  inline uint32_t version() const;
  inline virtual void printInfo() const override;
  inline virtual std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
  inline std::string getHumanData() const override;
  //inline virtual bool setNewData(std::string const &field, std::string const &data) override;
  inline unsigned int getField(std::string const &str) const;
 private:
  inline uint64_t dataSize() const;
};

inline DatabaseVersionFrame::DatabaseVersionFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline DatabaseVersionFrame::DatabaseVersionFrame(unsigned char *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count)
{
  //std::cout << "CREATING DATABASEVERSIONFRAME" << std::endl;
}

inline BackupFrame *DatabaseVersionFrame::create(unsigned char *bytes, size_t length, uint64_t count) // static
{
  return new DatabaseVersionFrame(bytes, length, count);
}

inline BackupFrame::FRAMETYPE DatabaseVersionFrame::frameType() const // virtual
{
  return FRAMETYPE::DATABASEVERSION;
}

inline uint32_t DatabaseVersionFrame::version() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::VERSION)
      return bytesToUint32(std::get<1>(p), std::get<2>(p));
  return 0;
}

inline void DatabaseVersionFrame::printInfo() const
{
  std::cout << "Frame number: " << d_count << std::endl;
  std::cout << "        Size: " << d_constructedsize << std::endl;
  std::cout << "        Type: DATABASEVERSION" << std::endl;
  std::cout << "         - Version: " << version() << std::endl;

  DEBUGOUT("Version: ", version());
}

inline uint64_t DatabaseVersionFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &p : d_framedata)
  {
    switch (std::get<0>(p))
    {
      case FIELD::VERSION:
      {
        uint32_t value = bytesToUint32(std::get<1>(p), std::get<2>(p));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
      }
    }
  }

  // for size of this frame.
  size += varIntSize(size);
  return ++size;  // for frametype and wiretype
}

inline std::pair<unsigned char *, uint64_t> DatabaseVersionFrame::getData() const
{

  // first write the frametype as and the wiretype
  // 0b01111000 == fieldnumber (== 5 for databaseversionframe)
  // 0b00000111 == wiretype (== 2, lengthdelim, for all frames except endframe which is bool?)

  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::DATABASEVERSION, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
  {
    //switch (std::get<0>(p))
      //{
      //case FIELD::VERSION:
      //uint32_t value = bytesToUint32(std::get<1>(p), std::get<2>(p));
      //datapos += setFieldAndWire(FIELD::VERSION, WIRETYPE::VARINT, data + datapos);
      //datapos += putVarInt(value, data + datapos);
    datapos += putVarIntType(fd, data + datapos);
      //}
  }

  //std::cout << bepaald::bytesToHexString(data, size)  << std::endl;

  return {data, size};
}

inline bool DatabaseVersionFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::VERSION)
      return false;
  }
  return true;
}

inline std::string DatabaseVersionFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::VERSION)
      data += "VERSION:uint32:" + bepaald::toString(version()) + "\n";
  return data;
}
/*
inline bool DatabaseVersionFrame::setNewData(std::string const &field, std::string const &data)
{
  if (field != "VERSION")
    return false;
  std::pair<unsigned char *, size_t> decdata = numToData(bepaald::swap_endian(std::stoul(data)));
  d_framedata.emplace_back(std::make_tuple(FIELD::VERSION, decdata.first, decdata.second));
  return true;
}
*/

inline unsigned int DatabaseVersionFrame::getField(std::string const &str) const
{
  if (str == "VERSION")
    return FIELD::VERSION;
  return FIELD::INVALID;
}

#endif
