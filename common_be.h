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
#include <cstdint>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <ctime>
#if __cpp_lib_format >= 201907L
#include <format>
#endif

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

using std::literals::string_literals::operator""s;

namespace bepaald
{
#if defined DEBUGMSG || DEBUGISSUE
  template<typename ...Args>
  inline void log(Args && ...args);
#endif
  template <typename T, typename S>
  T toNumber(S const &str, T def = 0, typename std::enable_if<std::is_integral<T>::value && (std::is_same_v<S, std::string> || std::is_same_v<S, std::string_view>)>::type *dummy = nullptr);
  template <typename T>
  T toNumber(std::string const &str, T def = 0, typename std::enable_if<!std::is_integral<T>::value>::type *dummy = nullptr);
  template <typename T>
  T toNumberFromHex(std::string const &str, T def = 0);
  template <typename P, typename T>
  void destroyPtr(P **p, T *psize);
  template <typename T>
  inline std::string toString(T const &num, typename std::enable_if<std::is_integral<T>::value>::type *dummy = nullptr);
  template <typename T>
  inline std::string toHexString(T const &num, typename std::enable_if<std::is_integral<T>::value>::type *dummy = nullptr);
  inline std::string toString(double num);
  inline constexpr int strlitLength(char const *str, int pos = 0);
  inline int strlitLength(std::string const &str);
  inline int numDigits(long long int num);
  inline std::string toDateString(std::time_t epoch, std::string const &format);
  inline std::string toLower(std::string s);
  inline std::string toUpper(std::string s);
  inline void replaceAll(std::string *in, char from, std::string const &to);
  inline void replaceAll(std::string *in, std::string const &from, std::string const &to);

  template <typename T, typename I>
  class container_has_contains
  {
    template<typename> static std::false_type test(...);
    template<typename U> static constexpr auto test(int) -> decltype(std::declval<U>().contains(std::declval<I>()), std::true_type());
   public:
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
  };

  template <typename T, typename I>
  class container_has_find
  {
    template<typename> static std::false_type test(...);
    template<typename U> static constexpr auto test(int) -> decltype(std::declval<U>().find(std::declval<I>()), std::true_type());
   public:
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
  };

  template <typename T, typename I>
  inline constexpr bool contains(T const &container, I const &item, typename std::enable_if<!std::is_pointer<T>::value>::type *dummy [[maybe_unused]] = nullptr)
  {
    if constexpr (container_has_contains<T, I>::value)
      return container.contains(item);
    else if constexpr (container_has_find<T, I>::value)
      return container.find(item) != container.end();
    else
      return std::find(container.begin(), container.end(), item) != container.end();
  }

  template <typename T, typename I>
  inline constexpr bool contains(T const *const container, I const &item)
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

#if defined DEBUGMSG || DEBUGISSUE
template<typename ...Args>
inline void bepaald::log(Args && ...args)
{
  std::cout.copyfmt(std::ios(nullptr));
  (std::cout << ... << args) << std::endl;
}
#endif

//#include <iostream>
template <typename T, typename S>
T bepaald::toNumber(S const &str, T def, typename std::enable_if<std::is_integral<T>::value && (std::is_same_v<S, std::string> || std::is_same_v<S, std::string_view>)>::type *)
{
  if (str.empty()) [[unlikely]]
    return def;

  int sign = 1;
  int lowestpos = 0;

  if (str[0] == '-') [[unlikely]]
  {
    ++lowestpos;
    sign = -1;
  }

  T value = 0;
  T multi = 1;
  for (int i = str.size() - 1; i >= lowestpos; --i)
  {
    value += static_cast<T>((str[i] - '0')) * multi;
    multi *= 10;
    if (str[i] > '9' || value < 0) [[unlikely]]
      return def;
  }
  return value * sign;
  // std::istringstream s(str);
  // T i = def;
  // if (!(s >> i)) [[unlikely]]
  //   return def;
  // return i;
}

// non-integral to number, not ever called I dont think
template <typename T>
T bepaald::toNumber(std::string const &str, T def, typename std::enable_if<!std::is_integral<T>::value>::type *)
{
  std::istringstream s(str);
  T i = def;
  if (!(s >> i)) [[unlikely]]
    return def;
  return i;
}

template <typename T>
T bepaald::toNumberFromHex(std::string const &str, T def)
{
  std::istringstream s(str);
  T i = def;
  if (!(s >> std::hex >> i >> std::dec)) [[unlikely]]
    return def;
  return i;
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
inline std::string bepaald::toString(T const &num, typename std::enable_if<std::is_integral<T>::value>::type *)
{
  return std::to_string(num);
  //std::ostringstream oss;
  //oss << std::dec << num;
  //return oss.str();
}

template <typename T>
inline std::string bepaald::toHexString(T const &num, typename std::enable_if<std::is_integral<T>::value>::type *)
{
#if __cpp_lib_format >= 201907L
  return std::format("{:x}", num);
#else
  std::ostringstream oss;
  oss << std::hex << num << std::dec;
  return oss.str();
#endif
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
#include <iostream>

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
