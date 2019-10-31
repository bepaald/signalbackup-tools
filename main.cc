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

#include "arg/arg.h"

#if __has_include("autoversion.h")
#include "autoversion.h"
#endif

int main(int argc, char *argv[])
{
#ifdef VERSIONDATE
  std::cout << "signalbackup-tools source version " << VERSIONDATE << std::endl;
#endif

  Arg arg(argc, argv);
  if (!arg.ok())
  {
    std::cout << "Error parsing arguments" << std::endl;
    return 1;
  }

  /*
    This is just temporary to investigate one specific issue for one specific user.
    Do not use this option.
    It will disappear soon enough without any notification
  */
  if (arg.elbrutalo())
  {
    SignalBackup sb(arg.input(), arg.password(), SignalBackup::LOWMEM);
    sb.runSimpleQuery("SELECT COUNT(*) AS num_sms, MIN(date), MAX(date) FROM sms");
    sb.runSimpleQuery("SELECT COUNT(*) AS doubles FROM (SELECT DISTINCT t1.* FROM sms AS t1 INNER JOIN sms AS t2 ON t1.date = t2.date AND t1.body = t2.body AND t1.thread_id = t2.thread_id AND t1.address = t2.address AND t1.date_sent = t2.date_sent AND t1._id <> t2._id)");
    sb.runSimpleQuery("SELECT COUNT(*) AS num_mms, MIN(date), MAX(date) FROM mms");
    sb.runSimpleQuery("SELECT COUNT(*) AS doubles FROM (SELECT DISTINCT t1.* FROM mms AS t1 INNER JOIN mms AS t2 ON t1.date = t2.date AND t1.body = t2.body AND t1.thread_id = t2.thread_id AND t1.address = t2.address AND t1.date_received = t2.date_received AND t1._id <> t2._id) AS doubles");
    sb.runSimpleQuery("SELECT COUNT(*) AS num_thread FROM thread");


    //sb.runSimpleQuery("SELECT sms.body AS union_body, sms._id AS [sms._id], '' AS [mms._id] "
    //                  "FROM 'sms' WHERE (sms.type & 0x10000 IS NOT 0"
    //                  " OR sms.type & 0x20000 IS NOT 0) UNION "
    //                  "SELECT mms.body AS union_body, '' AS [sms._id], mms._id AS [mms._id] "
    //                  "FROM mms WHERE (mms.msg_box & 0x10000 IS NOT 0"
    //                  " OR mms.msg_box & 0x20000 IS NOT 0)");

    return 0;
  }

  // open input
  std::unique_ptr<SignalBackup> sb;
  if (arg.password().empty())
    sb.reset(new SignalBackup(arg.input()));
  else
    sb.reset(new SignalBackup(arg.input(), arg.password(), (!arg.source().empty() || arg.listthreads()) ? SignalBackup::LOWMEM : false));

  //sb->exportXml("test.xml");

  if (arg.listthreads())
    sb->listThreads();

  if (arg.generatefromtruncated())
  {
    std::cout << "fillthread" << std::endl;
    sb->fillThreadTableFromMessages();
    sb->addEndFrame();
  }
  else if (!arg.source().empty())
  {
    for (uint i = 0; i < arg.importthreads().size(); ++i)
    {
      // read the database
      std::cout << std::endl << "Importing thread " << arg.importthreads()[i] << " from source file: " << arg.source() << std::endl;

      std::unique_ptr<SignalBackup> source;
      if (arg.sourcepassword().empty())
        source.reset(new SignalBackup(arg.source()));
      else
        source.reset(new SignalBackup(arg.source(), arg.sourcepassword(), SignalBackup::LOWMEM));

      sb->importThread(source.get(), arg.importthreads()[i]);
    }
  }

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

  // export output
  if (!arg.output().empty())
  {
    if (!arg.opassword().empty())
      sb->exportBackup(arg.output(), arg.opassword(), SignalBackup::DROPATTACHMENTDATA);
    else
      sb->exportBackup(arg.output());
  }



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
