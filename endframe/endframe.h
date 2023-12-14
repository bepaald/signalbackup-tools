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

#ifndef ENDFRAME_H_
#define ENDFRAME_H_

#include "../backupframe/backupframe.h"

class EndFrame: public BackupFrame
{
  static Registrar s_registrar;
 public:
  inline EndFrame(unsigned char *bytes, size_t length, uint64_t count = 0);
  inline virtual ~EndFrame() = default;
  inline static BackupFrame *create(unsigned char *bytes, size_t length, uint64_t count);
  inline virtual void printInfo() const override;
  inline virtual FRAMETYPE frameType() const override;
  inline std::pair<unsigned char *, uint64_t> getData() const override;
  inline virtual bool validate() const override;
 private:
  inline uint64_t dataSize() const;
};

inline EndFrame::EndFrame(unsigned char *, size_t length, uint64_t count)
  :
  BackupFrame(count) // endframe is a raw bool, not a message, so no field type (it sorta IS its only field), length is value
{
  unsigned char *valbytes = new unsigned char[sizeof(length)];
  intTypeToBytes(length, valbytes);
  d_framedata.push_back(std::make_tuple(0, valbytes, sizeof(length)));
}

inline BackupFrame *EndFrame::create(unsigned char *bytes, size_t length, uint64_t count) // static
{
  return new EndFrame(bytes, length, count);
}

inline void EndFrame::printInfo() const // virtual override
{
  Logger::message("Frame number: ", d_count);
  Logger::message("        Type: END");
  for (auto const &p : d_framedata)
    Logger::message("         - (value  : \"", std::boolalpha, (bytesToUint64(std::get<1>(p), std::get<2>(p)) ? true : false), "\")");
}

inline BackupFrame::FRAMETYPE EndFrame::frameType() const // virtual override
{
  return FRAMETYPE::END;
}

inline uint64_t EndFrame::dataSize() const
{
  uint64_t size = 0;
  // for size of this entire frame.
  size += varIntSize(size);
  return ++size;
}

inline std::pair<unsigned char *, uint64_t> EndFrame::getData() const
{
  uint64_t size = dataSize();
  unsigned char *data = new unsigned char[size];

  //uint64_t datapos = 0;
  //datapos += setFieldAndWire(FRAMETYPE::END, WIRETYPE::VARINT, data + datapos);
  //datapos += putVarInt(1, data + datapos);
  uint64_t datapos = setFieldAndWire(FRAMETYPE::END, WIRETYPE::VARINT, data);
  putVarInt(1, data + datapos);

  return {data, size};
}

inline bool EndFrame::validate() const
{
  return d_framedata.size() == 1 && std::get<2>(d_framedata.front()) == 8 &&
    (bytesToUint64(std::get<1>(d_framedata.front()), std::get<2>(d_framedata.front())) == 1 ||
     bytesToUint64(std::get<1>(d_framedata.front()), std::get<2>(d_framedata.front())) == 0);
}

#endif
