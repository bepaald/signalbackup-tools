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

#include <string>
#include <vector>

// the last part here is to work-around an apple clang thing. In true Apple fashion,
// they expose the feature flag without actually supporting (fully) span...
#if __cpp_lib_span >= 202002L && (!defined __apple_build_version__ || __apple_build_version__ >= 15000100)
#include <span>
#endif

#include "main.h"
#include "arg/arg.h"
#include "common_be.h"
#include "signalbackup/signalbackup.h"
#include "logger/logger.h"
#include "desktopdatabase/desktopdatabase.h"
#include "signalplaintextbackupdatabase/signalplaintextbackupdatabase.h"
#include "jsondatabase/jsondatabase.h"
#include "dummybackup/dummybackup.h"
#include "adbbackupdatabase/adbbackupdatabase.h"

#include "autoversion.h"

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

  Arg arg(argc, argv);

  if (!arg.logfile().empty())
    Logger::setFile(arg.logfile());

#ifdef VERSIONDATE
  #if defined(_WIN32) || defined(__MINGW64__)
  Logger::message("signalbackup-tools (", argv[0], ") source version ", VERSIONDATE, " (Win)", " (SQLite: ", SQLITE_VERSION, ", OpenSSL: ", OPENSSL_VERSION_TEXT, ")");
  #else
  Logger::message("signalbackup-tools (", argv[0], ") source version ", VERSIONDATE, " (SQLite: ", SQLITE_VERSION, ", OpenSSL: ", OPENSSL_VERSION_TEXT, ")");
  #endif
#endif

  if (!arg.ok())
  {
    //std::cout << "Error parsing arguments" << std::endl;
    //std::cout << "Try '" << argv[0] << " --help' for available options" << std::endl;
    Logger::error("Failed to parse arguments");
    Logger::error_indent("Try '", argv[0], " --help' for available options");

    return 1;
  }

  if (arg.verbose()) [[unlikely]]
    Logger::message("Parsed command line arguments.");
    //std::cout << "Parsed command line arguments." << std::endl;

  if (arg.help())
  {
    arg.usage();
    return 0;
  }

  SqliteDB::setConfigOptions();

  //**** OPTIONS THAT DO NOT REQUIRE SIGNAL BACKUP INPUT ****//
  std::unique_ptr<DesktopDatabase> ddb;
  std::unique_ptr<SignalPlaintextBackupDatabase> ptdb;
  std::unique_ptr<AdbBackupDatabase> adbdb;
  auto initDesktopDatabase = [&]()
  {
    if (!ddb)
      ddb.reset(new DesktopDatabase(arg.desktopdirs_1(), arg.desktopdirs_2(), arg.rawdesktopdb(), arg.desktopkey(),
                                    arg.verbose(), arg.ignorewal(), arg.desktopdbversion(), arg.truncate(),
                                    arg.showdesktopkey(), arg.dbusverbose()));
    return ddb->ok();
  };
#if __cpp_lib_span >= 202002L && (!defined __apple_build_version__ || __apple_build_version__ >= 15000100)
  auto initPlaintextDatabase = [&](std::span<std::string const> const &xmlfiles)
#else
  auto initPlaintextDatabase = [&](std::vector<std::string> const &xmlfiles)
#endif
  {
    if (!ptdb)
      ptdb.reset(new SignalPlaintextBackupDatabase(xmlfiles, arg.truncate(), arg.verbose(),
                                                   arg.mapxmlcontactnames(), arg.mapxmlcontactnamesfromfile(),
                                                   arg.mapxmladdresses(), arg.mapxmladdressesfromfile(),
                                                   arg.setcountrycode(), arg.xmlautogroupnames()));
    return ptdb->ok();
  };

  auto initAdbBackupDatabase = [&](std::string const &adbrootdir)
  {
    if (!adbdb)
      adbdb.reset(new AdbBackupDatabase(adbrootdir, arg.adbpassphrase(), arg.verbose()));
    return adbdb->ok();
  };

  if (!arg.generatedummy().empty())
  {
    DummyBackup d(arg.verbose(), arg.truncate(), arg.showprogress());
    if (!d.ok() ||
        !d.exportBackup(arg.generatedummy(), arg.opassphrase(), arg.overwrite(), SignalBackup::DROPATTACHMENTDATA, false /*onlydb*/))
      return 1;
  }
  if (!arg.generatedummyfordesktop().empty())
  {
    if (!initDesktopDatabase())
      return 1;
    DummyBackup d(ddb, arg.verbose(), arg.truncate(), arg.showprogress());
    if (!d.ok() ||
        !d.exportBackup(arg.generatedummyfordesktop(), arg.opassphrase(), arg.overwrite(), SignalBackup::DROPATTACHMENTDATA, false /*onlydb*/))
      return 1;
  }

  // show desktop key
  if (arg.showdesktopkey())
    if (!initDesktopDatabase())
      return 1;

  // run desktop sqlquery
  if (!arg.rundtsqlquery().empty())
  {
    if (!initDesktopDatabase())
      return 1;

    for (auto const &q : arg.rundtsqlquery())
      ddb->runQuery(q, arg.querymode());
  }
  if (!arg.rundtprettysqlquery().empty())
  {
    if (!initDesktopDatabase())
      return 1;

    for (auto const &q : arg.rundtprettysqlquery())
      ddb->runQuery(q, "pretty");
  }

  if (!arg.dumpdesktopdb().empty())
  {
    if (!initDesktopDatabase())
      return 1;

    if (!ddb->dumpDb(arg.dumpdesktopdb(), arg.overwrite()))
      return 1;
  }

  if (!arg.listjsonchats().empty())
  {
    JsonDatabase jdb(arg.listjsonchats(), arg.verbose(), arg.truncate());
    if (!jdb.ok())
      return 1;
    jdb.listChats();
  }

  if (!arg.listxmlcontacts().empty())
  {
    if (!initPlaintextDatabase(arg.listxmlcontacts()))
      return 1;
    ptdb->listContacts();
  }

  if (!arg.exportdesktophtml().empty() || !arg.exportdesktoptxt().empty())
  {
    if (!initDesktopDatabase())
      return 1;

    DummyBackup dummydb(ddb, arg.verbose(), arg.truncate(), arg.showprogress());
    if (!dummydb.ok())
    {
      if (arg.verbose()) [[unlikely]]
        Logger::error("DummyBackup not initialized ok");
      return 1;
    }

    if (!dummydb.importFromDesktop(ddb, true /*arg.skipmessagereorder()*/, arg.limittodates(), true /*addincompletedata*/,
                                   false /*importcontacts*/, false /*autolimittodates*/, true /*importstickers*/, arg.setselfid(),
                                   true /*targetisdummy*/, arg.migratedesktopdb()))
      return 1;

    if (!arg.exportdesktophtml().empty())
      if (!dummydb.exportHtml(arg.exportdesktophtml(), {} /*limittothreads*/, arg.limittodates(), arg.split_by(),
                              (arg.split_bool() ? arg.split() : -1), arg.setselfid(),  arg.includecalllog(), arg.searchpage(),
                              arg.stickerpacks(), arg.migratedb(), arg.overwrite(), arg.append(), arg.light(), arg.themeswitching(),
                              arg.addexportdetails(), arg.includeblockedlist(), arg.includefullcontactlist(), false /*arg.includesettings()*/,
                              arg.includereceipts(), arg.originalfilenames(), arg.linkify(), arg.chatfolders(), arg.compactfilenames(),
                              arg.htmlpagemenu(), arg.aggressivefilenamesanitizing(), arg.excludeexpiring(), arg.htmlfocusend(),
                              arg.htmlignoremediatypes()))
        return 1;

    if (!arg.exportdesktoptxt().empty())
      if (!dummydb.exportTxt(arg.exportdesktoptxt(), {} /*limittothreads*/, arg.limittodates(), arg.setselfid(),
                             arg.migratedb(), arg.aggressivefilenamesanitizing(), arg.overwrite()))
        return 1;
  }

  if (!arg.exportplaintextbackuphtml().empty())
  {
    // skip last entry, it is the output
#if __cpp_lib_span >= 202002L && (!defined __apple_build_version__ || __apple_build_version__ >= 15000100)
    if (!initPlaintextDatabase(std::span(arg.exportplaintextbackuphtml().begin(), arg.exportplaintextbackuphtml().end() - 1)))
#else
    std::vector<std::string> xmlfiles(arg.exportplaintextbackuphtml().begin(), arg.exportplaintextbackuphtml().end() - 1);
    if (!initPlaintextDatabase(xmlfiles))
#endif
      return 1;

    DummyBackup dummydb(ptdb, arg.setselfid(), arg.verbose(), arg.truncate(), arg.showprogress());
    if (!dummydb.ok())
      return 1;

    if (!dummydb.importFromPlaintextBackup(ptdb, true /*arg.skipmessagereorder()*/, arg.mapxmlcontacts(), arg.limittodates(),
                                           arg.selectxmlchats(), true /*addincompletedata*/, arg.xmlmarkdelivered(),
                                           arg.xmlmarkread(), false /*autolimittodates*/, arg.setselfid(), true /*isdummydb*/))
      return 1;

    if (!dummydb.exportHtml(arg.exportplaintextbackuphtml().back(), {} /*limittothreads*/, arg.limittodates(), arg.split_by(),
                            (arg.split_bool() ? arg.split() : -1), arg.setselfid(), arg.includecalllog(), arg.searchpage(),
                            arg.stickerpacks(), arg.migratedb(), arg.overwrite(), arg.append(), arg.light(), arg.themeswitching(),
                            arg.addexportdetails(), arg.includeblockedlist(), arg.includefullcontactlist(), false /*arg.includesettings()*/,
                            arg.includereceipts(), arg.originalfilenames(), arg.linkify(), arg.chatfolders(), arg.compactfilenames(),
                            arg.htmlpagemenu(), arg.aggressivefilenamesanitizing(), arg.excludeexpiring(), arg.htmlfocusend(),
                            arg.htmlignoremediatypes()))
      return 1;
  }

  if (!arg.exportadbbackuptohtml_1().empty())
  {
    if (!initAdbBackupDatabase(arg.exportadbbackuptohtml_1()))
      return 1;

    DummyBackup dummydb(adbdb, arg.setselfid(), arg.verbose(), arg.truncate(), arg.showprogress());
    if (!dummydb.ok())
      return 1;

    if (!dummydb.importFromAdbBackup(adbdb, arg.limittodates(), true /*isdummy*/))
      return 1;

    if (!dummydb.exportHtml(arg.exportadbbackuptohtml_2(), {} /*limittothreads*/, arg.limittodates(), arg.split_by(),
                            (arg.split_bool() ? arg.split() : -1), arg.setselfid(), arg.includecalllog(), arg.searchpage(),
                            arg.stickerpacks(), arg.migratedb(), arg.overwrite(), arg.append(), arg.light(), arg.themeswitching(),
                            arg.addexportdetails(), arg.includeblockedlist(), arg.includefullcontactlist(), false /*arg.includesettings()*/,
                            arg.includereceipts(), arg.originalfilenames(), arg.linkify(), arg.chatfolders(), arg.compactfilenames(),
                            arg.htmlpagemenu(), arg.aggressivefilenamesanitizing(), arg.excludeexpiring(), arg.htmlfocusend(),
                            arg.htmlignoremediatypes()))
      return 1;
  }

  // dump desktop attachments...


  //***** *****//
















  if (!arg.input_required() && arg.input().empty()) // no input is required -> all following operations require it
    return 0;                                       // -> none of the following was requested (but still decode if
                                                    // input was provided)

  if (arg.input().empty())  // at the very least an input file is needed
  {
    Logger::error("No input provided.");
    Logger::error_indent("Run with `", argv[0], " <INPUT> [<PASSPHRASE>] [OPTIONS]'");
    Logger::error_indent("Try '", argv[0], " --help' for available options");
    return 1;
  }

  if (arg.output() == arg.input())
  {
    Logger::error("Input and output refer to the same file. This is not supported.");
    return 1;
  }

  bool ipw_interactive = false;
  if ((arg.passphrase().empty() || arg.interactive()) && // prompt for input passphrase
      !bepaald::isDir(arg.input()))
  {
    std::string pw;
    Logger::message_start("Please provide passphrase for input file '", arg.input(), "': ");
    if (!getPassword(&pw))
    {
      Logger::error("Failed to set passphrase");
      return 1;
    }
    arg.setpassphrase(pw);
    ipw_interactive = true;
  }

  if (!arg.source().empty() && (arg.interactive() || arg.sourcepassphrase().empty()))
  {
    std::string spw;
    Logger::message_start("Please provide passphrase for source file '", arg.source(), "': ");
    if (!getPassword(&spw))
    {
      Logger::error("Failed to set passphrase");
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
    Logger::message_start("Please provide passphrase for output file '", arg.output(),
                          "' (leave empty to use input passphrase): ");
    if (!getPassword(&opw))
    {
      Logger::error("Failed to set passphrase");
      return 1;
    }
    arg.setopassphrase(opw);
  }

  // check output exists (file exists OR dir is not empty)
  if (!arg.output().empty() && bepaald::fileOrDirExists(arg.output()) &&
      ((!bepaald::isDir(arg.output()) || (/*bepaald::isDir(arg.output()) && */!bepaald::isEmpty(arg.output()))) &&
       !arg.overwrite()))
  {
    if (bepaald::isDir(arg.output()))
      Logger::error("Output directory `", arg.output(), "' not empty. Use --overwrite to clear contents before export.");
    else
      Logger::error("Output file `", arg.output(), "' exists. Use --overwrite to overwrite.");
    return 1;
  }

  MEMINFO("Start of program, before opening input");


#ifdef TIME_FUNCTIONS
  decltype(std::chrono::high_resolution_clock::now()) t1, t2;
#endif

#ifdef TIME_FUNCTIONS
  t1 = std::chrono::high_resolution_clock::now();
#endif

  // open input
  if (arg.verbose()) [[unlikely]]
    Logger::message("Opening input");
  std::unique_ptr<SignalBackup> sb(new SignalBackup(arg.input(), arg.passphrase(), arg.verbose(),
                                                    arg.truncate(), arg.showprogress(),
                                                    arg.replaceattachments_bool(),
                                                    arg.assumebadframesizeonbadmac(), arg.editattachmentsize(),
                                                    arg.stoponerror(), arg.fulldecode()));
  if (!sb->ok())
  {
    Logger::error("Failed to open backup");
    return 1;
  }
  if (arg.verbose()) [[unlikely]]
    Logger::message("Input opened successfully");

#ifdef TIME_FUNCTIONS
  t2 = std::chrono::high_resolution_clock::now();
  std::cout << " *** TIME: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms\n";
#endif


  MEMINFO("Input opened");

  std::vector<long long int> limittothreads = arg.limittothreads();
  if (!addThreadIdsFromString(sb.get(), arg.limittothreadsbyname(), &limittothreads))
    return 1;

  if (arg.listthreads())
    sb->listThreads();

  if (arg.listrecipients())
    sb->listRecipients();

  if (arg.showdbinfo())
    sb->showDBInfo();

  if (!arg.source().empty())
  {
    SignalBackup src(arg.source(), arg.sourcepassphrase(), arg.verbose(), arg.truncate(),
                     arg.showprogress(), !arg.replaceattachments().empty());
    std::vector<long long int> threads = arg.importthreads();
    if (threads.size() == 1 && threads[0] == -1) // import all threads!
    {

      MEMINFO("Before first time reading source");

      Logger::message("Requested ALL threads, reading source to get thread list");
      if (!src.ok())
      {
        Logger::error("Failed to open source database");
        return 1;
      }

      MEMINFO("After first time reading source");

      //src->summarize();
      //sourcesummarized = true;

      Logger::message("Getting list of thread id's...");
      threads = src.threadIds();
      // std::cout << "Got: " << std::flush;
      // for (unsigned int i = 0; i < threads.size(); ++i)
      //   std::cout << threads[i] << ((i < threads.size() - 1) ? "," : "\n");
      Logger::message("Got: ", threads);
    }

    // add any threads listed by thread name
    if (arg.importthreadsbyname().size())
      if (!addThreadIdsFromString(&src, arg.importthreadsbyname(), &threads))
        return 1;

    for (unsigned int i = 0; i < threads.size(); ++i)
    {

      MEMINFO("Before reading source: ", i + 1, "/", threads.size());

      Logger::message("\nImporting thread ", threads[i], " (", i + 1, "/", threads.size(), ") from source file: ", arg.source());

      SignalBackup sourcecopy(src);
      if (!sourcecopy.ok())
      {
        Logger::error("Failed to open source database");
        return 1;
      }
      // if (!sourcesummarized)
      // {
      //   source->summarize();
      //   sourcesummarized = true;
      // }
      MEMINFO("After reading source: ", i + 1, "/", threads.size(), " before import");
      if (!sb->importThread(&sourcecopy, threads[i]))
      {
        Logger::error("A fatal error occurred while trying to import thread ", threads[i], ". Aborting");
        //std::cout << "A fatal error occurred while trying to import thread " << threads[i] << ". Aborting" << std::endl;
        return 1;
      }
      MEMINFO("After import");
    }
  }

#if TIME_FUNCTIONS
  t1 = std::chrono::high_resolution_clock::now();
  std::cout << " *** TIME: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms\n";
#endif
  if (arg.importfromdesktop())
  {
    if (!initDesktopDatabase())
      return 1;

    MEMINFO("Before importfromdesktop");
    if (!sb->importFromDesktop(ddb, arg.skipmessagereorder(), arg.limittodates(),
                               (arg.addincompletedataforhtmlexport() || arg.importdesktopcontacts()),
                               arg.importdesktopcontacts(), arg.autolimitdates(),  arg.importstickers(),
                               arg.setselfid(), arg.targetisdummy(), arg.migratedesktopdb()))
      return 1;
    MEMINFO("After importfromdesktop");
  }
#if TIME_FUNCTIONS
  t2 = std::chrono::high_resolution_clock::now();
  std::cout << " *** TIME: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms\n";
#endif

  if (!arg.importplaintextbackup().empty())
  {
    if (!initPlaintextDatabase(arg.importplaintextbackup()))
      return 1;

    if (!sb->importFromPlaintextBackup(ptdb, arg.skipmessagereorder(), arg.mapxmlcontacts(), arg.limittodates(),
                                       arg.selectxmlchats(), arg.addincompletedataforhtmlexport(),
                                       arg.xmlmarkdelivered(), arg.xmlmarkread(), arg.autolimitdates(), arg.setselfid(),
                                       arg.targetisdummy()))
      return 1;
  }

  if (!arg.importadbbackup().empty())
  {
    if (!initAdbBackupDatabase(arg.importadbbackup()))
      return 1;

    if (!sb->importFromAdbBackup(adbdb, arg.limittodates(), arg.targetisdummy()))
      return 1;
  }

  if (!arg.importtelegram().empty())
    if (!sb->importTelegramJson(arg.importtelegram(), arg.selectjsonchats(), arg.mapjsoncontacts(), arg.preventjsonmapping(),
                                arg.jsonprependforward(), arg.skipmessagereorder(), arg.jsonmarkdelivered(), arg.jsonmarkread(),
                                arg.setselfid(), false /*onlyshowmapping*/))
      return 1;

  if (!arg.jsonshowcontactmap().empty())
    if (!sb->importTelegramJson(arg.jsonshowcontactmap(), arg.selectjsonchats(), arg.mapjsoncontacts(), arg.preventjsonmapping(),
                                arg.jsonprependforward(), arg.skipmessagereorder(), arg.jsonmarkdelivered(), arg.jsonmarkread(),
                                arg.setselfid(), true /*onlyshowmapping*/))
      return 1;

  if (arg.removedoubles_bool())
    sb->removeDoubles(arg.removedoubles());

  if (!arg.croptodates().empty())
  {
    if (arg.croptodates().size() % 2 != 0)
    {
      Logger::error("Wrong number of date-strings to croptodate");
      return 1;
    }
    std::vector<std::pair<std::string, std::string>> dates;
    for (unsigned int i = 0; i < arg.croptodates().size(); i += 2)
      dates.push_back({arg.croptodates()[i], arg.croptodates()[i + 1]});
    sb->cropToDates(dates);
    // e.g.: sb->cropToDates({{"2019-09-18 00:00:00", "2020-09-18 00:00:00"}});
  }

  if (!arg.croptothreads().empty() || !arg.croptothreadsbyname().empty())
  {
    std::vector<long long int> threads = arg.croptothreads();
    if (!addThreadIdsFromString(sb.get(), arg.croptothreadsbyname(), &threads))
      return 1;
    sb->cropToThread(threads);
  }

  if (!arg.mergerecipients().empty())
  {
    Logger::message("Merging recipients...");
    if (!sb->mergeRecipients(arg.mergerecipients()))
      return 1;
  }

  if (!arg.mergegroups().empty())
  {
    Logger::message("Merging groups...");
    sb->mergeGroups(arg.mergegroups());
  }

  if (!arg.dumpmedia().empty())
    if (!sb->dumpMedia(arg.dumpmedia(), arg.limittodates(), limittothreads, arg.excludestickers(),
                       arg.excludequotes(), arg.aggressivefilenamesanitizing(),  arg.overwrite()))
      return 1;

  if (!arg.dumpavatars().empty())
    if (!sb->dumpAvatars(arg.dumpavatars(), arg.limitcontacts(), arg.aggressivefilenamesanitizing(), arg.overwrite()))
      return 1;

  if (arg.deleteattachments() || !arg.replaceattachments().empty())
  {
    if (!sb->deleteAttachments(arg.onlyinthreads(), arg.onlyolderthan(), arg.onlynewerthan(), arg.onlylargerthan(), arg.onlytype(),
                               arg.appendbody(), arg.prependbody(), arg.replaceattachments()))
      return 1;
  }

  // if (!arg.importwachat().empty())
  //   if (!sb->importWAChat(arg.importwachat(), arg.setwatimefmt(), arg.setselfid()))
  //     return 1;

  if (!arg.setchatcolors().empty())
    if (!sb->setChatColors(arg.setchatcolors()))
      return 1;

  // temporary, to switch sender and recipient in single one-to-one conversation INCOMPLETE
  if (arg.hiperfall() != -1)
    if (!sb->hiperfall(arg.hiperfall(), arg.setselfid()))
    {
      Logger::error("Some error occurred in SignalBackup::hiperfall()...");
      return 1;
    }

  if (arg.autofixfkc())
    if (!sb->fixForeignKeyConstraintViolations())
      return 1;

  if (!arg.runsqlquery().empty())
    for (unsigned int i = 0; i < arg.runsqlquery().size(); ++i)
      sb->runQuery(arg.runsqlquery()[i], arg.querymode());

  if (!arg.runprettysqlquery().empty())
    for (unsigned int i = 0; i < arg.runprettysqlquery().size(); ++i)
      sb->runQuery(arg.runprettysqlquery()[i], "pretty");

  if (!arg.exporthtml().empty())
    if (!sb->exportHtml(arg.exporthtml(), limittothreads, arg.limittodates(), arg.split_by(), (arg.split_bool() ? arg.split() : -1),
                        arg.setselfid(), arg.includecalllog(), arg.searchpage(), arg.stickerpacks(), arg.migratedb(), arg.overwrite(),
                        arg.append(), arg.light(), arg.themeswitching(), arg.addexportdetails(), arg.includeblockedlist(),
                        arg.includefullcontactlist(), arg.includesettings(), arg.includereceipts(), arg.originalfilenames(),
                        arg.linkify(), arg.chatfolders(), arg.compactfilenames(), arg.htmlpagemenu(), arg.aggressivefilenamesanitizing(),
                        arg.excludeexpiring(), arg.htmlfocusend(), arg.htmlignoremediatypes()))
      return 1;

  if (!arg.exporttxt().empty())
    if (!sb->exportTxt(arg.exporttxt(), limittothreads, arg.limittodates(), arg.setselfid(),
                       arg.migratedb(), arg.aggressivefilenamesanitizing(), arg.overwrite()))
      return 1;

  if (!arg.exportcsv().empty())
    for (unsigned int i = 0; i < arg.exportcsv().size(); ++i)
      sb->exportCsv(arg.exportcsv()[i].second, arg.exportcsv()[i].first, arg.overwrite());

  if (!arg.exportxml().empty())
    if (!sb->exportXml(arg.exportxml(), arg.overwrite(), arg.setselfid(), arg.includemms(), SignalBackup::DROPATTACHMENTDATA))
    {
      Logger::error("Failed to export backup to '", arg.exportxml(), "'");
      return 1;
    }

  // // temporary, to generate truncated backup's missing data from Signal Desktop database INCOMPLETE
  // if (!arg.hhenkel().empty())
  // {
  //   sb->hhenkel(arg.hhenkel());
  // }

  // // temporary, to import messages from truncated database into older, but complete database
  // if (!arg.sleepyh34d().empty())
  // {
  //   if (!sb->sleepyh34d(arg.sleepyh34d()[0], (arg.sleepyh34d().size() > 1) ? arg.sleepyh34d()[1] : arg.passphrase()))
  //   {
  //     std::cout << "Error during import" << std::endl;
  //     return 1;
  //   }
  // }

  // temporary,
  // if (arg.arc() != -1)
  // {
  //   if (!sb->arc(arg.arc(), arg.setselfid()))
  //   {
  //     Logger::error("Failed somehow");
  //     return 1;
  //   }
  // }

  // // temporary, to investigate #95
  // if (!arg.carowit_1().empty())
  //   return sb->carowit(arg.carowit_1(), arg.carowit_2());

  if (arg.scanmissingattachments())
    sb->scanMissingAttachments();

  if (arg.findrecipient() != -1)
    sb->findRecipient(arg.findrecipient());

  if (arg.scramble())
    sb->scramble();

  if (arg.reordermmssmsids() ||
      !arg.source().empty()) // reorder mms after messing with mms._id
    if (!sb->reorderMmsSmsIds())
    {
      Logger::error("reordering mms");
      return 1;
    }

  if (arg.checkdbintegrity())
    sb->checkDbIntegrity();

  // if (arg.devcustom())
  // {
  //   sb->devCustom();
  //   return 0;
  // }

  /* CUSTOM */
  if (arg.custom_hugogithubs())
    if (!sb->custom_hugogithubs())
    {
      Logger::error("An error occurred running custom function");
      return 1;
    }

  // if (arg.migrate_to_191_CUSTOM())
  //   if (!sb->migrate_to_191_CUSTOM(arg.setselfid()))
  //   {
  //     Logger::error("Migration failed");
  //     return 1;
  //   }

  if (arg.migrate_to_191())
    if (!sb->migrate_to_191(arg.setselfid()))
    {
      Logger::error("Migration failed");
      return 1;
    }

  MEMINFO("Before output");

  // export output
  if (!arg.output().empty())
  {
    sb->checkDbIntegrityInternal(true /* warnonly */);
    if (!sb->exportBackup(arg.output(), arg.opassphrase(), arg.overwrite(), SignalBackup::DROPATTACHMENTDATA, arg.onlydb()))
    {
      Logger::error("Failed to export backup to '", arg.output(), "'");
      return 1;
    }
  }

  MEMINFO("After output");

#if defined(_WIN32) || defined(__MINGW64__)
  SetConsoleOutputCP(oldcodepage);
#endif

  MEMINFO("At program end");

  return 0;
}

bool addThreadIdsFromString(SignalBackup const *const backup, std::vector<std::string> const &names, std::vector<long long int> *threads)
{
  for (unsigned int i = 0; i < names.size(); ++i)
  {

    long long int r = backup->getRecipientIdFromName(names[i], true);
    if (r == -1)
      r = backup->getRecipientIdFromPhone(names[i], true);
    if (r == -1)
      r = backup->getRecipientIdFromUsername(names[i], true);
    if (r == -1)
    {
      Logger::error("Failed to find threadId for recipient '", names[i], "'");
      return false;
    }

    long long int t = backup->getThreadIdFromRecipient(r);
    if (t == -1)
    {
      Logger::error("Failed to find threadId for recipient '", names[i], "'");
      return false;
    }
    if (!bepaald::contains(threads, t))
      threads->push_back(t);
  }
  std::sort(threads->begin(), threads->end());
  return true;
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
