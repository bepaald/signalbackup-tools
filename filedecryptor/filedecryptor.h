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

#define FILEDECRYPTOR_H_

#include <cstring>
#include <fstream>

//#include "../cryptbase/cryptbase.h"
#include "../common_be.h"
#include "../backupframe/backupframe.h"
#include "../framewithattachment/framewithattachment.h"
#include "../basedecryptor/basedecryptor.h"

class  FileDecryptor : public BaseDecryptor//, public CryptBase
{
  std::unique_ptr<BackupFrame> d_headerframe;
  std::ifstream d_file;
  std::string d_filename;
  uint64_t d_framecount;
  uint64_t d_filesize;
  bool d_lazyload;
  bool d_badmac;
  bool d_assumebadframesize;
  std::vector<long long int> d_editattachments;

 public:
  FileDecryptor(std::string const &filename, std::string const &passphrase, bool lazy = true, bool assumebadframesize = false, std::vector<long long int> editattachments = std::vector<long long int>());
  FileDecryptor(FileDecryptor const &other) = delete;
  FileDecryptor operator=(FileDecryptor const &other) = delete;
  //inline ~FileDecryptor();
  inline bool ok() const;
  std::unique_ptr<BackupFrame> getFrame();
  inline uint64_t total() const;
  inline uint64_t curFilePos();
  inline bool badMac() const;
 private:
  inline uint32_t getNextFrameBlockSize();
  inline bool getNextFrameBlock(unsigned char *data, size_t length);
  BackupFrame *initBackupFrame(unsigned char *data, size_t length, uint64_t count = 0) const;
  //virtual int getAttachment(FrameWithAttachment *frame) override;

  std::unique_ptr<BackupFrame> bruteForceFrom(uint32_t filepos, uint32_t previousframelength);
  std::unique_ptr<BackupFrame> getFrameBrute(uint32_t offset, uint32_t previousframelength);
};

inline bool FileDecryptor::ok() const
{
  return d_ok;
}

inline uint64_t FileDecryptor::total() const
{
  return d_filesize;
}

inline uint64_t FileDecryptor::curFilePos()
{
  return d_file.tellg();
}

inline bool FileDecryptor::badMac() const
{
  return d_badmac;
}

inline uint32_t FileDecryptor::getNextFrameBlockSize()
{
  uint32_t headerlength = 0;
  if (!d_file.read(reinterpret_cast<char *>(&headerlength), 4))
  {
    std::cout << "Failed to read 4 bytes from file to get next frame size... (" << d_file.tellg()
              << " / " << d_filesize << ")" << std::endl;
    // print error
    return 0;
  }
  headerlength = bepaald::swap_endian<uint32_t>(headerlength);
  return headerlength;
}

inline bool FileDecryptor::getNextFrameBlock(unsigned char *data, size_t length)
{
  //std::cout << "reading " << length << " bytes" << std::endl;
  if (!d_file.read(reinterpret_cast<char *>(data), length))
    return false;
  return true;
}

/*
inline void FileDecryptor::destroyPtr(unsigned char **p, size_t *psize)
{
  if (*p)
  {
    delete *p;
    *p = nullptr;
    *psize = 0;
  }
}
*/

#endif
