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

if [ "$CONFIG" = "default" ] ; then
    COMPILER=$(which g++)
    if [ -z "$COMPILER" ] ; then echo "Failed to find g++ binary" && exit 1 ; fi

    echo "BUILDING (1/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/checkdbintegrity.o\" \"signalbackup/checkdbintegrity.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/checkdbintegrity.o" "signalbackup/checkdbintegrity.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/scanself.o\" \"signalbackup/scanself.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/scanself.o" "signalbackup/scanself.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/applyranges.o\" \"signalbackup/applyranges.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/applyranges.o" "signalbackup/applyranges.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/scramble.o\" \"signalbackup/scramble.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/scramble.o" "signalbackup/scramble.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlescapestring.o\" \"signalbackup/htmlescapestring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlescapestring.o" "signalbackup/htmlescapestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/insertattachments.o\" \"signalbackup/insertattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/insertattachments.o" "signalbackup/insertattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlprepbody.o\" \"signalbackup/htmlprepbody.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlprepbody.o" "signalbackup/htmlprepbody.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updategroupmembers.o\" \"signalbackup/updategroupmembers.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updategroupmembers.o" "signalbackup/updategroupmembers.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtgroupchangemessage.o\" \"signalbackup/handledtgroupchangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtgroupchangemessage.o" "signalbackup/handledtgroupchangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/insertrow.o\" \"signalbackup/insertrow.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/insertrow.o" "signalbackup/insertrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupv1migrationrecipients.o\" \"signalbackup/getgroupv1migrationrecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupv1migrationrecipients.o" "signalbackup/getgroupv1migrationrecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getattachmentmetadata.o\" \"signalbackup/getattachmentmetadata.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getattachmentmetadata.o" "signalbackup/getattachmentmetadata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getallthreadrecipients.o\" \"signalbackup/getallthreadrecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getallthreadrecipients.o" "signalbackup/getallthreadrecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getrecipientidfrom.o\" \"signalbackup/getrecipientidfrom.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getrecipientidfrom.o" "signalbackup/getrecipientidfrom.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importfromdesktop.o\" \"signalbackup/importfromdesktop.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importfromdesktop.o" "signalbackup/importfromdesktop.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getdtreactions.o\" \"signalbackup/getdtreactions.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getdtreactions.o" "signalbackup/getdtreactions.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteavatar.o\" \"signalbackup/htmlwriteavatar.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteavatar.o" "signalbackup/htmlwriteavatar.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics_html.o\" \"signalbackup/statics_html.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics_html.o" "signalbackup/statics_html.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setmessagedeliveryreceipts.o\" \"signalbackup/setmessagedeliveryreceipts.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setmessagedeliveryreceipts.o" "signalbackup/setmessagedeliveryreceipts.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/listthreads.o\" \"signalbackup/listthreads.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/listthreads.o" "signalbackup/listthreads.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlgetemojipos.o\" \"signalbackup/htmlgetemojipos.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlgetemojipos.o" "signalbackup/htmlgetemojipos.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/deleteattachments.o\" \"signalbackup/deleteattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/deleteattachments.o" "signalbackup/deleteattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupmembers.o\" \"signalbackup/getgroupmembers.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupmembers.o" "signalbackup/getgroupmembers.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtcalltypemessage.o\" \"signalbackup/handledtcalltypemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtcalltypemessage.o" "signalbackup/handledtcalltypemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics.o\" \"signalbackup/statics.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics.o" "signalbackup/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getnamefromrecipientid.o\" \"signalbackup/getnamefromrecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getnamefromrecipientid.o" "signalbackup/getnamefromrecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodeprofilechangemessage.o\" \"signalbackup/decodeprofilechangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodeprofilechangemessage.o" "signalbackup/decodeprofilechangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteindex.o\" \"signalbackup/htmlwriteindex.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteindex.o" "signalbackup/htmlwriteindex.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpavatars.o\" \"signalbackup/dumpavatars.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpavatars.o" "signalbackup/dumpavatars.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleanattachments.o\" \"signalbackup/cleanattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleanattachments.o" "signalbackup/cleanattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporthtml.o\" \"signalbackup/exporthtml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporthtml.o" "signalbackup/exporthtml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlescapeurl.o\" \"signalbackup/htmlescapeurl.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlescapeurl.o" "signalbackup/htmlescapeurl.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtexpirationchangemessage.o\" \"signalbackup/handledtexpirationchangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtexpirationchangemessage.o" "signalbackup/handledtexpirationchangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwrite.o\" \"signalbackup/htmlwrite.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwrite.o" "signalbackup/htmlwrite.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makefilenameunique.o\" \"signalbackup/makefilenameunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makefilenameunique.o" "signalbackup/makefilenameunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/missingattachmentexpected.o\" \"signalbackup/missingattachmentexpected.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/missingattachmentexpected.o" "signalbackup/missingattachmentexpected.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupupdaterecipients.o\" \"signalbackup/getgroupupdaterecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupupdaterecipients.o" "signalbackup/getgroupupdaterecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/insertreactions.o\" \"signalbackup/insertreactions.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/insertreactions.o" "signalbackup/insertreactions.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics_emoji.o\" \"signalbackup/statics_emoji.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics_emoji.o" "signalbackup/statics_emoji.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updategv1migrationmessage.o\" \"signalbackup/updategv1migrationmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updategv1migrationmessage.o" "signalbackup/updategv1migrationmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatereactionauthors.o\" \"signalbackup/updatereactionauthors.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatereactionauthors.o" "signalbackup/updatereactionauthors.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setrecipientinfo.o\" \"signalbackup/setrecipientinfo.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setrecipientinfo.o" "signalbackup/setrecipientinfo.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (84/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteattachment.o\" \"signalbackup/htmlwriteattachment.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteattachment.o" "signalbackup/htmlwriteattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (85/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getdesktopdir.o\" \"signalbackup/getdesktopdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getdesktopdir.o" "signalbackup/getdesktopdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (86/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (87/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (88/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (89/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (90/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (91/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (92/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (93/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (94/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"framewithattachment/o/setattachmentdata.o\" \"framewithattachment/setattachmentdata.cc\""
    if [ ! -d "framewithattachment/o" ] ; then mkdir "framewithattachment/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "framewithattachment/o/setattachmentdata.o" "framewithattachment/setattachmentdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (95/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (96/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (97/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (98/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (99/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (100/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (101/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (102/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (103/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (104/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (105/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (106/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/exec.o\" \"sqlitedb/exec.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/exec.o" "sqlitedb/exec.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (107/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (108/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/printlinemode.o\" \"sqlitedb/printlinemode.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/printlinemode.o" "sqlitedb/printlinemode.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (109/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (110/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (111/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (112/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (113/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (114/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"basedecryptor/o/getattachment.o\" \"basedecryptor/getattachment.cc\""
    if [ ! -d "basedecryptor/o" ] ; then mkdir "basedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "basedecryptor/o/getattachment.o" "basedecryptor/getattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (115/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (116/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (117/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (118/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (119/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (120/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (121/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (122/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (123/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (124/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (125/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/usage.o\" \"arg/usage.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/usage.o" "arg/usage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (126/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (127/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/cryptbase.o\" \"cryptbase/cryptbase.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/cryptbase.o" "cryptbase/cryptbase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (128/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (129/129): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto \"keyvalueframe/o/statics.o\" \"signalbackup/o/checkdbintegrity.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/scanself.o\" \"signalbackup/o/applyranges.o\" \"signalbackup/o/scramble.o\" \"signalbackup/o/htmlescapestring.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/insertattachments.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/htmlprepbody.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/updategroupmembers.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/handledtgroupchangemessage.o\" \"signalbackup/o/insertrow.o\" \"signalbackup/o/getgroupv1migrationrecipients.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/getattachmentmetadata.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/getallthreadrecipients.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/getrecipientidfrom.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/importfromdesktop.o\" \"signalbackup/o/getdtreactions.o\" \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/o/htmlwriteavatar.o\" \"signalbackup/o/statics_html.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/setmessagedeliveryreceipts.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/listthreads.o\" \"signalbackup/o/htmlgetemojipos.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/deleteattachments.o\" \"signalbackup/o/getgroupmembers.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/handledtcalltypemessage.o\" \"signalbackup/o/statics.o\" \"signalbackup/o/getnamefromrecipientid.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/decodeprofilechangemessage.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/htmlwriteindex.o\" \"signalbackup/o/dumpavatars.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/cleanattachments.o\" \"signalbackup/o/exporthtml.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/htmlescapeurl.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/handledtexpirationchangemessage.o\" \"signalbackup/o/htmlwrite.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/makefilenameunique.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/missingattachmentexpected.o\" \"signalbackup/o/getgroupupdaterecipients.o\" \"signalbackup/o/insertreactions.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/statics_emoji.o\" \"signalbackup/o/updategv1migrationmessage.o\" \"signalbackup/o/updatereactionauthors.o\" \"signalbackup/o/setrecipientinfo.o\" \"signalbackup/o/htmlwriteattachment.o\" \"signalbackup/o/getdesktopdir.o\" \"attachmentframe/o/statics.o\" \"databaseversionframe/o/statics.o\" \"endframe/o/statics.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"framewithattachment/o/setattachmentdata.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"backupframe/o/init.o\" \"memfiledb/o/statics.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/exec.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/printlinemode.o\" \"stickerframe/o/statics.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"o/main.o\" \"headerframe/o/statics.o\" \"basedecryptor/o/getattachment.o\" \"reactionlist/o/setauthor.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/o/encryptattachment.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/initbackupframe.o\" \"arg/o/usage.o\" \"arg/o/arg.o\" \"cryptbase/o/cryptbase.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o \"signalbackup-tools\""
    $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto "keyvalueframe/o/statics.o" "signalbackup/o/checkdbintegrity.o" "signalbackup/o/mergegroups.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/scanself.o" "signalbackup/o/applyranges.o" "signalbackup/o/scramble.o" "signalbackup/o/htmlescapestring.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/insertattachments.o" "signalbackup/o/setminimumid.o" "signalbackup/o/htmlprepbody.o" "signalbackup/o/croptodates.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/updategroupmembers.o" "signalbackup/o/exporttofile.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/handledtgroupchangemessage.o" "signalbackup/o/insertrow.o" "signalbackup/o/getgroupv1migrationrecipients.o" "signalbackup/o/importwachat.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/getattachmentmetadata.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/getallthreadrecipients.o" "signalbackup/o/croptothread.o" "signalbackup/o/initfromdir.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/getrecipientidfrom.o" "signalbackup/o/compactids.o" "signalbackup/o/importfromdesktop.o" "signalbackup/o/getdtreactions.o" "signalbackup/o/decodestatusmessage.o" "signalbackup/o/htmlwriteavatar.o" "signalbackup/o/statics_html.o" "signalbackup/o/exporttodir.o" "signalbackup/o/setmessagedeliveryreceipts.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/exportcsv.o" "signalbackup/o/listthreads.o" "signalbackup/o/htmlgetemojipos.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/deleteattachments.o" "signalbackup/o/getgroupmembers.o" "signalbackup/o/customs.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/importcsv.o" "signalbackup/o/handledtcalltypemessage.o" "signalbackup/o/statics.o" "signalbackup/o/getnamefromrecipientid.o" "signalbackup/o/importthread.o" "signalbackup/o/decodeprofilechangemessage.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/htmlwriteindex.o" "signalbackup/o/dumpavatars.o" "signalbackup/o/summarize.o" "signalbackup/o/removedoubles.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/cleanattachments.o" "signalbackup/o/exporthtml.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/htmlescapeurl.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/handledtexpirationchangemessage.o" "signalbackup/o/htmlwrite.o" "signalbackup/o/initfromfile.o" "signalbackup/o/makefilenameunique.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/missingattachmentexpected.o" "signalbackup/o/getgroupupdaterecipients.o" "signalbackup/o/insertreactions.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/exportxml.o" "signalbackup/o/statics_emoji.o" "signalbackup/o/updategv1migrationmessage.o" "signalbackup/o/updatereactionauthors.o" "signalbackup/o/setrecipientinfo.o" "signalbackup/o/htmlwriteattachment.o" "signalbackup/o/getdesktopdir.o" "attachmentframe/o/statics.o" "databaseversionframe/o/statics.o" "endframe/o/statics.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/o/decryptdata.o" "framewithattachment/o/setattachmentdata.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "backupframe/o/init.o" "memfiledb/o/statics.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/copydb.o" "sqlitedb/o/exec.o" "sqlitedb/o/print.o" "sqlitedb/o/printlinemode.o" "stickerframe/o/statics.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "o/main.o" "headerframe/o/statics.o" "basedecryptor/o/getattachment.o" "reactionlist/o/setauthor.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "fileencryptor/o/encryptattachment.o" "filedecryptor/o/getframe.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/filedecryptor.o" "filedecryptor/o/customs.o" "filedecryptor/o/initbackupframe.o" "arg/o/usage.o" "arg/o/arg.o" "cryptbase/o/cryptbase.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o "signalbackup-tools"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

