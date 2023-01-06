/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

#include "framewithattachment.h"

bool FrameWithAttachment::setAttachmentData(unsigned char *data) // override
{
  if (!data)
    return false;
  d_attachmentdata = data;
  d_attachmentdata_size = length();
  return true;
}

bool FrameWithAttachment::setAttachmentData(std::string const &filename) // override
{
  std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);
  if (!file.is_open())
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Setting attachment data. Failed to open '" << filename << "' for reading" << std::endl;
    return false;
  }
  file.seekg(0, std::ios_base::end);
  d_attachmentdata_size = file.tellg();
  if (d_attachmentdata_size == 0)
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Setting attachment data for file '" << filename << "'. Filesize 0." << std::endl;
    return false;
  }
  file.seekg(0);
  d_attachmentdata = new unsigned char[d_attachmentdata_size];
  if (!file.read(reinterpret_cast<char *>(d_attachmentdata), d_attachmentdata_size))
  {
    std::cout << bepaald::bold_on << "Error" << bepaald::bold_off
              << ": Failed to read data from '" << filename << "'" << std::endl;
    bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);
    return false;
  }
  return true;
}
