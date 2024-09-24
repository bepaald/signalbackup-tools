#include "signalbackup.ih"

#include "../common_filesystem.h"

SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                           bool truncate, bool showprogress, bool replaceattachments, bool assumebadframesizeonbadmac,
                           std::vector<long long int> const &editattachments, bool stoponerror, bool fulldecode)
  :
  d_filename(filename),
  d_passphrase(passphrase),
  d_found_sqlite_sequence_in_backup(false),
  d_ok(false),
  d_databaseversion(-1),
  d_backupfileversion(-1),
  d_showprogress(showprogress),
  d_stoponerror(stoponerror),
  d_verbose(verbose),
  d_truncate(truncate),
  d_fulldecode(fulldecode),
  d_selfid(-1)
{
  if (bepaald::isDir(filename))
    initFromDir(filename, replaceattachments);
  else // not directory
  {
    d_fd.reset(new FileDecryptor(d_filename, d_passphrase, d_verbose, d_stoponerror, assumebadframesizeonbadmac, editattachments));
    if (!d_fd->ok())
      return;
    initFromFile();
  }

  if (!d_ok)
    return;

  Logger::message("Database version: ", d_databaseversion);

  checkDbIntegrity(true);
}
