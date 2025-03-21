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

#include "signalbackup.ih"

#include "../attachmentmetadata/attachmentmetadata.h"

bool SignalBackup::updatePartTableForReplace(AttachmentMetadata const &data, long long int id)
{
  if (!updateRows(d_part_table,
                  {{d_part_ct, data.filetype},
                   {"data_size", data.filesize},
                   {"width", data.width},
                   {"height", data.height},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash") ? "data_hash" : ""), data.hash},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash_start") ? "data_hash_start" : ""), data.hash},
                   {(d_database.tableContainsColumn(d_part_table, "data_hash_end") ? "data_hash_end" : ""), data.hash}},
                  {{"_id", id}}) ||
      d_database.changed() != 1)
    return false;
  return true;
}
