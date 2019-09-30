/*
    Copyright (C) 2019  Selwin van Dijk

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

void SignalBackup::cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges)
{
  std::string smsq;
  std::string mmsq;
  std::vector<std::any> params;
  for (uint i = 0; i < dateranges.size(); ++i)
  {
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      std::cout << "Error: Skipping range: '" << dateranges[i].first << " - " << dateranges[i].second << "'. Failed to parse or invalid range." << std::endl;
      continue;
    }
    std::cout << "Using range: " << dateranges[i].first << " - " << dateranges[i].second << std::endl;
    std::cout << "             " << startrange << " - " << endrange << std::endl;

    // if called with "YYYY-MM-DD HH:MM:SS"
    endrange += 999; // to get everything in the second specified...

    if (i == 0)
    {
      smsq = "DELETE FROM sms WHERE ";
      mmsq = "DELETE FROM mms WHERE ";
    }
    else
    {
      smsq += "AND ";
      mmsq += "AND ";
    }
    smsq += "date NOT BETWEEN ? AND ?";
    mmsq += "date_received NOT BETWEEN ? AND ?";
    if (i < dateranges.size() - 1)
    {
      smsq += " ";
      mmsq += " ";
    }
    params.emplace_back(startrange);
    params.emplace_back(endrange);
  }
  if (smsq.empty() || mmsq.empty())
  {
    std::cout << "Error: Failed to get any date ranges.";
    return;
  }

  d_database.exec(smsq, params);
  d_database.exec(mmsq, params);
  cleanDatabaseByMessages();
}
