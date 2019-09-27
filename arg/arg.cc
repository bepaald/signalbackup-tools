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
  d_elbrutalo(false)
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
