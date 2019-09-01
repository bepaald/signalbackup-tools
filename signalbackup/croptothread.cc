#include "signalbackup.ih"

void SignalBackup::cropToThread(long long int threadid)
{
  cropToThread(std::vector<long long int>{threadid});
}

void SignalBackup::cropToThread(std::vector<long long int> const &threadids)
{
  std::string smsq;
  std::string mmsq;
  std::vector<std::any> tids;
  for (uint i = 0; i < threadids.size(); ++i)
  {
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

  std::cout << "Deleting messages not belonging to requested thread(s) from 'sms'" << std::endl;
  d_database.exec(smsq, tids);
  std::cout << "Deleting messages not belonging to requested thread(s) from 'mms'" << std::endl;
  d_database.exec(mmsq, tids);

  cleanDatabaseByMessages();
}
