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

#include "desktopattachmentreader.h"

#include "../common_filesystem.h"

BaseAttachmentReader::ReturnCode DesktopAttachmentReader::getAttachmentData(unsigned char **rawdata, bool verbose)
{
  if (verbose) [[unlikely]]
    Logger::message("Starting get encrypted DesktopAttachment data");

  // set AES+MAC key
  auto [tmpdata, key_data_length] = Base64::base64StringToBytes(d_key);
  std::unique_ptr<unsigned char[]> key_data(tmpdata);
  if (!tmpdata || key_data_length != 64) [[unlikely]]
  {
    Logger::error("Failed to get key data for decrypting attachment.");
    return ReturnCode::ERROR;
  }
  unsigned char *aeskey = key_data.get();
  uint64_t constexpr mackey_length = 32;
  unsigned char *mackey = key_data.get() + 32;

  // open file
  std::ifstream file(std::filesystem::path(d_path), std::ios_base::in | std::ios_base::binary);
  if (!file.is_open()) [[unlikely]]
  {
    Logger::error("Failed to open file '", d_path, "'");
    return ReturnCode::ERROR;
  }

  // set iv/data length.
  int64_t constexpr iv_length = 16;
  //file.seekg(0, std::ios_base::end);
  //int64_t data_length = file.tellg() - static_cast<int64_t>(iv_length + mackey_length);
  //file.seekg(0, std::ios_base::beg);
  int64_t data_length = bepaald::fileSize(d_path) - static_cast<int64_t>(iv_length + mackey_length);
  if (data_length <= 0) [[unlikely]]
  {
    Logger::error("Got bad data length (", data_length, ")");
    return ReturnCode::ERROR;
  }

  if (verbose) [[unlikely]]
  {
    Logger::message("Attachment length (iv + data + mackey): ", iv_length, " + ", data_length, " + ", mackey_length);
    Logger::message("                                d_size: ", d_size);
  }

  // set iv
  std::unique_ptr<unsigned char[]> iv(new unsigned char[iv_length]);
  if (!file.read(reinterpret_cast<char *>(iv.get()), iv_length) ||
      file.gcount() != iv_length) [[unlikely]]
  {
    Logger::error("Failed to read iv");
    return ReturnCode::ERROR;
  }
  if (verbose) [[unlikely]]
    Logger::message("Read IV: ", bepaald::bytesToHexString(iv.get(), iv_length));

  // set data to decrypt
  std::unique_ptr<unsigned char[]> data(new unsigned char[data_length]);
  if (!file.read(reinterpret_cast<char *>(data.get()), data_length) ||
      file.gcount() != data_length) [[unlikely]]
  {
    Logger::error("Failed to read in file data");
    return ReturnCode::ERROR;
  }
  if (verbose) [[unlikely]]
    Logger::message("Read data: ", bepaald::bytesToHexString(data.get(), data_length > 32 ? 32 : data_length), "... (", data_length, " bytes total)");

  // set theirMAC
  int64_t theirmac_length = 32;
  std::unique_ptr<unsigned char[]> theirmac(new unsigned char[32]);
  if (!file.read(reinterpret_cast<char *>(theirmac.get()), theirmac_length) ||
      file.gcount() != theirmac_length) [[unlikely]]
  {
    Logger::error("Failed to read theirmac");
    return ReturnCode::ERROR;
  }
  if (verbose) [[unlikely]]
    Logger::message("Read MAC: ", bepaald::bytesToHexString(theirmac.get(), theirmac_length));

  // calculate MAC
  evp_md_st const *digest = EVP_sha256();
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  char digestname[] = "SHA256";
  std::unique_ptr<EVP_MAC, decltype(&::EVP_MAC_free)> mac(EVP_MAC_fetch(nullptr, "hmac", nullptr), &::EVP_MAC_free);
  std::unique_ptr<EVP_MAC_CTX, decltype(&::EVP_MAC_CTX_free)> hctx(EVP_MAC_CTX_new(mac.get()), &::EVP_MAC_CTX_free);
  OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("digest", digestname, 0), OSSL_PARAM_construct_end()};
  if (EVP_MAC_init(hctx.get(), mackey, mackey_length, params) != 1) [[unlikely]]
  {
    Logger::error("Failed to initialize HMAC context");
    return ReturnCode::ERROR;
  }
  std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[EVP_MD_size(digest)]);
  if (EVP_MAC_update(hctx.get(), iv.get(), iv_length) != 1 ||
      EVP_MAC_update(hctx.get(), data.get(), data_length) != 1 ||
      EVP_MAC_final(hctx.get(), calculatedmac.get(), nullptr, EVP_MD_size(digest)) != 1) [[unlikely]]
  {
    Logger::error("Failed to update/finalize hmac");
    return ReturnCode::ERROR;
  }
#else // OPENSSL 1.x
  std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
  if (HMAC_Init_ex(hctx.get(), mackey, mackey_length, digest, nullptr) != 1) [[unlikely]]
  {
    Logger::error("Failed to initialize HMAC context");
    return ReturnCode::ERROR;
  }
  std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[32]);
  unsigned int finalsize = EVP_MD_size(digest);
  if (HMAC_Update(hctx.get(), iv.get(), iv_length) != 1 ||
      HMAC_Update(hctx.get(), data.get(), data_length) != 1 ||
      HMAC_Final(hctx.get(), calculatedmac.get(), &finalsize) != 1) [[unlikely]]
  {
    Logger::error("Failed to update/finalize hmac");
    return ReturnCode::ERROR;
  }
#endif

  if (std::memcmp(calculatedmac.get(), theirmac.get(), theirmac_length) != 0) [[unlikely]]
  {
    Logger::error("MAC failed! (theirMAC: ", bepaald::bytesToHexString(theirmac.get(), theirmac_length));
    Logger::error_indent("               ourMAC: ", bepaald::bytesToHexString(calculatedmac.get(), theirmac_length), ")");
    return ReturnCode::BADMAC;
  }
  if (verbose) [[unlikely]]
    Logger::message("MAC check OK!");

  // DECRYPT DATA:

  // init decryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (!ctx) [[unlikely]]
  {
    Logger::error("Failed to create decryption context");
    return ReturnCode::ERROR;
  }

  // init decrypt
  if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, aeskey, iv.get())) [[unlikely]]
  {
    Logger::error("Failed to initialize decryption operation");
    return ReturnCode::ERROR;
  }

  // decrypt update
  int out_len = 0;
  int output_length = data_length;
  std::unique_ptr<unsigned char[]> output(new unsigned char[output_length]);
  if (EVP_DecryptUpdate(ctx.get(), output.get(), &out_len, data.get(), output_length) != 1) [[unlikely]]
  {
    Logger::error("Failed to update decryption");
    return ReturnCode::ERROR;
  }

  // decrypt final
  int tail_len = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), output.get() + out_len, &tail_len) != 1) [[unlikely]]
  {
    Logger::error("Failed to finalize decryption");
    return ReturnCode::ERROR;
  }
  out_len += tail_len;
  //std::cout << out_len << std::endl;

  //std::cout << "Start of decrypted data: " << bepaald::bytesToHexString(output.get(), 64) << std::endl;
  *rawdata = output.release();

  if (verbose) [[unlikely]]
    Logger::message("Successfully got DesktopAttachment data");

  // I have a single sticker in the database with an incorrect 'size' column (in the message).
  // All attachments need their real (decrypted) size known in order to decrypt properly, the
  // encrypted size has iv+mac+an unknown amount of padding. This could cause a buffer overflow.
  // For the above mentioned sticker, the real size was in the 'sticker' table (as opposed to
  // the message), but this check is here to maybe catch other similar cases...
  if (d_size >= static_cast<uint64_t>(out_len)) [[unlikely]]
  {
    Logger::warning("Decrypted size is larger or equal to total data size.");
    Logger::warning_indent("The total size was likely imported from Desktop incorrectly");
  }
  return ReturnCode::OK;
}

BaseAttachmentReader::ReturnCode DesktopAttachmentReader::getEncryptedAttachment(FrameWithAttachment *frame, bool verbose)
{
  unsigned char *data = nullptr;
  ReturnCode ret = getAttachmentData(&data, verbose);
  if (ret == ReturnCode::OK) [[likely]]
    frame->setAttachmentDataBacked(data, d_size);
  return ret;
}
