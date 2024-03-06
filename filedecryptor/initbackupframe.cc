/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#include "filedecryptor.ih"

BackupFrame *FileDecryptor::initBackupFrame(unsigned char *data, size_t length, uint64_t count) const
{
  if (length < 1) [[unlikely]]
  {
    //std::cout << "ERROR: frame length < 1" << std::endl;
    return nullptr;
  }

  int fieldnum = BackupFrame::getFieldnumber(data[0]);
  if (fieldnum < 0) [[unlikely]]
  {
    //std::cout << "ERROR: field number < 0" << std::endl;
    return nullptr;
  }
  unsigned int wiretype = BackupFrame::wiretype(data[0]);
  unsigned int offset = 1;
  int64_t datalength = BackupFrame::getLength(data, &offset, length);

  DEBUGOUT("*** FRAMEDATA: ", bepaald::bytesToHexString(data, length));
  DEBUGOUT("FIELDNUMBER: ", fieldnum);
  DEBUGOUT("DATALENGTH: ", datalength);
  DEBUGOUT("OFFSET: ", offset);

  if (datalength < 0 ||
      (static_cast<BackupFrame::FRAMETYPE>(fieldnum) != BackupFrame::FRAMETYPE::END &&
       static_cast<uint64_t>(datalength) > length - offset)) [[unlikely]]
  {
    //std::cout << "ERROR: data length < 0" << std::endl;
    return nullptr;
  }

  if (static_cast<BackupFrame::FRAMETYPE>(fieldnum) == BackupFrame::FRAMETYPE::END) // is a raw bool type, not a message
  {
    if (wiretype == BackupFrame::WIRETYPE::VARINT && datalength == 1)
      return new EndFrame(data, datalength, count);
    else
      return nullptr;
  }
  else
  {
    if (wiretype != BackupFrame::WIRETYPE::LENGTHDELIM) [[unlikely]]
    {
      //std::cout << "ERROR: unexpected wiretype" << std::endl;
      return nullptr;
    }
    return BackupFrame::instantiate(static_cast<BackupFrame::FRAMETYPE>(fieldnum), data + offset, datalength, count);
  }
}
