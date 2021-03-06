/*
    Copyright (C) 2021  Selwin van Dijk

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
  d_assumebadframesizeonbadmac(false),
  d_editattachmentsize(std::vector<long long int>()),
  d_dumpdesktopdb(std::string()),
  d_dumpmedia(std::string()),
  d_hhenkel(std::string()),
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
  d_fast(false),
  d_reordermmssmsids(false),
  d_stoponbadmac(false),
  d_verbose(false)
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
        d_output = arguments[++i];
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
        d_opassword = arguments[++i];
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
        d_source = arguments[++i];
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
        d_sourcepassword = arguments[++i];
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
        d_exportxml = arguments[++i];
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
        d_dumpdesktopdb = arguments[++i];
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
        d_dumpmedia = arguments[++i];
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
        d_hhenkel = arguments[++i];
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--importcsv")
    {
      if (i < arguments.size() - 1)
        d_importcsv = arguments[++i];
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
        d_importwachat = arguments[++i];
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
        d_setwatimefmt = arguments[++i];
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
        d_setselfid = arguments[++i];
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
    if (option == "--fast")
    {
      d_fast = true;
      continue;
    }
    if (option == "--no-fast")
    {
      d_fast = false;
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
    if (option == "--stoponbadmac")
    {
      d_stoponbadmac = true;
      continue;
    }
    if (option == "--no-stoponbadmac")
    {
      d_stoponbadmac = false;
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
    if (option[0] != '-')
    {
      if (d_positionals >= 2)
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Unknown option. ]" << std::endl;
        ok = false;
      }
      if (d_positionals == 0)
      {
        d_input = arguments[i];
      }
      if (d_positionals == 1)
      {
        d_password = arguments[i];
      }
      ++d_positionals;
      continue;
    }
    std::cerr << "[ Error parsing command line option `" << option << "': Unknown option. ]" << std::endl;
    ok = false;

  }
  return ok;
}
