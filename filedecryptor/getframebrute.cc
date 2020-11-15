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

#include "filedecryptor.ih"

std::unique_ptr<BackupFrame> FileDecryptor::bruteForceFrom(uint32_t filepos, uint32_t previousframelength)
{
  std::cout << "Starting bruteforcing offset to next valid frame... starting after: " << filepos << std::endl;
  uint32_t skip = 1;
  std::unique_ptr<BackupFrame> ret(nullptr);
  while (filepos + skip < d_filesize)
  {
    d_file.clear();
    if (skip % 10 == 0)
      std::cout << "\rChecking offset " << skip << " bytes" << std::flush;
    d_file.seekg(filepos + skip, std::ios_base::beg);
    ret.reset(getFrameBrute(skip++, previousframelength).release());
    if (ret)
    {
      std::cout << std::endl << "Got frame, breaking" << std::endl;
      //break;
    }
  }
  return ret;
}

std::unique_ptr<BackupFrame> FileDecryptor::getFrameBrute(uint32_t offset, uint32_t previousframelength)
{
  if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
  {
    std::cout << "Read entire backup file..." << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  if (d_headerframe)
  {
    std::unique_ptr<BackupFrame> frame(d_headerframe.release());
    return frame;
  }

  uint32_t encryptedframelength = getNextFrameBlockSize();
  if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
  {
    //std::cout << "Framesize too big to be real" << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
  if (!getNextFrameBlock(encryptedframe.get(), encryptedframelength))
    return std::unique_ptr<BackupFrame>(nullptr);

#ifndef USE_CRYPTOPP

  // check hash
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
  if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
    return std::unique_ptr<BackupFrame>(nullptr);
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

    if (skipped > offset / 10) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
    {
      std::cout << std::endl << "No valid frame found at maximum frameskip for this offset..." << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    std::cout << "\rChecking if we skipped " << skipped << " frames... " << std::flush;

    uintToFourBytes(d_iv, d_counter + skipped);

    // create context
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

    // disable padding
    EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
    {
      std::cout << "CTX INIT FAILED" << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    int decodedframelength = encryptedframelength - MACSIZE;
    unsigned char *decodedframe = new unsigned char[decodedframelength];

    if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1)
    {
      std::cout << "Failed to decrypt data" << std::endl;
      delete[] decodedframe;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

    frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

    delete[] decodedframe;

    ++skipped;

#else

  // check hash
  unsigned char theirMac[MACSIZE]; // == 10
  std::memcpy(theirMac, encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE);

  //int const ourmacsize = CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE;
  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(d_mackey, d_mackey_size);
  hmac.Update(encryptedframe.get(), encryptedframelength - MACSIZE);
  hmac.Final(ourMac);

  if (std::memcmp(theirMac, ourMac, 10) != 0)
    return std::unique_ptr<BackupFrame>(nullptr);
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

    if (skipped > offset / 10) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
    {
      std::cout << std::endl << "No valid frame found at maximum frameskip for this offset..." << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    std::cout << "\rChecking if we skipped " << skipped << " frames... " << std::flush;

    int decodedframelength = encryptedframelength - MACSIZE;
    unsigned char *decodedframe = new unsigned char[encryptedframelength - MACSIZE];

    uintToFourBytes(d_iv, d_counter + skipped);
    CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption d(d_cipherkey, d_cipherkey_size, d_iv);
    d.ProcessData(decodedframe, encryptedframe.get(), encryptedframelength - MACSIZE);

    //std::string ps(reinterpret_cast<char *>(decodedframe), decodedframelength);
    //DEBUGOUT("Decoded plaintext: ", ps);
    DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

    frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

    delete[] decodedframe;

    ++skipped;

#endif

    if (!frame)
    {
      std::cout << "nope! :(" << std::flush;
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
        std::cout << "YEAH! :)" << std::endl;
        if (d_assumebadframesize && skipped == 1 /*NOTE, skipped was already upped*/)
        {
          std::cout << std::endl << " ! CORRECT FRAME_NUMBER:SIZE = " << frame->frameNumber() - 1 << ":"
                    << offset - previousframelength - MACSIZE - 4 << std::endl << std::endl;
        }
        std::cout << "Good frame: " << frame->frameNumber() << " (" << frame->frameTypeString() << ")" << std::endl;
        frame->printInfo();
        delete[] encryptedframe.release();
        break;
      }
      std::cout << "nope! :(" << std::flush;
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
        return std::unique_ptr<BackupFrame>(nullptr);
      }
    }
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;
  return frame;
}
