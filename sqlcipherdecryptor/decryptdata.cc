/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

#include "sqlcipherdecryptor.ih"

#include "../common_bytes.h"

bool SqlCipherDecryptor::decryptData(std::ifstream *dbfile)
{
  // decrypt data
  d_decrypteddata = new unsigned char[d_decrypteddatasize];
  unsigned int pos = 0;

  // write header
  std::memcpy(d_decrypteddata + pos, s_sqlliteheader, s_sqlliteheader_size);
  pos += s_sqlliteheader_size;

    /*
    PAGE

                                                         page_size
     -------------------------------------------------------------------------------------------------------------------------------
    /                                                                     real_page_size                                            \
    |                                 -----------------------------------------------------------------------------------------------|
    |                                /                                                                                               |
    |                                |                                                                                               |
    |                                |     page + (real_page_size - (digest_size + page_padding) - iv_size)                          |
    |                                |                v                                                                              |

    [salt, 16 bytes, only first page][encrypted bytes][iv, 16 bytes][mac, padded to 16 bytes (for version < 3, 20 bytes, padded to 32]


    */

  unsigned int iv_size = 16;
  unsigned int page_padding = (((d_digestsize - 1) | 15) + 1) - d_digestsize;  // pad to multiple of 16 bytes ??? (maybe 32?)
  std::unique_ptr<unsigned char[]> page(new unsigned char[d_pagesize]);
  unsigned int pagenumber = 1;

  // encryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> dctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);

  while (true)
  {
    unsigned int real_page_size = pagenumber == 1 ? d_pagesize - d_saltsize : d_pagesize;

    if (!dbfile->read(reinterpret_cast<char *>(page.get()), real_page_size))
    {
      if (dbfile->gcount() == 0 && dbfile->eof()) // all bytes were read
        break;

      // we failed to read an entire page, but we did read _some_ data from the file, this should not be possible
      Logger::error("Unexpectedly failed to read next block", (dbfile->eof() ? " (EOF)" : ""));
      return false;
    }

    // these pointers all point to specific data inside 'page' (which was just read from dbfile
    unsigned char *page_data_to_hash = page.get();
    unsigned int page_data_to_hash_size = real_page_size - (d_digestsize + page_padding);
    unsigned char *iv = page.get() + page_data_to_hash_size - iv_size;
    unsigned char *page_encrypted_data = page.get();
    unsigned int page_encrypted_data_size = page_data_to_hash_size - iv_size;

    // calculate MAC
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    std::unique_ptr<EVP_MAC, decltype(&::EVP_MAC_free)> mac(EVP_MAC_fetch(nullptr, "hmac", nullptr), &::EVP_MAC_free);
    std::unique_ptr<EVP_MAC_CTX, decltype(&::EVP_MAC_CTX_free)> hctx(EVP_MAC_CTX_new(mac.get()), &::EVP_MAC_CTX_free);
    OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("digest", d_digestname, 0), OSSL_PARAM_construct_end()};
    if (EVP_MAC_init(hctx.get(), d_hmackey, d_hmackeysize, params) != 1)
    {
      Logger::error("Failed to initialize HMAC context");
      return false;
    }
    std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[d_digestsize]);
    if (EVP_MAC_update(hctx.get(), page_data_to_hash, page_data_to_hash_size) != 1 ||
        EVP_MAC_update(hctx.get(), reinterpret_cast<unsigned char *>(&pagenumber), sizeof(pagenumber)) != 1 ||
        EVP_MAC_final(hctx.get(), calculatedmac.get(), nullptr, d_digestsize) != 1)
    {
      Logger::error("Failed to update/finalize hmac");
      return false;
    }
#else
    std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
    if (HMAC_Init_ex(hctx.get(), d_hmackey, d_hmackeysize, d_digest, nullptr) != 1)
    {
      Logger::error("Failed to initialize HMAC context");
      return false;
    }
    std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[d_digestsize]);
    if (HMAC_Update(hctx.get(), page_data_to_hash, page_data_to_hash_size) != 1 ||
        HMAC_Update(hctx.get(), reinterpret_cast<unsigned char *>(&pagenumber), sizeof(pagenumber)) != 1 ||
        HMAC_Final(hctx.get(), calculatedmac.get(), &d_digestsize) != 1)
    {
      Logger::error("Failed to update/finalize hmac");
      return false;
    }
#endif

    // compare calculated mac to the mac from file
    if (std::memcmp(page.get() + (real_page_size - (d_digestsize + page_padding)), calculatedmac.get(), d_digestsize) != 0) [[unlikely]]
    {
      // note: a bad mac can occur if the page is empty (all 0x00). An empty page is not an error, and should simply be skipped.
      bool containsdata = false;
      for (uint i = 0; i < page_data_to_hash_size; ++i)
      {
        if (page_data_to_hash[i] != 0x00)
        {
          containsdata = true;
          break;
        }
      }
      if (!containsdata) // UNTESTED  // skip decryption, but set entire page of zeros??
      {
        if (d_verbose) [[unlikely]]
          Logger::message("Read empty page from SqlCipherDatabase. Inserting empty page in output...");

        int decodedframelength = d_pagesize;
        if (pagenumber == 1)
          decodedframelength -= d_saltsize;

        std::memset(d_decrypteddata + pos, 0, decodedframelength); // write all-zero page
        pos += decodedframelength;

        ++pagenumber;
        continue;
      }
      else // mac did not match, but page contained data -> ERROR
      {
        Logger::error("BAD MAC! (pagenumber: ", pagenumber, " (at ", static_cast<unsigned int>(dbfile->tellg()) - (d_digestsize + page_padding), "/", d_decrypteddatasize, "))");
        Logger::error_indent("MAC in file: ", bepaald::bytesToHexString(page.get() + (real_page_size - (d_digestsize + page_padding)), d_digestsize));
        Logger::error_indent("Calculated : ", bepaald::bytesToHexString(calculatedmac.get(), d_digestsize));
        return false;
      }
    }

    //std::cout << ("MAC OK!" << std::endl;

    int decodedframelength = d_pagesize;
    if (pagenumber == 1)
      decodedframelength -= d_saltsize;

    // init decryptor
    if (EVP_DecryptInit_ex(dctx.get(), EVP_aes_256_cbc(), nullptr, d_key, iv) != 1)
    {
      Logger::error("CTX INIT FAILED");
      return false;
    }

    // disable padding
    EVP_CIPHER_CTX_set_padding(dctx.get(), 0);

    //std::cout << ("INIT OK!" << std::endl;
    int actualdecodedframelength = 0;
    if (EVP_DecryptUpdate(dctx.get(), d_decrypteddata + pos, &actualdecodedframelength, page_encrypted_data, page_encrypted_data_size) != 1)
    {
      Logger::error("Failed to update decryption context");
      ERR_print_errors_fp(stderr);
      return false;
    }
    //std::cout << ("DECRYPT OK!" << std::endl;
    // reset decryptor
    if (EVP_CIPHER_CTX_reset(dctx.get()) != 1)
    {
      Logger::error("CTX RESET FAILED");
      return false;
    }
    //std::cout << "RESET OK!" << std::endl;
    std::memset(d_decrypteddata + pos + page_encrypted_data_size, 0, decodedframelength - page_encrypted_data_size); // append zeros
    pos += decodedframelength;

    //std::cout << "Writing " << decodedframelength << " bytes to file" << std::endl;
    //outputdb.write(reinterpret_cast<char *>(decodedframe2.get()), decodedframelength);

    ++pagenumber;
  }
  return true;
}
