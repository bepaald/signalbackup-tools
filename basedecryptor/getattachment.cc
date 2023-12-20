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

#include "basedecryptor.ih"

int BaseDecryptor::getAttachment(FrameWithAttachment *frame, bool verbose) // static
{
  //std::cout << " *** REALLY GETTING ATTACHMENT ***" << std::endl;

  std::ifstream file(frame->filename(), std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    Logger::error("Failed to open backup file '", frame->filename(), "' for reading attachment");
    return 1;
  }

  if (frame->length() == 0) [[unlikely]]
    Logger::warning("Aksed to read 0-byte attachment");

  if (verbose) [[unlikely]]
    Logger::message("Decrypting attachment data, length: ", frame->length());

  //std::cout << "Getting attachment: " << frame->filepos() << " + " << frame->length() << std::endl;
  file.seekg(frame->filepos(), std::ios_base::beg);

  // RAW UNENCRYPTED FILE ATTACHMENT
  if (!frame->cipherkey() ||
      !frame->mackey() ||
      !frame->iv())
  {
    //std::cout << "GETTING RAW UNENCRYPTED DATA" << std::endl;

    std::unique_ptr<unsigned char[]> decryptedattachmentdata(new unsigned char[frame->length()]); // to hold the data
    if (!file.read(reinterpret_cast<char *>(decryptedattachmentdata.get()), frame->length()))
    {
      Logger::error("Failed to read raw attachment \"", frame->filename(), "\"");
      return 1;
    }
    frame->setAttachmentData(decryptedattachmentdata.release());
    return 0;
  }

  // to decrypt the data
  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  // init
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, frame->cipherkey(), frame->iv()) != 1)
  {
    Logger::error("CTX INIT FAILED");
    return 1;
  }

  // to calculate the MAC
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  std::unique_ptr<EVP_MAC, decltype(&::EVP_MAC_free)> mac(EVP_MAC_fetch(nullptr, "hmac", nullptr), &::EVP_MAC_free);
  std::unique_ptr<EVP_MAC_CTX, decltype(&::EVP_MAC_CTX_free)> hctx(EVP_MAC_CTX_new(mac.get()), &::EVP_MAC_CTX_free);
  char digest[] = "SHA256";
  OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("digest", digest, 0), OSSL_PARAM_construct_end()};
#else
  std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
#endif


#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  if (EVP_MAC_init(hctx.get(), frame->mackey(), frame->mackey_size(), params) != 1)
#else
  if (HMAC_Init_ex(hctx.get(), frame->mackey(), frame->mackey_size(), EVP_sha256(), nullptr) != 1)
#endif
  {
    Logger::error("Failed to initialize HMAC context");
    return 1;
  }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  if (EVP_MAC_update(hctx.get(), frame->iv(), frame->iv_size()) != 1)
#else
  if (HMAC_Update(hctx.get(), frame->iv(), frame->iv_size()) != 1)
#endif
  {
    Logger::error("Failed to update HMAC");
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
      Logger::error("STOPPING BEFORE END OF ATTACHMENT!!!", (file.eof() ? " (EOF) " : ""));
      return 1;
    }
    uint32_t read = file.gcount();

    // update MAC with read data
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    if (EVP_MAC_update(hctx.get(), encrypteddatabuffer, read) != 1)
#else
    if (HMAC_Update(hctx.get(), encrypteddatabuffer, read) != 1)
#endif
    {
      Logger::error("Failed to update HMAC");
      return 1;
    }

    // decrypt the read data;
    int spaceleft = size - processed;
    if (EVP_DecryptUpdate(ctx.get(), decryptedattachmentdata.get() + processed, &spaceleft, encrypteddatabuffer, read) != 1)
    {
      Logger::error("Failed to decrypt data");
      return 1;
    }

    processed += read;
    //return;
  }
  DEBUGOUT("Read ", processed, " bytes");

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  unsigned long int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  if (EVP_MAC_final(hctx.get(), hash, nullptr, digest_size) != 1)
#else
  unsigned int digest_size = SHA256_DIGEST_LENGTH;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  if (HMAC_Final(hctx.get(), hash, &digest_size) != 1)
#endif
  {
    Logger::error("Failed to finalize MAC");
    return 1;
  }

  unsigned char theirMac[MACSIZE];
  if (!file.read(reinterpret_cast<char *>(theirMac), MACSIZE))
  {
    Logger::error("STOPPING BEFORE END OF ATTACHMENT!!! 2 ");
    return 1;
  }
  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));

  bool badmac = false;

  if (std::memcmp(theirMac, hash, MACSIZE) != 0)
  {
    Logger::warning("Bad MAC in attachmentdata: theirMac: ", bepaald::bytesToHexString(theirMac, MACSIZE));
    Logger::warning_indent("                             ourMac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
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
