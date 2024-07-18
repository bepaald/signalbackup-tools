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

#ifndef COMMON_BE_H_
#define COMMON_BE_H_

#include <cstddef>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <ctime>
#if __cpp_lib_byteswap >= 202110L
#include <bit>
#endif

#include "logger/logger.h"

#ifdef DEBUGMSG
#define DEBUGOUT(...) bepaald::log("[DEBUG] : ", __PRETTY_FUNCTION__," : ", __VA_ARGS__);
#else
#define DEBUGOUT(...)
#endif

#ifdef DEBUGISSUE
#define DEBUGOUT2(...) bepaald::log("[DEBUG] : ", __VA_ARGS__);
#else
#define DEBUGOUT2(...)
#endif

#define STRLEN( STR ) (bepaald::strlitLength(STR))

#if __cpp_lib_starts_ends_with >= 201711L
#define STRING_STARTS_WITH( STR, SUB ) ( STR.starts_with(SUB) )
#else
#define STRING_STARTS_WITH( STR, SUB ) ( STR.substr(0, STRLEN(SUB)) == SUB )
#endif

typedef unsigned int uint;

using std::literals::string_literals::operator""s;

namespace bepaald
{
  template <typename T>
  inline T swap_endian(T u);
#if defined DEBUGMSG || DEBUGISSUE
  template<typename ...Args>
  inline void log(Args && ...args);
#endif
  template <typename T>
  T toNumber(std::string const &str, T def = 0);
  std::string bytesToHexString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted = false);
  std::string bytesToHexString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted = false);
  std::string bytesToHexString(unsigned char const *data, unsigned int length, bool unformatted = false);
  std::string bytesToString(unsigned char const *data, unsigned int length);
  std::string bytesToPrintableString(unsigned char const *data, unsigned int length);
  inline bool hexStringToBytes(unsigned char const *in, uint64_t insize, unsigned char *out, uint64_t outsize);
  inline bool hexStringToBytes(std::string const &in, unsigned char *out, uint64_t outsize);
  template <typename P, typename T>
  void destroyPtr(P **p, T *psize);
  template <typename T>
  inline std::string toString(T const &num, bool hex = false, typename std::enable_if<std::is_integral<T>::value>::type *dummy = nullptr);
  inline std::string toString(double num);
  inline constexpr int strlitLength(char const *str, int pos = 0);
  inline int strlitLength(std::string const &str);
  inline bool fileOrDirExists(std::string const &path);
  inline bool fileOrDirExists(std::filesystem::path const &path);
  inline bool isDir(std::string const &path);
  inline bool createDir(std::string const &path);
  inline bool isEmpty(std::string const &path);
  inline bool clearDirectory(std::string const &path);
  inline int numDigits(long long int num);
  // inline bool supportsAnsi();
  // inline bool isTerminal();
  //inline std::ostream &bold_on(std::ostream &os);
  //inline std::ostream &bold_off(std::ostream &os);
  inline std::string toDateString(std::time_t epoch, std::string const &format);
  inline std::string toLower(std::string s);
  inline std::string toUpper(std::string s);
  inline void replaceAll(std::string *in, char from, std::string const &to);
  inline void replaceAll(std::string *in, std::string const &from, std::string const &to);

  template <typename T, typename I>
  class container_has_contains
  {
    template<typename> static std::false_type test(...);
    template<typename U> static auto test(int) -> decltype(std::declval<U>().contains(std::declval<I>()), std::true_type());
   public:
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
  };

  template <typename T, typename I>
  class container_has_find
  {
    template<typename> static std::false_type test(...);
    template<typename U> static auto test(int) -> decltype(std::declval<U>().find(std::declval<I>()), std::true_type());
   public:
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
  };

  template <typename T, typename I>
  inline bool contains(T const &container, I const &item, typename std::enable_if<!std::is_pointer<T>::value>::type *dummy [[maybe_unused]] = nullptr)
  {
    if constexpr (container_has_contains<T, I>::value)
      return container.contains(item);
    else if constexpr (container_has_find<T, I>::value)
      return container.find(item) != container.end();
    else
      return std::find(container.begin(), container.end(), item) != container.end();
  }

  template <typename T, typename I>
  inline bool contains(T const *const container, I const &item)
  {
    if constexpr (container_has_contains<T, I>::value)
      return container->contains(item);
    else if constexpr (container_has_find<T, I>::value)
      return container->find(item) != container->end();
    else
      return std::find(container->begin(), container->end(), item) != container->end();
  }

  template <typename T, typename U>
  inline int findIdxOf(T const &container, U const &value);
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

#if defined DEBUGMSG || DEBUGISSUE
template<typename ...Args>
inline void bepaald::log(Args && ...args)
{
  std::cout.copyfmt(std::ios(nullptr));
  (std::cout << ... << args) << std::endl;
}
#endif

template <typename T>
T bepaald::toNumber(std::string const &str, T def)
{
  std::istringstream s(str);
  T i = def;
  if (!(s >> i)) [[unlikely]]
    return def;
  return i;
}

inline std::string bepaald::bytesToHexString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted)
{
  return bytesToHexString(data.first.get(), data.second, unformatted);
}

inline std::string bepaald::bytesToHexString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted/* = false*/)
{
  return bytesToHexString(data.first, data.second, unformatted);
}

inline std::string bepaald::bytesToHexString(unsigned char const *data, unsigned int length, bool unformatted/* = false*/)
{
  std::ostringstream oss;
  if (!unformatted)
    oss << "(hex:) ";
  for (uint i = 0; i < length; ++i)
    oss << std::hex << std::setfill('0') << std::setw(2)
        << (static_cast<int32_t>(data[i]) & 0xFF)
        << ((i == length - 1 || unformatted) ? "" : " ");
  return oss.str();
}

inline std::string bepaald::bytesToString(unsigned char const *data, unsigned int length)
{
  std::ostringstream oss;
  for (uint i = 0; i < length; ++i)
    oss << static_cast<char>(data[i]);
  return oss.str();
}

inline std::string bepaald::bytesToPrintableString(unsigned char const *data, unsigned int length)
{
  bool prevwashex = false;
  std::ostringstream oss;
  for (uint i = 0; i < length; ++i)
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
      outsize != insize / 2)
  {
    Logger::error("Invalid size for hex string or output array too small");
    return false;
  }

  auto charToInt = [] (char c)
  {
    if (c <= '9' && c >= '0')
      return c - '0';
    if (c <= 'F' && c >= 'A')
      return c - 'A' + 10;
    // if (c <= 'f' && c >= 'a') // lets assume input is valid...
    return c - 'a' + 10;
  };

  uint64_t outpos = 0;
  for (uint i = 0; i < insize - 1; i += 2)
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

template <typename P, typename T>
inline void bepaald::destroyPtr(P **p, T *psize)
{
  if (*p)
  {
    delete[] *p;
    *p = nullptr;
    *psize = 0;
  }
}

template <typename T>
inline std::string bepaald::toString(T const &num, bool hex, typename std::enable_if<std::is_integral<T>::value>::type *)
{
  std::ostringstream oss;
  oss << (hex ? std::hex : std::dec) << num << std::dec;
  return oss.str();
}

inline std::string bepaald::toString(double num)
{
  std::ostringstream oss;
  oss << std::defaultfloat << std::setprecision(17) << num;
  return oss.str();
}

inline constexpr int bepaald::strlitLength(char const *str, int pos)
{
  return str[pos] == '\0' ? 0 : 1 + strlitLength(str, pos + 1);
}

inline int bepaald::strlitLength(std::string const &str)
{
  return str.size();
}

inline bool bepaald::fileOrDirExists(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::exists(path, ec);
}

inline bool bepaald::fileOrDirExists(std::filesystem::path const &path)
{
  std::error_code ec;
  return std::filesystem::exists(path, ec);
}

inline bool bepaald::isDir(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::is_directory(path, ec);
}

inline bool bepaald::createDir(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::create_directory(path, ec);
}

inline bool bepaald::isEmpty(std::string const &path)
{
  std::error_code ec;
  for (auto const &p: std::filesystem::directory_iterator(path))
    if (p.exists(ec))
      return false;
  return true;
}

inline bool bepaald::clearDirectory(std::string const &path)
{
  std::error_code ec;
  for (auto const &p: std::filesystem::directory_iterator(path))
    if (std::filesystem::remove_all(p.path(), ec) == static_cast<std::uintmax_t>(-1))
      return false;
  return true;
}

inline int bepaald::numDigits(long long int num)
{
  int count = 0;
  while (num)
  {
    num /= 10;
    ++count;
  }
  return count;
}

inline std::string bepaald::toDateString(std::time_t epoch, std::string const &format)
{
  std::ostringstream tmp;
  tmp << std::put_time(std::localtime(&epoch), format.c_str());
  return tmp.str();
}

inline std::string bepaald::toLower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
  return s;
}

inline std::string bepaald::toUpper(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
  return s;
}

inline void bepaald::replaceAll(std::string *in, char from, std::string const &to)
{
  replaceAll(in, std::string(1, from), to);
}

inline void bepaald::replaceAll(std::string *in, std::string const &from, std::string const &to)
{
  size_t start_pos = 0;
  while ((start_pos = in->find(from, start_pos)) != std::string::npos)
  {
    in->replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

template <typename T, typename U>
inline int bepaald::findIdxOf(T const &container, U const &value)
{
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end())
    return -1;
  return std::distance(container.begin(), it);
}

#ifdef SIGNALBACKUP_TOOLS_REPORT_MEM
#define MEMINFO(...) process_mem_usage(__VA_ARGS__)

#include <unistd.h>
#include <fstream>
#include <string>

// code adapted from https://stackoverflow.com/questions/669438 by Don Wakefield
template<typename ...Args>
void process_mem_usage(Args && ...args)
{
  std::cout.copyfmt(std::ios(nullptr));
  (std::cout << ... << args) << std::endl;

  // the two fields we want
  unsigned long vsize = 0;
  long rss = 0;
  {
  // dummy vars for leading entries in stat that we don't care about
  std::string dummy;
  // 'file' stat seems to give the most reliable results
  std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);
  stat_stream >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy
              >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy
              >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy
              >> dummy >> dummy >> dummy >> dummy >> vsize >> rss; // don't care about the rest
  }

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  double vm_usage     = vsize / 1024.0;
  double resident_set = rss * page_size_kb;

  std::cout << " *** VM: " << std::fixed << std::setprecision(2) << (vm_usage / 1024) << "MB ; RSS: " << (resident_set / 1024) << "MB" << std::endl;
}
#else
#define MEMINFO(...)
#endif


#endif
