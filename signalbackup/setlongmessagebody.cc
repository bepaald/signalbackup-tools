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

#include "signalbackup.ih"

#include "../common_be.h"

void SignalBackup::setLongMessageBody(std::string *body, SqliteDB::QueryResults *attachment_results) const
{
  for (unsigned int ai = 0; ai < attachment_results->rows(); ++ai)
  {
    if (attachment_results->valueAsString(ai, d_part_ct) == "text/x-signal-plain") [[unlikely]]
    {
      //std::cout << "Got long message!" << std::endl;
      SqliteDB::QueryResults longmessage = attachment_results->getRow(ai);
      attachment_results->removeRow(ai);

      // get message:
      long long int rowid = longmessage.valueAsInt(0, "_id");
      long long int uniqueid = longmessage.valueAsInt(0, "unique_id");

      // get attachment:
      auto ait = d_attachments.find({rowid, uniqueid});
      if (ait == d_attachments.end()) [[unlikely]]
        continue;
      AttachmentFrame *a = ait->second.get();

      // set body
      *body = std::string(reinterpret_cast<char *>(a->attachmentData(d_verbose)), a->attachmentSize());
      a->clearData();
      break; // always max 1?
    }
  }
}
