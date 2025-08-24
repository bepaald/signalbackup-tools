/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

bool SignalBackup::cropToDates(std::vector<std::pair<std::string, std::string>> const &dateranges)
{
  Logger::message(__FUNCTION__);

  std::string smsq;
  std::string mmsq;
  std::string megaphoneq;
  std::vector<std::any> params;
  std::vector<std::any> params2;
  for (unsigned int i = 0; i < dateranges.size(); ++i)
  {
    bool needrounding = false;
    long long int startrange = dateToMSecsSinceEpoch(dateranges[i].first);
    long long int endrange   = dateToMSecsSinceEpoch(dateranges[i].second, &needrounding);
    if (startrange == -1 || endrange == -1 || endrange < startrange)
    {
      Logger::error("Invalid range: '", dateranges[i].first, "' - '", dateranges[i].second, "' (", startrange, " - ", endrange, ")");
      return false;
    }
    Logger::message("  Using range: ", dateranges[i].first, " - ", dateranges[i].second, "\n",
                    "               ", startrange, " - ", endrange);

    if (needrounding)// if called with "YYYY-MM-DD HH:MM:SS"
      endrange += 999; // to get everything in the second specified...

    if (i == 0)
    {
      smsq = "DELETE FROM sms WHERE ";
      mmsq = "DELETE FROM " + d_mms_table + " WHERE ";
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
    Logger::error("Failed to get any date ranges.");
    return false;
  }

  if (d_database.containsTable("sms"))
    if (!d_database.exec(smsq, params)) [[unlikely]]
      return false;
  if (!d_database.exec(mmsq, params)) [[unlikely]]
    return false;
  if (d_database.containsTable("megaphone"))
  {
    if (!d_database.exec(megaphoneq, params2)) [[unlikely]]
      return false;
    //std::cout << "changed: " << d_database.changed() << std::endl;
  }

  cleanDatabaseByMessages();
  return true;
}
