/*
  Copyright (C) 2019-2023  Selwin van Dijk

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

#include "signalbackup.ih"

bool SignalBackup::writeEncryptedFrameWithoutAttachment(std::ofstream &outputfile, std::pair<unsigned char *, uint64_t> framedata)
{
  // write frame (the non-attachmentdata part)
  std::pair<unsigned char *, uint64_t> encryptedframe = d_fe.encryptFrame(framedata);
  delete[] framedata.first;

  if (!encryptedframe.first)
  {
    Logger::error("Failed to encrypt framedata");
    return false;
  }
  bool writeok = !(outputfile.write(reinterpret_cast<char *>(encryptedframe.first), encryptedframe.second)).fail();

  delete[] encryptedframe.first;
  if (!writeok)
    Logger::error("Failed to write encrypted frame data to file");

  return writeok;
}

bool SignalBackup::writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame)
{
  std::pair<unsigned char *, uint64_t> framedata = frame->getData();
  if (!framedata.first)
  {
    Logger::error("Failed to get framedata from frame");
    return false;
  }

  uint32_t attachmentsize = 0;
  if ((attachmentsize = frame->attachmentSize()) > 0)
  {
    FrameWithAttachment *f = reinterpret_cast<FrameWithAttachment *>(frame);

    bool badmac = false;

    unsigned char *attachmentdata = f->attachmentData(&badmac);

    if (!attachmentdata)
    {
      delete[] framedata.first;
      if (badmac)
      {
        Logger::warning("Corrupted data encountered. Skipping frame.");
        return true;
      }
      return false;
    }

    if (!writeEncryptedFrameWithoutAttachment(outputfile, framedata)) [[unlikely]]
    {
      Logger::error("Failed to write encrypt and write BackupFrame. Info:");
      frame->printInfo();
      delete[] framedata.first;
      return false;
    }

    // write attachment data
    std::pair<unsigned char *, uint64_t> newdata = d_fe.encryptAttachment(attachmentdata, attachmentsize);
    //std::cout << "Writing attachment data..." << std::endl;
    outputfile.write(reinterpret_cast<char *>(newdata.first), newdata.second);
    delete[] newdata.first;

    if (!outputfile.good())
    {
      Logger::error("Failed to write encrypted attachmentdata to file");
      delete[] framedata.first;
      return false;
    }
  }
  else // not an attachmentframe, write it
    if (!writeEncryptedFrameWithoutAttachment(outputfile, framedata)) [[unlikely]]
    {
      Logger::error("Failed to write encrypt and write BackupFrame. Info:");
      frame->printInfo();
      delete[] framedata.first;
      return false;
    }

  return true;
}
