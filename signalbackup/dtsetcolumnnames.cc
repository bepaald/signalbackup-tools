/*
  Copyright (C) 2023-2024  Selwin van Dijk

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

void SignalBackup::dtSetColumnNames(SqliteDB *ddb)
{

  // conversations.uuid -> conversations.serviceid
  if (ddb->tableContainsColumn("conversations", "serviceId"))
    d_dt_c_uuid = "serviceId";
  else if (ddb->tableContainsColumn("conversations", "uuid"))
    d_dt_c_uuid = "uuid";


  // messages.sourceuuid -> messages.sourceserviceid
  if (ddb->tableContainsColumn("messages", "sourceServiceId"))
    d_dt_m_sourceuuid = "sourceServiceId";
  else if (ddb->tableContainsColumn("messages", "sourceUuid"))
    d_dt_m_sourceuuid = "sourceUuid";

}
