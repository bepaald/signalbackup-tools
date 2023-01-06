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

void SignalBackup::cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges)
{
  std::cout << __FUNCTION__ << std::endl;

  std::string smsq;
  std::string mmsq;
  std::string megaphoneq;
  std::vector<std::any> params;
  std::vector<std::any> params2;
  for (uint i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      std::cout << "Error: Skipping range: '" << dateranges[i].first << " - " << dateranges[i].second << "'. Failed to parse or invalid range." << std::endl;
      continue;
    }
    std::cout << "  Using range: " << dateranges[i].first << " - " << dateranges[i].second << std::endl;
    std::cout << "               " << startrange << " - " << endrange << std::endl;

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    if (i == 0)
    {
      smsq = "DELETE FROM sms WHERE ";
      mmsq = "DELETE FROM mms WHERE ";
      megaphoneq = "DELETE FROM megaphone WHERE ";
    }
    else
    {
      smsq += "AND ";
      mmsq += "AND ";
      megaphoneq += "AND ";
    }
    smsq += d_sms_date_received + " NOT BETWEEN ? AND ?";
    mmsq += "date_received NOT BETWEEN ? AND ?";
    megaphoneq += "first_visible >= ?";
    if (i < dateranges.size() - 1)
    {
      smsq += " ";
      mmsq += " ";
      megaphoneq += " ";
    }
    params.emplace_back(startrange);
    params.emplace_back(endrange);
    params2.emplace_back(endrange / 10); // the timestamp in megaphone is not in msecs (one digit less)
  }
  if (smsq.empty() || mmsq.empty())
  {
    std::cout << "Error: Failed to get any date ranges.";
    return;
  }

  d_database.exec(smsq, params);
  d_database.exec(mmsq, params);
  if (d_database.containsTable("megaphone"))
  {
    d_database.exec(megaphoneq, params2);
    //std::cout << "changed: " << d_database.changed() << std::endl;
  }

  cleanDatabaseByMessages();
}
