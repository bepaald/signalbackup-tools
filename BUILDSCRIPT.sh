#!/bin/sh

### This script is auto-generated

COMPILER=$(which g++)
if [ -z "$COMPILER" ] ; then echo "Failed to find g++ binary" && exit 1 ; fi
NUMCPU=1
EXTRAOPTIONS="$1"

echo -E "BUILDING (1/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (2/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (3/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (4/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (5/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (6/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (7/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (8/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (9/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (10/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (11/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (12/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (13/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (14/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (15/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (16/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (17/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (18/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (19/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (20/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (21/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (22/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (23/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (24/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (25/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (26/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (27/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (28/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (29/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (30/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (31/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (32/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (33/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (34/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (35/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (36/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (37/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (38/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (39/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (40/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (41/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (42/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (43/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (44/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (45/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (46/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (47/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
if [ ! -d "o" ] ; then mkdir "o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (48/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (49/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (50/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (51/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (52/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (53/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (54/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (55/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (56/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (57/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (58/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (59/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (60/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/supportsansi.o\" \"sqlitedb/supportsansi.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/supportsansi.o" "sqlitedb/supportsansi.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (61/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/isterminal.o\" \"sqlitedb/isterminal.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/isterminal.o" "sqlitedb/isterminal.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (62/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (63/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (64/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (65/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (66/66): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/filedecryptor.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/setminimumid.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"basedecryptor/o/getattachment.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/supportsansi.o\" \"sqlitedb/o/isterminal.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcryptopp -lsqlite3 -o \"signalbackup-tools\""
$COMPILER -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/filedecryptor.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/exportcsv.o" "signalbackup/o/exporttofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/exporttodir.o" "signalbackup/o/removedoubles.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/initfromdir.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/customs.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/mergegroups.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/initfromfile.o" "signalbackup/o/setminimumid.o" "o/main.o" "backupframe/o/init.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "basedecryptor/o/getattachment.o" "sqlitedb/o/exec.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/print.o" "sqlitedb/o/supportsansi.o" "sqlitedb/o/isterminal.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcryptopp -lsqlite3 -o "signalbackup-tools"
if [ $? -ne 0 ] ; then exit 1 ; fi
