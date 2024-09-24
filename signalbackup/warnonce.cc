#include "signalbackup.ih"

#include "../common_be.h"

void SignalBackup::warnOnce(std::string const &warning, bool error)
{
  if (!bepaald::contains(d_warningsgiven, warning))
  {
    if (error)
      Logger::error(warning);
    else
      Logger::warning(warning);
    d_warningsgiven.insert(warning);
  }
}
