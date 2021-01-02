/*
    Copyright (C) 2019-2021  Selwin van Dijk

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

#include <iostream>
#include <string>
#include <vector>

#include "arg/arg.h"
#include "common_be.h"
#include "signalbackup/signalbackup.h"
#include "sqlcipherdecryptor/sqlcipherdecryptor.h"

#if __has_include("autoversion.h")
#include "autoversion.h"
#endif

#if defined(_WIN32) || defined(__MINGW64__)
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#if defined(_WIN32) || defined(__MINGW64__)
  // set utf8 output
  unsigned int oldcodepage = GetConsoleOutputCP();
  SetConsoleOutputCP(65001);
  // enable ansi escape codes
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  GetConsoleMode(hConsole, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hConsole, mode);
#endif

#ifdef VERSIONDATE
#ifndef USE_CRYPTOPP
  std::cout << "signalbackup-tools (" << argv[0] << ") source version " << VERSIONDATE << " (OpenSSL)" << std::endl;
#else
  std::cout << "signalbackup-tools (" << argv[0] << ") source version " << VERSIONDATE << " (CryptoPP)" << std::endl;
#endif
#endif

  Arg arg(argc, argv);
  if (!arg.ok())
  {
    std::cout << "Error parsing arguments" << std::endl;
    return 1;
  }

  if (!arg.output().empty() && bepaald::fileOrDirExists(arg.output()) &&
      ((!bepaald::isDir(arg.output()) || (bepaald::isDir(arg.output()) && !bepaald::isEmpty(arg.output()))) &&
       !arg.overwrite()))
  {
    if (bepaald::isDir(arg.output()))
      std::cout << "Output directory `" << arg.output()
                << "' not empty. Use --overwrite to clear contents before export." << std::endl;
    else
      std::cout << "Output file `" << arg.output() << "' exists. Use --overwrite to overwrite." << std::endl;
    return 1;
  }

  // open input
  std::unique_ptr<SignalBackup> sb(new SignalBackup(arg.input(), arg.password(), arg.verbose(),
                                                    (!arg.source().empty() || arg.listthreads() ||
                                                     !arg.exportcsv().empty() || !arg.exportxml().empty() ||
                                                     !arg.runsqlquery().empty() || !arg.croptothreads().empty() ||
                                                     !arg.croptodates().empty() || arg.removedoubles() ||
                                                     !arg.runprettysqlquery().empty() || !arg.mergerecipients().empty() ||
                                                     arg.fast())
                                                    ? SignalBackup::LOWMEM : false, arg.showprogress(),
                                                    arg.assumebadframesizeonbadmac(), arg.editattachmentsize()));
  if (!sb->ok())
  {
    std::cout << "Failed to open backup" << std::endl;
    return 1;
  }

  if (arg.listthreads())
    sb->listThreads();

  if (arg.generatefromtruncated())
  {
    //std::cout << "fillthread" << std::endl;
    sb->fillThreadTableFromMessages();
    //sb->addEndFrame(); // this should not be necessary, if endframe is missing, it's added in init
  }

  if (!arg.source().empty())
  {
    std::unique_ptr<SignalBackup> source;
    std::vector<int> threads = arg.importthreads();
    if (threads.size() == 1 && threads[0] == -1) // import all threads!
    {
      std::cout << "Requested ALL threads, reading source to get thread list" << std::endl;
      source.reset(new SignalBackup(arg.source(), arg.sourcepassword(), arg.verbose(), SignalBackup::LOWMEM, arg.showprogress()));
      std::cout << "Getting list of thread id's..." << std::flush;
      threads = source->threadIds();
      std::cout << "Got: " << std::flush;
      for (uint i = 0; i < threads.size(); ++i)
        std::cout << threads[i] << ((i < threads.size() - 1) ? "," : "\n");
    }

    for (uint i = 0; i < threads.size(); ++i)
    {
      std::cout << std::endl << "Importing thread " << threads[i] << " from source file: " << arg.source() << std::endl;
      source.reset(new SignalBackup(arg.source(), arg.sourcepassword(), arg.verbose(), SignalBackup::LOWMEM, arg.showprogress()));
      sb->importThread(source.get(), threads[i]);
    }
  }

  if (arg.removedoubles())
    sb->removeDoubles();

  if (!arg.croptodates().empty())
  {
    if (arg.croptodates().size() % 2 != 0)
    {
      std::cout << "Wrong number of date-strings to croptodate" << std::endl;
      return 1;
    }
    std::vector<std::pair<std::string, std::string>> dates;
    for (uint i = 0; i < arg.croptodates().size(); i += 2)
      dates.push_back({arg.croptodates()[i], arg.croptodates()[i + 1]});
    sb->cropToDates(dates);
    //sb->cropToDates({{"2019-09-18 00:00:00", "2020-09-18 00:00:00"}});
  }

  if (!arg.croptothreads().empty())
    sb->cropToThread(arg.croptothreads());

  if (!arg.mergerecipients().empty())
  {
    std::cout << "Merging recipients..." << std::endl;
    sb->mergeRecipients(arg.mergerecipients(), arg.editgroupmembers());
  }

  if (!arg.mergegroups().empty())
  {
    std::cout << "Merging groups..." << std::endl;
    sb->mergeGroups(arg.mergegroups());
  }

  if (!arg.runsqlquery().empty())
    for (uint i = 0; i < arg.runsqlquery().size(); ++i)
      sb->runQuery(arg.runsqlquery()[i], false);

  if (!arg.runprettysqlquery().empty())
    for (uint i = 0; i < arg.runprettysqlquery().size(); ++i)
      sb->runQuery(arg.runprettysqlquery()[i], true);

  if (!arg.exportcsv().empty())
    for (uint i = 0; i < arg.exportcsv().size(); ++i)
      sb->exportCsv(arg.exportcsv()[i].second, arg.exportcsv()[i].first);

  if (!arg.exportxml().empty())
    if (!sb->exportXml(arg.exportxml(), arg.overwrite(), false /*include mms*/, SignalBackup::DROPATTACHMENTDATA))
    {
      std::cout << "Failed to export backup to '" << arg.exportxml() << "'" << std::endl;
      return 1;
    }

  // temporary, to generate truncated backup's missing data from Signal Desktop database
  if (!arg.hhenkel().empty())
  {
    sb->hhenkel(arg.hhenkel());
  }

  // export output
  if (!arg.output().empty())
    if (!sb->exportBackup(arg.output(), arg.opassword(), arg.overwrite(), SignalBackup::DROPATTACHMENTDATA))
    {
      std::cout << "Failed to export backup to '" << arg.output() << "'" << std::endl;
      return 1;
    }

  //sb->cropToThread({8, 10, 11});
  //sb->listThreads();

  // std::cout << "Starting export!" << std::endl;
  // sb.exportBackup("NEWFILE");
  // std::cout << "Finished" << std::endl;

  // // std::cout << "Starting export!" << std::endl;
  // // sb2.exportBackup("NEWFILE2");
  // // std::cout << "Finished" << std::endl;

  //auto itm = arg.importthreadsmanual();
  //for (uint i = 0; i < itm.size(); ++i)
  //{
  //  std::cout << itm[i].first << "  =  " << itm[i].second  << std::endl;
  //}

  // decode and dump Signal-Desktop database to 'desktop.db'.
  if (!arg.dumpdesktopdb().empty())
  {
    SqlCipherDecryptor db(arg.dumpdesktopdb());
    if (!db.ok() || !db.writeToFile("desktop.db", arg.overwrite()))
      std::cout << "Failed to dump desktop database" << std::endl;
  }



#if defined(_WIN32) || defined(__MINGW64__)
  SetConsoleOutputCP(oldcodepage);
#endif

  return 0;
}


  /* Database version notes


    In database versions <= 23: recipients_ids were phone numbers (eg "+31601513210" or "__textsecure_group__!...")
                                Avatars were linked to these contacts by AvatarFrame::name() which was also phone number -> "+31601513210"
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                23 < dbv <= 27: recipient ids were just id (eg "4")
                                Avatars were still identified by AvatarFrame::name() -> phone number
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                 27 < dbv < 54: recipient ids were just id (eg "4")
                                Avatars were identified by AvatarFrame::recipient() -> "4"
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                         >= 54: Same, groups have avatar in separate AvatarFrame, linked via groups.group_id == recipient.group_id -> recipient._id == AvatarFrame.recipient()
  */

  /* Notes on importing attachments

     - all values are imported, but
     "_data", "thumbnail" and "data_random" are reset by importer.
     thumbnail is set to NULL, so probably a good idea to unset aspect_ratio as well? It is called "THUMBNAIL_ASPECT_RATIO" in src.

     _id               : make sure not used already (int)
     mid               : belongs to certain specific mms._id (int)
     seq               : ??? take over? (int default 0)
     ct                : type, eg "image/jpeg" (text)
     name              : ??? not file_name, or maybe filename? (text)
     chset             : ??? (int)
     cd                : ??? content disposition (text)
     fn                : ??? (text)
     cid               : ??? (text)
     cl                : ??? content location (text)
     ctt_s             : ??? (int)
     ctt_t             : ??? (text)
     encrypted         : ??? (int)
     pending_push      : ??? probably can just take this over, or actually make sure its false (int)
     _data             : path to encrypted data, set when importing database (text)
     data_size         : set to size? (int)
     file_name         : filename? or maybe internal filename (/some/path/partXXXXXX.mms) (text)
     thumbnail         : set to NULL by importer (text)
     aspect_ratio      : maybe set to aspect ratio (if applicable) or Note: called THUMBNAIL_ASPECT_RATIO & THUMBNAIL = NULL (real)
     unique_id         : take over, it has to match AttFrame value (int)
     digest            : ??? (blob)
     fast_preflight    : ??? (text)
     voice_note        : indicates whether its a voice note i guess (int)
     data_random       : ??? set by importer (blob)
     thumbnail_random  : ??? (null)
     quote             : indicates whether the attachment is in a quote maybe?? (int)
     width             : width (int)
     height            : height (int)
     caption           : captino (text)
  */
