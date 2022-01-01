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

#include "backupframe.ih"

bool BackupFrame::init(unsigned char *data, size_t l, std::vector<std::tuple<unsigned int, unsigned char *, uint64_t>> *framedata)
{

  //std::cout << "INITIALIZING FRAME OF " << l << " BYTES" << std::endl;

  unsigned int processed = 0;
  while (processed < l)
  {
    //DEBUGOUT("PROCESSED ", processed, " OUT OF ", l, " BYTES!");
    int fieldnumber = getFieldnumber(data[processed]);
    if (fieldnumber < 0)
      return false;
    uint32_t type = wiretype(data[processed]);
    ++processed; // first byte was eaten

    //DEBUGOUT("FIELDNUMBER: ", fieldnumber);
    //DEBUGOUT("WIRETYPE   : ", type);

    switch (type)
    {
    case LENGTHDELIM: // need to read next bytes for length
      {
        int64_t length = getLength(data, &processed, l);
        [[unlikely]] if (length < 0 || processed + static_cast<uint64_t>(length) > l) // more then we have
          return false;
        unsigned char *fielddata = new unsigned char[length];
        std::memcpy(fielddata, data + processed, length);
        //DEBUGOUT("FIELDNUMB: ", fieldnumber);
        //DEBUGOUT("FIELDDATA: ", bepaald::bytesToHexString(fielddata, length));
        framedata->push_back(std::make_tuple(fieldnumber, fielddata, length));
        processed += length; // up to length was eaten
        break;
      }
    case VARINT:
      {
        int64_t val = getVarint(data, &processed, l); // for UNSIGNED varints
        [[unlikely]] if (val == -1 &&                // (possible?) invalid value
                         processed == l &&           // last byte was processed
                         data[l - 1] & 0b10000000)   // but last byte was not end of varint
              return false;
        //DEBUGOUT("Got varint: ", val);
        val = bepaald::swap_endian(val); // because java writes integers in big endian?
        //DEBUGOUT("Got varint: ", val);
        unsigned char *fielddata = new unsigned char[sizeof(decltype(val))];
        std::memcpy(fielddata, reinterpret_cast<unsigned char *>(&val), sizeof(decltype(val)));
        //DEBUGOUT("FIELDNUMB: ", fieldnumber);
        //DEBUGOUT("FIELDDATA: ", bepaald::bytesToHexString(fielddata, sizeof(decltype(val))));

        // this used to say sizeof(sizeof(decltype(val))), I assumed it was a mistake
        framedata->push_back(std::make_tuple(fieldnumber, fielddata, sizeof(decltype(val))));
        // processed is set in getVarInt
        break;
      }
    case FIXED32:
      {
        //std::cout << "BIT32 TYPE" << std::endl;
        unsigned int length = 4;
        if (processed + length > l) // more then we have
          return false;
        processed += length; // ????
        break;
      }
    case FIXED64:
      {
        unsigned int length = 8;
        if (processed + length > l) // more then we have
          return false;
        unsigned char *fielddata = new unsigned char[length];
        std::memcpy(fielddata, data + processed, length);
        //DEBUGOUT("FIELDNUMB: ", fieldnumber);
        //DEBUGOUT("FIELDDATA: ", bepaald::bytesToHexString(fielddata, length));
        framedata->push_back(std::make_tuple(fieldnumber, fielddata, length));
        processed += length;
        break;
      }
    case STARTTYPE:
      {
        //std::cout << "GOT STARTTYPE" << std::endl;
        unsigned int length = 0;
        processed += length; // ????
        break;
      }
    case ENDTYPE:
      {
        //std::cout << "GOT ENDTYPE" << std::endl;
        unsigned int length = 0;
        processed += length; // ????
        break;
      }
    }
    //DEBUGOUT("Offset: ", processed);
  }
  //DEBUGOUT("PROCESSED ", processed, " OUT OF ", l, " BYTES!");
  return (processed == l);
}
