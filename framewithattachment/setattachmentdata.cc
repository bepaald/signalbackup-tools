/*
  Copyright (C) 2022-2024  Selwin van Dijk

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

bool FrameWithAttachment::setAttachmentDataBacked(unsigned char *data, long long int datalength) // override
{
  if (!data)
    return false;
  d_attachmentdata = data;
  d_attachmentdata_size = datalength;
  return true;
}

bool FrameWithAttachment::setAttachmentDataUnbacked(unsigned char const *data, long long int datalength)
{
  bepaald::destroyPtr(&d_attachmentdata, &d_attachmentdata_size);

  d_attachmentdata_size = datalength;
  d_attachmentdata = new unsigned char[d_attachmentdata_size];
  std::memcpy(d_attachmentdata, data, datalength);

  /* used for importing LONGTEXT messages from desktop.
     While on desktop they are normal message bodies, on Android
     they are attachments. Since we are creating these attachments
     on import from bytes in memory (not file backed), these can
     not be clearData()'s at any point, since the data can not
     be read back in that case.
   */
  d_noclear = true;
  return true;
}
