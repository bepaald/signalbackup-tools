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

#include "signalbackup.ih"

bool SignalBackup::writeEncryptedFrameWithoutAttachment(std::ofstream &outputfile, std::pair<std::shared_ptr<unsigned char[]>, uint64_t> const &framedata)
{
  // write frame (the non-attachmentdata part)
  std::pair<unsigned char *, uint64_t> encryptedframe = d_fe.encryptFrame(framedata);

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
  std::pair<std::shared_ptr<unsigned char[]>, uint64_t> framedata(nullptr, 0);
  {
    std::pair<unsigned char *, uint64_t> framedataraw = frame->getData();
    framedata.first.reset(framedataraw.first);
    framedata.second = framedataraw.second;
  }

  if (!framedata.first) [[unlikely]]
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
      return false;
    }

    // we are done with framedata now... lets destroy it already...
    framedata.first.reset();

    // write attachment data
    std::pair<unsigned char *, uint64_t> newdata = d_fe.encryptAttachment(attachmentdata, attachmentsize);
    //std::cout << "Writing attachment data..." << std::endl;
    outputfile.write(reinterpret_cast<char *>(newdata.first), newdata.second);
    delete[] newdata.first;

    if (!outputfile.good()) [[unlikely]]
    {
      Logger::error("Failed to write encrypted attachmentdata to file");
      return false;
    }
  }
  else // not an attachmentframe, write it
    if (!writeEncryptedFrameWithoutAttachment(outputfile, framedata)) [[unlikely]]
    {
      Logger::error("Failed to write encrypt and write BackupFrame. Info:");
      frame->printInfo();
      return false;
    }

  return true;
}
