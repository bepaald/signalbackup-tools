/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

std::unique_ptr<BackupFrame> FileDecryptor::bruteForceFrom(uint64_t filepos, uint32_t previousframelength)
{
  Logger::message("Starting bruteforcing offset to next valid frame... starting after: ", filepos);
  uint64_t skip = 1;
  std::unique_ptr<BackupFrame> ret(nullptr);
  while (filepos + skip < d_filesize)
  {
    d_file.clear();
    if (skip % 10 == 0)
      Logger::message_overwrite("Checking offset ", skip, " bytes");
    d_file.seekg(filepos + skip, std::ios_base::beg);
    ret.reset(getFrameBrute(skip++, previousframelength).release());
    if (ret)
    {
      Logger::message("Got frame, breaking");
      break;
    }
  }
  return ret;
}

std::unique_ptr<BackupFrame> FileDecryptor::getFrameBrute(uint64_t offset, uint32_t previousframelength)
{
  if (static_cast<uint64_t>(d_file.tellg()) == d_filesize)
  {
    Logger::message("Read entire backup file...");
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

  // check hash
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
  if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, MACSIZE) != 0)
    return std::unique_ptr<BackupFrame>(nullptr);
  else
  {
    Logger::message("\nGOT GOOD MAC AT OFFSET ", offset, " BYTES!");
    Logger::message("Now let's try and find out how many frames we skipped to get here....");
    d_badmac = false;
  }

  // decode
  uint skipped = 0;
  std::unique_ptr<BackupFrame> frame(nullptr);
  while (!frame)
  {

    if (skipped > offset / 10) // a frame is at least 10 bytes? -> could probably safely set this higher. MAC alone is 10 bytes, there is also actual data
    {
      Logger::message("\nNo valid frame found at maximum frameskip for this offset...");
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    Logger::message_overwrite("Checking if we skipped ", skipped, " frames... ");

    uintToFourBytes(d_iv, d_counter + skipped);

    // create context
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

    // disable padding
    EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
    {
      Logger::error("CTX INIT FAILED");
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    int decodedframelength = encryptedframelength - MACSIZE;
    unsigned char *decodedframe = new unsigned char[decodedframelength];

    if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1)
    {
      Logger::error("Failed to decrypt data");
      delete[] decodedframe;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

    frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount + skipped));

    delete[] decodedframe;

    ++skipped;

    if (!frame)
    {
      Logger::message_overwrite("Checking if we skipped ", skipped, " frames... nope! :(");
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
        Logger::message_overwrite("Checking if we skipped ", skipped, " frames... YEAH! :)", Logger::Control::ENDOVERWRITE);
        if (d_assumebadframesize && skipped == 1 /*NOTE, skipped was already upped*/)
        {
          Logger::message("\n ! CORRECT FRAME_NUMBER:SIZE = ", frame->frameNumber() - 1, ":",
                          offset - previousframelength - MACSIZE - 4, "\n");
        }
        Logger::message("Good frame: ", frame->frameNumber(), " (", frame->frameTypeString(), ")");
        frame->printInfo();
        delete[] encryptedframe.release();
        break;
      }
      Logger::message_overwrite("Checking if we skipped ", skipped, " frames... nope! :(");
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
    if (d_verbose) [[unlikely]]
      Logger::message("Trying to read attachment (bruteforce)");

    uintToFourBytes(d_iv, d_counter++);

    reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;
  return frame;
}
