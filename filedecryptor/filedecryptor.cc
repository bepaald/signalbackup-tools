/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

#include "filedecryptor.ih"

#include "../common_filesystem.h"

FileDecryptor::FileDecryptor(std::string const &filename, std::string const &passphrase, bool verbose, bool stoponerror, bool assumebadframesize, std::vector<long long int> const &editattachments)
  :
  CryptBase(verbose),
  d_filename(filename),
  d_editattachments(editattachments),
  d_headerframe(nullptr),
  d_framecount(0),
  d_filesize(0),
  d_backupfileversion(0),
  d_badmac(false),
  d_assumebadframesize(assumebadframesize),
  d_stoponerror(stoponerror)
{
  std::ifstream file(d_filename, std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    Logger::error("Failed to open file '", d_filename, "'");
    return;
  }

  //file.seekg(0, std::ios_base::end);
  //d_filesize = file.tellg();
  //file.seekg(0);
  d_filesize = bepaald::fileSize(d_filename);

  // read first four bytes, they are the header size of the file:
  int32_t headerlength = getNextFrameBlockSize(file);
  DEBUGOUT("headerlength: ", headerlength);
  if (headerlength < 2 || headerlength > 10240 /* 10MB, this is probably still needlessly large */) [[unlikely]]
  {
    Logger::error("Invalid headerframe length (", headerlength, ")");
    return;
  }

  unsigned char *headerdata = new unsigned char[headerlength];
  getNextFrameBlock(file, headerdata, headerlength);

  BackupFrame *headerframe = initBackupFrame(headerdata, headerlength, d_framecount++);
  delete[] headerdata;
  if (!headerframe)
  {
    //std::cout << "Error: failed to retrieve HeaderFrame, length was " << headerlength << " bytes" << std::endl;
    Logger::error("failed to retrieve HeaderFrame, length was ", headerlength, " bytes");
    return;
  }

  if (headerframe->frameType() != BackupFrame::FRAMETYPE::HEADER)
  {
    //std::cout << "Error: First frame is not a HeaderFrame" << std::endl;
    Logger::error("First frame is not a HeaderFrame");
    delete headerframe;
    return;
  }

  d_iv_size = reinterpret_cast<HeaderFrame *>(headerframe)->iv_length();
  d_iv = new unsigned char[d_iv_size];
  std::memcpy(d_iv, reinterpret_cast<HeaderFrame *>(headerframe)->iv(), d_iv_size);

  d_counter = fourBytesToUint(d_iv);

  d_salt_size = reinterpret_cast<HeaderFrame *>(headerframe)->salt_length();
  d_salt = new unsigned char[d_salt_size];
  std::memcpy(d_salt, reinterpret_cast<HeaderFrame *>(headerframe)->salt(), d_salt_size);

  if (!getBackupKey(passphrase))
  {
    //std::cout << "Error: Failed to get backupkey from passphrase" << std::endl;
    Logger::error("Failed to get backupkey from passphrase");
    delete headerframe;
    return;
  }

  if (!getCipherAndMac(32, 64))
  {
    //std::cout << "Error: Failed to get Cipher and Mac" << std::endl;
    Logger::error("Failed to get Cipher and Mac");
    delete headerframe;
    return;
  }

  d_backupfileversion = reinterpret_cast<HeaderFrame *>(headerframe)->version();

  //headerframe->printInfo();

  /*
  DEBUGOUT("IV: ", bepaald::bytesToHexString(d_iv, d_iv_size));
  DEBUGOUT("SALT: ", bepaald::bytesToHexString(d_salt, d_salt_size));
  DEBUGOUT("BACKUPKEY: ", bepaald::bytesToHexString(d_backupkey, d_backupkey_size));
  DEBUGOUT("CIPHERKEY: ", bepaald::bytesToHexString(d_cipherkey, d_cipherkey_size));
  DEBUGOUT("MACKEY: ", bepaald::bytesToHexString(d_mackey, d_mackey_size));
  DEBUGOUT("BACKUPFILE VERSION: ", d_backupfileversion);
  DEBUGOUT("BACKUPFILE SIZE: ", d_filesize);
  DEBUGOUT("COUNTER: ", d_counter);
  */

  if (d_verbose) [[unlikely]]
  {
    // std::cout << "IV: " << bepaald::bytesToHexString(d_iv, d_iv_size) << " (size: " << d_iv_size << ")" << std::endl;
    // std::cout << "SALT: " << bepaald::bytesToHexString(d_salt, d_salt_size) << " (size: " << d_salt_size << ")" << std::endl;
    // std::cout << "BACKUPKEY: " << bepaald::bytesToHexString(d_backupkey, d_backupkey_size) << " (size: " << d_backupkey_size << ")" << std::endl;
    // std::cout << "CIPHERKEY: " << bepaald::bytesToHexString(d_cipherkey, d_cipherkey_size) << " (size: " << d_cipherkey_size << ")" << std::endl;
    // std::cout << "MACKEY: " << bepaald::bytesToHexString(d_mackey, d_mackey_size) << " (size: " << d_mackey_size << ")" << std::endl;

    Logger::message("IV: ", bepaald::bytesToHexString(d_iv, d_iv_size), " (size: ", d_iv_size, ")");
    Logger::message("SALT: ", bepaald::bytesToHexString(d_salt, d_salt_size), " (size: ", d_salt_size, ")");
    Logger::message("BACKUPKEY: ", bepaald::bytesToHexString(d_backupkey, d_backupkey_size), " (size: ", d_backupkey_size, ")");
    Logger::message("CIPHERKEY: ", bepaald::bytesToHexString(d_cipherkey, d_cipherkey_size), " (size: ", d_cipherkey_size, ")" );
    Logger::message("MACKEY: ", bepaald::bytesToHexString(d_mackey, d_mackey_size), " (size: ", d_mackey_size, ")");
  }
  // std::cout << "BACKUPFILE VERSION: " << d_backupfileversion << std::endl;
  // std::cout << "BACKUPFILE SIZE: " << d_filesize << std::endl;
  // std::cout << "COUNTER: " << d_counter << std::endl;
  Logger::message("BACKUPFILE VERSION: ", d_backupfileversion);
  Logger::message("BACKUPFILE SIZE: ", d_filesize);
  Logger::message("COUNTER: ", d_counter);

  d_headerframe.reset(headerframe);

  d_ok = true;
}
