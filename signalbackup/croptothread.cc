/*
  Copyright (C) 2019-2023  Selwin van Dijk

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
  std::cout << __FUNCTION__ << std::endl;

  std::string smsq;
  std::string mmsq;
  std::vector<std::any> tids;
  for (uint i = 0; i < threadids.size(); ++i)
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
    std::cout << "Error: building crop-to-thread statement resulted in invalid statement" << std::endl;
    return;
  }

  if (d_database.containsTable("sms"))
  {
    std::cout << "  Deleting messages not belonging to requested thread(s) from 'sms'" << std::endl;
    d_database.exec(smsq, tids);
  }
  std::cout << "  Deleting messages not belonging to requested thread(s) from 'mms'" << std::endl;
  d_database.exec(mmsq, tids);

  cleanDatabaseByMessages();
}
