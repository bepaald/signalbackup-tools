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

#ifndef BASE64_H_
#define BASE64_H_

#ifndef USE_CRYPTOPP
#include <openssl/evp.h>
#include <memory>
#include <cstring>
#else
#include <cryptopp/base64.h>
#endif

#include <string>

struct Base64
{
 public:
  inline static std::string bytesToBase64String(unsigned char const *data, size_t size);
  inline static std::pair<unsigned char*, size_t> base64StringToBytes(std::string const &str);
};


#include "../common_be.h"

inline std::string Base64::bytesToBase64String(unsigned char const *data, size_t size)
{
#ifndef USE_CRYPTOPP
  int base64length = ((4 * size / 3) + 3) & ~3;
  std::unique_ptr<unsigned char[]> output(new unsigned char[base64length + 1]); // +1 for terminating null
  if (EVP_EncodeBlock(output.get(), data, size) != base64length)
  {
    std::cout << "Failed to base64enc data" << std::endl;
    return std::string();
  }
  return std::string(reinterpret_cast<char *>(output.get()), base64length);
#else
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
#endif
}

inline std::pair<unsigned char*, size_t> Base64::base64StringToBytes(std::string const &str)
{
#ifndef USE_CRYPTOPP
  int binarylength = str.size() / 4 * 3;
  std::unique_ptr<unsigned char[]> output(new unsigned char[binarylength]);
  if (EVP_DecodeBlock(output.get(), reinterpret_cast<unsigned const char *>(str.data()), str.size()) == -1)
  {
    std::cout << "failed to base64dec data" << std::endl;
    return {nullptr, 0};
  }
  if (str.back() != '=')
    return {output.release(), binarylength};

  int realsize = binarylength - 1;
  if (str[str.size() - 2] == '=')
    realsize = binarylength - 2;

  unsigned char *unpaddedoutput = new unsigned char[realsize];
  std::memcpy(unpaddedoutput, output.get(), realsize);
  return {unpaddedoutput, realsize};
#else
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
#endif
}

#endif
