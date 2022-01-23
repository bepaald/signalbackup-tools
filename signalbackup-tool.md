# NAME

    signalbackup-tool - manipulate Signal backup files

**This man page is a work in progress and does contain errors. It is submitted as a basis for comment and future work.**

Signalbackup-tool can dump, edit, merge and filter the contents of Signal backup files. It can extract selected messages and attachments from an existing backup, and also create new Signal backup files based on previously extraced (and perhaps modified) Signal backup data.

# SYNOPSIS

    signalbackup-tools INPUT [password] [OPTIONS] --output OUTPUT [--opassword [PASSWORD]]

# Overview
                                                                                                                                
Signalback-tool operates in several basic modes:                                                                                                

* Message and media extraction, into another backup file or as individual files in a new directory
* Re-creating a Signal backup file from previously extracted data
* Repairing a Signal backup file
* Merge information from multiple backups into a single backup file
* Delete attachments from a backup file - to create a new smaller backup file and save space
* Extract data from a Signal backup file- performing direct SQL queries against the contents of the file.

# DESCRIPTION

## Message and Media Extraction

    signalbackups-tools INPUT [password] [SELECTORS] --output OUTPUT [--opassword newpassword]


Signalbackup-tool can extract SMS, MMS and media (attachments) such as images from a Signal backup file. It can selectively extract messages only, all media, or only avatars.  Selection arguments may be used to include or exclude specific messages or threads.

If OUTPUT exists and is a directory, each message and attachment will be extracted into an separate file in that directory. Otherwise a new Signal backup file will be created.


## Re-creating a Signal backup file

    signalbackups-tools backupdirectory --output [outputfile] (--opassword [newpassword])

Signalbackup-tool can create a new backup file by reading the contents of a directory that was previously created by the extraction process.

Files in the directory can be added, modified or deleted before being integrated into a new backup file.

## Backup file repair

Signalbackup-tool can be used to repair corrupt backup files. It will attempt to produce a valid Signal backup file by reading an existing backup file, correcting errors or deleting infomation as needed, to produce a new valid Signal backup file                                                         

## Merge multiple backup files

The tool can also merge multiple backup files into a single backups. Selection arguments can be used to determine which messages from each file will be retained in the new backup file.

## Delete attachments

Signalbackup-tool can be used to create a new backup file that does not contain attachments. This is useful for making a much smaller backup file that can then be placed back on the device, saving space.

(A feature to replace attachments with a common placeholder image instead of deleteing them is in development. Replacing attachment is not yet fully implemented, but will allow you to replace the attachments with a placeholder image instead of deleting them.)

## SQL queries

The program can run any sql queries on the database in the backup file and save the output. If you know the schema of the database and know what you're doing, feel free to run any query and save the output. Examples:


# ARGUMENTS

## Input and Output files

Signalbackup-tools can read from either Signal backup files, or a directory of files, depending on whether messages are being extracted, or a new backup file is being created.

At least one INPUT file or directory is required. It can be Signal backup file, or a directory of files. It can be specified using the positional argument, or using the **--source** option. Multiple input files (ex. for merging backups) requires use of **--source**

At least one OUTPUT file or directory is required.

For Signal backup files

* -s INPUT || --source INPUT <br>
source backup file, when merging, there can be TWO **( or more?)** source files

* -sp" || option == "--sourcepassword<br>
the password for the source backup file


* -o OUTPUT ||  --output  OUTPUT<br>
The output backup file or directory. If a directory, it must already exist.
* -op || --opassword PASSWORD<br>
Password for the new backup file. (Does not apply when creating directories.)

## Output formats

Data can be output in three main ways:

* a new file in Signal backup format;
* a new file in CSV or XML format;
* as a directory of individual files for each message and attachment.

If no file format argument is provided and the output is not a directopry, a Signal backup format file will be created.

* --exportcsv [filename]
    
    To export the tables to a file of comma separated values (CSV), use --exportcsv [table1]=[filename1],[table2]=[filename2],.... To get all messages from the database, only the 'sms' and 'mms' tables need to be exported.

* --exportxml [filename]
    
     NOTE: Currently this will only export the messages from the sms table, NOT the mms table. This should be exactly the same data the official Signal app used to output when exporting a plaintext backup. All messages with an attachment, as well as all outgoing group messages are in the mms database, and thus are skipped. Exporting the mms table is being worked on. This option should be considered experimental and may have issues with GroupV2-features as of 2021/jan/01



## SELECTORS - Message, thread and media type selection
There are many options that can be used to select specific messages or threads for extraction or deletion. Many of these options use these selectors:

* [list-of-threads] - The list supports commas for single ids and hyphens for ranges, for example: --onlyinthreads 1,2,5-8,10. To obtain the number-id of threads use --listthreads.  ALL is allowed.

* [DATETIME] - a date in ISO format ("YYYY-MM-DD hh:mm:ss") or as a single number of milliseconds since epoch. For example, the following date/times are equivalent (in my time zone) and both crop the database to the messages between Sept. 18 2019 and Sept 18 2020: --croptodates "2019-09-18 00:00:00","2020-09-18 00:00:00" or --croptodates 1568761200000,1600383600000. **FIXME - timezone information or example in GMT**

### Thread and message selection arguments.

* --listthreads<br>
    lists all threads and their thread IDs - useful to get thread IDs to examine/dump only selected threads (see limit threads)
* --no-listthreads<br>
    **why is this needed?? Need a use case**

* --importthreads [list-of-threads]<br>
If not all threads should be imported from the source, a list of thread ids can be supplied (eg --importthreads 1,2,3,8-16,20). The thread ids can be determined from the output of --listthreads. ALL is allowed

* --onlyinthreads [list-of-threads]<br>
Only messages in the selected theads will be extracted or deleted.

* --onlyolderthan [DATETIME]/--onlynewerthan [DATETIME]<br>
Only threads older (or newer) than the supplied DATETIME will be processed.
 
* --onlylargerthan [size]<br>
Only messages larger than [size] will be processed. The size is specified in bytes.

* --onlytype [mime type]<br>
This argument can be repeated. Only selects attachments which match 'mime type\*' (note the asterisk)<br>
For example --onlytype image/j will match both 'image/jpg' and 'image/jpeg'. To delete **(select?)** all image type attachments, simply use '--onlytype image'.

* --croptodates<br>
This argument is an extension of --onlyolderthan and --onlynewerthan, allowing the selection of a range of dates in a single argument.<BR>For example, to crop a backup file to certain dates, run:

    signalbackup-tools INPUT [password] --croptodates begindate1,enddate2(,begindate2,enddate2(,...)) --output [OUTPUT] (--opassword [newpassword])

    The 'begindate' and 'enddate' must always appear in pairs and can be either in "YYYY-MM-DD hh:mm:ss" format or as a single number of milliseconds since epoch. For example, the following commands are equivalent (in my time zone) and both crop the database to the messages between Sept. 18 2019 and Sept 18 2020: --croptodates "2019-09-18 00:00:00","2020-09-18 00:00:00" or --croptodates 1568761200000,1600383600000.


### Media type selection options

In addition to message and thread selectors, there are options to select the type of attachments for processing.

* --dumpmedia<br>
    dump only media attachements and avatars

* --dumpavatars<br>
    dump only avatars
* --onlydb<br>
    Dump only SMS and MMS messages, skip exporting media such as attachments, avatars and stickers
* --no-onlydb
    **why is this needed?**

# Message Processing Controls
One the message, thread and media type selctions have been made, other arguments control how extracted data is processed.

* --deleteattachments<br>
Do not include attachments in the output file or directory.
* --no-deleteattachments **why is this needed?**


* --prependbody [string]/--appendbody [string]<br>
Often used to annotate a message when attachments have been deleted. Prepend or append the message body with the supplied string. If the message was otherwise empty, the body will equal the supplied string. Otherwise, it will be appended or prepended and a blank line will be inserted automatically. When adding this specifying options, only attachments which match all given options are deleted. Suggested use:

    --prependbody "(One or more media attachments for this message were deleted)".


### SQL queries

Signal backup files are a SQL database. Signalbackup-tool can run arbitrary SQL queries against the backup file and display the results.

* --runsqlquery
* --runprettysqlquery<br>
run a provided SQL query against the DB


### As-yet undocumented features


* FIXME - no info in README    --limittothreads  **how is this different from --croptothreads??**
* FIXME - no info in README    --generatefromtruncated
* FIXME - no info in README    --no-generatefromtruncated
* FIXME - no info in README    --croptothreads **how is this different from --limittothreads??((
* FIXME - no info in README    --mergerecipients
* FIXME - no info in README    --mergegroups
* FIXME - no info in README    --sleepyh34d
* FIXME - no info in README    --limitcontacts
* FIXME - no info in README    --assumebadframesizeonbadmac
* FIXME - no info in README    --no-assumebadframesizeonbadmac
* FIXME - no info in README    --editattachmentsize
* FIXME - no info in README    --dumpdesktopdb
* FIXME - no info in README    --hhenkel
* FIXME - no info in README    --importcsv
* FIXME - no info in README    --mapcsvfields
* FIXME - no info in README    --importwachat
* FIXME - no info in README    --setwatimefmt
* FIXME - no info in README    --setselfid
* FIXME - no info in README    --overwrite
* FIXME - no info in README    --no-overwrite
* FIXME - no info in README    --editgroupmembers
* FIXME - no info in README    --no-editgroupmembers
* FIXME - no info in README    --showprogress
* FIXME - no info in README    --no-showprogress
* FIXME - no info in README    --removedoubles
* FIXME - no info in README    --no-removedoubles
* FIXME - no info in README    --reordermmssmsids
* FIXME - no info in README    --no-reordermmssmsids
* FIXME - no info in README    --stoponbadmac
* FIXME - no info in README    --no-stoponbadmac
* FIXME - no info in README    -v" || option == "--verbose
* FIXME - no info in README    --no-verbose
* FIXME - no info in README    --strugee
* FIXME - no info in README    --strugee3
* FIXME - no info in README    --ashmorgan
* FIXME - no info in README    --no-ashmorgan
* FIXME - no info in README    --strugee2
* FIXME - no info in README    --no-strugee2
* FIXME - no info in README    --hiperfall
* FIXME - no info in README    --replaceattachments
    
# Examples

## Backup file merge

To merge two backups, the backups must be at compatible database versions. The database version can be found by running signalbackup-tools [input] [password] --listthreads. Either both backups need to have database version <= 27, both >= 33, or both in between 27 and 33. If needed, import the backups into Signal and export them again to get them updated and at equal versions. To import all threads from one database into another, run:

    signalbackup-tools [first_database] [password] --importthreads ALL --source [second_database] --sourcepassword [password] --output [output_file] (--opassword [output password])

Always use the backup file with the highest database version as 'first_database' and the older version as source. If not all threads should be imported from the source, a list of thread ids can be supplied (eg --importthreads 1,2,3,8-16,20). The thread ids can be determined from the output of --listthreads.

**If you use this option and read this line, I would really appreciate it if you let me know the results. Either send me a mail (basjetimmer at yahoo-dot-com) or feel free to just open an issue on the tracker for feedback.**

## Deleting Attachments

 Deleting attachments
To remove attachments from the database, while keeping the message bodies (for example to shrink the size of the backup) the option --deleteattachments can be used:

    signalbackup-tools [input] [password] --detelattachments --output [output] (--opassword [newpassword])

To further specify precisely which attachments are to be deleted, the following options can be added:

--onlyinthreads [list-of-threads]. The list supports commas for single ids and hyphens for ranges, for example: --onlyinthreads 1,2,5-8,10. To obtain the number-id of threads use --listthreads.

--onlyolderthan [date]/--onlynewerthan [date]. Where 'date' supports the same format as the --croptodates option (here).

--onlylargerthan [size]. The size is specified in bytes.

--onlytype [mime type]. This argument can be repeated. Only selects attachments which match 'mime type*' (note the asterisk). For example --onlytype image/j will match both 'image/jpg' and 'image/jpeg'. To delete all image type attachments, simply use --onlytype image.

--prependbody [string]/--appendbody [string]. 

Prepend or append the message body with the supplied string. If the message was otherwise empty, the body will equal the supplied string. Otherwise, it will be appended or prepended and a blank line will be inserted automatically. Suggested use: --prependbody "(One or more media attachments for this message were deleted)".
When adding this specifying options, only attachments which match all given options are deleted

**Replacing attachments - is not yet fully implemented, but will allow you to replace the attachments with a placeholder image instead of deleting them.**

## SQL query examples

The program can run any sql queries on the database in the backup file and save the output. If you know the schema of the database and know what you're doing, feel free to run any query and save the output. Examples:

### delete all sms and mms messages from one thread:
    signalbackup-tools [input] [password] --runsqlquery "DELETE * FROM sms WHERE thread_id = 1" --runsqlquery "DELETE * FROM mms WHERE thread_id = 1" --output [output] (--opassword [newpassword])

### list all messages in the sms database where the message body was 'Yes'
    signalbackup-tools [input] [password] --runprettysqlquery "SELECT _id,body,DATETIME(ROUND(date / 1000), 'unixepoch') AS isodate,date FROM sms WHERE body == 'yes' COLLATE NOCASE"


### delete all sms and mms messages from one thread:

    signalbackup-tools [input] [password] --runsqlquery "DELETE * FROM sms WHERE thread_id = 1" --runsqlquery "DELETE * FROM mms WHERE thread_id = 1" --output [output] (--opassword [newpassword])

### list all messages in the sms database where the message body was 'Yes'

    $./signalbackup-tools [input] [password] --runprettysqlquery "SELECT _id,body,DATETIME(ROUND(date / 1000), 'unixepoch') AS isodate,date FROM sms WHERE body == 'yes' COLLATE NOCASE"

    signalbackup-tools source version 20191219.175337

    IV: (hex:) 12 16 72 95 7a 00 68 44 7e cf 7d 20 26 f9 d3 7d (size: 16)
    SALT: (hex:) cc 03 85 02 61 97 eb 5b ed 3e 05 00 c4 a8 77 40 28 08 aa 9f e5 a8 00 74 b4 f8 56 aa 24 57 a9 5d (size: 32)
    BACKUPKEY: (hex:) 8f ff df 2b 9f 96 73 9a 63 95 0f ea 3f b1 e5 a4 87 12 19 ca 93 31 86 2a 60 3f 41 ef 6d a4 08 44 (size: 32)
    CIPHERKEY: (hex:) ce 53 c1 f2 92 4b e3 b8 e1 56 85 61 14 96 82 8b 83 7f 07 21 83 52 1a c2 3f 6b 16 83 3e 33 94 a3 (size: 32)
    MACKEY: (hex:) c2 77 af 1e 4b 05 db 62 52 57 af 8a d6 a4 d4 e9 6c 93 53 81 9a e7 6f 12 2c ce 13 8f b3 5e 8d 3a (size: 32)
    COUNTER: 2907636
    Reading backup file...
    FRAME 4852 (100.0%)... Read entire backup file...

    done!
    Executing query: SELECT _id,body,DATETIME(ROUND(date / 1000), 'unixepoch') AS isodate,date FROM sms WHERE body == 'yes' COLLATE NOCASE


    ------------------------------------------------------
    | _id   | body | isodate             | date          |
    ------------------------------------------------------
    | 3235  | Yes  | 2017-10-21 17:10:15 | 1508605815286 |
    | 9345  | Yes  | 2017-12-18 22:18:36 | 1513635516440 |
    | 17125 | Yes  | 2018-02-02 15:46:16 | 1517586376228 |
    | 21300 | Yes  | 2018-05-10 21:14:49 | 1525986889325 |
    | 26317 | Yes  | 2018-10-25 15:16:58 | 1540480618238 |
    | 32433 | Yes  | 2019-05-10 14:22:25 | 1557498145794 |
    ------------------------------------------------------

### To change a specific message

The command will read a Signal backup file, change a single message, and create a new Signal backup file with all messages from the original, with one message altered.

    $ ./signalbackup-tools [input] [password] --runsqlquery "UPDATE sms SET body = 'No' WHERE _id == 21300" --ouput [output]

If you also need to edit the attachments, dump the backup to directory first (as described above) and do whatever you want, but realize when editing the .bin file, it will usually require changes to also be made to the .sbf file and the sql database to end up with a valid database.

# AUTHOR

https://github.com/bepaald

# REPORTING BUGS

https://github.com/bepaald/signalbackup-tools/issues

# LICENSE

GPL-3.0

# SEE ALSO

https://github.com/bepaald/signalbackup-tools






