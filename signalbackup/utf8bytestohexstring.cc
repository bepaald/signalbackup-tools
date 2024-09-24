/*
  Copyright (C) 2024  Selwin van Dijk

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

#include "signalbackup.ih"

#include "../common_bytes.h"

std::string SignalBackup::utf8BytesToHexString(unsigned char const *const data, size_t data_size) const
{
  // NOTE THIS IS NOT GENERIC UTF-8 CONVERSION, THIS
  // DATA IS GUARANTEED TO HAVE ONLY SINGLE- AND TWO-BYTE
  // CHARS (NO 3 OR 4-BYTE). THE TWO-BYTE CHARS NEVER
  // CONTAIN MORE THAN TWO BITS OF DATA
  unsigned char output[16]{0};
  unsigned int outputpos = 0;
  for (unsigned int i = 0; i < data_size; ++i)
  {
    if (outputpos >= 16) [[unlikely]]
      return std::string();

    if ((data[i] & 0b10000000) == 0) // single byte char
      output[outputpos++] += data[i];
    else // 2 byte char
      output[outputpos++] = ((data[i] & 0b00000011) << 6) | (data[i + 1] & 0b00111111), ++i;
  }
  return bepaald::bytesToHexString(output, 16, true);
}
