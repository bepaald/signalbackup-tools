/*
    Copyright (C) 2019-2020  Selwin van Dijk

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

#define BASE64_H_

#include <cryptopp/base64.h>
#include <string>

struct Base64
{
 public:
  inline static std::string bytesToBase64String(unsigned char const *data, size_t size);
  inline static std::pair<unsigned char*, size_t> base64StringToBytes(std::string const &str);
};

inline std::string Base64::bytesToBase64String(unsigned char const *data, size_t size)
{
  CryptoPP::Base64Encoder b64e(nullptr, false);
  b64e.Put(data, size);
  b64e.MessageEnd();
  uint64_t length = b64e.MaxRetrievable();
  if (length)
  {
    unsigned char *str = new unsigned char[length];
    b64e.Get(str, length);
    std::string result(reinterpret_cast<char *>(str), length);
    delete[] str;
    return result;
  }
  return std::string();
}

inline std::pair<unsigned char*, size_t> Base64::base64StringToBytes(std::string const &str)
{
  CryptoPP::Base64Decoder decoder;
  decoder.Put(reinterpret_cast<unsigned const char *>(str.data()), str.size());
  decoder.MessageEnd();

  uint64_t size = decoder.MaxRetrievable();
  if (size && size <= SIZE_MAX)
  {
    unsigned char *data = new unsigned char[size];
    decoder.Get(data, size);
    return {data, size};
  }
  return {nullptr, 0};
}

#endif
