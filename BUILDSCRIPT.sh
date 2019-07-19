#!/bin/sh
echo -E "BUILDING (1/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (2/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (3/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (4/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "endframe/o/statics.o" "endframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (5/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (6/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (7/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"filedecryptor/o/getattachment.o\" \"filedecryptor/getattachment.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "filedecryptor/o/getattachment.o" "filedecryptor/getattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (8/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (9/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (10/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (11/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (12/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (13/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (14/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (15/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (16/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (17/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"signalbackup/o/signalbackup.o\" \"signalbackup/signalbackup.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "signalbackup/o/signalbackup.o" "signalbackup/signalbackup.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (18/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"o/main.o\" \"main.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "o/main.o" "main.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (19/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "backupframe/o/init.o" "backupframe/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (20/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"sqlitedb/o/sqlitedb.o\" \"sqlitedb/sqlitedb.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "sqlitedb/o/sqlitedb.o" "sqlitedb/sqlitedb.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (21/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (22/22): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -o "headerframe/o/statics.o" "headerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "LINKING: g++ -Wall -Wextra -Wl,-z,now -O3 -s \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"endframe/o/statics.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/getattachment.o\" \"filedecryptor/o/filedecryptor.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/signalbackup.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlitedb/o/sqlitedb.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" -lcryptopp -lsqlite3 -o \"signalbackup-tools\""
g++ -Wall -Wextra -Wl,-z,now -O3 -s "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "endframe/o/statics.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/getattachment.o" "filedecryptor/o/filedecryptor.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/signalbackup.o" "o/main.o" "backupframe/o/init.o" "sqlitedb/o/sqlitedb.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" -lcryptopp -lsqlite3 -o "signalbackup-tools"
