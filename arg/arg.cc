/*
    Copyright (C) 2019  Selwin van Dijk

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
  d_importthreads(std::vector<int>()),
  d_input(std::string()),
  d_password(std::string()),
  d_output(std::string()),
  d_opassword(std::string()),
  d_source(std::string()),
  d_sourcepassword(std::string()),
  d_listthreads(false),
  d_generatefromtruncated(false),
  d_croptothreads(std::vector<long long int>()),
  d_croptodates(std::vector<std::string>()),
  d_elbrutalo(false),
  d_mergerecipients(std::vector<std::string>()),
  d_editgroupmembers(false),
  d_exportcsv(std::string()),
  d_exportxml(std::string()),
  d_runsqlquery(std::string()),
  d_runprettysqlquery(std::string()),
  d_showprogress(true),
  d_removedoubles(false),
  d_assumebadframesizeonbadmac(false),
  d_editattachmentsize(std::vector<long long int>())
{
  // vector to hold arguments
  std::vector<std::string> config;

  // add command line options.
  config.insert(config.end(), argv + 1, argv + argc);

  d_ok = parseArgs(config);
}

bool Arg::parseArgs(std::vector<std::string> const &arguments)
{
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
            std::cerr << "Bad special value in argument spec file!" << std::endl;
          d_importthreads.clear();
          d_importthreads.push_back(tmp);
          ++i;
          continue;
        }
        if (!parseNumberList(arguments[++i], &d_importthreads))
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
      }
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "-o" || option == "--output")
    {
      if (i < arguments.size() - 1)
        d_output = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "-op" || option == "--opassword")
    {
      if (i < arguments.size() - 1)
        d_opassword = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--source")
    {
      if (i < arguments.size() - 1)
        d_source = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--sourcepassword")
    {
      if (i < arguments.size() - 1)
        d_sourcepassword = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
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
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
      }
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--croptodates")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_croptodates))
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
      }
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--elbrutalo")
    {
      d_elbrutalo = true;
      continue;
    }
    if (option == "--no-elbrutalo")
    {
      d_elbrutalo = false;
      continue;
    }
    if (option == "--mergerecipients")
    {
      if (i < arguments.size() - 1)
      {
        if (!parseStringList(arguments[++i], &d_mergerecipients))
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
      }
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
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
    if (option == "--exportcsv")
    {
      if (i < arguments.size() - 1)
        d_exportcsv = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--exportxml")
    {
      if (i < arguments.size() - 1)
        d_exportxml = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--runsqlquery")
    {
      if (i < arguments.size() - 1)
        d_runsqlquery = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option == "--runprettysqlquery")
    {
      if (i < arguments.size() - 1)
        d_runprettysqlquery = arguments[++i];
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
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
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
      }
      else
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
      continue;
    }
    if (option[0] != '-')
    {
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

  }
  return true;
}
