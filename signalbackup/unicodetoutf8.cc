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

std::string SignalBackup::unicodeToUtf8(uint32_t unicode) const
{
  std::string utf8;
  if (unicode <= 0x7F) // 1 byte char
    utf8 = static_cast<uint8_t>(unicode);
  else if (unicode <= 0x7FF)
  {
    utf8  = static_cast<uint8_t>((unicode >> 6)         | 0b11000000);
    utf8 += static_cast<uint8_t>((unicode & 0b00111111) | 0b10000000);
  }
  else if (unicode <= 0xFFFF)
  {
    utf8  = static_cast<uint8_t>((unicode >> 12)               | 0b11100000);
    utf8 += static_cast<uint8_t>(((unicode >> 6) & 0b00111111) | 0b10000000);
    utf8 += static_cast<uint8_t>((unicode        & 0b00111111) | 0b10000000);
  }
  else // if (unicode <= 0x10FFF)
  {
    utf8  = static_cast<uint8_t>((unicode >> 18)                | 0b11110000);
    utf8 += static_cast<uint8_t>(((unicode >> 12) & 0b00111111) | 0b10000000);
    utf8 += static_cast<uint8_t>(((unicode >> 6)  & 0b00111111) | 0b10000000);
    utf8 += static_cast<uint8_t>((unicode         & 0b00111111) | 0b10000000);
  }
  return utf8;
}
