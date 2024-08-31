/*
  Copyright (C) 2024  Selwin van Dijk

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

int DesktopAttachmentReader::getAttachmentData(unsigned char **rawdata, bool verbose [[maybe_unused]])
{
  // set AES+MAC key
  auto [tmpdata, key_data_length] = Base64::base64StringToBytes(d_key);
  std::unique_ptr<unsigned char[]> key_data(tmpdata);
  //uint64_t aeskey_length = 32;
  unsigned char *aeskey = key_data.get();
  uint64_t mackey_length = 32;
  unsigned char *mackey = key_data.get() + 32;

  // open file
  std::ifstream file(d_path, std::ios_base::in | std::ios_base::binary);
  if (!file.is_open())
  {
    Logger::error("Failed to open file '", d_path, "'");
    return 1;
  }

  // set iv/data length.
  int64_t iv_length = 16;
  file.seekg(0, std::ios_base::end);
  int64_t data_length = file.tellg() - static_cast<int64_t>(iv_length + mackey_length);
  file.seekg(0, std::ios_base::beg);

  // set iv
  std::unique_ptr<unsigned char[]> iv(new unsigned char[iv_length]);
  if (!file.read(reinterpret_cast<char *>(iv.get()), iv_length) ||
      file.gcount() != iv_length)
  {
    Logger::error("Failed to read iv");
    return 1;
  }

  // set data to decrypt
  std::unique_ptr<unsigned char[]> data(new unsigned char[data_length]);
  if (!file.read(reinterpret_cast<char *>(data.get()), data_length) ||
      file.gcount() != data_length)
  {
    Logger::error("Failed to read in file data");
    return 1;
  }

  // set theirMAC
  int64_t theirmac_length = 32;
  std::unique_ptr<unsigned char[]> theirmac(new unsigned char[32]);
  if (!file.read(reinterpret_cast<char *>(theirmac.get()), theirmac_length) ||
      file.gcount() != theirmac_length)
  {
    Logger::error("Failed to read theirmac");
    return 1;
  }

  // calculate MAC
  evp_md_st const *digest = EVP_sha256();
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  char digestname[] = "SHA256";
  std::unique_ptr<EVP_MAC, decltype(&::EVP_MAC_free)> mac(EVP_MAC_fetch(nullptr, "hmac", nullptr), &::EVP_MAC_free);
  std::unique_ptr<EVP_MAC_CTX, decltype(&::EVP_MAC_CTX_free)> hctx(EVP_MAC_CTX_new(mac.get()), &::EVP_MAC_CTX_free);
  OSSL_PARAM params[] = {OSSL_PARAM_construct_utf8_string("digest", digestname, 0), OSSL_PARAM_construct_end()};
  if (EVP_MAC_init(hctx.get(), mackey, mackey_length, params) != 1)
  {
    Logger::error("Failed to initialize HMAC context");
    return false;
  }
  std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[EVP_MD_size(digest)]);
  if (EVP_MAC_update(hctx.get(), iv.get(), iv_length) != 1 ||
      EVP_MAC_update(hctx.get(), data.get(), data_length) != 1 ||
      EVP_MAC_final(hctx.get(), calculatedmac.get(), nullptr, EVP_MD_size(digest)) != 1)
  {
    Logger::error("Failed to update/finalize hmac");
    return false;
  }
#else
  std::unique_ptr<HMAC_CTX, decltype(&::HMAC_CTX_free)> hctx(HMAC_CTX_new(), &::HMAC_CTX_free);
  if (HMAC_Init_ex(hctx.get(), mackey, mackey_length, digest, nullptr) != 1)
  {
    Logger::error("Failed to initialize HMAC context");
    return false;
  }
  std::unique_ptr<unsigned char[]> calculatedmac(new unsigned char[32]);
  unsigned int finalsize = EVP_MD_size(digest);
  if (HMAC_Update(hctx.get(), iv.get(), iv_length) != 1 ||
      HMAC_Update(hctx.get(), data.get(), data_length) != 1 ||
      HMAC_Final(hctx.get(), calculatedmac.get(), &finalsize) != 1)
  {
    Logger::error("Failed to update/finalize hmac");
    return false;
  }
#endif

  if (std::memcmp(calculatedmac.get(), theirmac.get(), theirmac_length) != 0)
  {
    Logger::error("MAC failed! (theirMAC: ", bepaald::bytesToString(theirmac.get(), theirmac_length),
                  " ourMAC: ", bepaald::bytesToString(calculatedmac.get(), theirmac_length));
    return -1;
  }

  // DECRYPT DATA:

  // init decryption context
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), &::EVP_CIPHER_CTX_free);
  if (!ctx) [[unlikely]]
  {
    Logger::error("Failed to create decryption context");
    return 1;
  }

  // init decrypt
  if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, aeskey, iv.get())) [[unlikely]]
  {
    Logger::error("Failed to initialize decryption operation");
    return 1;
  }

  // decrypt update
  int out_len = 0;
  int output_length = data_length;
  std::unique_ptr<unsigned char[]> output(new unsigned char[output_length]);
  if (EVP_DecryptUpdate(ctx.get(), output.get(), &out_len, data.get(), output_length) != 1) [[unlikely]]
  {
    Logger::error("Failed to update decryption");
    return 1;
  }

  // decrypt final
  int tail_len = 0;
  if (EVP_DecryptFinal_ex(ctx.get(), output.get() + out_len, &tail_len) != 1) [[unlikely]]
  {
    Logger::error("Failed to finalize decryption");
    return 1;
  }
  out_len += tail_len;
  //std::cout << out_len << std::endl;

  //std::cout << "Start of decrypted data: " << bepaald::bytesToHexString(output.get(), 64) << std::endl;
  *rawdata = output.release();
  return 0;
}

int DesktopAttachmentReader::getEncryptedAttachment(FrameWithAttachment *frame, bool verbose)
{
  unsigned char *data = nullptr;
  int ret = getAttachmentData(&data, verbose);
  if (ret == 0) [[likely]]
    frame->setAttachmentDataBacked(data, d_size);

  return ret;
}
