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

#include "arg.h"

Arg::Arg(int argc, char *argv[])
  :
  d_ok(false),
  d_positionals(0),
  d_maxpositional(2),
  d_progname(argv[0]),
  d_input(std::string()),
  d_passphrase(std::string()),
  d_importthreads(std::vector<long long int>()),
  d_importthreadsbyname(std::vector<std::string>()),
  d_limittothreads(std::vector<long long int>()),
  d_limittothreadsbyname(std::vector<std::string>()),
  d_output(std::string()),
  d_opassphrase(std::string()),
  d_source(std::string()),
  d_sourcepassphrase(std::string()),
  d_croptothreads(std::vector<long long int>()),
  d_croptothreadsbyname(std::vector<std::string>()),
  d_croptodates(std::vector<std::string>()),
  d_mergerecipients(std::vector<std::string>()),
  d_mergegroups(std::vector<std::string>()),
  d_exportcsv(std::vector<std::pair<std::string,std::string>>()),
  d_exportxml(std::string()),
  d_querymode(std::string()),
  d_runsqlquery(std::vector<std::string>()),
  d_runprettysqlquery(std::vector<std::string>()),
  d_rundtsqlquery(std::vector<std::string>()),
  d_rundtprettysqlquery(std::vector<std::string>()),
  d_limitcontacts(std::vector<std::string>()),
  d_assumebadframesizeonbadmac(false),
  d_editattachmentsize(std::vector<long long int>()),
  d_dumpdesktopdb(std::string()),
  d_desktopdir(std::string()),
  d_desktopdirs_1(std::string()),
  d_desktopdirs_2(std::string()),
  d_rawdesktopdb(std::string()),
  d_desktopkey(std::string()),
  d_showdesktopkey(false),
  d_dumpmedia(std::string()),
  d_excludestickers(false),
  d_dumpavatars(std::string()),
  d_devcustom(false),
  d_importcsv(std::string()),
  d_mapcsvfields(std::vector<std::pair<std::string,std::string>>()),
  d_setselfid(std::string()),
  d_onlydb(bool()),
  d_overwrite(false),
  d_listthreads(false),
  d_listrecipients(false),
  d_showprogress(true),
  d_removedoubles(0),
  d_removedoubles_bool(false),
  d_reordermmssmsids(false),
  d_stoponerror(false),
  d_verbose(false),
  d_dbusverbose(false),
  d_strugee(-1),
  d_strugee3(-1),
  d_ashmorgan(false),
  d_strugee2(false),
  d_hiperfall(-1),
  d_arc(-1),
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
  d_importfromdesktop(false),
  d_limittodates(std::vector<std::string>()),
  d_autolimitdates(false),
  d_ignorewal(false),
  d_includemms(true),
  d_checkdbintegrity(false),
  d_interactive(false),
  d_exporthtml(std::string()),
  d_exportdesktophtml(std::string()),
  d_exportplaintextbackuphtml(std::vector<std::string>()),
  d_importplaintextbackup(std::vector<std::string>()),
  d_addexportdetails(false),
  d_includecalllog(false),
  d_includeblockedlist(false),
  d_includesettings(false),
  d_includefullcontactlist(false),
  d_themeswitching(false),
  d_searchpage(false),
  d_stickerpacks(false),
  d_includereceipts(false),
  d_chatfolders(false),
  d_split(1000),
  d_split_bool(false),
  d_split_by(std::string()),
  d_originalfilenames(false),
  d_addincompletedataforhtmlexport(false),
  d_importdesktopcontacts(false),
  d_light(false),
  d_exporttxt(std::string()),
  d_exportdesktoptxt(std::string()),
  d_append(false),
  d_desktopdbversion(4),
  d_migratedb(false),
  d_importstickers(false),
  d_findrecipient(-1),
  d_importtelegram(std::string()),
  d_listjsonchats(std::string()),
  d_selectjsonchats(std::vector<long long int>()),
  d_mapjsoncontacts(std::vector<std::pair<std::string, long long int>>()),
  d_preventjsonmapping(std::vector<std::string>()),
  d_jsonprependforward(false),
  d_jsonmarkdelivered(true),
  d_jsonmarkread(false),
  d_xmlmarkdelivered(true),
  d_xmlmarkread(false),
  d_fulldecode(false),
  d_logfile(std::string()),
  d_custom_hugogithubs(false),
  d_truncate(true),
  d_skipmessagereorder(false),
  d_migrate_to_191(false),
  d_mapxmlcontacts(std::vector<std::pair<std::string,long long int>>()),
  d_listxmlcontacts(std::vector<std::string>()),
  d_selectxmlchats(std::vector<std::string>()),
  d_linkify(true),
  d_setchatcolors(std::vector<std::pair<long long int, std::string>>()),
  d_mapxmlcontactnames(std::vector<std::pair<std::string, std::string>>()),
  d_mapxmlcontactnamesfromfile(std::string()),
  d_mapxmladdresses(std::vector<std::pair<std::string, std::string>>()),
  d_mapxmladdressesfromfile(std::string()),
  d_xmlautogroupnames(false),
  d_setcountrycode(std::string()),
  d_compactfilenames(false),
  d_generatedummy(std::string()),
  d_targetisdummy(false),
  d_htmlignoremediatypes(std::vector<std::string>()),
  d_htmlpagemenu(true),
  d_input_required(false)
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

  int argsize = arguments.size();
  for (int i = 0; i < argsize; ++i)
  {
    std::string option = arguments[i];

    if (option == "-i" || option == "--input")
    {
      if (i < argsize - 1)
      {
        d_input = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-p" || option == "--passphrase" || option == "--password")
    {
      if (i < argsize - 1)
      {
        d_passphrase = std::move(arguments[++i]);
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
      if (i < argsize - 1)
      {
        if (arguments[i + 1] == "all" || arguments[i + 1] == "ALL")
        {
          long long int tmp;
          if (!ston(&tmp, std::string("-1")))
          {
            std::cerr << "Bad special value in argument spec file!" << std::endl;
            ok = false;
          }
          d_importthreads.clear();
          d_importthreads.push_back(tmp);
          ++i;
          d_input_required = true;
          continue;
        }
        if (!parseNumberList(arguments[++i], &d_importthreads, true))
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
      d_input_required = true;
      continue;
    }
    if (option == "--importthreadsbyname")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_importthreadsbyname))
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
      d_input_required = true;
      continue;
    }
    if (option == "--limittothreads")
    {
      if (i < argsize - 1)
      {
        if (!parseNumberList(arguments[++i], &d_limittothreads, true))
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
    if (option == "--limittothreadsbyname")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_limittothreadsbyname))
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
      if (i < argsize - 1)
      {
        d_output = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "-op" || option == "--opassphrase" || option == "--opassword")
    {
      if (i < argsize - 1)
      {
        d_opassphrase = std::move(arguments[++i]);
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
      if (i < argsize - 1)
      {
        d_source = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "-sp" || option == "--sourcepassphrase" || option == "--sourcepassword")
    {
      if (i < argsize - 1)
      {
        d_sourcepassphrase = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--croptothreads")
    {
      if (i < argsize - 1)
      {
        if (!parseNumberList(arguments[++i], &d_croptothreads, true))
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
      d_input_required = true;
      continue;
    }
    if (option == "--croptothreadsbyname")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_croptothreadsbyname))
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
      d_input_required = true;
      continue;
    }
    if (option == "--croptodates")
    {
      if (i < argsize - 1)
      {
        std::regex validator("^(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+), *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+)(?:, *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+), *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+))*$");
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
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
      d_input_required = true;
      continue;
    }
    if (option == "--mergerecipients")
    {
      if (i < argsize - 1)
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
      d_input_required = true;
      continue;
    }
    if (option == "--mergegroups")
    {
      if (i < argsize - 1)
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
      d_input_required = true;
      continue;
    }
    if (option == "--exportcsv")
    {
      if (i < argsize - 1)
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
      d_input_required = true;
      continue;
    }
    if (option == "--exportxml")
    {
      if (i < argsize - 1)
      {
        d_exportxml = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--querymode")
    {
      if (i < argsize - 1)
      {
        std::regex validator("line|pretty|single", std::regex::icase);
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        d_querymode = std::move(arguments[++i]);
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
      if (i < argsize - 1)
      {
        d_runsqlquery.emplace_back(std::move(arguments[++i]));
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--runprettysqlquery")
    {
      if (i < argsize - 1)
      {
        d_runprettysqlquery.emplace_back(std::move(arguments[++i]));
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--rundtsqlquery")
    {
      if (i < argsize - 1)
      {
        d_rundtsqlquery.emplace_back(std::move(arguments[++i]));
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--rundtprettysqlquery")
    {
      if (i < argsize - 1)
      {
        d_rundtprettysqlquery.emplace_back(std::move(arguments[++i]));
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
      if (i < argsize - 1)
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
      if (i < argsize - 1)
      {
        if (!parseNumberList(arguments[++i], &d_editattachmentsize, false))
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
      if (i < argsize - 1)
      {
        d_dumpdesktopdb = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--desktopdir")
    {
      if (i < argsize - 1)
      {
        d_desktopdir = std::move(arguments[++i]);
        d_desktopdirs_1 = d_desktopdir;
        d_desktopdirs_2 = d_desktopdir;
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--desktopdirs")
    {
      if (i < argsize - 2)
      {
        d_desktopdirs_1 = std::move(arguments[++i]);
        d_desktopdirs_2 = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--rawdesktopdb")
    {
      if (i < argsize - 1)
      {
        d_rawdesktopdb = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--desktopkey")
    {
      if (i < argsize - 1)
      {
        std::regex validator("^[0-9a-fA-F]{64}$", std::regex::icase);
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        d_desktopkey = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--showdesktopkey")
    {
      d_showdesktopkey = true;
      continue;
    }
    if (option == "--no-showdesktopkey")
    {
      d_showdesktopkey = false;
      continue;
    }
    if (option == "--dumpmedia")
    {
      if (i < argsize - 1)
      {
        d_dumpmedia = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--excludestickers")
    {
      d_excludestickers = true;
      continue;
    }
    if (option == "--no-excludestickers")
    {
      d_excludestickers = false;
      continue;
    }
    if (option == "--dumpavatars")
    {
      if (i < argsize - 1)
      {
        d_dumpavatars = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
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
      if (i < argsize - 1)
      {
        d_importcsv = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--mapcsvfields")
    {
      if (i < argsize - 1)
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
    if (option == "--setselfid")
    {
      if (i < argsize - 1)
      {
        d_setselfid = std::move(arguments[++i]);
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
      d_input_required = true;
      continue;
    }
    if (option == "--no-listthreads")
    {
      d_listthreads = false;
      continue;
    }
    if (option == "--listrecipients")
    {
      d_listrecipients = true;
      d_input_required = true;
      continue;
    }
    if (option == "--no-listrecipients")
    {
      d_listrecipients = false;
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
      d_removedoubles_bool = true;
      if (i < argsize - 1 && !isOption(arguments[i + 1]))
      {
        if (!ston(&d_removedoubles, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      d_input_required = true;
      continue;
    }
    if (option == "--reordermmssmsids")
    {
      d_reordermmssmsids = true;
      d_input_required = true;
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
    if (option == "--dbusverbose")
    {
      d_dbusverbose = true;
      continue;
    }
    if (option == "--no-dbusverbose")
    {
      d_dbusverbose = false;
      continue;
    }
    if (option == "--strugee")
    {
      if (i < argsize - 1)
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
      if (i < argsize - 1)
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
      if (i < argsize - 1)
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
    if (option == "--arc")
    {
      if (i < argsize - 1)
      {
        if (!ston(&d_arc, arguments[++i]))
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
      d_input_required = true;
      continue;
    }
    if (option == "--no-deleteattachments")
    {
      d_deleteattachments = false;
      continue;
    }
    if (option == "--onlyinthreads")
    {
      if (i < argsize - 1)
      {
        if (!parseNumberList(arguments[++i], &d_onlyinthreads, true))
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
      if (i < argsize - 1)
      {
        std::regex validator("^(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+)$", std::regex::icase);
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        d_onlyolderthan = std::move(arguments[++i]);
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
      if (i < argsize - 1)
      {
        std::regex validator("^(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+)$", std::regex::icase);
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        d_onlynewerthan = std::move(arguments[++i]);
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
      if (i < argsize - 1)
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
      if (i < argsize - 1)
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
      if (i < argsize - 1)
      {
        d_appendbody = std::move(arguments[++i]);
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
      if (i < argsize - 1)
      {
        d_prependbody = std::move(arguments[++i]);
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
      if (i < argsize - 1 && !isOption(arguments[i + 1]))
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
      d_input_required = true;
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
      d_input_required = true;
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
      d_input_required = true;
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
      d_input_required = true;
      continue;
    }
    if (option == "--no-scramble")
    {
      d_scramble = false;
      continue;
    }
    if (option == "--importfromdesktop")
    {
      d_importfromdesktop = true;
      d_input_required = true;
      continue;
    }
    if (option == "--no-importfromdesktop")
    {
      d_importfromdesktop = false;
      continue;
    }
    if (option == "--limittodates")
    {
      if (i < argsize - 1)
      {
        std::regex validator("^(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+), *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+)(?:, *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+), *(?:(?:[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2})|[0-9]+))*$");
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        if (!parseStringList(arguments[++i], &d_limittodates))
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
    if (option == "--autolimitdates")
    {
      d_autolimitdates = true;
      continue;
    }
    if (option == "--no-autolimitdates")
    {
      d_autolimitdates = false;
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
      d_input_required = true;
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
    if (option == "--exporthtml")
    {
      if (i < argsize - 1)
      {
        d_exporthtml = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--exportdesktophtml")
    {
      if (i < argsize - 1)
      {
        d_exportdesktophtml = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--exportplaintextbackuphtml")
    {
      while (i < argsize - 1 && !isOption(arguments[i + 1]))
      {
        d_exportplaintextbackuphtml.emplace_back(std::move(arguments[++i]));
      }
      if (d_exportplaintextbackuphtml.size() < 2)
      {
        std::cerr << "[ Error parsing command line option `" << option << "': 2 arguments required, " << d_exportplaintextbackuphtml.size() << " provided ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--importplaintextbackup")
    {
      while (i < argsize - 1 && !isOption(arguments[i + 1]))
      {
        d_importplaintextbackup.emplace_back(std::move(arguments[++i]));
      }
      if (d_importplaintextbackup.size() < 1)
      {
        std::cerr << "[ Error parsing command line option `" << option << "': 1 arguments required, " << d_importplaintextbackup.size() << " provided ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--addexportdetails")
    {
      d_addexportdetails = true;
      continue;
    }
    if (option == "--no-addexportdetails")
    {
      d_addexportdetails = false;
      continue;
    }
    if (option == "--includecalllog")
    {
      d_includecalllog = true;
      continue;
    }
    if (option == "--no-includecalllog")
    {
      d_includecalllog = false;
      continue;
    }
    if (option == "--includeblockedlist")
    {
      d_includeblockedlist = true;
      continue;
    }
    if (option == "--no-includeblockedlist")
    {
      d_includeblockedlist = false;
      continue;
    }
    if (option == "--includesettings")
    {
      d_includesettings = true;
      continue;
    }
    if (option == "--no-includesettings")
    {
      d_includesettings = false;
      continue;
    }
    if (option == "--includefullcontactlist")
    {
      d_includefullcontactlist = true;
      continue;
    }
    if (option == "--no-includefullcontactlist")
    {
      d_includefullcontactlist = false;
      continue;
    }
    if (option == "--themeswitching")
    {
      d_themeswitching = true;
      continue;
    }
    if (option == "--no-themeswitching")
    {
      d_themeswitching = false;
      continue;
    }
    if (option == "--searchpage")
    {
      d_searchpage = true;
      continue;
    }
    if (option == "--no-searchpage")
    {
      d_searchpage = false;
      continue;
    }
    if (option == "--stickerpacks")
    {
      d_stickerpacks = true;
      continue;
    }
    if (option == "--no-stickerpacks")
    {
      d_stickerpacks = false;
      continue;
    }
    if (option == "--includereceipts")
    {
      d_includereceipts = true;
      continue;
    }
    if (option == "--no-includereceipts")
    {
      d_includereceipts = false;
      continue;
    }
    if (option == "--chatfolders")
    {
      d_chatfolders = true;
      continue;
    }
    if (option == "--no-chatfolders")
    {
      d_chatfolders = false;
      continue;
    }
    if (option == "--allhtmlpages")
    {
      d_includecalllog = true;
      d_includeblockedlist = true;
      d_includesettings = true;
      d_includefullcontactlist = true;
      d_themeswitching = true;
      d_searchpage = true;
      d_stickerpacks = true;
      d_addexportdetails = true;
      continue;
    }
    if (option == "--split")
    {
      d_split_bool = true;
      if (i < argsize - 1 && !isOption(arguments[i + 1]))
      {
        if (!ston(&d_split, arguments[++i]))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
        }
      }
      d_split_by.clear();
      continue;
    }
    if (option == "--split-by")
    {
      if (i < argsize - 1)
      {
        std::regex validator("year|month|week|day", std::regex::icase);
        if (!std::regex_match(arguments[i + 1], validator))
        {
          std::cerr << "[ Error parsing command line option `" << option << "': Bad argument. ]" << std::endl;
          ok = false;
          continue;
        }
        d_split_by = std::move(arguments[++i]);
        d_split_bool = false;
        d_split = -1;
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--originalfilenames")
    {
      d_originalfilenames = true;
      continue;
    }
    if (option == "--no-originalfilenames")
    {
      d_originalfilenames = false;
      continue;
    }
    if (option == "--addincompletedataforhtmlexport")
    {
      d_addincompletedataforhtmlexport = true;
      continue;
    }
    if (option == "--no-addincompletedataforhtmlexport")
    {
      d_addincompletedataforhtmlexport = false;
      continue;
    }
    if (option == "--importdesktopcontacts")
    {
      d_importdesktopcontacts = true;
      continue;
    }
    if (option == "--no-importdesktopcontacts")
    {
      d_importdesktopcontacts = false;
      continue;
    }
    if (option == "--light")
    {
      d_light = true;
      continue;
    }
    if (option == "--no-light")
    {
      d_light = false;
      continue;
    }
    if (option == "--exporttxt")
    {
      if (i < argsize - 1)
      {
        d_exporttxt = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--exportdesktoptxt")
    {
      if (i < argsize - 1)
      {
        d_exportdesktoptxt = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--append")
    {
      d_append = true;
      continue;
    }
    if (option == "--no-append")
    {
      d_append = false;
      continue;
    }
    if (option == "--desktopdbversion")
    {
      if (i < argsize - 1)
      {
        if (!ston(&d_desktopdbversion, arguments[++i]))
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
    if (option == "--migratedb")
    {
      d_migratedb = true;
      continue;
    }
    if (option == "--no-migratedb")
    {
      d_migratedb = false;
      continue;
    }
    if (option == "--importstickers")
    {
      d_importstickers = true;
      continue;
    }
    if (option == "--no-importstickers")
    {
      d_importstickers = false;
      continue;
    }
    if (option == "--findrecipient")
    {
      if (i < argsize - 1)
      {
        if (!ston(&d_findrecipient, arguments[++i]))
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
    if (option == "--importtelegram" || option == "--importjson")
    {
      if (i < argsize - 1)
      {
        d_importtelegram = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      d_input_required = true;
      continue;
    }
    if (option == "--listjsonchats")
    {
      if (i < argsize - 1)
      {
        d_listjsonchats = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--selectjsonchats")
    {
      if (i < argsize - 1)
      {
        if (!parseNumberList(arguments[++i], &d_selectjsonchats, true))
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
    if (option == "--mapjsoncontacts")
    {
      if (i < argsize - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_mapjsoncontacts, &error))
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
    if (option == "--preventjsonmapping")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_preventjsonmapping))
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
    if (option == "--jsonprependforward")
    {
      d_jsonprependforward = true;
      continue;
    }
    if (option == "--no-jsonprependforward")
    {
      d_jsonprependforward = false;
      continue;
    }
    if (option == "--jsonmarkdelivered")
    {
      d_jsonmarkdelivered = true;
      continue;
    }
    if (option == "--no-jsonmarkdelivered")
    {
      d_jsonmarkdelivered = false;
      continue;
    }
    if (option == "--jsonmarkread")
    {
      d_jsonmarkread = true;
      continue;
    }
    if (option == "--no-jsonmarkread")
    {
      d_jsonmarkread = false;
      continue;
    }
    if (option == "--xmlmarkdelivered")
    {
      d_xmlmarkdelivered = true;
      continue;
    }
    if (option == "--no-xmlmarkdelivered")
    {
      d_xmlmarkdelivered = false;
      continue;
    }
    if (option == "--xmlmarkread")
    {
      d_xmlmarkread = true;
      continue;
    }
    if (option == "--no-xmlmarkread")
    {
      d_xmlmarkread = false;
      continue;
    }
    if (option == "--fulldecode")
    {
      d_fulldecode = true;
      d_input_required = true;
      continue;
    }
    if (option == "--no-fulldecode")
    {
      d_fulldecode = false;
      continue;
    }
    if (option == "-l" || option == "--logfile")
    {
      if (i < argsize - 1)
      {
        d_logfile = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--custom_hugogithubs" || option == "--migrate214to215")
    {
      d_custom_hugogithubs = true;
      d_input_required = true;
      continue;
    }
    if (option == "--no-custom_hugogithubs")
    {
      d_custom_hugogithubs = false;
      continue;
    }
    if (option == "--truncate")
    {
      d_truncate = true;
      continue;
    }
    if (option == "--no-truncate")
    {
      d_truncate = false;
      continue;
    }
    if (option == "--skipmessagereorder")
    {
      d_skipmessagereorder = true;
      continue;
    }
    if (option == "--no-skipmessagereorder")
    {
      d_skipmessagereorder = false;
      continue;
    }
    if (option == "--migrate_to_191")
    {
      d_migrate_to_191 = true;
      d_input_required = true;
      continue;
    }
    if (option == "--no-migrate_to_191")
    {
      d_migrate_to_191 = false;
      continue;
    }
    if (option == "--mapxmlcontacts")
    {
      if (i < argsize - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_mapxmlcontacts, &error))
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
    if (option == "--listxmlcontacts")
    {
      while (i < argsize - 1 && !isOption(arguments[i + 1]))
      {
        d_listxmlcontacts.emplace_back(std::move(arguments[++i]));
      }
      if (d_listxmlcontacts.size() < 1)
      {
        std::cerr << "[ Error parsing command line option `" << option << "': 1 arguments required, " << d_listxmlcontacts.size() << " provided ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--selectxmlchats")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_selectxmlchats))
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
    if (option == "--linkify")
    {
      d_linkify = true;
      continue;
    }
    if (option == "--no-linkify")
    {
      d_linkify = false;
      continue;
    }
    if (option == "--setchatcolors")
    {
      if (i < argsize - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_setchatcolors, &error))
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
    if (option == "--mapxmlcontactnames")
    {
      if (i < argsize - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_mapxmlcontactnames, &error))
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
    if (option == "--mapxmlcontactnamesfromfile")
    {
      if (i < argsize - 1)
      {
        d_mapxmlcontactnamesfromfile = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--mapxmladdresses")
    {
      if (i < argsize - 1)
      {
        std::string error;
        if (!parsePairList(arguments[++i], "=", &d_mapxmladdresses, &error))
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
    if (option == "--mapxmladdressesfromfile")
    {
      if (i < argsize - 1)
      {
        d_mapxmladdressesfromfile = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--xmlautogroupnames")
    {
      d_xmlautogroupnames = true;
      continue;
    }
    if (option == "--no-xmlautogroupnames")
    {
      d_xmlautogroupnames = false;
      continue;
    }
    if (option == "--setcountrycode")
    {
      if (i < argsize - 1)
      {
        d_setcountrycode = std::move(arguments[++i]);
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--compactfilenames")
    {
      d_compactfilenames = true;
      continue;
    }
    if (option == "--no-compactfilenames")
    {
      d_compactfilenames = false;
      continue;
    }
    if (option == "--generatedummy")
    {
      if (i < argsize - 1)
      {
        d_generatedummy = std::move(arguments[++i]);
        d_opassphrase = "000000000000000000000000000001";
      }
      else
      {
        std::cerr << "[ Error parsing command line option `" << option << "': Missing argument. ]" << std::endl;
        ok = false;
      }
      continue;
    }
    if (option == "--targetisdummy")
    {
      d_targetisdummy = true;
      d_passphrase = "000000000000000000000000000001";
      d_opassphrase = "000000000000000000000000000001";
      continue;
    }
    if (option == "--no-targetisdummy")
    {
      d_targetisdummy = false;
      continue;
    }
    if (option == "--htmlignoremediatypes")
    {
      if (i < argsize - 1)
      {
        if (!parseStringList(arguments[++i], &d_htmlignoremediatypes))
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
    if (option == "--htmlpagemenu")
    {
      d_htmlpagemenu = true;
      continue;
    }
    if (option == "--no-htmlpagemenu")
    {
      d_htmlpagemenu = false;
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
        d_input = std::move(option);
        //std::cout << "Got 'input' at pos " << i << std::endl;
      }
      else if (i == 1)
      {
        d_passphrase = std::move(option);
        //std::cout << "Got 'passphrase' at pos " << i << std::endl;
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
