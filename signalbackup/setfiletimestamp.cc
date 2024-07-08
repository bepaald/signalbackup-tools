/*
  Copyright (C) 2021-2024  Selwin van Dijk

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

#include "signalbackup.ih"

#if !defined(_WIN32) && !defined(__MINGW64__)

#include <fcntl.h>
#include <sys/stat.h>

bool SignalBackup::setFileTimeStamp(std::string const &file, long long int time_usec) const
{
  struct timespec ntimes[] =
  {
    {                                   // ntimes[0] = // last access time
      time_usec / 1000,                 // tv_sec, seconds
      (time_usec % 1000) * 1000         // tv_usec, nanoseconds
    },
    {                                   // ntimes[1] = // last modification time
      time_usec / 1000,                 // tv_sec, seconds
      (time_usec % 1000) * 1000         // tv_usec, nanoseconds
    }
  };

  return (utimensat(AT_FDCWD, file.c_str(), ntimes, 0) == 0);
}

#else // this is poorly tested, I don't have windows

#include <winbase.h>
#include <fileapi.h>

bool SignalBackup::setFileTimeStamp(std::string const &file, long long int time_usec) const
{

  // get file handle
  HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return false;

  // windows epoch starts at 1601-01-01T00:00:00Z, 11644473600 seconds before the unix epoch.
  // then windows counts in 100 nsec chunks.
  long long unsigned int wintime = time_usec * 10000 + 116444736000000000;

  FILE_BASIC_INFO b;
  b.CreationTime.QuadPart = wintime;
  b.LastAccessTime.QuadPart = wintime;
  b.LastWriteTime.QuadPart = wintime;
  b.ChangeTime.QuadPart = wintime;
  b.FileAttributes = 0; // leave unchanged
  if (SetFileInformationByHandle(hFile, FileBasicInfo, &b, sizeof(b)) != 0)
  {
    // ignore for now...
  }

  return (CloseHandle(hFile));
}

#endif
