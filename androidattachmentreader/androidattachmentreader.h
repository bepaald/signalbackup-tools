/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef ANDROIDATTACHMENTREADER_H_
#define ANDROIDATTACHMENTREADER_H_

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

#include "../common_be.h"
#include "../common_bytes.h"
#include "../baseattachmentreader/baseattachmentreader.h"
#include "../framewithattachment/framewithattachment.h"
#include "../cryptbase/cryptbase.h"

class AndroidAttachmentReader : public AttachmentReader<AndroidAttachmentReader>
{
  std::string d_filename;
  uint64_t d_filepos;
  unsigned char *d_iv;
  uint64_t d_mackey_size;
  unsigned char *d_mackey;
  uint64_t d_cipherkey_size;
  unsigned char *d_cipherkey;
  uint32_t d_attachmentdata_size;
  uint32_t d_iv_size;
 public:
  inline AndroidAttachmentReader(unsigned char const *iv, uint32_t iv_size,
                                 unsigned char const *mackey, uint64_t mackey_size,
                                 unsigned char const *cipherkey, uint64_t cipherkey_size,
                                 uint32_t attsize, std::string const &filename, uint64_t filepos);
  inline AndroidAttachmentReader(AndroidAttachmentReader const &other);
  inline AndroidAttachmentReader(AndroidAttachmentReader &&other);
  inline AndroidAttachmentReader &operator=(AndroidAttachmentReader const &other);
  inline AndroidAttachmentReader &operator=(AndroidAttachmentReader &&other);
  inline virtual ~AndroidAttachmentReader() override;
  inline virtual int getAttachment(FrameWithAttachment *frame,  bool verbose) override;
};

inline AndroidAttachmentReader::AndroidAttachmentReader(unsigned char const *iv, uint32_t iv_size,
                                                        unsigned char const *mackey, uint64_t mackey_size,
                                                        unsigned char const *cipherkey, uint64_t cipherkey_size,
                                                        uint32_t attsize, std::string const &filename, uint64_t filepos)
  :
  d_filepos(0),
  d_iv(nullptr),
  d_mackey_size(0),
  d_mackey(nullptr),
  d_cipherkey_size(0),
  d_cipherkey(nullptr),
  d_attachmentdata_size(0),
  d_iv_size(0)
{
  d_iv_size = iv_size;
  if (iv)
  {
    d_iv = new unsigned char[d_iv_size];
    std::memcpy(d_iv, iv, d_iv_size);
  }

  d_cipherkey_size = cipherkey_size;
  if (cipherkey)
  {
    d_cipherkey = new unsigned char[d_cipherkey_size];
    std::memcpy(d_cipherkey, cipherkey, d_cipherkey_size);
  }

  d_mackey_size = mackey_size;
  if (mackey)
  {
    d_mackey = new unsigned char[d_mackey_size];
    std::memcpy(d_mackey, mackey, d_mackey_size);
  }

  d_attachmentdata_size = attsize;

  d_filename = filename;
  d_filepos = filepos;
}

inline AndroidAttachmentReader::AndroidAttachmentReader(AndroidAttachmentReader const &other)
  :
  AttachmentReader(other),
  d_filename(other.d_filename),
  d_filepos(other.d_filepos),
  d_iv(nullptr),
  d_mackey_size(other.d_mackey_size),
  d_mackey(nullptr),
  d_cipherkey_size(other.d_cipherkey_size),
  d_cipherkey(nullptr),
  d_attachmentdata_size(other.d_attachmentdata_size),
  d_iv_size(other.d_iv_size)
{
  if (other.d_iv)
  {
    d_iv = new unsigned char[d_iv_size];
    std::memcpy(d_iv, other.d_iv, d_iv_size);
  }

  if (other.d_mackey)
  {
    d_mackey = new unsigned char[d_mackey_size];
    std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
  }

  if (other.d_cipherkey)
  {
    d_cipherkey = new unsigned char[d_cipherkey_size];
    std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
  }
}

inline AndroidAttachmentReader::AndroidAttachmentReader(AndroidAttachmentReader &&other)
  :
  AttachmentReader(other),
  d_filename(std::move(other.d_filename)),
  d_filepos(std::move(other.d_filepos)),
  d_iv(std::move(other.d_iv)),
  d_mackey_size(std::move(other.d_mackey_size)),
  d_mackey(std::move(other.d_mackey)),
  d_cipherkey_size(std::move(other.d_cipherkey_size)),
  d_cipherkey(std::move(other.d_cipherkey)),
  d_attachmentdata_size(std::move(other.d_attachmentdata_size)),
  d_iv_size(std::move(other.d_iv_size))
{
  other.d_attachmentdata_size = 0;
  other.d_iv_size = 0;
  other.d_iv = nullptr;
  other.d_mackey_size = 0;
  other.d_mackey = nullptr;
  other.d_cipherkey_size = 0;
  other.d_cipherkey = nullptr;
}

inline AndroidAttachmentReader &AndroidAttachmentReader::operator=(AndroidAttachmentReader const &other)
{
  if (this != &other)
  {
    bepaald::destroyPtr(&d_iv, &d_iv_size);
    bepaald::destroyPtr(&d_mackey, &d_mackey_size);
    bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);

    d_iv_size = other.d_iv_size;
    d_mackey_size = other.d_mackey_size;
    d_cipherkey_size = other.d_cipherkey_size;

    if (other.d_iv)
    {
      d_iv = new unsigned char[d_iv_size];
      std::memcpy(d_iv, other.d_iv, d_iv_size);
    }

    if (other.d_mackey)
    {
      d_mackey = new unsigned char[d_mackey_size];
      std::memcpy(d_mackey, other.d_mackey, d_mackey_size);
    }

    if (other.d_cipherkey)
    {
      d_cipherkey = new unsigned char[d_cipherkey_size];
      std::memcpy(d_cipherkey, other.d_cipherkey, d_cipherkey_size);
    }

    d_filename = other.d_filename;
    d_filepos = other.d_filepos;
    d_attachmentdata_size = other.d_attachmentdata_size;
  }
  return *this;
}

inline AndroidAttachmentReader &AndroidAttachmentReader::operator=(AndroidAttachmentReader &&other)
{
  if (this != &other)
  {
    // destroy any data this already owns
    bepaald::destroyPtr(&d_iv, &d_iv_size);
    bepaald::destroyPtr(&d_mackey, &d_mackey_size);
    bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);

    // take over other's data
    d_iv = std::move(other.d_iv);
    d_iv_size = std::move(other.d_iv_size);
    d_mackey = std::move(other.d_mackey);
    d_mackey_size = std::move(other.d_mackey_size);
    d_cipherkey = std::move(other.d_cipherkey);
    d_cipherkey_size = std::move(other.d_cipherkey_size);
    d_filename = std::move(other.d_filename);
    d_filepos = std::move(other.d_filepos);
    d_attachmentdata_size = std::move(other.d_attachmentdata_size);

    // invalidate other
    other.d_iv = nullptr;
    other.d_iv_size = 0;
    other.d_mackey = nullptr;
    other.d_mackey_size = 0;
    other.d_cipherkey = nullptr;
    other.d_cipherkey_size = 0;
  }
  return *this;
}

inline AndroidAttachmentReader::~AndroidAttachmentReader()
{
  bepaald::destroyPtr(&d_iv, &d_iv_size);
  bepaald::destroyPtr(&d_mackey, &d_mackey_size);
  bepaald::destroyPtr(&d_cipherkey, &d_cipherkey_size);
}

inline int AndroidAttachmentReader::getAttachment(FrameWithAttachment *frame, bool verbose) // virtual
{
  //std::cout << " *** REALLY GETTING ATTACHMENT (ANDROID) ***" << std::endl;

  std::ifstream file(d_filename, std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    Logger::error("Failed to open backup file '", d_filename, "' for reading attachment");
    return 1;
  }

  if (d_attachmentdata_size == 0) [[unlikely]]
    Logger::warning("Asked to read 0-byte attachment");

  if (verbose) [[unlikely]]
    Logger::message("Decrypting attachment data, length: ", d_attachmentdata_size);

  //std::cout << "Getting attachment: " << frame->filepos() << " + " << frame->length() << std::endl;
  file.seekg(d_filepos, std::ios_base::beg);

  // to decrypt the data
  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  // disable padding
  EVP_CIPHER_CTX_set_padding(ctx.get(), 0);

  // init
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_ctr(), nullptr, d_cipherkey, d_iv) != 1)
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
  if (EVP_MAC_init(hctx.get(), d_mackey, d_mackey_size, params) != 1)
#else
  if (HMAC_Init_ex(hctx.get(), d_mackey, d_mackey_size, EVP_sha256(), nullptr) != 1)
#endif
  {
    Logger::error("Failed to initialize HMAC context");
    return 1;
  }
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  if (EVP_MAC_update(hctx.get(), d_iv, d_iv_size) != 1)
#else
  if (HMAC_Update(hctx.get(), d_iv, d_iv_size) != 1)
#endif
  {
    Logger::error("Failed to update HMAC");
    return 1;
  }

  // read and process attachment data in 8MB chunks
  uint32_t const BUFFERSIZE = 8 * 1024;
  unsigned char encrypteddatabuffer[BUFFERSIZE];
  uint32_t processed = 0;
  uint32_t size = d_attachmentdata_size;
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

  unsigned char theirMac[CryptBase::MACSIZE];
  if (!file.read(reinterpret_cast<char *>(theirMac), CryptBase::MACSIZE))
  {
    Logger::error("STOPPING BEFORE END OF ATTACHMENT!!! 2 ");
    return 1;
  }
  DEBUGOUT("theirMac         : ", bepaald::bytesToHexString(theirMac, CryptBase::MACSIZE));
  DEBUGOUT("ourMac           : ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));

  bool badmac = false;

  if (std::memcmp(theirMac, hash, CryptBase::MACSIZE) != 0)
  {
    Logger::warning("Bad MAC in attachmentdata: theirMac: ", bepaald::bytesToHexString(theirMac, CryptBase::MACSIZE));
    Logger::warning_indent("                             ourMac: ", bepaald::bytesToHexString(hash, SHA256_DIGEST_LENGTH));
    badmac = true;
  }
  else
    badmac = false;

  if (frame->setAttachmentDataBacked(decryptedattachmentdata.release(), d_attachmentdata_size))
  {
    if (badmac)
      return -1;
    return 0;
  }
  return 1;
}

#endif
