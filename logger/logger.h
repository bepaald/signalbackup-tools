/*
  Copyright (C) 2023-2024  Selwin van Dijk

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
#include <sstream>
#include <chrono>
#include <cstring>
#include <vector>
#include <iomanip>
#include <ctime>

#if defined(_WIN32) || defined(__MINGW64__)
#include <windows.h>
#else // !windows
#include <sys/ioctl.h>
#if __has_include("unistd.h")
#define HAS_UNISTD_H_
#include <unistd.h>
#endif
#endif

class Logger
{
  enum Flags
  {
    NONE = 0,
    OVERWRITE = 0b1,
    NONEWLINE = 0b10,
  };

 public:
  enum class Control
  {
    BOLD,
    NORMAL,
    ENDOVERWRITE,
  };
  struct ControlChar
  {
    std::string code;
    explicit ControlChar(std::string const &c) : code(c)
    {}
  };

  template <typename T>
  struct VECTOR
  {
    std::vector<T> data;
    std::string delim;
    VECTOR(std::vector<T> const &v, std::string const &d) :
      data(v),
      delim(d)
    {}
  };

 private:
  static std::unique_ptr<Logger> s_instance;
  std::ofstream *d_file;
  std::ostringstream *d_strstreambackend;
  std::basic_ostream<std::ofstream::char_type, std::ofstream::traits_type> *d_currentoutput;
  bool d_usetimestamps;
  bool d_used;
  bool d_controlcodessupported;
  bool d_overwriting;
  //std::sstream d_previousline;
 public:
  inline static void setFile(std::string const &f);
  inline static void setTimestamp(bool val);

  template <typename First, typename... Rest>
  inline static void message_overwrite(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void message(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void message_start(First const &f, Rest... r);
  inline static void message_end();

  template <typename First, typename... Rest>
  inline static void warning(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void warning_start(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void warning_indent(First const &f, Rest... r);

  template <typename First, typename... Rest>
  inline static void error(First const &f, Rest... r);
  template <typename First, typename... Rest>
  inline static void error_start(First const &f, Rest... r);
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
  inline static void messagePre();
  void outputHead(std::string const &file, std::string const &stdandardout, bool overwrite = false,
                  std::pair<std::string, std::string> const &prepost = std::pair<std::string, std::string>(),
                  std::pair<std::string, std::string> const &control = std::pair<std::string, std::string>());
  void outputHead(std::string const &head, bool overwrite = false,
                  std::pair<std::string, std::string> const &prepost = std::pair<std::string, std::string>(),
                  std::pair<std::string, std::string> const &control = std::pair<std::string, std::string>());

  template <typename First, typename... Rest>
  inline void outputMsg(Flags flags, First const &f, Rest... r);
  template <typename T>
  inline void outputMsg(Flags flags, T const &t);

  // specializations for controlchar
  template <typename... Rest>
  inline void outputMsg(Flags flags, Logger::ControlChar const &c, Rest... r);
  inline void outputMsg(Flags flags, Logger::ControlChar const &c);

  // specializations for vector type
  template <typename T, typename... Rest>
  inline void outputMsg(Flags flags, VECTOR<T> const &vec, Rest... r);
  template <typename T>
  inline void outputMsg(Flags flags, VECTOR<T> const &vec);
  template <typename T, typename... Rest>
  inline void outputMsg(Flags flags, std::vector<T> const &vec, Rest... r);
  template <typename T>
  inline void outputMsg(Flags flags, std::vector<T> const &vec);

  // specializations for control
  template <typename... Rest>
  inline void outputMsg(Flags flags, Control c, Rest... r);
  inline void outputMsg(Flags flags, Control c);

  static bool supportsAnsi();
  static bool isTerminal();

  Logger(Logger const &other) = delete;            // NI
  Logger &operator=(Logger const &other) = delete; // NI
};

inline Logger::Logger()
  :
  d_file(nullptr),
  d_strstreambackend(nullptr),
  d_currentoutput(d_file),
  d_usetimestamps(false),
  d_used(false),
  d_controlcodessupported(isTerminal() && supportsAnsi()),
  d_overwriting(false)
{}

inline Logger::~Logger()
{
  if (d_strstreambackend)
  {
    if (d_strstreambackend->tellp() != 0)
    {
      if (d_file)
        (*d_file) << s_instance->d_strstreambackend->str();
      d_strstreambackend->str("");
      d_strstreambackend->clear();
    }
    delete d_strstreambackend;
  }
  if (d_file)
  {
    (*d_file) << std::flush;
    d_file->close();
    delete d_file;
  }
}

inline void Logger::ensureLogger() // static
{
  if (!s_instance.get()) [[unlikely]]
    s_instance.reset(new Logger);
}

// prints out a header containing current date and time
inline void Logger::firstUse() // static
{
  if (!s_instance->d_used) [[unlikely]]
  {
    s_instance->d_used = true;

    std::time_t cur = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (s_instance->d_currentoutput)
      *(s_instance->d_currentoutput) << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " *** " << "\n";
    std::cout << " *** Starting log: " << std::put_time(std::localtime(&cur), "%F %T") << " *** " << std::endl;
  }
}

inline std::ostream &Logger::dispTime(std::ostream &stream) // static
{
  std::time_t cur = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  return stream << std::put_time(std::localtime(&cur), "%Y-%m-%d %H:%M:%S"); // %F and %T do not work on mingw
}

inline void Logger::messagePre() //static
{
  ensureLogger();
  firstUse();

  if (s_instance->d_overwriting) [[unlikely]]
  {
    std::cout << std::endl;
    s_instance->d_overwriting = false;
    s_instance->d_currentoutput = s_instance->d_file;

    if (s_instance->d_strstreambackend && s_instance->d_strstreambackend->tellp() != 0)
    {
      (*s_instance->d_currentoutput) << s_instance->d_strstreambackend->str();
      s_instance->d_strstreambackend->str("");
      s_instance->d_strstreambackend->clear();
    }
  }
}

inline void Logger::setFile(std::string const &f) // static
{
  ensureLogger();
  if (s_instance->d_file)
    return;

  s_instance->d_file = new std::ofstream(f);
  s_instance->d_currentoutput = s_instance->d_file;

  if (!s_instance->d_strstreambackend)
    s_instance->d_strstreambackend = new std::ostringstream();

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
  if (s_instance->d_strstreambackend)
  {
    s_instance->d_strstreambackend->str("");
    s_instance->d_strstreambackend->clear();
    s_instance->d_currentoutput = s_instance->d_strstreambackend;
  }
  s_instance->outputHead("", true);
  s_instance->outputMsg(Flags::OVERWRITE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::message(First const &f, Rest... r) // static
{
  messagePre();
  //outputHead("[MESSAGE] ", "[MESSAGE] ");
  s_instance->outputHead("", false, {"", ": "});
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::message_start(First const &f, Rest... r) // static
{
  messagePre();
  //outputHead("[MESSAGE] ", "[MESSAGE] ");
  s_instance->outputHead("", false, {"", ": "});
  s_instance->outputMsg(Flags::NONEWLINE, f, r...);
}

inline void Logger::message_end() // static
{
  message("");
}

template <typename First, typename... Rest>
inline void Logger::warning(First const &f, Rest... r) // static
{
  messagePre();
  //outputHead("[WARNING] ", "[\033[38;5;37mWARNING\033[0m] ");
  s_instance->outputHead("Warning", false, {"[", "]: "}, std::make_pair<std::string, std::string>("\033[1m", "\033[0m"));
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::warning_start(First const &f, Rest... r) // static
{
  messagePre();
  s_instance->outputHead("       ", false, {" ", "   "});
  s_instance->outputMsg(Flags::NONEWLINE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::warning_indent(First const &f, Rest... r) // static
{
  messagePre();
  s_instance->outputHead("       ", false, {" ", "   "});
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::error(First const &f, Rest... r) // static
{
  messagePre();
  //outputHead("[ ERROR ] ", "[ \033[1;31mERROR\033[0m ] ");
  s_instance->outputHead("Error", false, {"[", "]: "}, std::make_pair<std::string, std::string>("\033[1m", "\033[0m"));
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::error_start(First const &f, Rest... r) // static
{
  messagePre();
  s_instance->outputHead("     ", false, {" ", "   "});
  s_instance->outputMsg(Flags::NONEWLINE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::error_indent(First const &f, Rest... r) // static
{
  messagePre();
  s_instance->outputHead("     ", false, {" ", "   "});
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::output_indent(int indent, First const &f, Rest... r) // static
{
  messagePre();
  s_instance->outputHead(std::string(indent, ' '));
  s_instance->outputMsg(Flags::NONE, f, r...);
}

template <typename First, typename... Rest>
inline void Logger::outputMsg(Flags flags, First const &f, Rest... r)
{
  if (d_currentoutput)
    *(d_currentoutput) << f;

  std::cout << f;
  s_instance->outputMsg(flags, r...);
}

template <typename T>
inline void Logger::outputMsg(Flags flags, T const &t)
{
  if (d_currentoutput)
  {
    *(d_currentoutput) << t;
    if (!(flags & Flags::NONEWLINE)) [[likely]]
      *(d_currentoutput) << "\n";
  }

  if (flags & Flags::OVERWRITE || flags & Flags::NONEWLINE) [[unlikely]]
    std::cout << t << std::flush;
  else
    std::cout << t << std::endl;
}

template <typename T, typename... Rest>
inline void Logger::outputMsg(Flags flags, VECTOR<T> const &vec, Rest... r)
{
  if (d_currentoutput)
    for (unsigned int i = 0; i < vec.data.size(); ++i)
      *(d_currentoutput) << vec.data[i] << ((i < vec.data.size() - 1) ? vec.delim : "");

  for (unsigned int i = 0; i < vec.data.size(); ++i)
    std::cout << vec.data[i] << ((i < vec.data.size() - 1) ? vec.delim : "");

  outputMsg(flags, r...);
}

template <typename T>
inline void Logger::outputMsg(Flags flags, VECTOR<T> const &vec)
{
  if (d_currentoutput)
  {
    for (unsigned int i = 0; i < vec.data.size(); ++i)
      *(d_currentoutput) << vec.data[i] << ((i < vec.data.size() - 1) ? vec.delim : "");
    if (!(flags & Flags::NONEWLINE)) [[likely]]
      *(d_currentoutput) << "\n";
  }

  for (unsigned int i = 0; i < vec.data.size(); ++i)
    std::cout << vec.data[i] << ((i < vec.data.size() - 1) ? vec.delim : "");
  if (flags & Flags::OVERWRITE || flags & Flags::NONEWLINE) [[unlikely]]
    std::cout << std::flush;
  else
    std::cout << std::endl;
}

template <typename T, typename... Rest>
inline void Logger::outputMsg(Flags flags, std::vector<T> const &vec, Rest... r)
{
  outputMsg(flags, VECTOR(vec, ","), r...);
}

template <typename T>
inline void Logger::outputMsg(Flags flags, std::vector<T> const &vec)
{
  outputMsg(flags, VECTOR(vec, ","));
}

template <typename... Rest>
inline void Logger::outputMsg(Flags flags, Logger::ControlChar const &c, Rest... r)
{
  if (d_controlcodessupported)
    std::cout << c.code;
  s_instance->outputMsg(flags, r...);
}

inline void Logger::outputMsg(Flags flags, Logger::ControlChar const &c)
{
  if (d_currentoutput)
    if (!(flags & Flags::NONEWLINE)) [[likely]]
      *(d_currentoutput) << "\n";

  if (flags & Flags::OVERWRITE || flags & Flags::NONEWLINE) [[unlikely]]
    std::cout << (d_controlcodessupported ? c.code : "") << std::flush;
  else
    std::cout << (d_controlcodessupported ? c.code : "") << std::endl;
}

template <typename... Rest>
inline void Logger::outputMsg(Flags flags, Control c, Rest... r)
{
  // (no control codes to file)

  if (d_controlcodessupported)
  {
    switch(c)
    {
      case Control::BOLD:
        std::cout << "\033[1m";
        break;
      case Control::NORMAL:
        std::cout << "\033[0m";
        break;
      case Control::ENDOVERWRITE: // not likely here
        if (flags & Flags::OVERWRITE)
        {
          d_overwriting = false;
          d_currentoutput = s_instance->d_file;

          if (d_strstreambackend && d_strstreambackend->tellp() != 0)
          {
            (*d_currentoutput) << s_instance->d_strstreambackend->str() << "\n";
            d_strstreambackend->str("");
            d_strstreambackend->clear();
          }
          std::cout << std::endl;
        }
        break;
    }
  }

  outputMsg(flags, r...);
}

inline void Logger::outputMsg(Flags flags, Control c)
{
  // (no control codes to file)

  if (d_controlcodessupported)
  {
    switch(c)
    {
      case Control::BOLD:
        std::cout << "\033[1m";
        break;
      case Control::NORMAL:
        std::cout << "\033[0m";
        break;
      case Control::ENDOVERWRITE:
        if (flags & Flags::OVERWRITE)
        {
          d_overwriting = false;
          d_currentoutput = s_instance->d_file;

          if (d_strstreambackend && d_strstreambackend->tellp() != 0)
          {
            (*d_currentoutput) << s_instance->d_strstreambackend->str() << "\n";
            d_strstreambackend->str("");
            d_strstreambackend->clear();
          }
          std::cout << std::endl;
        }
        break;
    }
  }
  if (flags & Flags::OVERWRITE || flags & Flags::NONEWLINE) [[unlikely]]
    std::cout << std::flush;
  else
    std::cout << std::endl;
}

#endif
