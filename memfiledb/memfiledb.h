/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

#ifndef MEMFILEDB_H_
#define MEMFILEDB_H_

#include <sqlite3.h>
#include <utility>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <memory>

#include "../common_be.h"

class MemFileDB
{
  struct MemFile
  {
    sqlite3_file base; // Base class. Must be first.
    unsigned char *data;
    uint64_t datasize;
  };
  static sqlite3_vfs s_demovfs;
  static char constexpr s_name[] = {'M', 'e', 'm', 'f', 'i', 'l', 'e', 'V', 'F', 'S'};
 public:
  static sqlite3_vfs *sqlite3_demovfs(std::pair<unsigned char *, uint64_t> *data);
  static char const *vfsName()
  {
    return s_name;
  }
 private:
  static int ioWrite(sqlite3_file *, const void *, int, sqlite_int64);
  static int ioClose(sqlite3_file *pFile);
  static int ioRead(sqlite3_file *pFile, void *zBuf, int iAmt, sqlite_int64 iOfst);
  static int ioTruncate(sqlite3_file *pFile, sqlite_int64 size);
  static int ioSync(sqlite3_file *pFile, int flags);
  static int ioFileSize(sqlite3_file *pFile, sqlite_int64 *pSize);
  static int ioLock(sqlite3_file *pFile, int eLock);
  static int ioUnlock(sqlite3_file *pFile, int eLock);
  static int ioCheckReservedLock(sqlite3_file *pFile, int *pResOut);
  static int ioFileControl(sqlite3_file *pFile, int op, void *pArg);
  static int ioSectorSize(sqlite3_file *pFile);
  static int ioDeviceCharacteristics(sqlite3_file *pFile);
  static int open(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile, int flags, int *pOutFlags);
  static int del(sqlite3_vfs *pVfs, const char *zPath, int dirSync);
  static int access(sqlite3_vfs *pVfs, const char *zPath, int flags, int *pResOut);
  static int fullPathname(sqlite3_vfs *pVfs, const char *zPath, int nPathOut, char *zPathOut);

  static sqlite3_io_methods constexpr  s_io = {1,                                     /* iVersion */
                                               MemFileDB::ioClose,                    /* xClose */
                                               MemFileDB::ioRead,                     /* xRead */
                                               MemFileDB::ioWrite,                    /* xWrite */
                                               MemFileDB::ioTruncate,                 /* xTruncate */
                                               MemFileDB::ioSync,                     /* xSync */
                                               MemFileDB::ioFileSize,                 /* xFileSize */
                                               MemFileDB::ioLock,                     /* xLock */
                                               MemFileDB::ioUnlock,                   /* xUnlock */
                                               MemFileDB::ioCheckReservedLock,        /* xCheckReservedLock */
                                               MemFileDB::ioFileControl,              /* xFileControl */
                                               MemFileDB::ioSectorSize,               /* xSectorSize */
                                               MemFileDB::ioDeviceCharacteristics,    /* xDeviceCharacteristics */

                                               /* since we specified iversion == 1 above, the next fields actually
                                                  should not exist, but just to suppress gcc warnings....  */

                                               nullptr,     /* xShmMap */
                                               nullptr,     /* xShmLock */
                                               nullptr,     /* xShmBarrier */
                                               nullptr,     /* xShmUnmap */
                                               nullptr,     /* xFetch */
                                               nullptr,     /* xUnfetch */};
};

inline int MemFileDB::ioClose(sqlite3_file *pFile [[maybe_unused]])
{
  //std::cout << "Called: " << __FUNCTION__ << std::endl;
  return SQLITE_OK;
}

inline int MemFileDB::ioRead(sqlite3_file *pFile, void *zBuf, int iAmt, sqlite_int64 iOfst)
{

  //std::cout << "Called: " << __FUNCTION__ << std::endl;

  if (static_cast<uint64_t>(iOfst) >= reinterpret_cast<MemFile *>(pFile)->datasize ||
      !reinterpret_cast<MemFile *>(pFile)->data)
  {
    std::cout << " !!! ERROR_READ !!!" << std::endl;
    return SQLITE_IOERR_READ;
  }

  int toread = iAmt;
  bool shortread = false;
  if (static_cast<uint64_t>(iOfst + iAmt) > reinterpret_cast<MemFile *>(pFile)->datasize)
  {
    std::cout << "SHORTREAD" << std::endl;
    toread -= ((iOfst + iAmt) - reinterpret_cast<MemFile *>(pFile)->datasize);
    shortread = true;
  }

  std::memcpy(zBuf, reinterpret_cast<MemFile *>(pFile)->data + iOfst, toread);

  if (shortread)
    return SQLITE_IOERR_SHORT_READ;

  return SQLITE_OK;
}

inline int MemFileDB::ioWrite(sqlite3_file *pFile [[maybe_unused]], const void *, int, sqlite_int64)
{
  //std::cout << " !!! ERROR_WRITE !!!" << std::endl;
  return SQLITE_READONLY;
}

inline int MemFileDB::ioTruncate(sqlite3_file *pFile [[maybe_unused]], sqlite_int64 size [[maybe_unused]])
{
  //std::cout << " !!! ERROR_TRUNC !!!" << std::endl;
  return SQLITE_IOERR_TRUNCATE;
}

inline int MemFileDB::ioSync(sqlite3_file *pFile [[maybe_unused]], int flags [[maybe_unused]])
{
  //return SQLITE_OK; // read only, there's never anything to sync
  return SQLITE_IOERR_FSYNC;
}

inline int MemFileDB::ioFileSize(sqlite3_file *pFile, sqlite_int64 *pSize)
{
  *pSize = reinterpret_cast<MemFile *>(pFile)->datasize;
  return SQLITE_OK;
}

inline int MemFileDB::ioLock(sqlite3_file *pFile [[maybe_unused]], int eLock [[maybe_unused]])
{
  return SQLITE_OK;
}

inline int MemFileDB::ioUnlock(sqlite3_file *pFile [[maybe_unused]], int eLock [[maybe_unused]])
{
  return SQLITE_OK;
}

inline int MemFileDB::ioCheckReservedLock(sqlite3_file *pFile [[maybe_unused]], int *pResOut)
{
  *pResOut = 0;
  return SQLITE_OK;
}

inline int MemFileDB::ioFileControl(sqlite3_file *pFile [[maybe_unused]], int op [[maybe_unused]], void *pArg [[maybe_unused]])
{
  return SQLITE_NOTFOUND;
}

inline int MemFileDB::ioSectorSize(sqlite3_file *pFile [[maybe_unused]])
{
  return 0;
}

inline int MemFileDB::ioDeviceCharacteristics(sqlite3_file *pFile [[maybe_unused]])
{
  return 0;
}

inline int MemFileDB::fullPathname(sqlite3_vfs *pVfs [[maybe_unused]],   /* VFS */
                                   const char *zPath [[maybe_unused]],   /* Input path (possibly a relative path) */
                                   int nPathOut [[maybe_unused]],        /* Size of output buffer in bytes */
                                   char *zPathOut)                       /* Pointer to output buffer */
{
  //std::cout << "Called: " << __FUNCTION__ << std::endl;
  zPathOut[0] = '\0';
  return SQLITE_OK;
}

inline int MemFileDB::open(sqlite3_vfs *pVfs,                        /* VFS */
                           const char *zName [[maybe_unused]],       /* File to open, or 0 for a temp file */
                           sqlite3_file *pFile,                      /* Pointer to DemoFile struct to populate */
                           int flags [[maybe_unused]],               /* Input SQLITE_OPEN_XXX flags */
                           int *pOutFlags)                           /* Output SQLITE_OPEN_XXX flags (or NULL) */
{

  //std::cout << "Called: " << __FUNCTION__ << std::endl;

  MemFile *p = reinterpret_cast<MemFile*>(pFile); /* Populate this structure */
  //std::memset(p, 0, sizeof(MemFile));
  if (pOutFlags)
    *pOutFlags = SQLITE_READONLY;

  p->base.pMethods = &s_io;

  p->data = reinterpret_cast<std::pair<unsigned char *, uint64_t> const *>(pVfs->pAppData)->first;
  p->datasize = reinterpret_cast<std::pair<unsigned char *, uint64_t> const *>(pVfs->pAppData)->second;

  return SQLITE_OK;
}

inline int MemFileDB::del(sqlite3_vfs *pVfs [[maybe_unused]], const char *zPath [[maybe_unused]], int dirSync [[maybe_unused]])
{
  //return SQLITE_OK;
  //std::cout << " !!! ERROR_DEL !!!" << std::endl;
  return SQLITE_IOERR_DELETE;
}

inline int MemFileDB::access(sqlite3_vfs *pVfs, const char *zPath [[maybe_unused]], int flags [[maybe_unused]], int *pResOut)
{

  //std::cout << "Called: " << __FUNCTION__ << std::endl;

  if (reinterpret_cast<std::pair<unsigned char *, uint64_t> const *>(pVfs->pAppData)->first &&
      reinterpret_cast<std::pair<unsigned char *, uint64_t> const *>(pVfs->pAppData)->second > 0)
  {
    *pResOut = 0;
    return SQLITE_OK;
  }
  std::cout << " !!! ERROR_ACCESS !!! "  << std::endl;
  return SQLITE_IOERR_ACCESS;
}

inline sqlite3_vfs *MemFileDB::sqlite3_demovfs(std::pair<unsigned char *, uint64_t> *data)
{
  std::memset(&s_demovfs, 0, sizeof(s_demovfs));

  s_demovfs = {1,                     /* iVersion */
               sizeof(MemFile),       /* szOsFile */
               8,                     /* mxPathname */ // set this to 7 and it fails
               0,                     /* pNext */
               vfsName(),             /* zName */
               data,                  /* pAppData */
               open,                  /* xOpen */
               del,                   /* xDelete */
               access,                /* xAccess */
               fullPathname,          /* xFullPathname */
               nullptr,               /* xDlOpen */
               nullptr,               /* xDlError */
               nullptr,               /* xDlSym */
               nullptr,               /* xDlClose */
               nullptr,               /* xRandomness */
               nullptr,               /* xSleep */
               nullptr,               /* xCurrentTime */
               nullptr,               /* xGetLastError */

               /* since we specified iversion == 1 above, the next fields actually
                  should not exist, but just to suppress gcc warnings....  */

               nullptr,               /* xCurrentTimeInt64 */
               nullptr,               /* xSetSystemCall */
               nullptr,               /* xGetSystemCall */
               nullptr,               /* xNextSystemCall */
  };

  return &s_demovfs;
}

#endif
