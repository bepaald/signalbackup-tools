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

std::unique_ptr<BackupFrame> FileDecryptor::getFrame()
{
  long long int filepos = d_file.tellg();
  //std::cout << "Getting frame at filepos: " << filepos << std::endl;

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
  //if (encryptedframelength > 3145728/*= 3MB*/ /*115343360 / * =110MB*/ || encryptedframelength < 11)
  //{
  //  std::cout << "Suspicious framelength" << std::endl;
  //  bruteForceFrom(filepos)???
  //}

  DEBUGOUT("Framelength: ", encryptedframelength);

  unsigned char *encryptedframe = new unsigned char[encryptedframelength];
  if (!getNextFrameBlock(encryptedframe, encryptedframelength))
  {
    delete[] encryptedframe;
    std::cout << "Failed to read next frame (" << encryptedframelength << " bytes at filepos " << filepos << ")" << std::endl;

    // maybe hide this behind option
    return bruteForceFrom(filepos, encryptedframelength);

    return std::unique_ptr<BackupFrame>(nullptr);
  }

  // check hash
  unsigned char theirMac[MACSIZE]; // == 10
  std::memcpy(theirMac, encryptedframe + (encryptedframelength - MACSIZE), MACSIZE);

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
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption d(d_cipherkey, d_cipherkey_size, d_iv);
  d.ProcessData(decodedframe, encryptedframe, encryptedframelength - MACSIZE);

  delete[] encryptedframe;

  std::unique_ptr<BackupFrame> frame(initBackupFrame(decodedframe, decodedframelength, d_framecount++));


  if (!d_editattachments.empty() && frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
    for (uint i = 0; i < d_editattachments.size(); i += 2)
      if (frame->frameNumber() == static_cast<uint64_t>(d_editattachments[i]))
      {
        std::cout << "Editing attachment data size in AttachmentFrame "
                  << reinterpret_cast<AttachmentFrame *>(frame.get())->length() << " -> ";
        reinterpret_cast<AttachmentFrame *>(frame.get())->setLengthField(d_editattachments[i + 1]);
        std::cout << reinterpret_cast<AttachmentFrame *>(frame.get())->length() << std::endl;
        std::cout << "Frame has _id = " << reinterpret_cast<AttachmentFrame *>(frame.get())->rowId()
                  << ", unique_id = " << reinterpret_cast<AttachmentFrame *>(frame.get())->attachmentId() << std::endl;
        break;
      }

  delete[] decodedframe;

  if (!frame)
  {
    std::cout << "Failed to get valid frame from decoded data..." << std::endl;
    //std::cout << "Data: " << bepaald::bytesToHexString(decodedframe, decodedframelength) << std::endl;
    if (d_badmac)
    {
      std::cout << "Encrypted data had failed verification (Bad MAC)" << std::endl;
      return bruteForceFrom(filepos, encryptedframelength);
    }
    else
      std::cout << "Data was verified ok, but does not represent a valid frame... Don't know what happened, but it's bad... Aborting :(" << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  uint32_t attsize = 0;
  if (!d_badmac && (attsize = frame->attachmentSize()) > 0 &&
      (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
       frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
       frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
  {
    uintToFourBytes(d_iv, d_counter++);

    reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);

    if (!d_lazyload) // immediately decrypt i guess...
    {
      int getatt = getAttachment(reinterpret_cast<FrameWithAttachment *>(frame.get()));
      if (getatt > 0)
      {
        std::cout << "Failed to get attachment data for FrameWithAttachment... info:" << std::endl;
        frame->printInfo();
        return std::unique_ptr<BackupFrame>(nullptr);
      }
      if (getatt < 0)
      {
        d_badmac = true;
        if (d_assumebadframesize)
        {
          std::unique_ptr<BackupFrame> f = bruteForceFrom(filepos, encryptedframelength);
          //long long int curfilepos = d_file.tellg();
          //std::cout << "curpso: " << curfilepos << std::endl;
          //std::cout << "ATTACHMENT LENGTH SHOULD HAVE BEEN: " << curfilepos - filepos - encryptedframelength - MACSIZE << std::endl;
          return f;
        }
      }
    }
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;
  return frame;
}
