/*
    Copyright (C) 2019  Selwin van Dijk

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

std::unique_ptr<BackupFrame> FileDecryptor::bruteForceFrom(uint32_t filepos)
{
  std::cout << "Starting bruteforcing offset to next valid frame..." << std::endl;
  uint32_t skip = 1;
  std::unique_ptr<BackupFrame> ret(nullptr);
  while (filepos + skip < d_filesize)
  {
    d_file.clear();
    if (skip % 10 == 0)
      std::cout << "\rChecking offset " << skip << " bytes" << std::flush;
    d_file.seekg(filepos + skip, std::ios_base::beg);
    ret.reset(getFrame2(skip++).release());
    if (ret)
    {
      std::cout << std::endl << "Got frame, breaking" << std::endl;
      break;
    }
  }
  return ret;
}

std::unique_ptr<BackupFrame> FileDecryptor::getFrame2(uint32_t offset)
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

  unsigned char *encryptedframe = new unsigned char[encryptedframelength];
  if (!getNextFrameBlock(encryptedframe, encryptedframelength))
  {
    delete[] encryptedframe;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  // check hash
  unsigned char theirMac[MACSIZE]; // == 10
  std::memcpy(theirMac, encryptedframe + (encryptedframelength - MACSIZE), MACSIZE);

  //int const ourmacsize = CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE;
  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(d_mackey, d_mackey_size);
  hmac.Update(encryptedframe, encryptedframelength - MACSIZE);
  hmac.Final(ourMac);

  if (std::memcmp(theirMac, ourMac, 10) != 0)
  {
    delete[] encryptedframe;
    return std::unique_ptr<BackupFrame>(nullptr);
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

    if (skipped > offset / 10) // a frame is at least 10 bytes?
    {
      std::cout << "No valid frame found at maximum frameskip for this offset..." << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }

    std::cout << "Checking if we skipped " << skipped << " frames... " << std::flush;

    int decodedframelength = encryptedframelength - MACSIZE;
    unsigned char *decodedframe = new unsigned char[encryptedframelength - MACSIZE];

    uintToFourBytes(d_iv, d_counter + skipped);
    CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption d(d_cipherkey, d_cipherkey_size, d_iv, d_iv_size);
    d.ProcessData(decodedframe, encryptedframe, encryptedframelength - MACSIZE);

    //std::string ps(reinterpret_cast<char *>(decodedframe), decodedframelength);
    //DEBUGOUT("Decoded plaintext: ", ps);
    DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

    frame.reset(initBackupFrame(decodedframe, decodedframelength, d_framecount++));

    delete[] decodedframe;

    ++skipped;

    if (!frame)
    {
      std::cout << "nope! :(" << std::endl;
      //if (skipped >
    }
    else
    {
      if (frame->validate())
      {
        d_counter += skipped;
        std::cout << "YEAH!" << std::endl;
        frame->printInfo();
        delete[] encryptedframe;
        break;
      }
      std::cout << "nope! :(" << std::endl;
      frame.reset();
    }
  }

  DEBUGOUT("FRAMETYPE: ", frame->frameType());
  //frame->printInfo();
  //std::cout << "HEADERTYPE: " << frame->frameType() << std::endl;

  uint32_t attsize = 0;
  if (!d_badmac && (attsize = frame->attachmentSize()) > 0)
  {
    uintToFourBytes(d_iv, d_counter++);

    if (frame->frameType() == ATTACHMENT || frame->frameType() == AVATAR)
      reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, attsize, d_file.tellg(), this);

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

    if (!d_lazyload) // immediately decrypt i guess...
    {
      if (frame->frameType() != ATTACHMENT && frame->frameType() != AVATAR)
        return std::unique_ptr<BackupFrame>(nullptr);

      if (!getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get())))
        return std::unique_ptr<BackupFrame>(nullptr);
    }
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;
  return frame;
}

std::unique_ptr<BackupFrame> FileDecryptor::getFrame()
{
  long int filepos = d_file.tellg();

  if (static_cast<uint64_t>(filepos) == d_filesize)
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

  DEBUGOUT("Framelength: ", encryptedframelength);

  unsigned char *encryptedframe = new unsigned char[encryptedframelength];
  if (!getNextFrameBlock(encryptedframe, encryptedframelength))
  {
    delete[] encryptedframe;
    //std::cout << encryptedframelength << std::endl;
    std::cout << "Failed to read next frame (" << encryptedframelength << " bytes at filepos " << filepos << ")" << std::endl;
    //std::cout << "Filepos is now " << d_file.tellg() << std::endl;
    //std::cout << "Stat: " << d_file.good() << std::endl;

    // maybe hide this behind option
    return bruteForceFrom(filepos);

    return std::unique_ptr<BackupFrame>(nullptr);
  }

  // check hash
  unsigned char theirMac[MACSIZE]; // == 10
  std::memcpy(theirMac, encryptedframe + (encryptedframelength - MACSIZE), MACSIZE);

  //int const ourmacsize = CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE;
  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(d_mackey, d_mackey_size);
  hmac.Update(encryptedframe, encryptedframelength - MACSIZE);
  hmac.Final(ourMac);

  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE));
  if (std::memcmp(theirMac, ourMac, 10) != 0)
  {
    std::cout << "" << std::endl;
    std::cout << "WARNING: Bad MAC in frame: theirMac: " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    std::cout << "                             ourMac: " << bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE) << std::endl;

    d_badmac = true;
  }
  else
    d_badmac = false;

  // decode
  int decodedframelength = encryptedframelength - MACSIZE;
  unsigned char *decodedframe = new unsigned char[encryptedframelength - MACSIZE];

  uintToFourBytes(d_iv, d_counter++);
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption d(d_cipherkey, d_cipherkey_size, d_iv, d_iv_size);
  d.ProcessData(decodedframe, encryptedframe, encryptedframelength - MACSIZE);

  delete[] encryptedframe;

  //std::string ps(reinterpret_cast<char *>(decodedframe), decodedframelength);
  //DEBUGOUT("Decoded plaintext: ", ps);
  DEBUGOUT("Decoded hex      : ", bepaald::bytesToHexString(decodedframe, decodedframelength));

  std::unique_ptr<BackupFrame> frame(initBackupFrame(decodedframe, decodedframelength, d_framecount++));

  delete[] decodedframe;

  if (!frame)
  {
    //std::cout << "DONE 2" << std::endl;
    std::cout << "Failed to get valid frame from decoded data..." << std::endl;
    std::cout << "Data: " << bepaald::bytesToHexString(decodedframe, decodedframelength) << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  //std::cout << "Got frame at filepos 0x" << std::hex << filepos << std::dec << ", counter: " << d_counter - 1 << std::endl;

  DEBUGOUT("FRAMETYPE: ", frame->frameType());
  //frame->printInfo();
  //std::cout << "HEADERTYPE: " << frame->frameType() << std::endl;

  uint32_t attsize = 0;
  if (!d_badmac && (attsize = frame->attachmentSize()) > 0)
  {
    uintToFourBytes(d_iv, d_counter++);

    if (frame->frameType() == ATTACHMENT || frame->frameType() == AVATAR)
      reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, attsize, d_file.tellg(), this);

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

    if (!d_lazyload) // immediately decrypt i guess...
    {
      if (frame->frameType() != ATTACHMENT && frame->frameType() != AVATAR)
        return std::unique_ptr<BackupFrame>(nullptr);

      if (!getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get())))
      {
        std::cout << "Failed to get attachment data for FrameWithAttachment... info:" << std::endl;
        frame->printInfo();
        return std::unique_ptr<BackupFrame>(nullptr);
      }
      // else
      // {
      //   if (frame->frameType() == ATTACHMENT)
      //   {
      //     AttachmentFrame *tmp = reinterpret_cast<AttachmentFrame *>(frame.get());
      //     std::cout << "got attachment data for rowid: " << tmp->rowId() << " uniqueid: " << tmp->attachmentId() << std::endl;
      //   }
      // }
    }
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;
  return frame;
}
