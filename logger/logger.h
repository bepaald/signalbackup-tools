/*
  Copyright (C) 2023  Selwin van Dijk

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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include "../common_be.h"

class Logger
{
 public:
  enum class Control
  {
    BOLD,
    NORMAL,
  };
 private:
  static std::unique_ptr<Logger> s_instance;
  std::ofstream *d_file;
  bool d_usetimestamps;
  bool d_used;
  bool d_controlcodessupported;
  bool d_overwriting;
 public:
  inline static void setFile(std::string const &f);
  inline static void setTimestamp(bool val);

  template <typename First, typename... Rest>
  inline static void message_overwrite(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void message(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void warning(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void warning_indent(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void error(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void error_indent(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void output_indent(int indent, First const &f, Rest... r);

  inline ~Logger();
 private:
  inline Logger();
  inline static void ensureLogger();
  inline static void firstUse();
  inline static std::ostream &dispTime(std::ostream &stream);
  static void outputHead(std::string const &file, std::string const &stdandardout, bool overwrite = false,
                                std::pair<std::string, std::string> const &control = std::pair<std::string, std::string>());
  static void outputHead(std::string const &head, bool overwrite = false,
                                std::pair<std::string, std::string> const &control = std::pair<std::string, std::string>());

  template <typename First, typename... Rest>
  inline static void outputMsg(bool overwrite, First const &f, Rest... r);
  template <typename T>
  inline static void outputMsg(bool overwrite, T const &t);

  // specializations for vector type
  template <typename T, typename... Rest>
  inline static void outputMsg(bool overwrite, std::vector<T> const &vec, Rest... r);
  template <typename T>
  inline static void outputMsg(bool overwrite, std::vector<T> const &vec);

  // specializations for control
  template <typename... Rest>
  inline static void outputMsg(bool overwrite, Control c, Rest... r);
  inline static void outputMsg(bool overwrite, Control c);

  Logger(Logger const &other) = delete;            // NI
  Logger &operator=(Logger const &other) = delete; // NI
};

inline Logger::Logger()
  :
  d_file(nullptr),
  d_usetimestamps(false),
  d_used(false),
  d_controlcodessupported(bepaald::isTerminal() && bepaald::supportsAnsi()),
  d_overwriting(false)
{}

inline Logger::~Logger()
{
  if (d_file)
  {
    (*d_file) << std::flush;
    d_file->close();
    delete d_file;
  }
}

inline void Logger::ensureLogger() // static
{
  if (!s_instance.get())
    s_instance.reset(new Logger);
}

// prints out a header containing current date and time
inline void Logger::firstUse() // static
{
  if (!s_instance->d_used)
  {
    s_instance->d_used = true;

    std::time_t cur = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (s_instance->d_file)
      *(s_instance->d_file) << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " *** " << "\n";
    std::cout << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " *** " << std::endl;
  }
}

inline std::ostream &Logger::dispTime(std::ostream &stream) // static
{
  std::time_t cur = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  // replace rest with when put_time is there...
  //return stream  << std::put_time(std::localtime(&cur), "%T"); // %F and %T do not work on mingw
  return stream << std::put_time(std::localtime(&cur), "%Y-%m-%d %H:%M:%S");
}

inline void Logger::setFile(std::string const &f) // static
{
  ensureLogger();
  if (s_instance->d_file)
    return;
  s_instance->d_file = new std::ofstream(f);
  firstUse();
}

inline void Logger::setTimestamp(bool val) // static
{
  ensureLogger();
  s_instance->d_usetimestamps = val;
  firstUse();
}

template <typename First, typename... Rest>
inline void Logger::message_overwrite(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  s_instance->d_overwriting = true;

  outputHead("", true);
  outputMsg(true, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::message(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  //outputHead("[MESSAGE] ", "[MESSAGE] ");
  outputHead("", true);
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::warning(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  //outputHead("[WARNING] ", "[\033[38;5;37mWARNING\033[0m] ");
  outputHead("Warning", false, std::make_pair<std::string, std::string>("\033[1m", "\033[0m"));
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::warning_indent(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  outputHead("       ", false);
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::error(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  //outputHead("[ ERROR ] ", "[ \033[1;31mERROR\033[0m ] ");
  outputHead("Error", false, std::make_pair<std::string, std::string>("\033[1m", "\033[0m"));
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::error_indent(First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  outputHead("     ", false);
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::output_indent(int indent, First const &f, Rest... r) // static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
  }

  outputHead(std::string(indent, ' '));
  outputMsg(false, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::outputMsg(bool overwrite, First const &f, Rest... r) // static
{
  if (s_instance->d_file)
    *(s_instance->d_file) << f;

  std::cout << f;
  outputMsg(overwrite, r...);
}

template <typename T>
inline void Logger::outputMsg(bool overwrite, T const &t) // static
{
  if (s_instance->d_file)
    *(s_instance->d_file) << t << "\n";

  if (overwrite) [[unlikely]]
    std::cout << t << std::flush;
  else
    std::cout << t << std::endl;
}

template <typename T, typename... Rest>
inline void Logger::outputMsg(bool overwrite, std::vector<T> const &vec, Rest... r) // static
{
  if (s_instance->d_file)
  {
    for (uint i = 0; i < vec.size(); ++i)
      *(s_instance->d_file) << vec[i] << ((i < vec.size() - 1) ? "," : "");
  }

  for (uint i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ((i < vec.size() - 1) ? "," : "");

  outputMsg(overwrite, r...);
}

template <typename T>
inline void Logger::outputMsg(bool overwrite, std::vector<T> const &vec) // static
{
  if (s_instance->d_file)
  {
    for (uint i = 0; i < vec.size(); ++i)
      *(s_instance->d_file) << vec[i] << ((i < vec.size() - 1) ? "," : "");
    *(s_instance->d_file) << "\n";
  }

  for (uint i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ((i < vec.size() - 1) ? "," : "");
  if (overwrite) [[unlikely]]
    std::cout << std::flush;
  else
    std::cout << std::endl;
}

template <typename... Rest>
inline void Logger::outputMsg(bool overwrite, Control c, Rest... r) // static
{
  // (no control codes to file)

  if (s_instance->d_controlcodessupported)
  {
    switch(c)
    {
      case Control::BOLD:
        std::cout << "\033[1m";
        break;
      case Control::NORMAL:
        std::cout << "\033[0m";
        break;
    }
  }

  outputMsg(overwrite, r...);
}

inline void Logger::outputMsg(bool overwrite, Control c) // static
{
  // (no control codes to file)

  if (s_instance->d_controlcodessupported)
  {
    switch(c)
    {
      case Control::BOLD:
        std::cout << "\033[1m";
        break;
      case Control::NORMAL:
        std::cout << "\033[0m";
        break;
    }
  }
  if (overwrite) [[unlikely]]
    std::cout << std::flush;
  else
    std::cout << std::endl;
}

#endif
