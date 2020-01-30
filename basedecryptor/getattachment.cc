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

#include "basedecryptor.ih"

#ifndef USE_CRYPTOPP

int BaseDecryptor::getAttachment(FrameWithAttachment *frame) // static
{
  std::ifstream file(frame->filename(), std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    std::cout << "Failed to open backup file for reading attachment" << std::endl;
    return 1;
  }

  //std::cout << "Getting attachment: " << frame->filepos() << " + " << frame->length() << std::endl;
  file.seekg(frame->filepos(), std::ios_base::beg);

  // to decrypt the data
  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  // init
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, frame->cipherkey(), frame->iv()) != 1)
  {
    std::cout << "CTX INIT FAILED" << std::endl;
    return 1;
  }

  // to calculate the MAC
  std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
  if (HMAC_Init_ex(hctx.get(), frame->mackey(), frame->mackey_size(), EVP_sha256(), nullptr) != 1)
  {
    std::cout << "Failed to initialize HMAC context" << std::endl;
    return 1;
  }
  if (HMAC_Update(hctx.get(), frame->iv(), frame->iv_size()) != 1)
  {
    std::cout << "Failed to update HMAC" << std::endl;
    return 1;
  }

  // read and process attachment data in 8MB chunks
  uint32_t const BUFFERSIZE = 8 * 1024;
  unsigned char encrypteddatabuffer[BUFFERSIZE];
  uint32_t processed = 0;
  uint32_t size = frame->length();
  std::unique_ptr<unsigned char[]> decryptedattachmentdata(new unsigned char[size]); // to hold the data
  while (processed < size)
  {
    if (!file.read(reinterpret_cast<char *>(encrypteddatabuffer), std::min(size - processed, BUFFERSIZE)))
    {
      std::cout << " STOPPING BEFORE END OF ATTACHMENT!!!" << (file.eof() ? " (EOF) " : "") << std::endl;
      return 1;
    }
    uint32_t read = file.gcount();

    // update MAC with read data
    if (HMAC_Update(hctx.get(), encrypteddatabuffer, read) != 1)
    {
      std::cout << "Failed to update HMAC" << std::endl;
      return 1;
    }

    // decrypt the read data;
    int spaceleft = size - processed;
    if (EVP_DecryptUpdate(ctx.get(), decryptedattachmentdata.get() + processed, &spaceleft, encrypteddatabuffer, read) != 1)
    {
      std::cout << "Failed to decrypt data" << std::endl;
      return 1;
    }

    processed += read;
    //return;
  }
  DEBUGOUT("Read ", processed, " bytes");

  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  if (HMAC_Final(hctx.get(), hash, &digest_size) != 1)
  {
    std::cout << "Failed to finalize MAC" << std::endl;
    return 1;
  }

  unsigned char theirMac[MACSIZE];
  if (!file.read(reinterpret_cast<char *>(theirMac), MACSIZE))
  {
    std::cout << " STOPPING BEFORE END OF ATTACHMENT!!! 2 " << std::endl;
    return 1;
  }
  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));

  bool badmac = false;

  if (std::memcmp(theirMac, hash, MACSIZE) != 0)
  {
    std::cout << "" << std::endl;
    std::cout << "WARNING: Bad MAC in attachmentdata: theirMac: " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    std::cout << "                                      ourMac: " << bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH) << std::endl;

    badmac = true;
  }
  else
    badmac = false;

  if (frame->setAttachmentData(decryptedattachmentdata.release()))
  {
    if (badmac)
      return -1;
    return 0;
  }
  return 1;
}

#else

int BaseDecryptor::getAttachment(FrameWithAttachment *frame) // static
{
  std::ifstream file(frame->filename(), std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    std::cout << "Failed to open backup file for reading attachment" << std::endl;
    return 1;
  }

  //std::cout << "Getting attachment: " << frame->filepos() << " + " << frame->length() << std::endl;
  file.seekg(frame->filepos(), std::ios_base::beg);

  //uintToFourBytes(d_iv, d_counter++); // done in getFrame
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption decryptor(frame->cipherkey(), frame->cipherkey_size(), frame->iv());

  CryptoPP::HMAC<CryptoPP::SHA256> hmac(frame->mackey(), frame->mackey_size());
  hmac.Update(frame->iv(), frame->iv_size());

  // read and process attachment data in 8MB chunks
  uint32_t const BUFFERSIZE = 8 * 1024;
  unsigned char encrypteddatabuffer[BUFFERSIZE];
  uint32_t processed = 0;
  uint32_t size = frame->length();
  unsigned char *decryptedattachmentdata = new unsigned char[size]; // to hold the data
  while (processed < size)
  {
    if (!file.read(reinterpret_cast<char *>(encrypteddatabuffer), std::min(size - processed, BUFFERSIZE)))
    {
      std::cout << " STOPPING BEFORE END OF ATTACHMENT!!!" << (file.eof() ? " (EOF) " : "") << std::endl;
      delete[] decryptedattachmentdata;
      return 1;
    }
    uint32_t read = file.gcount();

    hmac.Update(encrypteddatabuffer, read);

    decryptor.ProcessData(decryptedattachmentdata + processed, encrypteddatabuffer, read);

    processed += read;
    //return;
  }
  DEBUGOUT("Read ", processed, " bytes");

  unsigned char ourMac[CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE];
  hmac.Final(ourMac);

  unsigned char theirMac[MACSIZE];
  if (!file.read(reinterpret_cast<char *>(theirMac), MACSIZE))
  {
    std::cout << " STOPPING BEFORE END OF ATTACHMENT!!! 2 " << std::endl;
    delete[] decryptedattachmentdata;
    return 1;
  }
  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE));

  bool badmac = false;

  if (std::memcmp(theirMac, ourMac, 10) != 0)
  {
    std::cout << "" << std::endl;
    std::cout << "WARNING: Bad MAC in attachmentdata: theirMac: " << bepaald::bytesToHexString(theirMac, MACSIZE) << std::endl;
    std::cout << "                                      ourMac: " << bepaald::bytesToHexString(ourMac, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE) << std::endl;

    badmac = true;
  }
  else
    badmac = false;

  if (frame->setAttachmentData(decryptedattachmentdata))
  {
    if (badmac)
      return -1;
    return 0;
  }
  return 1;
}

#endif
