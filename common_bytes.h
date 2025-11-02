/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef COMMON_BYTES_H_
#define COMMON_BYTES_H_

#if defined(__has_include) && __has_include("version")
#include <version>
#endif

#include <climits>
#include <memory>
#include <algorithm>
#include <cstddef>
#include <string>
#include <sstream>
#include <iomanip>
#if __cpp_lib_byteswap >= 202110L
#include <bit>
#endif

#include "logger/logger.h"

using std::literals::string_literals::operator""s;

namespace bepaald
{
  template <typename T>
  inline T swap_endian(T u);
  inline std::string bytesToHexString(std::pair<std::unique_ptr<unsigned char []>, unsigned int> const &data, bool unformatted = false);
  inline std::string bytesToHexString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted = false);
  inline std::string bytesToHexString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted = false);
  inline std::string bytesToHexString(std::unique_ptr<unsigned char []> const &data, unsigned int length, bool unformatted = false);
  inline std::string bytesToHexString(std::shared_ptr<unsigned char []> const &data, unsigned int length, bool unformatted = false);
  inline std::string bytesToHexString(unsigned char const *data, unsigned int length, bool unformatted = false);
  inline std::string bytesToString(unsigned char const *data, unsigned int length);
  inline std::string bytesToPrintableString(unsigned char const *data, unsigned int length);
  inline bool hexStringToBytes(unsigned char const *in, uint64_t insize, unsigned char *out, uint64_t outsize);
  inline bool hexStringToBytes(std::string const &in, unsigned char *out, uint64_t outsize);
  template <typename T, typename U>
  inline T reinterpret(U const *u);
}

template <typename T>
inline T bepaald::swap_endian(T u)
{
#if __cpp_lib_byteswap >= 202110L
  return std::byteswap(u);
#else
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

  union
  {
    T u;
    unsigned char u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); ++k)
    dest.u8[k] = source.u8[sizeof(T) - k - 1];

  return dest.u;
#endif
}

inline std::string bepaald::bytesToHexString(std::pair<std::unique_ptr<unsigned char []>, unsigned int> const &data, bool unformatted)
{
  return bytesToHexString(data.first.get(), data.second, unformatted);
}

inline std::string bepaald::bytesToHexString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted)
{
  return bytesToHexString(data.first.get(), data.second, unformatted);
}

inline std::string bepaald::bytesToHexString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted/* = false*/)
{
  return bytesToHexString(data.first, data.second, unformatted);
}

inline std::string bepaald::bytesToHexString(std::unique_ptr<unsigned char []> const &data, unsigned int length, bool unformatted)
{
  return bytesToHexString(data.get(), length, unformatted);
}

inline std::string bepaald::bytesToHexString(std::shared_ptr<unsigned char []> const &data, unsigned int length, bool unformatted)
{
  return bytesToHexString(data.get(), length, unformatted);
}

inline std::string bepaald::bytesToHexString(unsigned char const *data, unsigned int length, bool unformatted/* = false*/)
{
  std::ostringstream oss;
  if (!unformatted)
    oss << "(hex:) ";
  for (unsigned int i = 0; i < length; ++i)
    oss << std::hex << std::setfill('0') << std::setw(2)
        << (static_cast<int32_t>(data[i]) & 0xFF)
        << ((i == length - 1 || unformatted) ? "" : " ");
  return oss.str();
}

inline std::string bepaald::bytesToString(unsigned char const *data, unsigned int length)
{
  return std::string(reinterpret_cast<char const *>(data), length);
  /*
    // this may have been the stupidest bit of code in this project
    // saved here for all to see
  std::ostringstream oss;
  for (unsigned int i = 0; i < length; ++i)
    oss << static_cast<char>(data[i]);
  return oss.str();
  */
}

inline std::string bepaald::bytesToPrintableString(unsigned char const *data, unsigned int length)
{
  bool prevwashex = false;
  std::ostringstream oss;
  for (unsigned int i = 0; i < length; ++i)
  {
    bool curishex = !std::isprint(static_cast<char>(data[i]));

    if (curishex != prevwashex && i > 0)
      oss << " ";

    if (curishex)
      oss << "0x" << std::hex << std::setfill('0') << std::setw(2)
          << (static_cast<int32_t>(data[i]) & 0xFF)
          << (i == length - 1 ? "" : " ");
    else
      oss << static_cast<char>(data[i]);

    prevwashex = curishex;
  }
  return oss.str();
}

inline bool bepaald::hexStringToBytes(unsigned char const *in, uint64_t insize, unsigned char *out, uint64_t outsize)
{
  if (insize % 2 ||
      outsize != insize / 2) [[unlikely]]
  {
    Logger::error("Invalid size for hex string or output array too small");
    out = nullptr;
    return false;
  }

  auto charToInt = [] (unsigned char c)
  {
    if (c <= '9' && c >= '0')
      return c - '0';
    if (c <= 'F' && c >= 'A')
      return c - 'A' + 10;
    // if (c <= 'f' && c >= 'a') // lets assume input is valid...
    return c - 'a' + 10;
  };

  uint64_t outpos = 0;
  for (unsigned int i = 0; i < insize - 1; i += 2)
    out[outpos++] = charToInt(in[i]) * 16 + charToInt(in[i + 1]);

  return true;
}

inline bool bepaald::hexStringToBytes(std::string const &in, unsigned char *out, uint64_t outsize)
{
  // sanitize input;
  std::string input = in;
  auto newend = std::remove_if(input.begin(), input.end(), [](char c) {
    return (c > '9' || c < '0') && (c > 'F' || c < 'A') && (c > 'f' || c < 'a'); });
  input.erase(newend, input.end());

  return hexStringToBytes(reinterpret_cast<unsigned char const *>(input.c_str()), input.size(), out, outsize);
}

template <typename T, typename U>
inline T bepaald::reinterpret(U const *u)
{
  T t;
  std::memcpy(&t, u, sizeof(T));
  return t;
}

#endif
