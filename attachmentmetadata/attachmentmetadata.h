/*
  Copyright (C) 2024  Selwin van Dijk

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

#ifndef ATTACHMENTMETADATA_H_
#define ATTACHMENTMETADATA_H_

#include <string>
#include <cstdint>

struct AttachmentMetadata
{
  int width;
  int height;
  std::string filetype;
  uint64_t filesize;
  std::string hash;
  std::string filename;
  operator bool() const
  {
    return (width != -1 && height != -1 && !filetype.empty() && filesize != 0);
  }

  static AttachmentMetadata getAttachmentMetaData(std::string const &filename, bool skiphash = false);
  static AttachmentMetadata getAttachmentMetaData(std::string const &filename, unsigned char *data,
                                                  uint64_t data_size, bool skiphash = false);
};

#endif
