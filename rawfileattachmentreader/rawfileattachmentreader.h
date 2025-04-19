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

#ifndef RAWATTACHMENTREADER_H_
#define RAWATTACHMENTREADER_H_

#include "../baseattachmentreader/baseattachmentreader.h"
#include "../framewithattachment/framewithattachment.h"

#include "../common_filesystem.h"

class RawFileAttachmentReader : public AttachmentReader<RawFileAttachmentReader>
{
  std::string d_filename;
 public:
  inline explicit RawFileAttachmentReader(std::string const &filename);
  RawFileAttachmentReader(RawFileAttachmentReader const &other) = default;
  RawFileAttachmentReader(RawFileAttachmentReader &&other) = default;
  RawFileAttachmentReader &operator=(RawFileAttachmentReader const &other) = default;
  RawFileAttachmentReader &operator=(RawFileAttachmentReader &&other) = default;
  virtual ~RawFileAttachmentReader() override = default;

  inline virtual ReturnCode getAttachment(FrameWithAttachment *frame, bool verbose) override;
};

inline RawFileAttachmentReader::RawFileAttachmentReader(std::string const &filename)
  :
  d_filename(filename)
{}

inline BaseAttachmentReader::ReturnCode RawFileAttachmentReader::getAttachment(FrameWithAttachment *frame, bool verbose) // virtual
{
  //std::cout << " *** REALLY GETTING ATTACHMENT (RAW) ***" << std::endl;

  std::ifstream file(std::filesystem::path(d_filename), std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    Logger::error("Failed to open file '", d_filename, "' for reading attachment");
    return ReturnCode::ERROR;
  }
  //file.seekg(0, std::ios_base::end);
  //int64_t attachmentdata_size = file.tellg();
  //file.seekg(0, std::ios_base::beg);
  uint64_t attachmentdata_size = bepaald::fileSize(d_filename);

  if (attachmentdata_size == 0) [[unlikely]]
    Logger::warning("Asked to read 0-byte attachment");

  if (verbose) [[unlikely]]
    Logger::message("Reading attachment data, length: ", attachmentdata_size);

  //std::cout << "Getting attachment: " << d_filename << std::endl;

  std::unique_ptr<unsigned char[]> decryptedattachmentdata(new unsigned char[attachmentdata_size]); // to hold the data
  if (!file.read(reinterpret_cast<char *>(decryptedattachmentdata.get()), attachmentdata_size))
  {
    Logger::error("Failed to read raw attachment \"", d_filename, "\"");
    return ReturnCode::ERROR;
  }
  frame->setAttachmentDataBacked(decryptedattachmentdata.release(), attachmentdata_size);
  return ReturnCode::OK;
}

#endif
