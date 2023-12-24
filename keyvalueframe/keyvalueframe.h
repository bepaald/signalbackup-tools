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

#ifndef KEYVALUEFRAME_H_
#define KEYVALUEFRAME_H_

#include "../backupframe/backupframe.h"
#include "../base64/base64.h"

class KeyValueFrame : public BackupFrame
{
  enum FIELD
  {
    INVALID = 0,
    KEY = 1,          // string
    BLOBVALUE = 2,    // bytes
    BOOLEANVALUE = 3, // bool
    FLOATVALUE = 4,   // float
    INTEGERVALUE = 5, // int32
    LONGVALUE = 6,    // int64
    STRINGVALUE = 7   // string
  };

  static Registrar s_registrar;
 public:
  inline explicit KeyValueFrame(uint64_t count = 0);
  inline KeyValueFrame(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual ~KeyValueFrame() = default;
  inline static BackupFrame *create(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string const &str) const;
  inline std::string key() const;
  inline std::string value() const;
 private:
  inline uint64_t dataSize() const;
};

inline KeyValueFrame::KeyValueFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline KeyValueFrame::KeyValueFrame(unsigned char *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count)
{}

inline BackupFrame *KeyValueFrame::create(unsigned char *bytes, size_t length, uint64_t count)
{
  return new KeyValueFrame(bytes, length, count);
}

inline void KeyValueFrame::printInfo() const // virtual
{
  //DEBUGOUT("TYPE: KEYVALUEFRAME");
  Logger::message("Frame number: ", d_count);
  Logger::message("        Size: ", d_constructedsize);
  Logger::message("        Type: KEYVALUEFRAME");
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::KEY)
      Logger::message("         - (key  : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::BLOBVALUE)
      Logger::message("         - (blobvalue   : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      Logger::message("         - (booleanvalue : \"", std::boolalpha, (bytesToInt64(std::get<1>(p), std::get<2>(p)) ? true : false), "\")");
    else if (std::get<0>(p) == FIELD::FLOATVALUE) // note, this is untested, none of my backups contain a KVFrame with this field
      Logger::message("         - (floatvalue : \"", bepaald::toString(*reinterpret_cast<float *>(std::get<1>(p))), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::INTEGERVALUE)
      Logger::message("         - (integervalue : \"", bytesToInt32(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::LONGVALUE)
      Logger::message("         - (longvalue : \"", bytesToUint64(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::STRINGVALUE)
      Logger::message("         - (stringvalue : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
  }
}

inline BackupFrame::FRAMETYPE KeyValueFrame::frameType() const // virtual override
{
  return FRAMETYPE::KEYVALUE;
}

inline uint64_t KeyValueFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::KEY:
      case FIELD::STRINGVALUE:
      case FIELD::BLOBVALUE:
      {
        uint64_t stringsize = std::get<2>(fd);
        size += varIntSize(stringsize);
        size += stringsize + 1; // +1 for fieldtype + wiretype
        break;
      }
      case FIELD::INTEGERVALUE:
      case FIELD::LONGVALUE:
      case FIELD::BOOLEANVALUE:
      {
        uint64_t value = bytesToInt64(std::get<1>(fd), std::get<2>(fd));
        size += varIntSize(value);
        size += 1; // for fieldtype + wiretype
        break;
      }
      case FIELD::FLOATVALUE: // note, this is untested, none of my backups contain a KVFrame with this field
      {
        size += 5; // fixed32? +1 for fieldtype + wiretype
        break;
      }
    }
  }

  // for size of this entire frame.
  size += varIntSize(size);
  return ++size;
}

inline std::pair<unsigned char *, uint64_t> KeyValueFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];
  uint64_t datapos = 0;

  datapos += setFieldAndWire(FRAMETYPE::KEYVALUE, WIRETYPE::LENGTHDELIM, data + datapos);
  datapos += setFrameSize(size, data + datapos);

  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::KEY:
      case FIELD::STRINGVALUE:
      case FIELD::BLOBVALUE:
      {
        datapos += putLengthDelimType(fd, data + datapos);
        break;
      }
      case FIELD::INTEGERVALUE:
      case FIELD::LONGVALUE:
      case FIELD::BOOLEANVALUE:
      {
        datapos += putVarIntType(fd, data + datapos);
        break;
      }
      case FIELD::FLOATVALUE:
      {
        datapos += putFixed32Type(fd, data + datapos); // untested
        break;
      }
    }
  }
  return {data, size};

}

// not sure about the requirements, but I'm guessing
// 1 key and at least one value is required
inline bool KeyValueFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  bool foundkey = false;
  bool foundvalue = false;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::KEY)
      foundkey = true;
    if (std::get<0>(p) == FIELD::STRINGVALUE ||
        std::get<0>(p) == FIELD::BOOLEANVALUE ||
        std::get<0>(p) == FIELD::BLOBVALUE ||
        std::get<0>(p) == FIELD::INTEGERVALUE ||
        std::get<0>(p) == FIELD::LONGVALUE ||
        std::get<0>(p) == FIELD::FLOATVALUE)
      foundvalue = true;
  }

  return foundkey && foundvalue;
}

inline std::string KeyValueFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::KEY)
      data += "KEY:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::BLOBVALUE)
      data += "BLOBVALUE:bytes:" + Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      data += "BOOLEANVALUE:bool:" + (bytesToInt64(std::get<1>(p), std::get<2>(p)) ? "true"s : "false"s) + "\n";
    else if (std::get<0>(p) == FIELD::FLOATVALUE)
      data += "FLOATVALUE:float:" + Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p)) + "\n"; // warning, untested
    else if (std::get<0>(p) == FIELD::INTEGERVALUE)
      data += "INTEGERVALUE:int32:" + bepaald::toString(bytesToInt32(std::get<1>(p), std::get<2>(p))) + "\n";
    else if (std::get<0>(p) == FIELD::LONGVALUE)
      data += "LONGVALUE:int64:" + bepaald::toString(bytesToInt64(std::get<1>(p), std::get<2>(p))) + "\n";
    else if (std::get<0>(p) == FIELD::STRINGVALUE)
      data += "STRINGVALUE:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
  }
  return data;
}

inline unsigned int KeyValueFrame::getField(std::string const &str) const
{
  if (str == "KEY")
    return FIELD::KEY;
  if (str == "BLOBVALUE")
    return FIELD::BLOBVALUE;
  if (str == "BOOLEANVALUE")
    return FIELD::BOOLEANVALUE;
  if (str == "FLOATVALUE")
    return FIELD::FLOATVALUE;
  if (str == "LONGVALUE")
    return FIELD::LONGVALUE;
  if (str == "INTEGERVALUE")
    return FIELD::INTEGERVALUE;
  if (str == "STRINGVALUE")
    return FIELD::STRINGVALUE;
  return FIELD::INVALID;
}

inline std::string KeyValueFrame::key() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::KEY)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
  return std::string();
}

inline std::string KeyValueFrame::value() const
{
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::STRINGVALUE)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));

    if (std::get<0>(p) == FIELD::INTEGERVALUE || std::get<0>(p) == FIELD::LONGVALUE)
      return bepaald::toString(bytesToInt64(std::get<1>(p), std::get<2>(p)));

    if (std::get<0>(p) == FIELD::BLOBVALUE || std::get<0>(p) == FIELD::FLOATVALUE) // float is untested
      return Base64::bytesToBase64String(std::get<1>(p), std::get<2>(p));

    if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      return (bytesToInt64(std::get<1>(p), std::get<2>(p)) ? "true"s : "false"s);
  }
  return std::string();
}

#endif
