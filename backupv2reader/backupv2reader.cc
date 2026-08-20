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

#include "../common_be.h"
#include "../common_filesystem.h"

/*
  V2 backup:

  d_rootdir
  |- files/
  |  |- XX/ [probably many directories with 2 char hex names]
  |- d_snapshotdir (signal-backup-YYYY-MM-DD-hh:mm:ss/)
  |  |- files
  |  |- main
  |  |- metadata


  - from the passphrase (64 bytes alpha num) we hkdf derive key1
  - we hkdf derive key2 from key1.
  - we read d_snapshotdir/metadata, which contains an IV and
    an encrypted backupId.
  - we decrypt the backupId from the metadata with key2, and the IV
    (padded to 16 bytes)
  - from the backupid and key1, we hkdf derive the AESkey and MACkey
  - then we can read and decrypt d_snapshotdir/main with the
    AESkey and MACkey. The decrypted data is gzipped, so we gunzip it
    and process the protobuf Frames inside.
*/

BackupV2Reader::BackupV2Reader(std::string const &inputdir, std::string_view passphrase, bool verbose)
  :
  d_main_data_gzipped_size(0),
  d_gunzip_buffer(487),//(10 * 1024 * 1024), // 10MB // interesting values: 44, 487
  d_gzip_res(0),
  d_gzip_previous_blocks_in(0),
  d_gzip_current_block_in(0),
  d_gzip_pos(0),
  d_hmackey(nullptr),
  d_aeskey(nullptr),
  d_gzipstream_initialized(false),
  d_have_backupinfo(false),
  d_gzip_reset(true),
  d_verbose(verbose),
  d_ok(false)
{
  if (d_verbose) [[unlikely]]
    Logger::message("Creating Backupv2Reader with input='", inputdir, "' and passphrase = '", passphrase, "'");

  if (!bepaald::isDir(inputdir)) [[unlikely]]
  {
    Logger::error("Input is not a directory");
    return;
  }

  auto isSnapshot = [](std::string const &dir) STATICLAMBDA { return
      bepaald::isRegularFile(dir + "/main") && bepaald::isRegularFile(dir + "/metadata"); };

  // if dir is specific snapshot
  if (isSnapshot(inputdir))
  {
    d_snapshotdir = inputdir;
    while (d_snapshotdir.back() == '/' ||
           d_snapshotdir.back() == std::filesystem::path::preferred_separator)
      d_snapshotdir.pop_back();

    d_rootdir = std::filesystem::path(d_snapshotdir).parent_path();
  }
  else // weve been passed rootdir, find newest snapshot ourselves...
  {
    d_rootdir = inputdir;
    for (auto const &dir_entry : std::filesystem::directory_iterator{d_rootdir})
      if (isSnapshot(dir_entry.path()) &&
          dir_entry.path().string() > d_snapshotdir)
        d_snapshotdir = dir_entry.path();
  }
  d_filesdir = d_rootdir + "/files";

  d_passphrase = readPassphrase(passphrase);

  d_ok = (d_passphrase.size() == 64 &&
          bepaald::isDir(d_snapshotdir) &&
          bepaald::isDir(d_filesdir) &&
          bepaald::isDir(d_rootdir)) &&
          getKeys() &&
          readMain();
}

#endif
