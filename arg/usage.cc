/*
  Copyright (C) 2022-2025  Selwin van Dijk

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

#include <iostream>

void Arg::usage() const
{
  std::cout << R"*(
Usage: )*" + d_progname + R"*( <INPUT> [<PASSPHRASE>] [OPTIONS]

<INPUT> must be either a regular file, a backup file as exported by the Signal Android app, or a
directory containing an unpacked backup as created by this program. In case the input <INPUT> is
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
-h, --help                     Show this help message
-l, --logfile <LOG>            Write programs output to file <LOG>. If the output file exists, it will
                               be overwritten without warning.
--interactive                  Prompt for all passphrases
--runsqlquery <QUERY>          Run <QUERY> against the backup's internal SQL database.
--runprettysqlquery <QUERY>    As above, but try show output in a pretty table. If the output is not too
                               large for your terminal, this is often much more readable.
--no-truncate                  By default, `--runprettysqlquery' truncates table columns to fit the
                               terminal. With this option, this can be disabled, useful when redirecting
                               output to file (or using the `--logfile' option.
--listthreads                  List the threads in the database with their `_id' numbers. These id's are
                               required input for various other options.
--listrecipients               List all recipients in the database with their `_id'. These id's are
                               required input for various other options.
--setselfid <PHONENUMBER>      Various options need to know which recipient in the backup is 'self': the
                               originator of the backup. These functions generally scan the backup to
                               automatically determine the correct recipient. If this fails, this option
                               can be used to set the <PHONENUMBER> of the backups owner, in the format
                               it appears in the database, usually `+12345678901'.

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
                                         This causes only the SQLite database to be written to disk.
--dumpmedia <DIRECTORY>                  Save all media attachments to DIRECTORY. An attempt is made to
                                         give each attachment a correct name and timestamp as well as to
                                         place the attachments in sub-folders for each conversation.
   --limittothreads <THREADS_LIST>       Optional modifier for `--dumpmedia'. Only save the attachments
                                         from the listed threads. List format same as `--croptothreads'
   --limittothreadsbyname <THREADS_LIST> Optional modifier for `--dumpmedia'. Only save the attachments
                                         from the listed threads. List format same as
                                         `--croptothreadsbyname'
   --limittodates <LIST_OF_DATES>        Optional modifier for `--dumpmedia'. Only export messages within
                                         the ranges defined by LIST_OF_DATES. List format is the same as
                                         `--croptodates'.
   --excludequotes                       Optional modifier for `--dumpmedia'. Exclude quoted media from
                                         the media dump.
   --excludestickers                     Optional modifier for `--dumpmedia'. Exclude stickers from the
                                         media dump.
--dumpavatars <DIRECTORY>                Save all avatars to DIRECTORY.
   --limitcontacts <CONTACTS>            Optional modifier for `--dumpavatars'. Only the avatars of
                                         listed contacts are saved. CONTACTS is a list "Name 1,Name
                                         2(,...)", where each name is exactly as it appears in Signal's
                                         conversation overview or from this program's `--listrecipients'
                                         output.
--exportxml <FILE>                       Export the messages from the internal sms table to XML file FILE.)*"
//   --includemms
//   --includeattachmentdata
//   --setselfid
R"*(
--exporthtml <DIRECTORY>                 Export the messages to HTML files. Each conversation will be
                                         placed in a separate subdirectory.
   --limittothreads <THREADS_LIST>       Optional modifier for `--exporthtml'. Only export the listed
                                         threads. List format same as `--croptothreads'
   --limittothreadsbyname <THREADS_LIST> Optional modifier for `--exporthtml'. Only save the attachments
                                         from the listed threads. List format same as
                                         `--croptothreadsbyname'
   --limittodates <LIST_OF_DATES>        Optional modifier for `--exporthtml'. Only export messages within
                                         the ranges defined by LIST_OF_DATES. List format is the same as
                                         `--croptodates'.
   --migratedb                           Optional modifier for `--exporthtml'. Some older databases require
                                         this option to (attempt) to update the database format to a newer
                                         supported format. The program will tell you when you need to add
                                         this option.
   --append                              Optional modifier for `--exporthtml'. Causes `--exporthtml' to
                                         not show an error when DIRECTORY is not empty, but also not
                                         overwrite existing media files. Still regenerates and overwrites
                                         existing HTML files.
   --split [N]                           Optional modifier for `--exporthtml'. Splits the generated HTML
                                         files to a maximum of N messages per page. By default, the pages
                                         are not split. When this option is given without a value for N,
                                         N is 1000. Can not be combined with `--split-by`.
   --split-by [PERIOD]                   Optional modifier for `--exporthtml'. Splits the generated HTML
                                         files by calendar PERIOD. Supported values for PERIOD are 'year',
                                         'month', 'week', and 'day'. Can not be combined with `--split`.
   --light                               By default a dark theme is used for the rendered HTML. Add this
                                         option to output in a light theme instead.
   --includereceipts                     Optional modifier for `--exporthtml'. Adds available info from
                                         message receipts to the HTML page. Note, this potentionally slows
                                         down page loading for large conversations significantly.
   --originalfilenames                   Optional modifier for `--exporthtml'. Use the original filenames
                                         for attached media when available. This option can not be used
                                         together with `--append'.
   --compactfilenames                    Use (very) short filenames for the generated HTML pages. May be
                                         of use when running into maximum path length limitations on
                                         Windows.
   --htmlignoremediatypes [MIME_LIST]    Treat message attachments with mimetype in `MIME_LIST' as
                                         non-media attachment. Useful for types not commonly supported by
                                         browsers, such as `video/3gpp'.
   --excludeexpiring                     Optional modifier for `--exporthtml'. Excludes all messages with
                                         an active expiration timer from being exported.
   --htmlfocusend                        Optional modifier for `--exporthtml'. Causes the conversations to
                                         open focused on the newest message instead of the oldest.
   --allhtmlpages                        Optional modifier for `--exporthtml'. Convenience option that
                                         enables all the modifying options for `--exporthtml' listed below.
   --themeswitching                      Optional modifier for `--exporthtml'. Adds a button to the HTML
                                         to switch the theme between light and dark. This adds a bit of
                                         JavaScript to the page, and sets a cookie when switching.
   --searchpage                          Optional modifier for `--exporthtml'. Generates a page from where
                                         conversations can be searched. This adds JavaScript to the page.
                                         Also, a search index is generated to facilitate searching.
   --includecalllog                      Optional modifier for `--exporthtml'. Generate a call log-page.
   --stickerpacks                        Optional modifier for `--exporthtml'. Generate an overview of
                                         installed and known stickerpacks.
   --includeblockedlist                  Optional modifier for `--exporthtml'. Generate an overview of
                                         blocked contacts in the database.
   --includesettings                     Optional modifier for `--exporthtml'. Generate a page showing
                                         settings found in the backup file.
   --includefullcontactlist              Optional modifier for `--exporthtml'. Generate a page showing all
                                         contacts present in the backups database. These include hidden
                                         and blocked contacts, but also system contacts who might not use
                                         Signal at all.
   --addexportdetails                    Optional modifier for `--exporthtml'. Adds some metadata about this
                                         tools and the backup to the pages when printing.
   --linkify                             Optional modifier for `--exporthtml'. Attempts to turn URLs in
                                         messages actual clickable links. This option is enabled by deault,
                                         and can be disabled by `--no-linkify'.
   --chatfolders                         Optional modifier for `--exporthtml'. Exports chat folders. This
                                         option may interact poorly with the `--limitto[xxx]' options.
--exporttxt <DIRECTORY>                  Export the messages to plain text file. Attachments are omitted.
                                         This option also supports the `--limittothreads',
                                         `--limittothreadbybame', `--limittodates', and `--migratedb'
                                         modifiers as mentioned above.
--exportcsv <MAP_OF_FILES>               Export the database to file of comma separated values. Argument:
                                         "tablename1=filename1,tablename2=filename2(,...)"
--overwrite                              Optional modifier for all output operations. Overwrite output
                                         files if they exist. When <OUTPUT> is a directory this will
                                         delete ALL directory contents.
)*"
R"*(
 = EDITING OPTIONS =
--croptothreads <THREADS_LIST>           Crop database to list of threads. The list supports comma
                                         separated numbers or ranges (for example: "1,3,4-8,13") or the
                                         special keyword `ALL'. Threads are specified by their id (see:
                                         `--listthreads').
--croptothreadsbyname <THREADS_LIST>     Crop database to list of threads. The list is a comma separated
                                         list of conversation names (for example: "Alice","Bobby C.")
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
--importfromdesktop                      Import messages from Signal Desktop. See the README for more
                                         information.
   --importdesktopcontacts               Optional modifier for `--importfromdesktop`. Normally, threads are
                                         only imported when they can be matched to existing recipients in
                                         the Android backup. This option allows to create recipients from
                                         the desktop data.
   --desktopdir <DIR>                    Optional modifier for `--importfromdesktop` and `--dumpdesktopdb`.
                                         If the program fails to find your Signal-Desktop installation or it
                                         is not in a standard location, <DIR> can be provided. See the README
                                         for more information about default locations.
   --ignorewal                           Optional modifier for `--importfromdesktop' and `--dumpdesktopdb`.
                                         Ignores an existing WAL file when opening Signal Desktop database.
   --limittodates <LIST_OF_DATES>        Optional modifier for `--importfromdesktop'. Limit the messages
                                         imported to the specified date ranges. The format of the list of
                                         list of dates is the same as `--croptodates'.
   --autolimitdates                      Optional modifier for `--importfromdesktop'. Automatically limit
                                         the import of messages to those older than the first and newer
                                         than the last message in the INPUT backup file.
   --desktopkey <HEXKEY>                 Optional modifier for `--importfromdesktop` and `--dumpdesktopdb`.
                                         Provide the decrypted SQLCipher key for decrypting the desktop
                                         database (see README).
   --generatemissingstoragekeys          Optional modifier for `--importfromdesktop`. Generate random
                                         storageIds, if they are missing from the database. Please see
                                         github issue #352 for details before using this option.
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
                                         type FILETYPE. The FILETYPE does not need to be complete
                                         (i.e. `video/m' will match both `video/mp4' and `video/mpeg').
   --appendbody <STRING>                 Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. For each message whose attachment is
                                         deleted/replaced, append STRING to the message body.
   --prependbody <STRING>                Optional modifier for `--deleteattachments' and
                                         `--replaceattachments'. For each message whose attachment is
                                         deleted/replaced, prepend STRING to the message body.
--replaceattachments [LIST]              Replace attachments of type with placeholder image. Argument:
                                         "default=filename,mimetype1=filename1,mimetype2=filename2,.."
--importtelegram <JSONFILE>              Import messages from a JSON file as exported by Telegram.
   --jsonshowcontactmap <JSONFILE>       Show the mapping of contacts found in the JSON file to those
                                         in the Android backup.
   --mapjsoncontacts <Name1=id1,...>     Optional modifier for `--importtelegram' and `--jsonshowcontactmap'.
                                         Maps contacts found in the JSON file to those in the input backup
                                         file.
   --listjsonchats <JSONFILE>            Lists the chats found in JSON file.
   --selectjsonchats <LIST_OF_IDX>       Optional modifier for `--importtelegram'. Only import the given
                                         chats into the backup file. The indices are obtained from
                                         `--listjsonchats'.
   --jsonprependforward                  Optional modifier for `--importtelegram'. Causes forwarded messages
                                         in the JSON file to be prepended with the text "Forwarded from
                                         [name]:".
   --preventjsonmapping <Name1,...>      Optional modifier for `--importtelegram'. Prevents the automatic
                                         mapping of the listed JSON contacts to Signal contacts.
   --jsonmarkdelivered                   Optional modifier for `--importtelegram'. Mark imported messages
                                         as delivered.
   --jsonmarkread                        Optional modifier for `--importtelegram'. Mark imported messages
                                         as read."
)*"
R"*(
 = VARIOUS =
The following options are also supported in this program, and listed here for completeness. Some of them
are mostly useful for the developer, others were custom functions for specific problems that are not
expected to be very useful to other people. Most of these functions are poorly tested (if at all) and
possibly outdated. Some will probably eventually be renamed and more thoroughly documented others will
be removed.
--showdbinfo                               Prints a list of all tables and columns in the backups
                                           SQLite database.
--showdesktopkey                           Show the (hex) SQLCipher key used for the desktop database.
--dumpdesktopdb <OUTPUT>                   Decrypt the Signal Desktop database and saves it to <OUTPUT>.
--scramble                                 Poorly censors backups, replacing all characters with 'x'.
                                           Useful to make screenshots.
--scanmissingattachments                   If you see "warning attachment data not found" messages,
                                           feel free to use this option and provide the output to the
                                           developer.)*";
// --hiperfall <THREAD_ID>                       Switch sender and recipient. See
//                                               https://github.com/bepaald/signalbackup-tools/issues/44
//    --setselfid <PHONENUMBER>                  Optional modifier for `--hiperfall' and `--importwachat'
// --importwachat <FILE>                         Import whatsapp data. See
//                                               https://github.com/bepaald/signalbackup-tools/issues/19
//    --setwatimefmt <TIMEFMT>                   Required modifier for `--importwachat'.
// --dumpdesktopdb <DIR1><DIR2>                  Decrypts the Signal Desktop database and saves it to the
//                                               file `desktop.db'. PATH is the base path of Signal
//                                               Desktop's data (eg `~/.config/Signal' on Linux. The program
//                                               stupidly still needs an <INPUT> and <PASSPHRASE> parameter
//                                               to actually run.
std::cout << R"*(
--assumebadframesizeonbadmac               Used to fix a specific (long fixed) bug in Signal. See
                                           https://github.com/signalapp/Signal-Android/issues/9154
                                           This option and the modifier below should be considered
                                           deprecated and are disabled for newer backups. Contact the
                                           developer if you believe you need this on a newer backup file.
--editattachmentsize                       Modifier for `--assumebadframesizeonbadmac'
--removedoubles [N]                        Attempt to remove doubled messages from the database. May be
                                           useful when importing partially overlapping backup files.
                                           Optional N: time in milliseconds for messages to be
                                           considered potential duplicates (default 0).
--reordersmsmmsids                         Makes sure sms and mms entries are sorted chronologically
                                           in the database. This option exists for backups edited by
                                           this program before this was done automatically (as it is
                                           now)
--stoponerror                              Do not try to recover automatically when encountering bad
                                           data while reading the input file.
-v, --verbose                              Makes the output even more verbose than it already is.
--mergerecipients <OLDNUMBER,NEWNUMBER>    Can be used to change a contacts number (for example when
                                           they get a new phone). Messages from OLDNUMBER are changed
                                           so they appear as coming from NEWNUMBER, and the threads
                                           are merged.)*";
//--mergegroups <OLD_GROUP_ID,NEW_GROUPD_ID>    Merge all messages from OLD_GROUP into NEW_GROUP.
std::cout << R"*(
--migrate214to215                          Migrate a v214 database to v215. Changes in the database
                                           prevent v214 and v215 from being compatible for merging. This
                                           function attempts to migrate the older database so it can be
                                           used as a source for `--importthreads'. See also
                                           https://github.com/bepaald/signalbackup-tools/issues/184
--fulldecode                               Ensures all attachments are immediately decrypted when opening
                                           a backup file. Normally this is only done when the
                                           attachment data is actually needed.
--checkdbintegrity                         Does a full integrity check on the SQLite database in the
                                           backup file.
--autofixfkc                               Attempts to automatically fix any foreign key constraint
                                           violations in the database (as reported by `--checkdbintegrity')
                                           This will delete data from the database. To see some details
                                           of what is deleted, run with `-v/--verbose'.
)*";

// --editgroupmembers                         Optional modifier for `--mergerecipients'. Also changes
//                                            groups members from OLDNUMBER to NEWNUMBER. Might not
//                                            always be wanted if the NEWNUMBER was already added to the
//                                            group.
//--sleepyh34d <FILE[,PASSWD]>                  Try to import messages from a truncated backup file into a
//                                              complete one. See
//                                              https://github.com/bepaald/signalbackup-tools/issues/32
//std::cout << R"*(
//--hhenkel <STRING>                            See https://github.com/bepaald/signalbackup-tools/issues/17
//)*";

}
