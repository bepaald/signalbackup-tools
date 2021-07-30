#!/bin/sh

### This script is auto-generated

NUMCPU=1
CONFIG="default"
EXTRAOPTIONS=""
EXTRALINKOPTIONS=""

while [ $# -gt 0 ] ; do
    if [ "$1" = "--config" ] && [ $# -gt 1 ] ; then
        CONFIG="$2"
        shift
    else
        if [ -z "$EXTRAOPTIONS" ] ; then
            EXTRAOPTIONS="${EXTRAOPTIONS}$1"
        else
            EXTRAOPTIONS="${EXTRAOPTIONS} $1"
        fi
    fi
    shift
done

if [ "$CONFIG" = "clang" ] ; then
    COMPILER=$(which clang++)
    if [ -z "$COMPILER" ] ; then echo "Failed to find clang++ binary" && exit 1 ; fi

    echo "BUILDING (1/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
    if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -Wno-unknown-attributes -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -flto -O3 -s \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"reactionlist/o/setauthor.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/filedecryptor.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/decodestatusmessage.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"basedecryptor/o/getattachment.o\" \"keyvalueframe/o/statics.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o \"signalbackup-tools\""
    $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -flto -O3 -s "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "reactionlist/o/setauthor.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/customs.o" "filedecryptor/o/filedecryptor.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/exportcsv.o" "signalbackup/o/exporttofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/exporttodir.o" "signalbackup/o/removedoubles.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/initfromdir.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/importwachat.o" "signalbackup/o/customs.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/summarize.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/mergegroups.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/initfromfile.o" "signalbackup/o/setminimumid.o" "signalbackup/o/importcsv.o" "signalbackup/o/decodestatusmessage.o" "o/main.o" "backupframe/o/init.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "basedecryptor/o/getattachment.o" "keyvalueframe/o/statics.o" "sqlitedb/o/exec.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/print.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o "signalbackup-tools"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

if [ "$CONFIG" = "cryptopp" ] ; then
    COMPILER=$(which g++)
    if [ -z "$COMPILER" ] ; then echo "Failed to find g++ binary" && exit 1 ; fi

    echo "BUILDING (1/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
    if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -DUSE_CRYPTOPP -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"reactionlist/o/setauthor.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/filedecryptor.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/decodestatusmessage.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"basedecryptor/o/getattachment.o\" \"keyvalueframe/o/statics.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcryptopp -lsqlite3 $EXTRALINKOPTIONS -o \"signalbackup-tools\""
    $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "reactionlist/o/setauthor.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/customs.o" "filedecryptor/o/filedecryptor.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/exportcsv.o" "signalbackup/o/exporttofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/exporttodir.o" "signalbackup/o/removedoubles.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/initfromdir.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/importwachat.o" "signalbackup/o/customs.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/summarize.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/mergegroups.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/initfromfile.o" "signalbackup/o/setminimumid.o" "signalbackup/o/importcsv.o" "signalbackup/o/decodestatusmessage.o" "o/main.o" "backupframe/o/init.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "basedecryptor/o/getattachment.o" "keyvalueframe/o/statics.o" "sqlitedb/o/exec.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/print.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcryptopp -lsqlite3 $EXTRALINKOPTIONS -o "signalbackup-tools"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

if [ "$CONFIG" = "default" ] ; then
    COMPILER=$(which g++)
    if [ -z "$COMPILER" ] ; then echo "Failed to find g++ binary" && exit 1 ; fi

    echo "BUILDING (1/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
    if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"reactionlist/o/setauthor.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/filedecryptor.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/decodestatusmessage.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"basedecryptor/o/getattachment.o\" \"keyvalueframe/o/statics.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o \"signalbackup-tools\""
    $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "reactionlist/o/setauthor.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/customs.o" "filedecryptor/o/filedecryptor.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/exportcsv.o" "signalbackup/o/exporttofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/exporttodir.o" "signalbackup/o/removedoubles.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/initfromdir.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/importwachat.o" "signalbackup/o/customs.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/summarize.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/mergegroups.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/initfromfile.o" "signalbackup/o/setminimumid.o" "signalbackup/o/importcsv.o" "signalbackup/o/decodestatusmessage.o" "o/main.o" "backupframe/o/init.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "basedecryptor/o/getattachment.o" "keyvalueframe/o/statics.o" "sqlitedb/o/exec.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/print.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o "signalbackup-tools"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

if [ "$CONFIG" = "windows" ] ; then
    COMPILER=$(which x86_64-w64-mingw32-g++)
    if [ -z "$COMPILER" ] ; then echo "Failed to find x86_64-w64-mingw32-g++ binary" && exit 1 ; fi

    echo "BUILDING (1/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
    if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"sqlitedb/o/widestring.o\" \"sqlitedb/widestring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "sqlitedb/o/widestring.o" "sqlitedb/widestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/83): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -D_WIN32_WINNT=0x600 -I/usr/x86_64-w64-mingw32/include/ -std=c++2a -O3 -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,--as-needed -static-libgcc -static-libstdc++ -static -L/usr/x86_64-w64-mingw32/lib/ -O3 -s -flto=auto \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" \"memfiledb/o/statics.o\" \"endframe/o/statics.o\" \"reactionlist/o/setauthor.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/filedecryptor.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"arg/o/arg.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"attachmentframe/o/statics.o\" \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/decodestatusmessage.o\" \"o/main.o\" \"backupframe/o/init.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"basedecryptor/o/getattachment.o\" \"keyvalueframe/o/statics.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/widestring.o\" \"databaseversionframe/o/statics.o\" \"headerframe/o/statics.o\" \"stickerframe/o/statics.o\" -lcrypto -lsqlite3 -lssp -luser32 -lcrypt32 -ladvapi32 -lgdi32 -lws2_32 $EXTRALINKOPTIONS -o \"signalbackup-tools_win.exe\""
    $COMPILER -Wall -Wextra -Wl,--as-needed -static-libgcc -static-libstdc++ -static -L/usr/x86_64-w64-mingw32/lib/ -O3 -s -flto=auto "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" "memfiledb/o/statics.o" "endframe/o/statics.o" "reactionlist/o/setauthor.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/getframe.o" "filedecryptor/o/initbackupframe.o" "filedecryptor/o/customs.o" "filedecryptor/o/filedecryptor.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "arg/o/arg.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "attachmentframe/o/statics.o" "fileencryptor/o/encryptattachment.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/exportcsv.o" "signalbackup/o/exporttofile.o" "signalbackup/o/importthread.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/exportxml.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/exporttodir.o" "signalbackup/o/removedoubles.o" "signalbackup/o/croptothread.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/initfromdir.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/croptodates.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/importwachat.o" "signalbackup/o/customs.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/summarize.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/mergegroups.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/compactids.o" "signalbackup/o/initfromfile.o" "signalbackup/o/setminimumid.o" "signalbackup/o/importcsv.o" "signalbackup/o/decodestatusmessage.o" "o/main.o" "backupframe/o/init.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "basedecryptor/o/getattachment.o" "keyvalueframe/o/statics.o" "sqlitedb/o/exec.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/copydb.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/print.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/widestring.o" "databaseversionframe/o/statics.o" "headerframe/o/statics.o" "stickerframe/o/statics.o" -lcrypto -lsqlite3 -lssp -luser32 -lcrypt32 -ladvapi32 -lgdi32 -lws2_32 $EXTRALINKOPTIONS -o "signalbackup-tools_win.exe"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

