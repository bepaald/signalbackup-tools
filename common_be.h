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

#if defined(__linux__) && !defined(__MINGW64__)
#include <sys/ioctl.h>
  #if __has_include("unistd.h")
  #define HAS_UNISTD_H_
  #include <unistd.h>
  #endif
#endif
#if defined(_WIN32) || defined(__MINGW64__)
#include <windows.h>
#endif

#ifdef DEBUGMSG
#define DEBUGOUT(...) bepaald::log("[DEBUG] : ", __PRETTY_FUNCTION__," : ", __VA_ARGS__);
#else
#define DEBUGOUT(...)
#endif

#define STRLEN( STR ) (std::integral_constant<int, bepaald::strlitLength(STR)>::value)

#if __cplusplus > 201703L
#define STRING_STARTS_WITH( STR, SUB ) ( STR.starts_with(SUB) )
#else
#define STRING_STARTS_WITH( STR, SUB ) ( STR.substr(0, STRLEN(SUB)) == SUB )
#endif

typedef unsigned int uint;

namespace bepaald
{
  template <typename T>
  inline T swap_endian(T u);
#ifdef DEBUG
  template<typename ...Args>
  inline void log(Args && ...args);
#endif
  template <typename T>
  T toNumber(std::string const &str);
  //std::wstring bytesToHexWString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted = false);
  //std::wstring bytesToHexWString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted = false);
  //std::wstring bytesToHexWString(unsigned char const *data, unsigned int length, bool unformatted = false);
  //std::wstring bytesToWString(unsigned char const *data, unsigned int length);
  //std::wstring bytesToPrintableWString(unsigned char const *data, unsigned int length);
  std::string bytesToHexString(std::pair<std::shared_ptr<unsigned char []>, unsigned int> const &data, bool unformatted = false);
  std::string bytesToHexString(std::pair<unsigned char *, unsigned int> const &data, bool unformatted = false);
  std::string bytesToHexString(unsigned char const *data, unsigned int length, bool unformatted = false);
  std::string bytesToString(unsigned char const *data, unsigned int length);
  std::string bytesToPrintableString(unsigned char const *data, unsigned int length);
  template <typename T>
  void destroyPtr(unsigned char **p, T *psize);
  //template <typename T>
  //inline std::wstring toWString(T const &num, typename std::enable_if<std::is_integral<T>::value >::type *dummy = nullptr);
  //inline std::wstring toWString(double num);
  template <typename T>
  inline std::string toString(T const &num, typename std::enable_if<std::is_integral<T>::value >::type *dummy = nullptr);
  inline std::string toString(double num);
  inline constexpr int strlitLength(char const *str, int pos = 0);
  inline bool fileOrDirExists(std::string const &path);
  inline bool isDir(std::string const &path);
  inline bool isEmpty(std::string const &path);
  inline bool clearDirectory(std::string const &path);
  inline int numDigits(long long int num);
  inline bool supportsAnsi();
  inline bool isTerminal();
  inline std::ostream &bold_on(std::ostream &os);
  inline std::ostream &bold_off(std::ostream &os);
}

template <typename T>
inline T bepaald::swap_endian(T u)
{
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
}

#ifdef DEBUG
template<typename ...Args>
inline void bepaald::log(Args && ...args)
{
  std::cout.copyfmt(std::ios(nullptr));
  (std::cout << ... << args) << std::endl;
}
#endif

template <typename T>
T bepaald::toNumber(std::string const &str)
{
  std::istringstream s(str);
  T i = 0;
  s >> i;
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

template <typename T>
inline void bepaald::destroyPtr(unsigned char **p, T *psize)
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
  std::ostringstream oss;
  oss << num;
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

inline bool bepaald::fileOrDirExists(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::exists(path, ec);
}

inline bool bepaald::isDir(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::is_directory(path, ec);
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
  for (auto const &p: std::filesystem::directory_iterator(path))
    if (!std::filesystem::remove(p.path()))
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

// This function was taken from https://github.com/agauniyal/rang/
// Used here to (poorly!) detect support for ansi escape codes
inline bool bepaald::supportsAnsi()
{
#if defined(_WIN32) || defined(__MINGW64__)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hConsole, &mode);
  return mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
#endif
  static const bool result = []
  {
    const char *Terms[] =
      { "ansi",    "color",  "console", "cygwin", "gnome",
        "konsole", "kterm",  "linux",   "msys",   "putty",
        "rxvt",    "screen", "vt100",   "xterm" };
    const char *env_p = std::getenv("TERM");
    if (env_p == nullptr)
      return false;
    return std::any_of(std::begin(Terms), std::end(Terms),
                       [&](const char *term) { return std::strstr(env_p, term) != nullptr; });
  }();
  return result;
}

inline bool bepaald::isTerminal()
{
#ifdef HAS_UNISTD_H_ // defined in .ih if unistd.h is available
  static const bool result = []
  {
    return isatty(STDOUT_FILENO);
  }();
  return result;
#else
#if defined(_WIN32) || defined(__MINGW64__)
  DWORD filetype = GetFileType(GetStdHandle(STD_OUTPUT_HANDLE));
  return filetype != FILE_TYPE_PIPE &&  filetype != FILE_TYPE_DISK; // this is not foolproof (eg output is printer)...
#endif
  return false;
#endif
}

inline std::ostream &bepaald::bold_on(std::ostream &os)
{
  [[unlikely]] if (!supportsAnsi() || !isTerminal())
    return os;
  return os << "\033[1m";
}

inline std::ostream &bepaald::bold_off(std::ostream &os)
{
  [[unlikely]] if (!supportsAnsi() || !isTerminal())
    return os;
  return os << "\033[0m";
}

#endif
