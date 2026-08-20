/*
  Copyright (C) 2026  Selwin van Dijk

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

#ifdef BEPAALD_BV2_ENABLED

#include "backupv2reader.h"

#include <cstring>
#include <memory>

#include "../common_crypto.h"
#include "../common_filesystem.h"

bool BackupV2Reader::readMain()
{
  // now open 'main' and decrypt it:
  auto [main_file_data, main_file_size] = bepaald::readFileFully(d_snapshotdir + "/main");
  if (!main_file_data || main_file_size == 0) [[unlikely]]
    return false;

  unsigned char *main_iv = main_file_data.get();
  unsigned char *main_data_encrypted = main_file_data.get() + 16;
  unsigned char *main_mac = main_file_data.get() + (main_file_size - 32);
  if (d_verbose) [[unlikely]]
  {
    Logger::message("  IV: ", bepaald::bytesToHexString(main_iv, 16));
    Logger::message("  MAC: ", bepaald::bytesToHexString(main_mac, 32));
  }

  // check MAC
  if (!bepaald::checkHmac_sha256(d_hmackey, d_hmackey_size,
                                 main_iv, main_file_size - 32,
                                 main_mac, SHA256_DIGEST_LENGTH)) [[unlikely]]
  {
    Logger::error("Message authentication checksum failed for '", d_snapshotdir, "/main'. Your file may be corrupted");
    return false;
  }

  // decrypt
  int main_data_encrypted_length = main_file_size - 16 - 32;
  size_t main_data_gzipped_size_padded = 0;
  std::tie(d_main_data_gzipped, main_data_gzipped_size_padded) =
    bepaald::decrypt_aes_256_cbc(d_aeskey, main_iv, main_data_encrypted, main_data_encrypted_length);
  if (!d_main_data_gzipped || main_data_gzipped_size_padded == 0) [[unlikely]]
  {
    Logger::error("Failed to decrypt '", d_snapshotdir, "/main'.");
    return false;
  }

  if (d_verbose) [[unlikely]]
  {
    Logger::message("Encrypted main (truncated): ", bepaald::bytesToHexString(main_data_encrypted, 35), "...");
    Logger::message("Decrypted main (truncated): ", bepaald::bytesToHexString(d_main_data_gzipped, 35), "... (", main_data_gzipped_size_padded, " bytes total (padded))");
  }





  // gunzip this decrypted main-data, and proces frames as we do....

  // the last 4 bytes of the _actual_ gzip data stream are the uncompressed size
  // the gzipped data is 0-padded, of course the uncompressed size can contain
  // 0s but we will be off by at most 3 bytes, and get a more accurate progress
  // indicator.
  d_main_data_gzipped_size = main_data_gzipped_size_padded;
  for (unsigned int i = main_data_gzipped_size_padded; i--; )
    if (d_main_data_gzipped[i] > 0)
    {
      d_main_data_gzipped_size = i + 4;
      break;
    }

  // set input
  d_gunzip_stream.next_in = d_main_data_gzipped.get();
  d_gunzip_stream.avail_in = d_main_data_gzipped_size;
  d_gunzip_stream.zalloc = Z_NULL; // use default
  d_gunzip_stream.zfree = Z_NULL;
  d_gunzip_stream.opaque = Z_NULL;

  d_gzip_output.reset(new unsigned char[d_gunzip_buffer]);

  d_gunzip_stream.next_out = d_gzip_output.get();
  d_gunzip_stream.avail_out = d_gunzip_buffer;

  // initialize
  if (inflateInit2(&d_gunzip_stream, MAX_WBITS + 16) != Z_OK) [[unlikely]] // add 16 [to windowBits] to decode only the gzip format
  {
    Logger::error("Error initializing gunzip stream.");
    return false;
  }
  d_gzipstream_initialized = true;

  return true;
}

#endif
