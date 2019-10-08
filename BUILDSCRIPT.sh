#!/bin/sh

### This script is auto-generated

COMPILER=$(which g++)
if [ -z "$COMPILER" ] ; then echo "Failed to find g++ binary" && exit 1 ; fi
NUMCPU=1

echo -E "BUILDING (1/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (2/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (3/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (4/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (5/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "endframe/o/statics.o" "endframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (6/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (7/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (8/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (9/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"arg/o/arg.o\" \"arg/arg.cc\""
if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "arg/o/arg.o" "arg/arg.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (10/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (11/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (12/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (13/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (14/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (15/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (16/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (17/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (18/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (19/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (20/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/writeencrypteddatatofile.o\" \"signalbackup/writeencrypteddatatofile.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/writeencrypteddatatofile.o" "signalbackup/writeencrypteddatatofile.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (21/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (22/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (23/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (24/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (25/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (26/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (27/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (28/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (29/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (30/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (31/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (32/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (33/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (34/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/exportbackup.o\" \"signalbackup/exportbackup.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/exportbackup.o" "signalbackup/exportbackup.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (35/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (36/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (37/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (38/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"signalbackup/o/signalbackup.o\" \"signalbackup/signalbackup.cc\""
if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "signalbackup/o/signalbackup.o" "signalbackup/signalbackup.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (39/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"o/main.o\" \"main.cc\""
if [ ! -d "o" ] ; then mkdir "o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "o/main.o" "main.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (40/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "backupframe/o/init.o" "backupframe/init.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (41/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (42/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (43/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (44/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (45/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (46/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (47/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (48/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "headerframe/o/statics.o" "headerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "BUILDING (49/49): $COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
if [ $? -ne 0 ] ; then exit 1 ; fi

echo -E "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/filedecryptor.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencrypteddatatofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/exportbackup.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/signalbackup.o\" \"o/main.o\" \"backupframe/o/init.o\" \"basedecryptor/o/getattachment.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcryptopp -lsqlite3 -o \"signalbackup-tools\""
$COMPILER -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/filedecryptor.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencrypteddatatofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/exportbackup.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/setminimumid.o" "signalbackup/o/signalbackup.o" "o/main.o" "backupframe/o/init.o" "basedecryptor/o/getattachment.o" "sqlitedb/o/exec.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcryptopp -lsqlite3 -o "signalbackup-tools"
if [ $? -ne 0 ] ; then exit 1 ; fi
