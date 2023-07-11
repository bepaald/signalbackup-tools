/*
  Copyright (C) 2022-2023  Selwin van Dijk

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

void Arg::usage() const
{
  std::cout << R"*(
Usage: )*" + d_progname + R"*( <INPUT> [<PASSPHRASE>] [OPTIONS]

<INPUT> must be either a regular file, a backup file as exported by the Signal Android app, or a
directory containing an unpacked backup as createed by this program. In case the input <INPUT> is
a regular file, a <PASSPHRASE> is required. If it is omitted from the command line, a prompt is
presented during runtime.
Be sure to read the README at https://github.com/bepaald/signalbackup-tools/ for more detailed
instructions for the core functions and examples.
Note: this program never modifies the input, if you wish to alter the backup in any way and save your
changes, you must provide one of the output options.

 = COMMON OPTIONS =
-i, --input <INPUT>            If for whatever reason you do not wish to pass the input as the first
                               argument, you can use this option anywhere in the list of arguments
-p, --passphrase <PASSPHRASE>  If for whatever reason you do not wish to pass the input as the second
                               argument, you can use this option anywhere in the list of arguments. If
                               this option is omitted, but <INPUT> is a regular file, a prompt is
                               presented to enter the passphrase at runtime (also see `--interactive').
--no-showprogress              Do not output the progress percentage. Especially useful when redirecting
                               output to file.
--listthreads                  List the threads in the database with their `_id' numbers. These id's are
                               required input for various other options.
-h, --help                     Show this help message
--interactive                  Prompt for all passphrases
--runsqlquery <QUERY>          Run <QUERY> against the backup's internal SQL database.
--runprettysqlquery <QUERY>    As above, but try show output in a pretty table. If the output is not too
                               large for your terminal, this is often much more readable.

 = OUTPUT OPTIONS =
-o, --output <OUTPUT>                    Either a file or a directory. When output is a file, this will
                                         be a normal backup file, compatible with the Signal Android app.
                                         When output is a directory, the backup's separate parts (frames,
                                         SQL database and media) are written to that directory
                                         unencrypted. This directory can later be used as <INPUT> to
                                         create a working backup file.
-op, --opassphrase <PASSPHRASE>          When output is a file, this will be the backups passphrase. May
                                         be omitted (in which case the <INPUT> passphrase is used.
   --onlydb                              Optional modifier for `--output', when <OUTPUT> is a directory.
                                         This causes only the SQlite database to be written to disk.
--dumpmedia <DIRECTORY>                  Save all media attachments to DIRECTORY. An attempt is made to
                                         give each attachment a correct name and timestamp as well as to
                                         place the attachments in subfolders for each conversation.
   --limittothreads <LIST_OF_THREADS>    Optional modifier for `--dumpmedia'. Only save the attachments
                                         from the listed threads. List format same as `--croptothreads'
--dumpavatars <DIRECTORY>                Save all avatars to DIRECTORY.
   --limitcontacts <CONTACTS>            Optional modifier for `--dumpavatars'. Only the avatars of
                                         listed contacts are saved. CONTACTS is a list "Name 1,Name
                                         2(,...)", where each name is exactly as it appears in Signal's
                                         conversation overview or from this program's `--listhtreads'
                                         output.
--exportxml <FILE>                       Export the messages from the internal sms table to xml file FILE.)*"
//   --includemms
//   --includeattachmentdata
//   --setselfid
R"*(
--exporthtml <DIRECTORY>                 Export the messages to HTML files. Each conversation will be
                                         placed in a separate subdirectory.
   --limittothreads <LIST_OF_THREADS>    Optional modifier for `--exporthtml'. Only export the listed
                                         threads. List format same as `--croptothreads'
   --limittothreads <LIST_OF_DATES>      Optional modifier for `--exporthtml'. Only export messages within
                                         the ranges defined by LIST_OF_DATES. List format is the same as
                                         `--croptodates'.
   --append                              Optional modifier for `--exporthtml'. Causes `--exporthtml' to
                                         not show an error when DIRECTORY is not empty, but also not
                                         overwrite existing media files. Still regenerates the HTML file.
   --split [N]                           Optional modifier for `--exporthtml'. Splits the generated HTML
                                         files to a maximum of N messages per page. By default, the pages
                                         are not split. When this option is given without a value for N,
                                         N is 1000.
   --light                               By default a dark theme is used for the rendered HTML. Add this
                                         option to output in a light theme instead.
   --themeswitching                      Optional modifier for `--exporthtml'. Adds a button to the HTML
                                         to switch the theme between light and dark. This adds a bit of
                                         javascript to the page, and sets a cookie when switching.
--exportcsv <MAP_OF_FILES>               Export the database to file of comma separated values. Argument:
                                         "tablename1=filename1,tablename2=filename2(,...)"
--exporthtml <DIRECTORY>                 Export the messages to plain text file. Attachments are omitted.
                                         This option also supports the `--limittothreads' and
                                         `--limittodates' modifiers as mentioned above.
--overwrite                              Optional modifier for all output operations. Overwrite output
                                         files if they exist. When <OUTPUT> is a directory this will
                                         delete ALL directory contents.

 = EDITING OPTIONS =
--croptothreads <LIST_OF_THREADS>        Crop database to list of threads. The list supports comma
                                         separated numbers or ranges (for example: "1,3,4-8,13") or the
                                         special keyword `ALL'. Threads are specified by their id (see:
                                         `--listthreads').
--croptodates <LIST_OF_DATES>            Crop database to certain time periods. The list of dates is
                                         structured as `begindate1,enddate1(,begindate2,enddate2,...)',
                                         where a date is either "YYYY-MM-DD hh:mm:ss" or a date in
                                         milliseconds since epoch
--importthreads <LIST_OF_THREADS>        Import LIST_OF_THREADS into <INPUT> database, the list format is
                                         the same as `--croptothreads'. This operation requires the
                                         `--source' option to be passed as well.
   -s, --source <SOURCE>                 Required modifier for `--importthreads'. The source backup from
                                         which to import threads (see `--importthreads'). The input can be
                                         a file or directory. When it is a file, a passphrase is required
   -sp, --sourcepassphrase <PASSPHRASE>  The 30 digit passphrase for the backup file specified by `--source'.
--importfromdesktop [DIR1][DIR2]         Import messages from Signal Desktop. If the program fails to find
                                         your Signal-Desktop installation or it is in a non-standard location,
                                         the optional [DIR1] and [DIR2] can be provided. See the README for
                                         more information.
   --ignorewal                           Optional modifier for `--importfromdesktop'. Ignores an existing WAL
                                         file when opening the Signal Desktop database.
   --limittodates <LIST_OF_DATES>        Optional modifier for `--importfromdesktop'. Limit the messages
                                         imported to the specified date ranges. The format of the list of
                                         list of dates is the same as `--croptodates'.
   --autolimitdates                      Optional modifier for `--importfromdesktop'. Automatically limit
                                         the import of messages to those older than the first message in the
                                         INPUT backup file.
--deleteattachments                      Delete attachments from backup file.
   --onlyinthreads <LIST_OF_THREADS>     Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. Only deal with attachments within these
                                         threads. For list format see `--croptothreads'.
   --onlyolderthan <DATE>                Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. Only deal with attachments for messages
                                         older than DATE. Date format is same as with `--croptodates'.
   --onlynewerthan <DATE>                Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. Only deal with attachments for messages
                                         newer than DATE. Date format is same as with `--croptodates'.
   --onlylargerthan <SIZE>               Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. Delete attachments only if larger than
                                         SIZE bytes.
   --onlytype <FILETYPE>                 Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. Delete attachments only if matching mime
                                         type FILETYPE. The type is need not be complete (ie. `video/m'
                                         will match both `video/mp4' and `video/mpeg'
   --appendbody <STRING>                 Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. For each message whose attachment is
                                         deleted/replaced, append STRING to the message body.
   --prependbody <STRING>                Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. For each message whose attachment is
                                         deleted/replaced, prepend STRING to the message body.
--replaceattachments [LIST]              Replace attachments of type with placeholder image. Argument:
                                         "default=filename,mimetype1=filename1,mimetype2=filename2,.."

 = VARIOUS =
The following options are also supported in this program, and listed here for completeness. Some of them
are mostly useful for the developer, others were custom functions for specific problems that are not
expected to be very useful to other people. Most of these functions are poorly tested (if at all) and
possibly outdated. Some will probably eventually be renamed and more thoroughly documented others will
be removed.
--showdbinfo                                  Prints list of all tables and columns in the backups Sqlite
                                              database.
--scramble                                    Poorly censors backups, replacing all characters with 'x'.
                                              Useful to make screenshots.
--scanmissingattachments                      If you see "warning attachment data not found" messages,
                                              feel free to use this option and provide the output to the
                                              developer.
--hiperfall <THREAD_ID>                       Switch sender and recipient. See
                                              https://github.com/bepaald/signalbackup-tools/issues/44
   --setselfid <PHONENUMBER>                  Optional modifier for `--hiperfall' and `--importwachat'
--importwachat <FILE>                         Import whatsapp data. See
                                              https://github.com/bepaald/signalbackup-tools/issues/19
   --setwatimefmt <TIMEFMT>                   Required modifier for `--importwachat'.
--dumpdesktopdb <DIR1><DIR2>                  Decrypts the Signal Desktop database and saves it to the
                                              file `desktop.db'. PATH is the base path of Signal
                                              Desktop's data (eg `~/.config/Signal' on Linux. The program
                                              stupidly still needs an <INPUT> and <PASSPHRASE> parameter
                                              to actually run.
--assumebadframesizeonbadmac                  Used to fix a specific (long fixed) bug in Signal. See
                                              https://github.com/signalapp/Signal-Android/issues/9154
--editattachmentsize                          Modifier for `--assumebadframesizeonbadmac'
--removedoubles                               Attempt to remove doubled messages in the database. May be
                                              useful when importing partially overlapping backup files.
--reordersmsmmsids                            Makes sure sms and mms entries are sorted chronologically
                                              in the database. This option exists for backups edited by
                                              this program before this was done automatically (as it is
                                              now)
--stoponerror                                 Do not try to recover automatically when encountering bad
                                              data.
-v, --verbose                                 Makes the output even more verbose than it is already.
--mergerecipients <OLDNUMBER,NEWNUMBER>       Can be used to change a contacts number (for example when
                                              they get a new phone). Messages from OLDNUMBER are changed
                                              so they appear as coming from NEWNUMBER, and the threads
                                              are merged.
   --editgroupmembers                         Optional modifier for `--mergerecipients'. Also changes
                                              groups members from OLDNUMBER to NEWNUMBER. Might not
                                              always be wanted if the NEWNUMBER was already added to the
                                              group.
--mergegroups <OLD_GROUP_ID,NEW_GROUPD_ID>    Merge all messages from OLD_GROUP into NEW_GROUP.
--sleepyh34d <FILE[,PASSWD]>                  Try to import messages from a truncated backup file into a
                                              complete one. See
                                              https://github.com/bepaald/signalbackup-tools/issues/32
--hhenkel <STRING>                            See https://github.com/bepaald/signalbackup-tools/issues/17
--strugee <N>                                 Tries to verify a specific type of corruption of the backup
                                              file. N is a file offset in bytes. See
                                              https://github.com/bepaald/signalbackup-tools/issues/37
--strugee2                                    Shows the tables present in a truncated backup. See
                                              https://github.com/bepaald/signalbackup-tools/issues/37
--strugee3 <N>                                Tries to verify a specific type of corruption of the backup
                                              file. N is a file offset in bytes. See
                                              https://github.com/bepaald/signalbackup-tools/issues/37
--ashmorgan                                   See https://github.com/bepaald/signalbackup-tools/issues/40
)*";
}


// checkdbintegrity
