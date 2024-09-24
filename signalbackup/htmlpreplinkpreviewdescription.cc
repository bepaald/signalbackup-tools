#include "signalbackup.ih"

#include "../common_be.h"

std::string SignalBackup::HTMLprepLinkPreviewDescription(std::string const &in) const
{
  // link preview can contain html, this is problematic for the export +
  // in the app the tags are stripped, and underscores are replaced with spaces
  // for some reason

  std::string cleaned = in;

  while (cleaned.find("<") != std::string::npos)
  {
    auto startpos = cleaned.find("<");
    auto endpos = cleaned.find(">") + 1;

    if (endpos != std::string::npos)
      cleaned.erase(startpos, endpos - startpos);
  }

  bepaald::replaceAll(&cleaned, "_", " ");
  return cleaned;
}
