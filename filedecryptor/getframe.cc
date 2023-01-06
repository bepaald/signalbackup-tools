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

std::unique_ptr<BackupFrame> FileDecryptor::getFrame()
{
  unsigned long long int filepos = d_file.tellg();

  if (d_verbose) [[unlikely]]
    std::cout << "Getting frame at filepos: " << filepos << " (COUNTER: " << d_counter << ")" << std::endl;

  if (static_cast<uint64_t>(filepos) == d_filesize) [[unlikely]]
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

  if (encryptedframelength == 0 && d_file.eof()) [[unlikely]]
  {
    std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off << " Unexpectedly hit end of file!" << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  DEBUGOUT("Framelength: ", encryptedframelength);
  if (d_verbose) [[unlikely]]
    std::cout << "Framelength: " << encryptedframelength << std::endl;

  std::unique_ptr<unsigned char[]> encryptedframe(new unsigned char[encryptedframelength]);
  if (encryptedframelength > 115343360 /*110MB*/ || encryptedframelength < 11 || !getNextFrameBlock(encryptedframe.get(), encryptedframelength)) [[unlikely]]
  {
    std::cout << "Failed to read next frame (" << encryptedframelength << " bytes at filepos " << filepos << ")" << std::endl;

    if (d_stoponerror)
      return std::unique_ptr<BackupFrame>(nullptr);

    return bruteForceFrom(filepos, encryptedframelength);
  }

#ifndef USE_CRYPTOPP

  // check hash
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  HMAC(EVP_sha256(), d_mackey, d_mackey_size, encryptedframe.get(), encryptedframelength - MACSIZE, hash, &digest_size);
  if (std::memcmp(encryptedframe.get() + (encryptedframelength - MACSIZE), hash, 10) != 0) [[unlikely]]
  {
    std::cout << "" << std::endl;
    std::cout << bepaald::bold_on << "WARNING" << bepaald::bold_off << " : Bad MAC in frame: theirMac: "
              << bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE) << std::endl;
    std::cout << "                              ourMac: " << bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH) << std::endl;

    if (d_framecount == 1) [[unlikely]]
      std::cout << bepaald::bold_on << " *** NOTE : IT IS LIKELY AN INCORRECT PASSWORD WAS PROVIDED ***" << bepaald::bold_off << std::endl;

    d_badmac = true;
    if (d_stoponerror)
    {
      std::cout << "Stop reading backup. Next frame would be read at offset " << filepos + encryptedframelength << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }
  }
  else
  {
    d_badmac = false;
    if (d_verbose) [[unlikely]]
    {
      std::cout << "Calculated mac: " << bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH) << std::endl;
      std::cout << "Mac in file   : " << bepaald::bytesToHexString(encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE) << std::endl;
    }
  }

  // decode
  uintToFourBytes(d_iv, d_counter++);

  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1) [[unlikely]]
  {
    std::cout << "CTX INIT FAILED" << std::endl;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  int decodedframelength = encryptedframelength - MACSIZE;
  unsigned char *decodedframe = new unsigned char[decodedframelength];

  if (EVP_DecryptUpdate(ctx.get(), decodedframe, &decodedframelength, encryptedframe.get(), encryptedframelength - MACSIZE) != 1) [[unlikely]]
  {
    std::cout << "Failed to decrypt data" << std::endl;
    delete[] decodedframe;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

#else

  // check hash
  unsigned char theirMac[MACSIZE]; // == 10
  std::memcpy(theirMac, encryptedframe.get() + (encryptedframelength - MACSIZE), MACSIZE);

  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(d_mackey, d_mackey_size);
  hmac.Update(encryptedframe.get(), encryptedframelength - MACSIZE);
  hmac.Final(ourMac);

  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE));
  if (std::memcmp(theirMac, ourMac, 10) != 0) [[unlikely]]
  {
    std::cout << "" << std::endl;
    std::cout << "WARNING: Bad MAC in frame: theirMac: " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    std::cout << "                             ourMac: " << bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE) << std::endl;

    d_badmac = true;
    if (d_stoponerror)
    {
      std::cout << "Stop reading backup. Next frame would be read at offset " << filepos + encryptedframelength << std::endl;
      return std::unique_ptr<BackupFrame>(nullptr);
    }
  }
  else
  {
    d_badmac = false;
    if (d_verbose) [[unlikely]]
    {
      std::cout << "Calculated mac: " << bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE) << std::endl;
      std::cout << "Mac in file   : " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    }
  }

  // decode
  int decodedframelength = encryptedframelength - MACSIZE;
  unsigned char *decodedframe = new unsigned char[encryptedframelength - MACSIZE];

  uintToFourBytes(d_iv, d_counter++);
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption d(d_cipherkey, d_cipherkey_size, d_iv);
  d.ProcessData(decodedframe, encryptedframe.get(), encryptedframelength - MACSIZE);

#endif

  delete[] encryptedframe.release(); // free up already....

  std::unique_ptr<BackupFrame> frame(initBackupFrame(decodedframe, decodedframelength, d_framecount++));

  if (!d_editattachments.empty() && frame && frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT) [[unlikely]]
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

  if (!frame) [[unlikely]]
  {
    std::cout << "Failed to get valid frame from decoded data..." << std::endl;
    if (d_badmac)
    {
      std::cout << "Encrypted data had failed verification (Bad MAC)" << std::endl;
      delete[] decodedframe;
      return bruteForceFrom(filepos, encryptedframelength);
    }
    else
    {
      std::cout << "Data was verified ok, but does not represent a valid frame... Don't know what happened, but it's bad... :(" << std::endl;
      std::cout << "Decrypted frame data: " << bepaald::bytesToHexString(decodedframe, decodedframelength) << std::endl;
      delete[] decodedframe;
      return std::make_unique<InvalidFrame>();
    }
    delete[] decodedframe;
    return std::unique_ptr<BackupFrame>(nullptr);
  }

  delete[] decodedframe;

  uint32_t attsize = frame->attachmentSize();
  if (!d_badmac && attsize > 0 &&
      (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT ||
       frame->frameType() == BackupFrame::FRAMETYPE::AVATAR ||
       frame->frameType() == BackupFrame::FRAMETYPE::STICKER))
  {

    if ((d_file.tellg() < 0 && d_file.eof()) || (attsize + static_cast<uint64_t>(d_file.tellg()) > d_filesize)) [[unlikely]]
      if (!d_assumebadframesize)
      {
        std::cout << bepaald::bold_on << "ERROR" << bepaald::bold_off << " Unexpectedly hit end of file while reading attachment!" << std::endl;
        return std::unique_ptr<BackupFrame>(nullptr);
      }

    uintToFourBytes(d_iv, d_counter++);

    reinterpret_cast<FrameWithAttachment *>(frame.get())->setLazyData(d_iv, d_iv_size, d_mackey, d_mackey_size, d_cipherkey, d_cipherkey_size, attsize, d_filename, d_file.tellg());

    d_file.seekg(attsize + MACSIZE, std::ios_base::cur);
  }

  //std::cout << "FILEPOS: " << d_file.tellg() << std::endl;

  //delete frame;

  return frame;
}
