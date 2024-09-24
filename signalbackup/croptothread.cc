/*
  Copyright (C) 2019-2024  Selwin van Dijk

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

void SignalBackup::cropToThread(long long int threadid)
{
  cropToThread(std::vector<long long int>{threadid});
}

void SignalBackup::cropToThread(std::vector<long long int> const &threadids)
{
  Logger::message(__FUNCTION__);

  std::string smsq;
  std::string mmsq;
  std::vector<std::any> tids;
  for (unsigned int i = 0; i < threadids.size(); ++i)
  {
    if (i == 0)
    {
      smsq = "DELETE FROM sms WHERE ";
      mmsq = "DELETE FROM " + d_mms_table + " WHERE ";
    }
    else
    {
      smsq += "AND ";
      mmsq += "AND ";
    }
    smsq += "thread_id != ?";
    mmsq += "thread_id != ?";
    if (i < threadids.size() - 1)
    {
      smsq += " ";
      mmsq += " ";
    }
    tids.emplace_back(threadids[i]);
  }

  if (smsq.empty() || mmsq.empty() || tids.empty())
  {
    Logger::error("building crop-to-thread statement resulted in invalid statement");
    return;
  }

  if (d_database.containsTable("sms"))
  {
    Logger::message("  Deleting messages not belonging to requested thread(s) from 'sms'");
    d_database.exec(smsq, tids);
  }
  Logger::message("  Deleting messages not belonging to requested thread(s) from 'mms'");
  d_database.exec(mmsq, tids);

  cleanDatabaseByMessages();
}
