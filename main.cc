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

#include <iostream>
#include <string>
#include <vector>

#include "arg/arg.h"
#include "common_be.h"
#include "main.h"
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
    std::cout << "Try '" << argv[0] << " --help' for available options" << std::endl;
    return 1;
  }

  if (arg.help())
  {
    arg.usage();
    return 0;
  }

  bool ipw_interactive = false;
  if ((arg.passphrase().empty() || arg.interactive()) && // prompt for input passphrase
      !bepaald::isDir(arg.input()))
  {
    std::string pw;
    std::cout << "Please provide passphrase for input file '" << arg.input() << "': "  << std::flush;
    if (!getPassword(&pw))
    {
      std::cout << "Failed to set passphrase" << std::endl;
      return 1;
    }
    arg.setpassphrase(pw);
    ipw_interactive = true;
  }

  if (!arg.source().empty() && (arg.interactive() || arg.sourcepassphrase().empty()))
  {
    std::string spw;
    std::cout << "Please provide passphrase for source file '" << arg.source() << "': "  << std::flush;
    if (!getPassword(&spw))
    {
      std::cout << "Failed to set passphrase" << std::endl;
      return 1;
    }
    arg.setsourcepassphrase(spw);
  }

  // Ask for output password if
  // output is written
  // AND its a regular file (not dir)
  // AND input password was not _initially_ set
  // AND either interactive is requested, output password was not provided
  if (!arg.output().empty() &&
      ((bepaald::fileOrDirExists(arg.output()) && !bepaald::isDir(arg.output())) ||
       (!bepaald::fileOrDirExists(arg.output()) && (arg.output().back() != '/' && arg.output().back() != std::filesystem::path::preferred_separator))) &&
      ipw_interactive &&
      (arg.interactive() || arg.opassphrase().empty()))
  {
    std::string opw;
    std::cout << "Please provide passphrase for output file '" << arg.output() << "' (leave empty to use input passphrase): "  << std::flush;
    if (!getPassword(&opw))
    {
      std::cout << "Failed to set passphrase" << std::endl;
      return 1;
    }
    arg.setopassphrase(opw);
  }

  // check output exists (file exists OR dir is not empty)
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

  // temporary. REMOVE THIS
  if (arg.strugee() != -1)
  {
    std::cout << "TEMP FUNCTION (#37)" << std::endl;
    FileDecryptor fd(arg.input(), arg.passphrase(), arg.verbose(), false, false);
    fd.strugee(arg.strugee());
    return 0;
  }
  if (arg.strugee3() != -1)
  {
    std::cout << "TEMP FUNCTION (#37)" << std::endl;
    FileDecryptor fd(arg.input(), arg.passphrase(), arg.verbose(), false, false);
    fd.strugee3(arg.strugee3());
    return 0;
  }
  if (arg.ashmorgan())
  {
    std::cout << "TEMP FUNCTION (#40)" << std::endl;
    FileDecryptor fd(arg.input(), arg.passphrase(), arg.verbose(), false, false);
    fd.ashmorgan();
    return 0;
  }
  else if (arg.strugee2())
  {
    std::cout << "TEMP FUNCTION 2 (#37)" << std::endl;
    FileDecryptor fd(arg.input(), arg.passphrase(), arg.verbose(), false, false);
    fd.strugee2();
    return 0;
  }

  MEMINFO("Start of program, before opening input");

  // open input
  std::unique_ptr<SignalBackup> sb(new SignalBackup(arg.input(), arg.passphrase(), arg.verbose(), arg.showprogress(),
                                                    arg.replaceattachments_bool(),
                                                    arg.assumebadframesizeonbadmac(), arg.editattachmentsize(),
                                                    arg.stoponerror()));
  if (!sb->ok())
  {
    std::cout << "Failed to open backup" << std::endl;
    return 1;
  }

  MEMINFO("Input opened");

  if (arg.listthreads())
    sb->listThreads();

  if (arg.showdbinfo())
    sb->showDBInfo();

  if (arg.generatefromtruncated())
  {
    //std::cout << "fillthread" << std::endl;
    sb->fillThreadTableFromMessages();
    //sb->addEndFrame(); // this should not be necessary, if endframe is missing, it's added in init
  }

  if (!arg.source().empty())
  {
    std::cout << "Target database info:" << std::endl;
    sb->summarize();
    bool sourcesummarized = false;

    std::unique_ptr<SignalBackup> source;
    std::vector<long long int> threads = arg.importthreads();
    if (threads.size() == 1 && threads[0] == -1) // import all threads!
    {

      MEMINFO("Before first time reading source");

      std::cout << "Requested ALL threads, reading source to get thread list" << std::endl;
      source.reset(new SignalBackup(arg.source(), arg.sourcepassphrase(), arg.verbose(), arg.showprogress(), !arg.replaceattachments().empty()));
      if (!source->ok())
      {
        std::cout << "Error opening source database" << std::endl;
        return 1;
      }

      MEMINFO("After first time reading source");

      source->summarize();
      sourcesummarized = true;

      std::cout << "Getting list of thread id's..." << std::flush;
      threads = source->threadIds();
      std::cout << "Got: " << std::flush;
      for (uint i = 0; i < threads.size(); ++i)
        std::cout << threads[i] << ((i < threads.size() - 1) ? "," : "\n");
    }

    for (uint i = 0; i < threads.size(); ++i)
    {

      MEMINFO("Before reading source: ", i + 1, "/", threads.size());

      std::cout << std::endl << "Importing thread " << threads[i] << " (" << i + 1 << "/" << threads.size() << ") from source file: " << arg.source() << std::endl;
      source.reset(new SignalBackup(arg.source(), arg.sourcepassphrase(), arg.verbose(), arg.showprogress(), !arg.replaceattachments().empty()));
      if (!source->ok())
      {
        std::cout << "Error opening source database" << std::endl;
        return 1;
      }
      if (!sourcesummarized)
      {
        source->summarize();
        sourcesummarized = true;
      }
      MEMINFO("After reading source: ", i + 1, "/", threads.size(), " before import");
      if (!sb->importThread(source.get(), threads[i]))
      {
        std::cout << "A fatal error occurred while trying to import thread " << threads[i] << ". Aborting" << std::endl;
        return 1;
      }
      MEMINFO("After import");
    }
  }

  MEMINFO("Before importfromdesktop");
  if (arg.importfromdesktop_bool())
    if (!sb->importFromDesktop(arg.importfromdesktop_1(), arg.importfromdesktop_2(), arg.desktopdbversion(),
                               arg.limittodates(), arg.autolimitdates(), arg.ignorewal()))
      return 1;
  MEMINFO("After importfromdesktop");

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
    // e.g.: sb->cropToDates({{"2019-09-18 00:00:00", "2020-09-18 00:00:00"}});
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

  if (!arg.dumpmedia().empty())
    if (!sb->dumpMedia(arg.dumpmedia(), arg.limittothreads(), arg.overwrite()))
      return 1;

  if (!arg.dumpavatars().empty())
    if (!sb->dumpAvatars(arg.dumpavatars(), arg.limitcontacts(), arg.overwrite()))
      return 1;

  if (arg.deleteattachments() || !arg.replaceattachments().empty())
  {
    if (!sb->deleteAttachments(arg.onlyinthreads(), arg.onlyolderthan(), arg.onlynewerthan(), arg.onlylargerthan(), arg.onlytype(), arg.appendbody(), arg.prependbody(), arg.replaceattachments()))
      return 1;
  }

  if (!arg.importwachat().empty())
    if (!sb->importWAChat(arg.importwachat(), arg.setwatimefmt(), arg.setselfid()))
      return 1;

  if (!arg.runsqlquery().empty())
    for (uint i = 0; i < arg.runsqlquery().size(); ++i)
      sb->runQuery(arg.runsqlquery()[i], false);

  if (!arg.runprettysqlquery().empty())
    for (uint i = 0; i < arg.runprettysqlquery().size(); ++i)
      sb->runQuery(arg.runprettysqlquery()[i], true);

  if (!arg.exporthtml().empty())
    if (!sb->exportHtml(arg.exporthtml(), arg.limittothreads(), (arg.split_bool() ? arg.split() : -1), arg.overwrite(), arg.append()))
      return 1;

  if (!arg.exportcsv().empty())
    for (uint i = 0; i < arg.exportcsv().size(); ++i)
      sb->exportCsv(arg.exportcsv()[i].second, arg.exportcsv()[i].first);

  if (!arg.exportxml().empty())
    if (!sb->exportXml(arg.exportxml(), arg.overwrite(), arg.setselfid(), arg.includemms(), SignalBackup::DROPATTACHMENTDATA))
    {
      std::cout << "Failed to export backup to '" << arg.exportxml() << "'" << std::endl;
      return 1;
    }

  // temporary, to generate truncated backup's missing data from Signal Desktop database INCOMPLETE
  if (!arg.hhenkel().empty())
  {
    sb->hhenkel(arg.hhenkel());
  }

  // temporary, to switch sender and recipient in single one-to-one conversation INCOMPLETE
  if (arg.hiperfall() != -1)
    if (!sb->hiperfall(arg.hiperfall(), arg.setselfid()))
    {
      std::cout << "Some error occurred..." << std::endl;
      return 1;
    }

  // temporary, to import messages from truncated database into older, but complete database
  if (!arg.sleepyh34d().empty())
  {
    if (!sb->sleepyh34d(arg.sleepyh34d()[0], (arg.sleepyh34d().size() > 1) ? arg.sleepyh34d()[1] : arg.passphrase()))
    {
      std::cout << "Error during import" << std::endl;
      return 1;
    }
  }

  // temporary, to investigate #95
  if (!arg.carowit_1().empty())
    return sb->carowit(arg.carowit_1(), arg.carowit_2());

  if (arg.scanmissingattachments())
    sb->scanMissingAttachments();

  if (arg.scramble())
    sb->scramble();

  if (arg.reordermmssmsids() ||
      !arg.source().empty()) // reorder mms after messing with mms._id
    if (!sb->reorderMmsSmsIds())
    {
      std::cout << "Error while reordering mms" << std::endl;
      return 1;
    }

  if (arg.checkdbintegrity())
    sb->checkDbIntegrity();

  if (arg.devcustom())
  {
    sb->devCustom();
    return 0;
  }

  MEMINFO("Before output");

  // export output
  if (!arg.output().empty())
  {
    sb->checkDbIntegrity(true);
    if (!sb->exportBackup(arg.output(), arg.opassphrase(), arg.overwrite(), SignalBackup::DROPATTACHMENTDATA, arg.onlydb()))
    {
      std::cout << "Failed to export backup to '" << arg.output() << "'" << std::endl;
      return 1;
    }
  }

  MEMINFO("After output");

  // decode and dump Signal-Desktop database to 'desktop.db'.
  if (!arg.dumpdesktopdb_1().empty())
  {
    SqlCipherDecryptor db(arg.dumpdesktopdb_1(), arg.dumpdesktopdb_2(), arg.desktopdbversion());
    if (!db.ok() || !db.writeToFile("desktop.db", arg.overwrite()))
      std::cout << "Failed to dump desktop database" << std::endl;
  }



#if defined(_WIN32) || defined(__MINGW64__)
  SetConsoleOutputCP(oldcodepage);
#endif

  MEMINFO("At program end");

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


/*

  OLD GROUPS STATUS MESSAGES

  Signal-Android/libsignal/service/src/main/proto/SignalService.proto:

  message AttachmentPointer {
  enum Flags {
  VOICE_MESSAGE = 1;
  }

  optional fixed64 id          = 1;
  optional string  contentType = 2;
  optional bytes   key         = 3;
  optional uint32  size        = 4;
  optional bytes   thumbnail   = 5;
  optional bytes   digest      = 6;
  optional string  fileName    = 7;
  optional uint32  flags       = 8;
  optional uint32  width       = 9;
  optional uint32  height      = 10;
  }
  message GroupContext {
  enum Type {
  UNKNOWN      = 0;
  UPDATE       = 1;
  DELIVER      = 2;
  QUIT         = 3;
  REQUEST_INFO = 4;
  }
  optional bytes             id      = 1;
  optional Type              type    = 2;
  optional string            name    = 3;
  repeated string            members = 4;
  optional AttachmentPointer avatar  = 5;
  }


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
