/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

#ifndef MAIN_H_
#define MAIN_H_

#include <string>

#if defined(_WIN32) || defined(__MINGW64__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <conio.h>
#else // !windows
#include <unistd.h>
#include <termios.h>
#endif

class SignalBackup;

inline bool getPassword(std::string *pw)
{
  if (!pw)
    return false;

#if defined(_WIN32) || defined(__MINGW64__)
  int constexpr backspace = 8;
  int constexpr enter = 13;
  #define GETCHAR _getch // we're using _getch instead of getchar because then we'd need to disable line buffering
  #define PUTCHAR _putch // but windows then still (incorrectly?) buffers the enter key breaking things.
#else
  struct termios tty_attr;
  if (tcgetattr(STDIN_FILENO, &tty_attr) < 0)
    return false;

  const tcflag_t c_lflag = tty_attr.c_lflag; // Save old flags
  tty_attr.c_lflag &= ~ICANON; // disable canonicle mode (not line-buffered)
  tty_attr.c_lflag &= ~ECHO;   // disable echo
  if (tcsetattr(STDIN_FILENO, 0, &tty_attr) < 0)
    return false;

  int constexpr backspace = 127;
  int constexpr enter = 10;
  #define GETCHAR getchar
  #define PUTCHAR putchar
#endif

  char replacement = '*';
  //for (char c = 0; (c = GETCHAR()) != enter && c != EOF;)
  for (char c = 0; (c = GETCHAR()) != enter && c != std::char_traits<char>::eof();)
  {

#if defined(_WIN32) || defined(__MINGW64__)
    // windows _getch() swallows C-c and C-z (even though docs say it
    // doesn't). So we check for them here. Not sure what the proper
    // response to C-z actually is...
    if (c == 3) // C-c
      GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    if (c == 26) // C-z
      break;
#endif

    if (c == backspace) // backspace
    {
      PUTCHAR(0x8);
      PUTCHAR(' ');
      PUTCHAR(0x8);
      if (pw->size())
        pw->pop_back();
      continue;
    }

    if (c == ' ' || c == '-') // dont replace these common separators for readability
      PUTCHAR(c);
    else
      PUTCHAR(replacement);

    pw->push_back(c);
  }
  PUTCHAR('\n');

#if defined(_WIN32) || defined(__MINGW64__)
#else // !windows
  // restore terminal settings
  tty_attr.c_lflag = c_lflag;

  if (tcsetattr(STDIN_FILENO, 0, &tty_attr) < 0)
    return false;
#endif

  return true;
}

inline bool addThreadIdsFromString(SignalBackup const *const backup,
                                   std::vector<std::string> const &names,
                                   std::vector<long long int> *threads);

#endif
