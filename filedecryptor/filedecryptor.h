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

#ifndef FILEDECRYPTOR_H_
#define FILEDECRYPTOR_H_

#include <cstring>
#include <fstream>

//#include "../cryptbase/cryptbase.h"
#include "../common_be.h"
#include "../backupframe/backupframe.h"
#include "../framewithattachment/framewithattachment.h"
#include "../basedecryptor/basedecryptor.h"
#include "../invalidframe/invalidframe.h"

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
  bool d_verbose;

 public:
  FileDecryptor(std::string const &filename, std::string const &passphrase, bool verbose, bool lazy = true, bool assumebadframesize = false, std::vector<long long int> editattachments = std::vector<long long int>());
  FileDecryptor(FileDecryptor const &other) = delete;
  FileDecryptor operator=(FileDecryptor const &other) = delete;
  //inline ~FileDecryptor();
  inline bool ok() const;
  std::unique_ptr<BackupFrame> getFrame();
  inline uint64_t total() const;
  inline uint64_t curFilePos();
  inline bool badMac() const;

  inline void hhenkel(uint64_t pos);

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

#ifndef USE_CRYPTOPP
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#endif

inline void FileDecryptor::hhenkel(uint64_t pos)
{
#ifndef USE_CRYPTOPP

  uint offset = 0;

  d_file.seekg(pos, std::ios_base::beg);

  std::cout << "Getting frame at filepos: " << d_file.tellg() << std::endl;

  if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
  {
    std::cout << "Read entire backup file..." << std::endl;
    return;
  }

  uint32_t encryptedframelength = getNextFrameBlockSize();
  if (encryptedframelength > 3145728 || encryptedframelength < 11)
  {
    std::cout << "Framesize too big to be real" << std::endl;
    return;
  }

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
  if (!getNextFrameBlock(encryptedframe.get(), encryptedframelength))
    return;

  // check hash
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
  if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
  {
    std::cout << "BAD MAC!" << std::endl;
    return;
  }
  else
  {
    std::cout << "" << std::endl;
    std::cout << "GOT GOOD MAC AT OFFSET " << offset << " BYTES!" << std::endl;
    std::cout << "Now let's try and find out how many frames we skipped to get here...." << std::endl;
    d_badmac = false;
  }

  // decode
  uint skipped = 0;
  std::unique_ptr<BackupFrame> frame(nullptr);
  while (!frame)
  {

    if (skipped > 1000000) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
    {
      std::cout << "TESTED 1000000 frames" << std::endl;
      return;
    }

    if (skipped % 100 == 0)
      std::cout << "\rChecking if we skipped " << skipped << " frames... " << std::flush;

    uintToFourBytes(d_iv, d_counter + skipped);

    // create context
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

    // disable padding
    EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
    {
      std::cout << "CTX INIT FAILED" << std::endl;
      return;
    }

    int decodedframelength = encryptedframelength - MACSIZE;
    unsigned char *decodedframe = new unsigned char[decodedframelength];

    if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1)
    {
      std::cout << "Failed to decrypt data" << std::endl;
      delete[] decodedframe;
      return;
    }

    DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

    frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

    delete[] decodedframe;

    ++skipped;
    if (!frame)
    {
      std::cout << "\rChecking if we skipped " << skipped << " frames... nope! :(" << std::flush;
      //if (skipped >
    }
    else
    {
      if (frame->validate() &&
          frame->frameType() != BackupFrame::FRAMETYPE::HEADER && // it is impossible to get in this function without the headerframe, and there is only one
          (frame->frameType() != BackupFrame::FRAMETYPE::END || static_cast<uint64_t>(d_file.tellg()) == d_filesize))
      {
        d_counter += skipped;
        d_framecount += skipped;
        std::cout << "\rChecking if we skipped " << skipped << " frames... YEAH! :)" << std::endl;
        std::cout << "Good frame: " << frame->frameNumber() << " (" << frame->frameTypeString() << ")" << std::endl;
        frame->printInfo();
        //delete[] encryptedframe.release();
        frame.reset();
      }
      std::cout << "\rChecking if we skipped " << skipped << " frames... nope! :(" << std::flush;
      frame.reset();
    }
  }

  //frame->printInfo();
  //std::cout << "HEADERTYPE: " << frame->frameType() << std::endl;

  uint32_t attsize = 0;
  if (!d_badmac && (attsize = frame->attachmentSize()) > 0 &&
      (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
       frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
       frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
  {
    [[unlikely]] if (d_verbose)
      std::cout << "Trying to read attachment (bruteforce)" << std::endl;

    uintToFourBytes(d_iv, d_counter++);

    reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

    if (!d_lazyload) // immediately decrypt i guess...
    {
      [[unlikely]] if (d_verbose)
        std::cout << "Getting attachment at file pos " << d_file.tellg() << " (size: " << attsize << ")" << std::endl;

      int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get()));
      if (getatt != 0)
      {
        if (getatt < 0)
          d_badmac = true;
        return;
      }
    }
  }
#endif
}

*/

#endif
