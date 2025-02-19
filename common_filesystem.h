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

#ifndef COMMON_FILESYSTEM_H_
#define COMMON_FILESYSTEM_H_

#include <filesystem>

#include "logger/logger.h"

using std::literals::string_literals::operator""s;

#if defined(_WIN32) || defined(__MINGW64__)
//#define WIN_LONGPATH(...) bepaald::windows_long_file( __VA_ARGS__ )
#define WIN_LONGPATH(...) __VA_ARGS__
#else
#define WIN_LONGPATH(...) __VA_ARGS__
#endif

namespace bepaald
{
  inline bool fileOrDirExists(std::string const &path);
  inline bool fileOrDirExists(std::filesystem::path const &path);
  inline bool isDir(std::string const &path);
  inline bool createDir(std::string const &path);
  inline bool isEmpty(std::string const &path);
  inline bool clearDirectory(std::string const &path);
  inline uint64_t fileSize(std::string const &path);
#if defined(_WIN32) || defined(__MINGW64__)
  inline std::string windows_long_file(std::string const &path);
  inline long long int abs_path_length(std::string const &path);
#endif
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

inline uint64_t bepaald::fileSize(std::string const &path)
{
  std::error_code ec;
  return std::filesystem::file_size(std::filesystem::path(path), ec);
}

#if defined(_WIN32) || defined(__MINGW64__)
inline std::string bepaald::windows_long_file(std::string const &path)
{
  //Logger::message("WINDOWS_LONG_PATH: input \"", path, "\"");

  std::error_code ec;
  auto abs_path = std::filesystem::absolute(path, ec);
  if (ec)
  {
    Logger::error("Failed to get an absolute path for '", path, "'");
    return path;
  }

  //Logger::message("WINDOWS_LONG_PATH: output \"", "\\\\?\\" + abs_path.string(), "\"");

  // prepend windows magic long path prefix:
  return R"(\\?\)" + abs_path.string();
}
#endif

#if defined(_WIN32) || defined(__MINGW64__)
inline long long int bepaald::abs_path_length(std::string const &path)
{
  //Logger::message("WINDOWS_LONG_PATH: input \"", path, "\"");

  std::error_code ec;
  auto abs_path = std::filesystem::absolute(path, ec);
  if (ec)
  {
    Logger::error("Failed to get an absolute path for '", path, "'");
    return -1;
  }

  //Logger::message("WINDOWS_PATH_LENGTH: \"", abs_path.string(), "\", ", abs_path.string().size());

  return abs_path.string().size();
}
#endif

#endif
