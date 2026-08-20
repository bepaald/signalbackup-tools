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

#ifndef BACKUPV2READER_H_
#define BACKUPV2READER_H_

#include <memory>
#include <string>

#include "../logger/logger.h"

#include <zlib.h>

class BackupV2Reader
{
  static constexpr size_t d_hmackey_size{32};
  static constexpr size_t d_aeskey_size{32};

  std::pair<std::unique_ptr<unsigned char []>, size_t> d_hmac_aes_keys;
  std::unique_ptr<unsigned char []> d_main_data_gzipped;
  std::unique_ptr<unsigned char []> d_gzip_output;
  size_t d_main_data_gzipped_size;
  size_t d_gunzip_buffer;
  int d_gzip_res;
  float d_gzip_previous_blocks_in;
  float d_gzip_current_block_in;
  unsigned int d_gzip_pos;
  z_stream d_gunzip_stream;
  unsigned char *d_hmackey;
  unsigned char *d_aeskey;
  std::string d_passphrase;
  std::string d_rootdir;
  std::string d_snapshotdir;
  std::string d_filesdir;
  bool d_gzipstream_initialized;
  bool d_have_backupinfo;
  bool d_gzip_reset;
  bool d_verbose;
  bool d_ok;
 public:
  BackupV2Reader(std::string const &inputdir, std::string_view passphrase, bool verbose);
  BackupV2Reader(BackupV2Reader const &other) = delete;
  BackupV2Reader(BackupV2Reader &&other) = default;
  inline ~BackupV2Reader();
  inline bool ok() const;
  std::pair<unsigned char *, size_t> getFrame();
  inline float progress() const;

 private:
  std::string readPassphrase(std::string_view passphrase) const;
  bool getKeys();
  bool readMain();

  //bool handleFrame(unsigned char const *const data, size_t size, SignalBackup *sb) const;
  //bool handleRecipientFrame(BackupV2::Frame const &f, SignalBackup *sb) const;
};

inline BackupV2Reader::~BackupV2Reader()
{
  if (d_gzipstream_initialized)
    if (inflateEnd(&d_gunzip_stream) != Z_OK)
      Logger::error("Failed to end gunzip stream");
}

inline bool BackupV2Reader::ok() const
{
  return d_ok;
}

inline float BackupV2Reader::progress() const
{
  // std::cout << d_gzip_previous_blocks_in << std::endl;
  // std::cout << d_main_data_gzipped_size << std::endl;
  // std::cout << d_gzip_pos << std::endl;
  // std::cout << d_gunzip_buffer << std::endl;
  // std::cout << d_gunzip_stream.avail_out << std::endl;
  // std::cout << d_gzip_current_block_in << std::endl;

  return ((d_gzip_previous_blocks_in / d_main_data_gzipped_size) +
          ((static_cast<float>(d_gzip_pos) /
            (d_gunzip_buffer - (d_gunzip_buffer == d_gunzip_stream.avail_out ? 0 : d_gunzip_stream.avail_out))) *
           (d_gzip_current_block_in / d_main_data_gzipped_size))) * 100;
}

#endif

#endif
