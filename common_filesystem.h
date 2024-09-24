#ifndef COMMON_FILESYSTEM_H_
#define COMMON_FILESYSTEM_H_

#include <filesystem>

using std::literals::string_literals::operator""s;

namespace bepaald
{
  inline bool fileOrDirExists(std::string const &path);
  inline bool fileOrDirExists(std::filesystem::path const &path);
  inline bool isDir(std::string const &path);
  inline bool createDir(std::string const &path);
  inline bool isEmpty(std::string const &path);
  inline bool clearDirectory(std::string const &path);
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

#endif
