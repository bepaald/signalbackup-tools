/*
    Copyright (C) 2019-2020  Selwin van Dijk

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


bool SignalBackup::writeEncryptedFrame(std::ofstream &outputfile, BackupFrame *frame)
{
  // if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
  // {
  //   if (reinterpret_cast<AttachmentFrame *>(frame)->rowId() == 8 &&
  //       reinterpret_cast<AttachmentFrame *>(frame)->attachmentId() == 1529478705314)
  //     reinterpret_cast<AttachmentFrame *>(frame)->setLengthField(80000);
  // }

  std::pair<unsigned char *, uint64_t> framedata = frame->getData();
  if (!framedata.first)
  {
    std::cout << "Failed to get framedata from frame" << std::endl;
    return false;
  }

  std::pair<unsigned char *, uint64_t> encryptedframe = d_fe.encryptFrame(framedata);
  delete[] framedata.first;

  if (!encryptedframe.first)
  {
    std::cout << "Failed to encrypt framedata" << std::endl;
    return false;
  }

  bool writeok = writeFrameDataToFile(outputfile, encryptedframe);
  delete[] encryptedframe.first;

  if (!writeok)
  {
    std::cout << "Failed to write encrypted frame data to file" << std::endl;
    return false;
  }

  // if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
  // {
  //   if (reinterpret_cast<AttachmentFrame *>(frame)->rowId() == 8 &&
  //       reinterpret_cast<AttachmentFrame *>(frame)->attachmentId() == 1529478705314)
  //     reinterpret_cast<AttachmentFrame *>(frame)->setLengthField(78947);
  // }

  uint32_t attachmentsize = 0;
  if ((attachmentsize = frame->attachmentSize()) > 0)
  {
    FrameWithAttachment *f = reinterpret_cast<FrameWithAttachment *>(frame);
    unsigned char *attachmentdata = f->attachmentData();\
    if (!attachmentdata)
      return false;
    std::pair<unsigned char *, uint64_t> newadata = d_fe.encryptAttachment(attachmentdata, attachmentsize);
    //std::cout << "Writing attachment data..." << std::endl;
    outputfile.write(reinterpret_cast<char *>(newadata.first), newadata.second);
    delete[] newadata.first;

    if (!outputfile.good())
    {
      std::cout << "Failed to write encrypted attachmentdata to file" << std::endl;
      return false;
    }
  }

  return true;
}
