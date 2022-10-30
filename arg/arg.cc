/*
  Copyright (C) 2021-2022  Selwin van Dijk

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

#include "arg.h"

Arg::Arg(int argc, char *argv[])
  :
  d_ok(false),
  d_positionals(0),
  d_maxpositional(2),
  d_progname(argv[0]),
  d_input(std::string()),
  d_password(std::string()),
  d_importthreads(std::vector<int>()),
  d_limittothreads(std::vector<int>()),
  d_output(std::string()),
  d_opassword(std::string()),
  d_source(std::string()),
  d_sourcepassword(std::string()),
  d_generatefromtruncated(false),
  d_croptothreads(std::vector<long long int>()),
  d_croptodates(std::vector<std::string>()),
  d_mergerecipients(std::vector<std::string>()),
  d_mergegroups(std::vector<std::string>()),
  d_sleepyh34d(std::vector<std::string>()),
  d_exportcsv(std::vector<std::pair<std::string,std::string>>()),
  d_exportxml(std::string()),
  d_runsqlquery(std::vector<std::string>()),
  d_runprettysqlquery(std::vector<std::string>()),
  d_limitcontacts(std::vector<std::string>()),
  d_assumebadframesizeonbadmac(false),
  d_editattachmentsize(std::vector<long long int>()),
  d_dumpdesktopdb(std::string()),
  d_dumpmedia(std::string()),
  d_dumpavatars(std::string()),
  d_hhenkel(std::string()),
  d_devcustom(false),
  d_importcsv(std::string()),
  d_mapcsvfields(std::vector<std::pair<std::string,std::string>>()),
  d_importwachat(std::string()),
  d_setwatimefmt(std::string()),
  d_setselfid(std::string()),
  d_onlydb(bool()),
  d_overwrite(false),
  d_listthreads(false),
  d_editgroupmembers(false),
  d_showprogress(true),
  d_removedoubles(false),
  d_reordermmssmsids(false),
  d_stoponerror(false),
  d_verbose(false),
  d_strugee(-1),
  d_strugee3(-1),
  d_ashmorgan(false),
  d_strugee2(false),
  d_hiperfall(-1),
  d_deleteattachments(false),
  d_onlyinthreads(std::vector<long long int>()),
  d_onlyolderthan(std::string()),
  d_onlynewerthan(std::string()),
  d_onlylargerthan(-1),
  d_onlytype(std::vector<std::string>()),
  d_appendbody(std::string()),
  d_prependbody(std::string()),
  d_replaceattachments(std::vector<std::pair<std::string,std::string>>()),
  d_replaceattachments_bool(false),
  d_help(false),
  d_scanmissingattachments(false),
  d_showdbinfo(false),
  d_scramble(false),
  d_importfromdesktop(std::string()),
  d_importfromdesktop_bool(false),
  d_ignorewal(false),
  d_includemms(false),
  d_checkdbintegrity(false),
  d_interactive(false)
{
  // vector to hold arguments
  std::vector<std::string> config;

  // add command line options.
  config.insert(config.end(), argv + 1, argv + argc);

  d_ok = parseArgs(config);
}

bool Arg::parseArgs(std::vector<std::string> const &arguments)
{

  bool ok = true;

  for (size_t i = 0; i < arguments.size(); ++i)
  {
    std::string option = arguments[i];

    if (option == "-i" || option == "--input")
    {
      if (i < arguments.size() - 1)
      {
        d_input = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-p" || option == "--password")
    {
      if (i < arguments.size() - 1)
      {
        d_password = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--importthreads")
    {
      if (i < arguments.size() - 1)
      {
        if (arguments[i + 1] == "all" || arguments[i + 1] == "ALL")
        {
          int tmp;
          if (!ston(&tmp, std::string("-1")))
          {
            std::cerr << "Bad special value in argument spec file!" << std::endl;
            ok = false;
          }
          d_importthreads.clear();
          d_importthreads.push_back(tmp);
          ++i;
          continue;
        }
        if (!parseNumberList(arguments[++i], &d_importthreads))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--limittothreads")
    {
      if (i < arguments.size() - 1)
      {
        if (arguments[i + 1] == "all" || arguments[i + 1] == "ALL")
        {
          int tmp;
          if (!ston(&tmp, std::string("-1")))
          {
            std::cerr << "Bad special value in argument spec file!" << std::endl;
            ok = false;
          }
          d_limittothreads.clear();
          d_limittothreads.push_back(tmp);
          ++i;
          continue;
        }
        if (!parseNumberList(arguments[++i], &d_limittothreads))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-o" || option == "--output")
    {
      if (i < arguments.size() - 1)
      {
        d_output = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-op" || option == "--opassword")
    {
      if (i < arguments.size() - 1)
      {
        d_opassword = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-s" || option == "--source")
    {
      if (i < arguments.size() - 1)
      {
        d_source = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-sp" || option == "--sourcepassword")
    {
      if (i < arguments.size() - 1)
      {
        d_sourcepassword = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--generatefromtruncated")
    {
      d_generatefromtruncated = true;
      continue;
    }
    if (option == "--no-generatefromtruncated")
    {
      d_generatefromtruncated = false;
      continue;
    }
    if (option == "--croptothreads")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseNumberList(arguments[++i], &d_croptothreads))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--croptodates")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_croptodates))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--mergerecipients")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_mergerecipients))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--mergegroups")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_mergegroups))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--sleepyh34d")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_sleepyh34d))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--exportcsv")
    {
      if (i < arguments.size() - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_exportcsv, &error))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': " << error << " ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--exportxml")
    {
      if (i < arguments.size() - 1)
      {
        d_exportxml = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--runsqlquery")
    {
      if (i < arguments.size() - 1)
      {
          d_runsqlquery.push_back(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--runprettysqlquery")
    {
      if (i < arguments.size() - 1)
      {
          d_runprettysqlquery.push_back(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--limitcontacts")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_limitcontacts))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--assumebadframesizeonbadmac")
    {
      d_assumebadframesizeonbadmac = true;
      continue;
    }
    if (option == "--no-assumebadframesizeonbadmac")
    {
      d_assumebadframesizeonbadmac = false;
      continue;
    }
    if (option == "--editattachmentsize")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseNumberList(arguments[++i], &d_editattachmentsize))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--dumpdesktopdb")
    {
      if (i < arguments.size() - 1)
      {
        d_dumpdesktopdb = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--dumpmedia")
    {
      if (i < arguments.size() - 1)
      {
        d_dumpmedia = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--dumpavatars")
    {
      if (i < arguments.size() - 1)
      {
        d_dumpavatars = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--hhenkel")
    {
      if (i < arguments.size() - 1)
      {
        d_hhenkel = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--devcustom")
    {
      d_devcustom = true;
      continue;
    }
    if (option == "--no-devcustom")
    {
      d_devcustom = false;
      continue;
    }
    if (option == "--importcsv")
    {
      if (i < arguments.size() - 1)
      {
        d_importcsv = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--mapcsvfields")
    {
      if (i < arguments.size() - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_mapcsvfields, &error))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': " << error << " ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--importwachat")
    {
      if (i < arguments.size() - 1)
      {
        d_importwachat = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--setwatimefmt")
    {
      if (i < arguments.size() - 1)
      {
        d_setwatimefmt = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--setselfid")
    {
      if (i < arguments.size() - 1)
      {
        d_setselfid = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--onlydb")
    {
      d_onlydb = true;
      continue;
    }
    if (option == "--no-onlydb")
    {
      d_onlydb = false;
      continue;
    }
    if (option == "--overwrite")
    {
      d_overwrite = true;
      continue;
    }
    if (option == "--no-overwrite")
    {
      d_overwrite = false;
      continue;
    }
    if (option == "--listthreads")
    {
      d_listthreads = true;
      continue;
    }
    if (option == "--no-listthreads")
    {
      d_listthreads = false;
      continue;
    }
    if (option == "--editgroupmembers")
    {
      d_editgroupmembers = true;
      continue;
    }
    if (option == "--no-editgroupmembers")
    {
      d_editgroupmembers = false;
      continue;
    }
    if (option == "--showprogress")
    {
      d_showprogress = true;
      continue;
    }
    if (option == "--no-showprogress")
    {
      d_showprogress = false;
      continue;
    }
    if (option == "--removedoubles")
    {
      d_removedoubles = true;
      continue;
    }
    if (option == "--no-removedoubles")
    {
      d_removedoubles = false;
      continue;
    }
    if (option == "--reordermmssmsids")
    {
      d_reordermmssmsids = true;
      continue;
    }
    if (option == "--no-reordermmssmsids")
    {
      d_reordermmssmsids = false;
      continue;
    }
    if (option == "--stoponerror")
    {
      d_stoponerror = true;
      continue;
    }
    if (option == "--no-stoponerror")
    {
      d_stoponerror = false;
      continue;
    }
    if (option == "-v" || option == "--verbose")
    {
      d_verbose = true;
      continue;
    }
    if (option == "--no-verbose")
    {
      d_verbose = false;
      continue;
    }
    if (option == "--strugee")
    {
      if (i < arguments.size() - 1)
      {
        if (!ston(&d_strugee, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--strugee3")
    {
      if (i < arguments.size() - 1)
      {
        if (!ston(&d_strugee3, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--ashmorgan")
    {
      d_ashmorgan = true;
      continue;
    }
    if (option == "--no-ashmorgan")
    {
      d_ashmorgan = false;
      continue;
    }
    if (option == "--strugee2")
    {
      d_strugee2 = true;
      continue;
    }
    if (option == "--no-strugee2")
    {
      d_strugee2 = false;
      continue;
    }
    if (option == "--hiperfall")
    {
      if (i < arguments.size() - 1)
      {
        if (!ston(&d_hiperfall, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--deleteattachments")
    {
      d_deleteattachments = true;
      continue;
    }
    if (option == "--no-deleteattachments")
    {
      d_deleteattachments = false;
      continue;
    }
    if (option == "--onlyinthreads")
    {
      if (i < arguments.size() - 1)
      {
        if (arguments[i + 1] == "all" || arguments[i + 1] == "ALL")
        {
          long long int tmp;
          if (!ston(&tmp, std::string("-1")))
          {
            std::cerr << "Bad special value in argument spec file!" << std::endl;
            ok = false;
          }
          d_onlyinthreads.clear();
          d_onlyinthreads.push_back(tmp);
          ++i;
          continue;
        }
        if (!parseNumberList(arguments[++i], &d_onlyinthreads))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--onlyolderthan")
    {
      if (i < arguments.size() - 1)
      {
        d_onlyolderthan = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--onlynewerthan")
    {
      if (i < arguments.size() - 1)
      {
        d_onlynewerthan = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--onlylargerthan")
    {
      if (i < arguments.size() - 1)
      {
        if (!ston(&d_onlylargerthan, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--onlytype")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_onlytype))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--appendbody")
    {
      if (i < arguments.size() - 1)
      {
        d_appendbody = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--prependbody")
    {
      if (i < arguments.size() - 1)
      {
        d_prependbody = arguments[++i];
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--replaceattachments")
    {
      if (i < arguments.size() - 1 && !isOption(arguments[i + 1]))
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_replaceattachments, &error))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': " << error << " ]" << std::endl;
          ok = false;
        }
        d_replaceattachments_bool = true;
      }
      else
        d_replaceattachments_bool = true;
      continue;
    }
    if (option == "-h" || option == "--help")
    {
      d_help = true;
      continue;
    }
    if (option == "--no-help")
    {
      d_help = false;
      continue;
    }
    if (option == "--scanmissingattachments")
    {
      d_scanmissingattachments = true;
      continue;
    }
    if (option == "--no-scanmissingattachments")
    {
      d_scanmissingattachments = false;
      continue;
    }
    if (option == "--showdbinfo")
    {
      d_showdbinfo = true;
      continue;
    }
    if (option == "--no-showdbinfo")
    {
      d_showdbinfo = false;
      continue;
    }
    if (option == "--scramble")
    {
      d_scramble = true;
      continue;
    }
    if (option == "--no-scramble")
    {
      d_scramble = false;
      continue;
    }
    if (option == "--importfromdesktop")
    {
      if (i < arguments.size() - 1 && !isOption(arguments[i + 1]))
      {
        d_importfromdesktop = arguments[++i];
        d_importfromdesktop_bool = true;
      }
      else
        d_importfromdesktop_bool = true;
      continue;
    }
    if (option == "--ignorewal")
    {
      d_ignorewal = true;
      continue;
    }
    if (option == "--no-ignorewal")
    {
      d_ignorewal = false;
      continue;
    }
    if (option == "--includemms")
    {
      d_includemms = true;
      continue;
    }
    if (option == "--no-includemms")
    {
      d_includemms = false;
      continue;
    }
    if (option == "--checkdbintegrity")
    {
      d_checkdbintegrity = true;
      continue;
    }
    if (option == "--no-checkdbintegrity")
    {
      d_checkdbintegrity = false;
      continue;
    }
    if (option == "--interactive")
    {
      d_interactive = true;
      continue;
    }
    if (option == "--no-interactive")
    {
      d_interactive = false;
      continue;
    }
    if (option[0] != '-')
    {
      if (d_positionals >= 2)
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Unknown option. ]" << std::endl;
        ok = false;
      }
      if (i == 0)
      {
        d_input = arguments[i];
        //std::cout << "Got 'input' at pos " << i << std::endl;
      }
      else if (i == 1)
      {
        d_password = arguments[i];
        //std::cout << "Got 'password' at pos " << i << std::endl;
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Unknown option. ]" << std::endl;
        ok = false;
      }
      ++d_positionals;
      continue;
    }
    std::cerr << "[ Error parsing command line option `" << option << "': Unknown option. ]" << std::endl;
    ok = false;

  }
  return ok;
}
