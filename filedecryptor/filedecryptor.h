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

#ifndef FILEDECRYPTOR_H_
#define FILEDECRYPTOR_H_

#include <cstring>
#include <fstream>

#include "../common_be.h"
#include "../backupframe/backupframe.h"
#include "../framewithattachment/framewithattachment.h"
#include "../basedecryptor/basedecryptor.h"
#include "../invalidframe/invalidframe.h"
#include "../logger/logger.h"

class  FileDecryptor : public BaseDecryptor
{
  std::unique_ptr<BackupFrame> d_headerframe;
  std::string d_filename;
  uint64_t d_framecount;
  uint64_t d_filesize;
  bool d_badmac;
  bool d_assumebadframesize;
  std::vector<long long int> d_editattachments;
  bool d_stoponerror;
  uint32_t d_backupfileversion;

 public:
  FileDecryptor(std::string const &filename, std::string const &passphrase, bool verbose, bool stoponerror = false, bool assumebadframesize = false, std::vector<long long int> editattachments = std::vector<long long int>());
  inline FileDecryptor(FileDecryptor const &other);
  inline FileDecryptor &operator=(FileDecryptor const &other);
  inline FileDecryptor(FileDecryptor &&other);
  inline FileDecryptor &operator=(FileDecryptor &&other);
  inline bool ok() const;
  std::unique_ptr<BackupFrame> getFrameOld(std::ifstream &file);
  std::unique_ptr<BackupFrame> getFrame(std::ifstream &file);
  inline uint64_t total() const;
  inline bool badMac() const;

  // temporary /* CUSTOMS */
  // void ashmorgan(std::ifstream &file);
  // void strugee(std::ifstream &file, uint64_t pos);
  // std::unique_ptr<BackupFrame> getFrameStrugee2(std::ifstream &file);
  // void strugee2(std::ifstream &file);
  // void strugee3Helper(std::ifstream &file,
  //                     std::vector<std::pair<std::unique_ptr<unsigned char[]>, uint64_t>> *macs_and_positions);
  // void strugee3(std::ifstream &file, uint64_t pos);

 private:
  inline uint32_t getNextFrameBlockSize(std::ifstream &file);
  inline bool getNextFrameBlock(std::ifstream &file, unsigned char *data, size_t length);
  BackupFrame *initBackupFrame(unsigned char *data, size_t length, uint64_t count = 0) const;
  //virtual int getAttachment(FrameWithAttachment *frame) override;

  std::unique_ptr<BackupFrame> bruteForceFrom(std::ifstream &file, uint64_t filepos, uint32_t previousframelength);
  std::unique_ptr<BackupFrame> getFrameBrute(std::ifstream &file, uint64_t offset, uint32_t previousframelength);
};

inline FileDecryptor::FileDecryptor(FileDecryptor const &other)
  :
  BaseDecryptor(other),
  d_headerframe(nullptr),
  d_filename(other.d_filename),
  d_framecount(other.d_framecount),
  d_filesize(other.d_filesize),
  d_badmac(other.d_badmac),
  d_assumebadframesize(other.d_assumebadframesize),
  d_editattachments(other.d_editattachments),
  d_stoponerror(other.d_stoponerror),
  d_backupfileversion(other.d_backupfileversion)
{
  d_ok = false;

  // headerfame...
  if (other.d_headerframe)
    d_headerframe.reset(other.d_headerframe->clone());

  d_ok = other.d_ok;
}

inline FileDecryptor &FileDecryptor::operator=(FileDecryptor const &other)
{
  if (this != &other)
  {
    BaseDecryptor::operator=(other);
    if (other.d_headerframe)
      d_headerframe.reset(other.d_headerframe->clone());
    d_filename = other.d_filename;
    d_framecount = other.d_framecount;
    d_filesize = other.d_filesize;
    d_badmac = other.d_badmac;
    d_assumebadframesize = other.d_assumebadframesize;
    d_editattachments = other.d_editattachments;
    d_stoponerror = other.d_stoponerror;
    d_backupfileversion = other.d_backupfileversion;
    d_ok = other.d_ok;
  }
  return *this;
}

inline FileDecryptor::FileDecryptor(FileDecryptor &&other)
  :
  BaseDecryptor(std::move(other)),
  d_headerframe(std::move(other.d_headerframe)),
  d_filename(std::move(other.d_filename)),
  d_framecount(std::move(other.d_framecount)),
  d_filesize(std::move(other.d_filesize)),
  d_badmac(std::move(other.d_badmac)),
  d_assumebadframesize(std::move(other.d_assumebadframesize)),
  d_editattachments(std::move(other.d_editattachments)),
  d_stoponerror(std::move(other.d_stoponerror)),
  d_backupfileversion(std::move(other.d_backupfileversion))
{}

inline FileDecryptor &FileDecryptor::operator=(FileDecryptor &&other)
{
  if (this != &other)
  {
    BaseDecryptor::operator=(std::move(other));
    d_headerframe = std::move(other.d_headerframe);
    d_filename = std::move(other.d_filename);
    d_framecount = std::move(other.d_framecount);
    d_filesize = std::move(other.d_filesize);
    d_badmac = std::move(other.d_badmac);
    d_assumebadframesize = std::move(other.d_assumebadframesize);
    d_editattachments = std::move(other.d_editattachments);
    d_stoponerror = std::move(other.d_stoponerror);
    d_backupfileversion = std::move(other.d_backupfileversion);
  }
  return *this;
}

inline bool FileDecryptor::ok() const
{
  return d_ok;
}

inline uint64_t FileDecryptor::total() const
{
  return d_filesize;
}

inline bool FileDecryptor::badMac() const
{
  return d_badmac;
}

// only used by getFrameOld(), used in older backups where frame length was not encrypted
inline uint32_t FileDecryptor::getNextFrameBlockSize(std::ifstream &file)
{
  uint32_t headerlength = 0;
  if (!file.read(reinterpret_cast<char *>(&headerlength), sizeof(decltype(headerlength))))
  {
    Logger::error("Failed to read 4 bytes from file to get next frame size... (", file.tellg(),
                  " / ", d_filesize, ")");
    return 0;
  }
  return bepaald::swap_endian<uint32_t>(headerlength);
}

// only used by getFrameOld(), used in older backups where frame length was not encrypted
inline bool FileDecryptor::getNextFrameBlock(std::ifstream &file, unsigned char *data, size_t length)
{
  //std::cout << "reading " << length << " bytes" << std::endl;
  if (!file.read(reinterpret_cast<char *>(data), length))
    return false;
  return true;
}

#endif
