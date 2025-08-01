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
#include "../adbbackupdatabase/adbbackupdatabase.h"

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

  auto data = AdbBackupDatabase::decrypt(encryptedfiledata.get(), encryptedfile_size,
                                         d_mackey.get(), d_mackey_length,
                                         d_encryptionkey.get(), d_encryptionkey_length);
  if (!data.has_value()) [[unlikely]]
    return ReturnCode::ERROR;

  // std::cout << "Filesize: " << encryptedfile_size << std::endl;
  // std::cout << "decrytpsize: " << data.value().second << std::endl;
  // std::cout << bepaald::bytesToHexString(data.value().first.get(), data.value().second) << std::endl;

  frame->setAttachmentDataBacked(data.value().first.release(), data.value().second); // the frame will now own the data...

  return ReturnCode::OK;
}
