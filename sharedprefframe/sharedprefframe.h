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

#ifndef SHAREDPREFFRAME_H_
#define SHAREDPREFFRAME_H_

#include <string_view>

#include "../common_bytes.h"
#include "../backupframe/backupframe.h"

class SharedPrefFrame : public BackupFrame
{
  enum FIELD
  {
    INVALID = 0,
    FILE = 1, // string
    KEY = 2,  // string
    VALUE = 3, // string
    BOOLEANVALUE = 4, // bool
    STRINGSETVALUE = 5, // string (repeated)
    ISSTRINGSETVALUE = 6 // bool
  };

  static Registrar s_registrar;
 public:
  inline explicit SharedPrefFrame(uint64_t count = 0);
  inline SharedPrefFrame(unsigned char const *bytes, size_t length, uint64_t count = 0);
  inline virtual ~SharedPrefFrame() override = default;
  inline virtual SharedPrefFrame *clone() const override;
  inline virtual SharedPrefFrame *move_clone() override;
  inline static BackupFrame *create(unsigned char const *bytes, size_t length, uint64_t count = 0);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
  inline std::string getHumanData() const override;
  inline unsigned int getField(std::string_view const &str) const;
  inline std::string key() const;
  inline std::vector<std::string> value() const;
  inline std::string valueType() const;
 private:
  inline uint64_t dataSize() const override;
};

inline SharedPrefFrame::SharedPrefFrame(uint64_t count)
  :
  BackupFrame(count)
{}

inline SharedPrefFrame::SharedPrefFrame(unsigned char const *bytes, size_t length, uint64_t count)
  :
  BackupFrame(bytes, length, count)
{}

inline SharedPrefFrame *SharedPrefFrame::clone() const
{
  return new SharedPrefFrame(*this);
}

inline SharedPrefFrame *SharedPrefFrame::move_clone()
{
  return new SharedPrefFrame(std::move(*this));
}

inline BackupFrame *SharedPrefFrame::create(unsigned char const *bytes, size_t length, uint64_t count)
{
  return new SharedPrefFrame(bytes, length, count);
}

inline void SharedPrefFrame::printInfo() const // virtual
{
  Logger::message("Frame number: ", d_count);
  Logger::message("        Size: ", d_constructedsize);
  Logger::message("        Type: SHAREDPREFERENCEFRAME");
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::FILE)
      Logger::message("         - (file  : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::KEY)
      Logger::message("         - (key   : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::VALUE)
      Logger::message("         - (value : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      Logger::message("         - (booleanvalue : \"", std::boolalpha, (bytesToUint64(std::get<1>(p), std::get<2>(p)) ? true : false), "\")");
    else if (std::get<0>(p) == FIELD::STRINGSETVALUE)
      Logger::message("         - (stringsetvalue : \"", bepaald::bytesToString(std::get<1>(p), std::get<2>(p)), "\" (", std::get<2>(p), " bytes)");
    else if (std::get<0>(p) == FIELD::ISSTRINGSETVALUE)
      Logger::message("         - (isstringsetvalue : \"", std::boolalpha, (bytesToUint64(std::get<1>(p), std::get<2>(p)) ? true : false), "\")");
  }
}

inline BackupFrame::FRAMETYPE SharedPrefFrame::frameType() const // virtual override
{
  return FRAMETYPE::SHAREDPREFERENCE;
}

inline uint64_t SharedPrefFrame::dataSize() const
{
  uint64_t size = 0;
  for (auto const &fd : d_framedata)
  {
    switch (std::get<0>(fd))
    {
      case FIELD::FILE:
      case FIELD::KEY:
      case FIELD::VALUE:
      case FIELD::STRINGSETVALUE:
      {
        uint64_t stringsize = std::get<2>(fd);
        size += varIntSize(stringsize);
        size += stringsize + 1; // +1 for fieldtype + wiretype
        break;
      }
      case FIELD::BOOLEANVALUE:
      case FIELD::ISSTRINGSETVALUE:
      {
        uint64_t val = bytesToInt64(std::get<1>(fd), std::get<2>(fd));
        size += varIntSize(val);
        size += 1; // for fieldtype + wiretype
        break;
      }
    }
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
  {
    switch (std::get<0>(fd))
    {
      case FIELD::FILE:
      case FIELD::KEY:
      case FIELD::VALUE:
      case FIELD::STRINGSETVALUE:
      {
        datapos += putLengthDelimType(fd, data + datapos);
        break;
      }
      case FIELD::BOOLEANVALUE:
      case FIELD::ISSTRINGSETVALUE:
      {
        datapos += putVarIntType(fd, data + datapos);
        break;
      }
    }
  }

  return {data, size};

}

// not sure about the requirements, but at least _a_ field should be set
// also a key needs a value, and a value needs a key
inline bool SharedPrefFrame::validate() const
{
  if (d_framedata.empty())
    return false;

  int foundfile = 0;
  int foundkey = 0;
  int foundvalue = 0;
  bool isstringsetvalue = false;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) != FIELD::FILE &&
        std::get<0>(p) != FIELD::KEY &&
        std::get<0>(p) != FIELD::VALUE &&
        std::get<0>(p) != FIELD::BOOLEANVALUE &&
        std::get<0>(p) != FIELD::STRINGSETVALUE &&
        std::get<0>(p) != FIELD::ISSTRINGSETVALUE)
      return false;

    if (std::get<0>(p) == FIELD::FILE)
      ++foundfile;

    if (std::get<0>(p) == FIELD::KEY)
      ++foundkey;

    if (std::get<0>(p) == FIELD::VALUE ||
        std::get<0>(p) == FIELD::BOOLEANVALUE ||
        std::get<0>(p) == FIELD::STRINGSETVALUE ||
        std::get<0>(p) == FIELD::ISSTRINGSETVALUE)
      ++foundvalue;

    if (std::get<0>(p) == FIELD::ISSTRINGSETVALUE)
      isstringsetvalue = (bytesToUint64(std::get<1>(p), std::get<2>(p)) ? true : false);
  }

  return (foundfile + foundkey + foundvalue) > 0 &&
    (isstringsetvalue ? (foundkey <= foundvalue) : (foundkey == foundvalue));
}

inline std::string SharedPrefFrame::getHumanData() const
{
  std::string data;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::FILE)
      data += "FILE:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::KEY)
      data += "KEY:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::VALUE)
      data += "VALUE:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      data += "BOOLEANVALUE:bool:" + (bytesToInt64(std::get<1>(p), std::get<2>(p)) ? "true"s : "false"s) + "\n";
    else if (std::get<0>(p) == FIELD::STRINGSETVALUE)
      data += "STRINGSETVALUE:string:" + bepaald::bytesToString(std::get<1>(p), std::get<2>(p)) + "\n";
    else if (std::get<0>(p) == FIELD::ISSTRINGSETVALUE)
      data += "ISSTRINGSETVALUE:bool:" + (bytesToInt64(std::get<1>(p), std::get<2>(p)) ? "true"s : "false"s) + "\n";
  }
  return data;
}

inline unsigned int SharedPrefFrame::getField(std::string_view const &str) const
{
  if (str == "FILE")
    return FIELD::FILE;
  if (str == "KEY")
    return FIELD::KEY;
  if (str == "VALUE")
    return FIELD::VALUE;
  if (str == "BOOLEANVALUE")
    return FIELD::BOOLEANVALUE;
  if (str == "STRINGSETVALUE")
    return FIELD::STRINGSETVALUE;
  if (str == "ISSTRINGSETVALUE")
    return FIELD::ISSTRINGSETVALUE;
  return FIELD::INVALID;
}


inline std::string SharedPrefFrame::key() const
{
  for (auto const &p : d_framedata)
    if (std::get<0>(p) == FIELD::KEY)
      return bepaald::bytesToString(std::get<1>(p), std::get<2>(p));
  return std::string();
}

inline std::vector<std::string> SharedPrefFrame::value() const
{
  std::vector<std::string> ret;
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::VALUE) // string
    {
      ret.push_back(bepaald::bytesToString(std::get<1>(p), std::get<2>(p)));
      break;
    }

    if (std::get<0>(p) == FIELD::BOOLEANVALUE)
    {
      ret.emplace_back((bytesToInt64(std::get<1>(p), std::get<2>(p)) ? "true"s : "false"s));
      break;
    }

    if (std::get<0>(p) == FIELD::STRINGSETVALUE)
      ret.emplace_back(bepaald::bytesToString(std::get<1>(p), std::get<2>(p)));
  }
  return ret;
}

inline std::string SharedPrefFrame::valueType() const
{
  for (auto const &p : d_framedata)
  {
    if (std::get<0>(p) == FIELD::VALUE)
      return "STRING";

    if (std::get<0>(p) == FIELD::BOOLEANVALUE)
      return "BOOL";

    if (std::get<0>(p) == FIELD::STRINGSETVALUE)
      return "STRINGSET";

    if (std::get<0>(p) == FIELD::ISSTRINGSETVALUE)
      if (bytesToInt64(std::get<1>(p), std::get<2>(p)) == true)
        return "STRINGSET";
  }
  Logger::warning("Currently unsupported value type requested from SharedPrefrenceFrame");
  return std::string();
}

#endif
