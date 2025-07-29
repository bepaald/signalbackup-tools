/*
  Copyright (C) 2025  Selwin van Dijk

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

#include "adbbackupattachmentreader.h"
#include "../framewithattachment/framewithattachment.h"
#include "../common_filesystem.h"

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

inline BaseAttachmentReader::ReturnCode AdbBackupAttachmentReader::getAttachment(FrameWithAttachment *frame, bool verbose) // virtual override
{
  if (verbose) [[unlikely]]
    Logger::message("Starting get encrypted AdbBackupAttachment data");

  std::ifstream encryptedfile(d_path, std::ios_base::in | std::ios_base::binary);
  if (!encryptedfile.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open file '", d_path, "'");
    return ReturnCode::ERROR;
  }

  int encryptedfile_size = bepaald::fileSize(d_path);
  std::unique_ptr<unsigned char[]> encryptedfiledata(new unsigned char[encryptedfile_size]);
  if (!encryptedfile.read(reinterpret_cast<char *>(encryptedfiledata.get()), encryptedfile_size) ||
      encryptedfile.gcount() != encryptedfile_size)
  {
    Logger::error("Failed to read data from file '", d_path, "'");
    return ReturnCode::ERROR;
  }

  // check HMAC
  unsigned int digest_size = SHA_DIGEST_LENGTH;
  std::unique_ptr<unsigned char []> hash(new unsigned char[digest_size]);
  HMAC(EVP_sha1(), d_mackey.get(), d_mackey_length, encryptedfiledata.get(),
       encryptedfile_size - SHA_DIGEST_LENGTH, hash.get(), &digest_size);
  if (std::memcmp(hash.get(), encryptedfiledata.get() + (encryptedfile_size - SHA_DIGEST_LENGTH), SHA_DIGEST_LENGTH) != 0) [[unlikely]]
  {
    Logger::error("HMAC failed for part '", d_path, "'");
    return ReturnCode::ERROR;
  }

  // decrypt file

  unsigned char *iv = encryptedfiledata.get(); /// first 16 bytes is IV
  // create context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), nullptr, d_encryptionkey.get(), iv) != 1) [[unlikely]]
  {
    Logger::error("CTX INIT FAILED");
    return ReturnCode::ERROR;
  }

  int out_len = encryptedfile_size - (16 + SHA_DIGEST_LENGTH); // this includes padding, the EVP_ routines will shrink this number
  std::unique_ptr<unsigned char []> out(new unsigned char[out_len]);
  if (EVP_DecryptUpdate(ctx.get(), out.get(), &out_len,
                        encryptedfiledata.get() + 16, encryptedfile_size - (16 + SHA_DIGEST_LENGTH)) != 1) [[unlikely]]
  {
    Logger::error("Failed to decrypt data");
    return ReturnCode::ERROR;
  }

  //std::cout << "Filesize: " << filedata_size << std::endl;
  //std::cout << "decrytpsize: " << padded_out_len << std::endl;

  // Finalize: decrypt trailling bytes, check the padding, and discard if ok
  int tail_len = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), out.get() + out_len, &tail_len) != 1) [[unlikely]]
  {
    Logger::error("Failed to finalize decryption");
    return ReturnCode::ERROR;
  }
  out_len += tail_len;

  //std::cout << "Filesize: " << filedata_size << std::endl;
  //std::cout << "decrytpsize: " << padded_out_len << std::endl;
  //std::cout << bepaald::bytesToHexString(out.get(), padded_out_len) << std::endl;

  frame->setAttachmentDataBacked(out.release(), out_len); // the frame will now own the data...

  return ReturnCode::OK;
}
