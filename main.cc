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

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>

#include "backupframe/backupframe.h"
#include "headerframe/headerframe.h"
#include "sqlstatementframe/sqlstatementframe.h"
#include "filedecryptor/filedecryptor.h"
#include "fileencryptor/fileencryptor.h"
#include "attachmentframe/attachmentframe.h"

#include "sqlitedb/sqlitedb.h"

#include "signalbackup/signalbackup.h"

std::string signalfile;
std::string passphrase;
std::string newpassphrase;
std::string outfilename;
bool dumpattachments = true;
bool dontbreak = true;
std::string rawoutdir;
std::string rawindir;

bool parseArgs(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " [signal backup file] [PASSPHRASE 30 digits] [optional new output file] [optional new passphrase]" << std::endl;
    std::cout << " - If no new output file name is given any info on bad frames is printed to stdout and the attachment data is dumped to file. Otherwise, a new backup file is created after dropping the bad attachment frames and removing the references to them from the sqlite database. If no new passphrase is given, the old one will be used." << std::endl;
    return false;
  }

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg.substr(0, 2) != "--") // not a switch
    {
      if (rawindir.empty() && signalfile.empty())
        signalfile = arg;
      else if (rawindir.empty() && passphrase.empty())
        passphrase = arg;
      else if (outfilename.empty())
        outfilename = arg;
      else if (newpassphrase.empty())
        newpassphrase = arg;
    }
    else if (arg == "--dumptodir")
    {
      if (i == argc - 1)
      {
        std::cout << "Missing parameter for arg" << std::endl;
        return false;
      }
      rawoutdir = argv[++i];
    }
    else if (arg == "--readrawfromdir")
    {
      if (i == argc - 1)
      {
        std::cout << "Missing parameter for arg" << std::endl;
        return false;
      }
      rawindir = argv[++i];
    }
    else
    {
      std::cout << "Ignoring unknown option: '" << arg << "'" << std::endl;
    }
  }

  if (newpassphrase.empty())
    newpassphrase = passphrase;

  return (!signalfile.empty() && !passphrase.empty()) || !rawindir.empty();
}

int main(int argc, char *argv[])
{
  //return 0;

  if (!parseArgs(argc, argv))
  {
    std::cout << "Error parsing arguments" << std::endl;
    return 1;
  }

  std::unique_ptr<SignalBackup> sb;

  if (!signalfile.empty())
    sb.reset(new SignalBackup(signalfile, passphrase));
  else
    sb.reset(new SignalBackup(rawindir));

  //sb->listThreads();
  //SignalBackup source2("../../PHONE/merge/signal-2019-08-17-09-04-39.backup", "871668681636341580140408145443");
  //sb->importThread(&source2, 2);

  if (!outfilename.empty())
  {
    if (sb->dropBadFrames())
      sb->exportBackup(outfilename, newpassphrase);
  }
  else if (!rawoutdir.empty())
    sb->exportBackup(rawoutdir);



  //sb->cropToThread({8, 10, 11});
  //sb->listThreads();


  // std::cout << "Starting export!" << std::endl;
  // sb.exportBackup("NEWFILE");
  // std::cout << "Finished" << std::endl;

  // // std::cout << "Starting export!" << std::endl;
  // // sb2.exportBackup("NEWFILE2");
  // // std::cout << "Finished" << std::endl;

  return 0;
}


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
