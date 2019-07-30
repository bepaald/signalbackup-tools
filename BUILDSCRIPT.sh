#!/bin/sh
echo -E "BUILDING (1/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (2/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (3/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (4/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "endframe/o/statics.o" "endframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (5/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (6/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (7/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/getattachment.o\" \"filedecryptor/getattachment.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/getattachment.o" "filedecryptor/getattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (8/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (9/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (10/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (11/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (12/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (13/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (14/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (15/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (16/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (17/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/signalbackup.o\" \"signalbackup/signalbackup.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/signalbackup.o" "signalbackup/signalbackup.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (18/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"o/main.o\" \"main.cc\""
if [ ! -d "o" ] ; then mkdir "o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "o/main.o" "main.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (19/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "backupframe/o/init.o" "backupframe/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (20/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/sqlitedb.o\" \"sqlitedb/sqlitedb.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/sqlitedb.o" "sqlitedb/sqlitedb.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (21/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (22/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "headerframe/o/statics.o" "headerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "BUILDING (23/23): g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
g++ -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi
echo -E "LINKING: g++ -Wall -Wextra -Wl,-z,now -O3 -s -flto=8 \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"endframe/o/statics.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/getattachment.o\" \"filedecryptor/o/filedecryptor.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/signalbackup.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlitedb/o/sqlitedb.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcryptopp -lsqlite3 -o \"signalbackup-tools\""
g++ -Wall -Wextra -Wl,-z,now -O3 -s -flto=8 "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "endframe/o/statics.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/getattachment.o" "filedecryptor/o/filedecryptor.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/signalbackup.o" "o/main.o" "backupframe/o/init.o" "sqlitedb/o/sqlitedb.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcryptopp -lsqlite3 -o "signalbackup-tools"
