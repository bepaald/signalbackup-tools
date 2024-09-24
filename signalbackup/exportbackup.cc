#include "signalbackup.ih"

#include "../common_filesystem.h"

bool SignalBackup::exportBackup(std::string const &filename, std::string const &passphrase, bool overwrite,
                                bool keepattachmentdatainmemory, bool onlydb)
{
  // if output is existing directory, or doesn't exist but ends in directory delim. -> output to dir
  if ((bepaald::fileOrDirExists(filename) && bepaald::isDir(filename)) ||
      (!bepaald::fileOrDirExists(filename) &&
      (filename.back() == '/' || filename.back() == std::filesystem::path::preferred_separator)))
    return exportBackupToDir(filename, overwrite, keepattachmentdatainmemory, onlydb);

  // export to file
  return exportBackupToFile(filename, passphrase, overwrite, keepattachmentdatainmemory);
}
