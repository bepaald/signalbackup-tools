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

#ifndef BASE64_H_
#define BASE64_H_

#include <openssl/evp.h>
#include <memory>
#include <cstring>
#include <string>

#include "../logger/logger.h"
#include "../common_bytes.h"

struct Base64
{
 public:
  inline static std::string bytesToBase64String(unsigned char const *data, size_t size);
  template <typename T>
  inline static std::pair<unsigned char*, size_t> base64StringToBytes(T const &str,
                                                                      typename std::enable_if<std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>>::type *dummy = nullptr);
};

inline std::string Base64::bytesToBase64String(unsigned char const *data, size_t size)
{
  int base64length = ((4 * size / 3) + 3) & ~3;
  std::unique_ptr<unsigned char[]> output(new unsigned char[base64length + 1]); // +1 for terminating null
  if (EVP_EncodeBlock(output.get(), data, size) != base64length)
  {
    Logger::error("Failed to base64 encode data");
    return std::string();
  }
  return std::string(reinterpret_cast<char *>(output.get()), base64length);
}

template <typename T>
inline std::pair<unsigned char*, size_t> Base64::base64StringToBytes(T const &str,
                                                                     typename std::enable_if<std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>>::type *)
{
  int binarylength = str.size() / 4 * 3;
  std::unique_ptr<unsigned char[]> output(new unsigned char[binarylength]);
  if (EVP_DecodeBlock(output.get(), reinterpret_cast<unsigned const char *>(str.data()), str.size()) == -1)
  {
    Logger::error("Failed to base64 decode data: ",
                  bepaald::bytesToHexString(reinterpret_cast<unsigned char const *>(str.data()), str.size()));
    return {nullptr, 0};
  }
  if (str.empty() || str.back() != '=')
    return {output.release(), binarylength};

  int realsize = binarylength - 1;
  if (str.size() >= 2 && str[str.size() - 2] == '=')
    realsize = binarylength - 2;

  unsigned char *unpaddedoutput = new unsigned char[realsize];
  std::memcpy(unpaddedoutput, output.get(), realsize);
  return {unpaddedoutput, realsize};
}

#endif
