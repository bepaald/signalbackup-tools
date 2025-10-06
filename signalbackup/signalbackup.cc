/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#include "../common_filesystem.h"

SignalBackup::SignalBackup(std::string const &filename, std::string const &passphrase, bool verbose,
                           bool truncate, bool showprogress, bool replaceattachments, bool assumebadframesizeonbadmac,
                           std::vector<long long int> const &editattachments, bool stoponerror, bool fulldecode)
  :
  d_filename(filename),
  d_passphrase(passphrase),
  d_selfid(-1),
  d_databaseversion(-1),
  d_backupfileversion(-1),
  d_aggressive_filename_sanitizing(false),
  d_showprogress(showprogress),
  d_stoponerror(stoponerror),
  d_verbose(verbose),
  d_truncate(truncate),
  d_fulldecode(fulldecode),
  d_ok(false),
  d_found_sqlite_sequence_in_backup(false)
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

  checkDbIntegrityInternal(true /* onlywarn */);
}
