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

    echo "BUILDING (1/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"keyvalueframe/o/statics.o\" \"keyvalueframe/statics.cc\""
    if [ ! -d "keyvalueframe/o" ] ; then mkdir "keyvalueframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "keyvalueframe/o/statics.o" "keyvalueframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (2/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgmapcontacts.o\" \"signalbackup/tgmapcontacts.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgmapcontacts.o" "signalbackup/tgmapcontacts.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (3/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgbuildbody.o\" \"signalbackup/tgbuildbody.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgbuildbody.o" "signalbackup/tgbuildbody.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (4/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/checkdbintegrity.o\" \"signalbackup/checkdbintegrity.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/checkdbintegrity.o" "signalbackup/checkdbintegrity.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (5/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergegroups.o\" \"signalbackup/mergegroups.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergegroups.o" "signalbackup/mergegroups.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (6/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/writeencryptedframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/writeencryptedframe.o" "signalbackup/writeencryptedframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (7/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/scanself.o\" \"signalbackup/scanself.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/scanself.o" "signalbackup/scanself.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (8/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/applyranges.o\" \"signalbackup/applyranges.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/applyranges.o" "signalbackup/applyranges.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (9/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/prepareoutputdirectory.o\" \"signalbackup/prepareoutputdirectory.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/prepareoutputdirectory.o" "signalbackup/prepareoutputdirectory.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (10/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/scramble.o\" \"signalbackup/scramble.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/scramble.o" "signalbackup/scramble.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (11/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlescapestring.o\" \"signalbackup/htmlescapestring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlescapestring.o" "signalbackup/htmlescapestring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (12/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/cleandatabasebymessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleandatabasebymessages.o" "signalbackup/cleandatabasebymessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (13/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setminimumid.o\" \"signalbackup/setminimumid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setminimumid.o" "signalbackup/setminimumid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (14/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlprepbody.o\" \"signalbackup/htmlprepbody.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlprepbody.o" "signalbackup/htmlprepbody.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (15/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptodates.o\" \"signalbackup/croptodates.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptodates.o" "signalbackup/croptodates.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (16/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/datetomsecssinceepoch.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/datetomsecssinceepoch.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (17/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updategroupmembers.o\" \"signalbackup/updategroupmembers.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updategroupmembers.o" "signalbackup/updategroupmembers.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (18/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtimportlongtext.o\" \"signalbackup/dtimportlongtext.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtimportlongtext.o" "signalbackup/dtimportlongtext.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (19/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttofile.o\" \"signalbackup/exporttofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttofile.o" "signalbackup/exporttofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (20/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setcolumnnames.o\" \"signalbackup/setcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setcolumnnames.o" "signalbackup/setcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (21/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtgroupchangemessage.o\" \"signalbackup/handledtgroupchangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtgroupchangemessage.o" "signalbackup/handledtgroupchangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (22/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgimportmessages.o\" \"signalbackup/tgimportmessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgimportmessages.o" "signalbackup/tgimportmessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (23/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritefullcontacts.o\" \"signalbackup/htmlwritefullcontacts.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritefullcontacts.o" "signalbackup/htmlwritefullcontacts.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (24/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtupdateprofile.o\" \"signalbackup/dtupdateprofile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtupdateprofile.o" "signalbackup/dtupdateprofile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (25/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getrecipientinfofrommap.o\" \"signalbackup/getrecipientinfofrommap.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getrecipientinfofrommap.o" "signalbackup/getrecipientinfofrommap.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (26/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/insertrow.o\" \"signalbackup/insertrow.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/insertrow.o" "signalbackup/insertrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (27/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupv1migrationrecipients.o\" \"signalbackup/getgroupv1migrationrecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupv1migrationrecipients.o" "signalbackup/getgroupv1migrationrecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (28/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteblockedlist.o\" \"signalbackup/htmlwriteblockedlist.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteblockedlist.o" "signalbackup/htmlwriteblockedlist.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (29/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/listrecipients.o\" \"signalbackup/listrecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/listrecipients.o" "signalbackup/listrecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (30/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importwachat.o\" \"signalbackup/importwachat.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importwachat.o" "signalbackup/importwachat.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (31/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpmedia.o\" \"signalbackup/dumpmedia.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpmedia.o" "signalbackup/dumpmedia.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (32/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getattachmentmetadata.o\" \"signalbackup/getattachmentmetadata.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getattachmentmetadata.o" "signalbackup/getattachmentmetadata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (33/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makeidsunique.o\" \"signalbackup/makeidsunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makeidsunique.o" "signalbackup/makeidsunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (34/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtimportstickerpacks.o\" \"signalbackup/dtimportstickerpacks.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtimportstickerpacks.o" "signalbackup/dtimportstickerpacks.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (35/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getallthreadrecipients.o\" \"signalbackup/getallthreadrecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getallthreadrecipients.o" "signalbackup/getallthreadrecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (36/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/croptothread.o\" \"signalbackup/croptothread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/croptothread.o" "signalbackup/croptothread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (37/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromdir.o\" \"signalbackup/initfromdir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromdir.o" "signalbackup/initfromdir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (38/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/fillthreadtablefrommessages.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/fillthreadtablefrommessages.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (39/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtsetavatar.o\" \"signalbackup/dtsetavatar.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtsetavatar.o" "signalbackup/dtsetavatar.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (40/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtinsertattachments.o\" \"signalbackup/dtinsertattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtinsertattachments.o" "signalbackup/dtinsertattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (41/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getrecipientidfrom.o\" \"signalbackup/getrecipientidfrom.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getrecipientidfrom.o" "signalbackup/getrecipientidfrom.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (42/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/compactids.o\" \"signalbackup/compactids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/compactids.o" "signalbackup/compactids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (43/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importfromdesktop.o\" \"signalbackup/importfromdesktop.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importfromdesktop.o" "signalbackup/importfromdesktop.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (44/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritecalllog.o\" \"signalbackup/htmlwritecalllog.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritecalllog.o" "signalbackup/htmlwritecalllog.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (45/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttxt.o\" \"signalbackup/exporttxt.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttxt.o" "signalbackup/exporttxt.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (46/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getdtreactions.o\" \"signalbackup/getdtreactions.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getdtreactions.o" "signalbackup/getdtreactions.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (47/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/decodestatusmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodestatusmessage.o" "signalbackup/decodestatusmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (48/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteavatar.o\" \"signalbackup/htmlwriteavatar.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteavatar.o" "signalbackup/htmlwriteavatar.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (49/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/scanmissingattachments.o\" \"signalbackup/scanmissingattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/scanmissingattachments.o" "signalbackup/scanmissingattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (50/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics_html.o\" \"signalbackup/statics_html.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics_html.o" "signalbackup/statics_html.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (51/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporttodir.o\" \"signalbackup/exporttodir.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporttodir.o" "signalbackup/exporttodir.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (52/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getfreedateformessage.o\" \"signalbackup/getfreedateformessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getfreedateformessage.o" "signalbackup/getfreedateformessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (53/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getrecipientidfrommapped.o\" \"signalbackup/getrecipientidfrommapped.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getrecipientidfrommapped.o" "signalbackup/getrecipientidfrommapped.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (54/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/setfiletimestamp.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setfiletimestamp.o" "signalbackup/setfiletimestamp.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (55/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportcsv.o\" \"signalbackup/exportcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportcsv.o" "signalbackup/exportcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (56/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtgroupv1migration.o\" \"signalbackup/handledtgroupv1migration.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtgroupv1migration.o" "signalbackup/handledtgroupv1migration.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (57/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/migratedatabase.o\" \"signalbackup/migratedatabase.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/migratedatabase.o" "signalbackup/migratedatabase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (58/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/listthreads.o\" \"signalbackup/listthreads.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/listthreads.o" "signalbackup/listthreads.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (59/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlgetemojipos.o\" \"signalbackup/htmlgetemojipos.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlgetemojipos.o" "signalbackup/htmlgetemojipos.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (60/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/updatethreadsentries.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatethreadsentries.o" "signalbackup/updatethreadsentries.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (61/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getcustomcolor.o\" \"signalbackup/getcustomcolor.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getcustomcolor.o" "signalbackup/getcustomcolor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (62/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterecipientid.o\" \"signalbackup/updaterecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterecipientid.o" "signalbackup/updaterecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (63/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatesnippetextrasrecipient.o\" \"signalbackup/updatesnippetextrasrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatesnippetextrasrecipient.o" "signalbackup/updatesnippetextrasrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (64/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupinfo.o\" \"signalbackup/getgroupinfo.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupinfo.o" "signalbackup/getgroupinfo.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (65/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/deleteattachments.o\" \"signalbackup/deleteattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/deleteattachments.o" "signalbackup/deleteattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (66/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupmembers.o\" \"signalbackup/getgroupmembers.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupmembers.o" "signalbackup/getgroupmembers.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (67/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/customs.o\" \"signalbackup/customs.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/customs.o" "signalbackup/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (68/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/getminmaxusedid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getminmaxusedid.o" "signalbackup/getminmaxusedid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (69/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/reordermmssmsids.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/reordermmssmsids.o" "signalbackup/reordermmssmsids.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (70/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importcsv.o\" \"signalbackup/importcsv.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importcsv.o" "signalbackup/importcsv.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (71/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtcalltypemessage.o\" \"signalbackup/handledtcalltypemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtcalltypemessage.o" "signalbackup/handledtcalltypemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (72/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriterevision.o\" \"signalbackup/htmlwriterevision.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriterevision.o" "signalbackup/htmlwriterevision.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (73/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics.o\" \"signalbackup/statics.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics.o" "signalbackup/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (74/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getnamefromrecipientid.o\" \"signalbackup/getnamefromrecipientid.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getnamefromrecipientid.o" "signalbackup/getnamefromrecipientid.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (75/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritemsgreceiptinfo.o\" \"signalbackup/htmlwritemsgreceiptinfo.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritemsgreceiptinfo.o" "signalbackup/htmlwritemsgreceiptinfo.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (76/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updaterows.o\" \"signalbackup/updaterows.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updaterows.o" "signalbackup/updaterows.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (77/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importthread.o\" \"signalbackup/importthread.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importthread.o" "signalbackup/importthread.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (78/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritesettings.o\" \"signalbackup/htmlwritesettings.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritesettings.o" "signalbackup/htmlwritesettings.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (79/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/decodeprofilechangemessage.o\" \"signalbackup/decodeprofilechangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/decodeprofilechangemessage.o" "signalbackup/decodeprofilechangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (80/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/remaprecipients.o\" \"signalbackup/remaprecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/remaprecipients.o" "signalbackup/remaprecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (81/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteindex.o\" \"signalbackup/htmlwriteindex.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteindex.o" "signalbackup/htmlwriteindex.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (82/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpavatars.o\" \"signalbackup/dumpavatars.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpavatars.o" "signalbackup/dumpavatars.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (83/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/summarize.o\" \"signalbackup/summarize.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/summarize.o" "signalbackup/summarize.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (84/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/removedoubles.o\" \"signalbackup/removedoubles.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/removedoubles.o" "signalbackup/removedoubles.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (85/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/getthreadidfromrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/getthreadidfromrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (86/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/cleanattachments.o\" \"signalbackup/cleanattachments.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/cleanattachments.o" "signalbackup/cleanattachments.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (87/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exporthtml.o\" \"signalbackup/exporthtml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exporthtml.o" "signalbackup/exporthtml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (88/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/buildsqlstatementframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/buildsqlstatementframe.o" "signalbackup/buildsqlstatementframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (89/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlescapeurl.o\" \"signalbackup/htmlescapeurl.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlescapeurl.o" "signalbackup/htmlescapeurl.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (90/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/gettranslatedname.o\" \"signalbackup/gettranslatedname.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/gettranslatedname.o" "signalbackup/gettranslatedname.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (91/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/importtelegramjson.o\" \"signalbackup/importtelegramjson.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/importtelegramjson.o" "signalbackup/importtelegramjson.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (92/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/escapexmlstring.o\" \"signalbackup/escapexmlstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/escapexmlstring.o" "signalbackup/escapexmlstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (93/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgsetquote.o\" \"signalbackup/tgsetquote.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgsetquote.o" "signalbackup/tgsetquote.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (94/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handlewamessage.o\" \"signalbackup/handlewamessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handlewamessage.o" "signalbackup/handlewamessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (95/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/addsmsmessage.o\" \"signalbackup/addsmsmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/addsmsmessage.o" "signalbackup/addsmsmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (96/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgsetbodyranges.o\" \"signalbackup/tgsetbodyranges.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgsetbodyranges.o" "signalbackup/tgsetbodyranges.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (97/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/handledtexpirationchangemessage.o\" \"signalbackup/handledtexpirationchangemessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/handledtexpirationchangemessage.o" "signalbackup/handledtexpirationchangemessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (98/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritestickerpacks.o\" \"signalbackup/htmlwritestickerpacks.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritestickerpacks.o" "signalbackup/htmlwritestickerpacks.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (99/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtsetsharedcontactsjsonstring.o\" \"signalbackup/dtsetsharedcontactsjsonstring.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtsetsharedcontactsjsonstring.o" "signalbackup/dtsetsharedcontactsjsonstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (100/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwrite.o\" \"signalbackup/htmlwrite.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwrite.o" "signalbackup/htmlwrite.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (101/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/initfromfile.o\" \"signalbackup/initfromfile.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/initfromfile.o" "signalbackup/initfromfile.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (102/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/makefilenameunique.o\" \"signalbackup/makefilenameunique.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/makefilenameunique.o" "signalbackup/makefilenameunique.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (103/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/sanitizefilename.o\" \"signalbackup/sanitizefilename.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/sanitizefilename.o" "signalbackup/sanitizefilename.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (104/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/mergerecipients.o\" \"signalbackup/mergerecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/mergerecipients.o" "signalbackup/mergerecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (105/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwritesearchpage.o\" \"signalbackup/htmlwritesearchpage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwritesearchpage.o" "signalbackup/htmlwritesearchpage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (106/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/missingattachmentexpected.o\" \"signalbackup/missingattachmentexpected.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/missingattachmentexpected.o" "signalbackup/missingattachmentexpected.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (107/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtcreaterecipient.o\" \"signalbackup/dtcreaterecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtcreaterecipient.o" "signalbackup/dtcreaterecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (108/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/getgroupupdaterecipients.o\" \"signalbackup/getgroupupdaterecipients.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/getgroupupdaterecipients.o" "signalbackup/getgroupupdaterecipients.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (109/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/insertreactions.o\" \"signalbackup/insertreactions.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/insertreactions.o" "signalbackup/insertreactions.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (110/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/dumpinfoonbadframe.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/dumpinfoonbadframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (111/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dropbadframes.o\" \"signalbackup/dropbadframes.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dropbadframes.o" "signalbackup/dropbadframes.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (112/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtsetcolumnnames.o\" \"signalbackup/dtsetcolumnnames.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtsetcolumnnames.o" "signalbackup/dtsetcolumnnames.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (113/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/tgsetattachment.o\" \"signalbackup/tgsetattachment.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/tgsetattachment.o" "signalbackup/tgsetattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (114/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/exportxml.o\" \"signalbackup/exportxml.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/exportxml.o" "signalbackup/exportxml.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (115/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/statics_emoji.o\" \"signalbackup/statics_emoji.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/statics_emoji.o" "signalbackup/statics_emoji.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (116/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updategv1migrationmessage.o\" \"signalbackup/updategv1migrationmessage.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updategv1migrationmessage.o" "signalbackup/updategv1migrationmessage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (117/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updatereactionauthors.o\" \"signalbackup/updatereactionauthors.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updatereactionauthors.o" "signalbackup/updatereactionauthors.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (118/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/setrecipientinfo.o\" \"signalbackup/setrecipientinfo.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/setrecipientinfo.o" "signalbackup/setrecipientinfo.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (119/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/findrecipient.o\" \"signalbackup/findrecipient.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/findrecipient.o" "signalbackup/findrecipient.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (120/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/updateavatars.o\" \"signalbackup/updateavatars.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/updateavatars.o" "signalbackup/updateavatars.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (121/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/htmlwriteattachment.o\" \"signalbackup/htmlwriteattachment.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/htmlwriteattachment.o" "signalbackup/htmlwriteattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (122/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"signalbackup/o/dtsetmessagedeliveryreceipts.o\" \"signalbackup/dtsetmessagedeliveryreceipts.cc\""
    if [ ! -d "signalbackup/o" ] ; then mkdir "signalbackup/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "signalbackup/o/dtsetmessagedeliveryreceipts.o" "signalbackup/dtsetmessagedeliveryreceipts.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (123/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"attachmentframe/o/statics.o\" \"attachmentframe/statics.cc\""
    if [ ! -d "attachmentframe/o" ] ; then mkdir "attachmentframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "attachmentframe/o/statics.o" "attachmentframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (124/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"logger/o/isterminal.o\" \"logger/isterminal.cc\""
    if [ ! -d "logger/o" ] ; then mkdir "logger/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "logger/o/isterminal.o" "logger/isterminal.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (125/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"logger/o/supportsansi.o\" \"logger/supportsansi.cc\""
    if [ ! -d "logger/o" ] ; then mkdir "logger/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "logger/o/supportsansi.o" "logger/supportsansi.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (126/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"logger/o/statics.o\" \"logger/statics.cc\""
    if [ ! -d "logger/o" ] ; then mkdir "logger/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "logger/o/statics.o" "logger/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (127/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"logger/o/outputhead.o\" \"logger/outputhead.cc\""
    if [ ! -d "logger/o" ] ; then mkdir "logger/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "logger/o/outputhead.o" "logger/outputhead.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (128/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"databaseversionframe/o/statics.o\" \"databaseversionframe/statics.cc\""
    if [ ! -d "databaseversionframe/o" ] ; then mkdir "databaseversionframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "databaseversionframe/o/statics.o" "databaseversionframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (129/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"endframe/o/statics.o\" \"endframe/statics.cc\""
    if [ ! -d "endframe/o" ] ; then mkdir "endframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "endframe/o/statics.o" "endframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (130/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getencryptedkey.o\" \"sqlcipherdecryptor/getencryptedkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getencryptedkey.o" "sqlcipherdecryptor/getencryptedkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (131/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/getkey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/getkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (132/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/destructor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/destructor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (133/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/gethmackey.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/gethmackey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (134/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/sqlcipherdecryptor.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/sqlcipherdecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (135/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlcipherdecryptor/o/decryptdata.o\" \"sqlcipherdecryptor/decryptdata.cc\""
    if [ ! -d "sqlcipherdecryptor/o" ] ; then mkdir "sqlcipherdecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlcipherdecryptor/o/decryptdata.o" "sqlcipherdecryptor/decryptdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (136/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"framewithattachment/o/setattachmentdata.o\" \"framewithattachment/setattachmentdata.cc\""
    if [ ! -d "framewithattachment/o" ] ; then mkdir "framewithattachment/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "framewithattachment/o/setattachmentdata.o" "framewithattachment/setattachmentdata.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (137/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sharedprefframe/o/statics.o\" \"sharedprefframe/statics.cc\""
    if [ ! -d "sharedprefframe/o" ] ; then mkdir "sharedprefframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sharedprefframe/o/statics.o" "sharedprefframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (138/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"avatarframe/o/statics.o\" \"avatarframe/statics.cc\""
    if [ ! -d "avatarframe/o" ] ; then mkdir "avatarframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "avatarframe/o/statics.o" "avatarframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (139/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/statics.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/statics.o" "sqlstatementframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (140/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlstatementframe/o/buildstatement.o\" \"sqlstatementframe/buildstatement.cc\""
    if [ ! -d "sqlstatementframe/o" ] ; then mkdir "sqlstatementframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlstatementframe/o/buildstatement.o" "sqlstatementframe/buildstatement.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (146/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"backupframe/o/init.o\" \"backupframe/init.cc\""
    if [ ! -d "backupframe/o" ] ; then mkdir "backupframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "backupframe/o/init.o" "backupframe/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (147/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"desktopattachmentreader/o/getencryptedattachment.o\" \"desktopattachmentreader/getencryptedattachment.cc\""
    if [ ! -d "desktopattachmentreader/o" ] ; then mkdir "desktopattachmentreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "desktopattachmentreader/o/getencryptedattachment.o" "desktopattachmentreader/getencryptedattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (148/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"memfiledb/o/statics.o\" \"memfiledb/statics.cc\""
    if [ ! -d "memfiledb/o" ] ; then mkdir "memfiledb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "memfiledb/o/statics.o" "memfiledb/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (149/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasstring.o\" \"sqlitedb/valueasstring.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasstring.o" "sqlitedb/valueasstring.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (150/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/prettyprint.o\" \"sqlitedb/prettyprint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/prettyprint.o" "sqlitedb/prettyprint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (151/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/renamecolumn.o\" \"sqlitedb/renamecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/renamecolumn.o" "sqlitedb/renamecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (152/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/availablewidth.o\" \"sqlitedb/availablewidth.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/availablewidth.o" "sqlitedb/availablewidth.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (153/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/removecolumn.o\" \"sqlitedb/removecolumn.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/removecolumn.o" "sqlitedb/removecolumn.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (154/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/valueasint.o\" \"sqlitedb/valueasint.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/valueasint.o" "sqlitedb/valueasint.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (155/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/copydb.o\" \"sqlitedb/copydb.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/copydb.o" "sqlitedb/copydb.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (156/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/print.o\" \"sqlitedb/print.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/print.o" "sqlitedb/print.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (157/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"sqlitedb/o/printlinemode.o\" \"sqlitedb/printlinemode.cc\""
    if [ ! -d "sqlitedb/o" ] ; then mkdir "sqlitedb/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "sqlitedb/o/printlinemode.o" "sqlitedb/printlinemode.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (158/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"stickerframe/o/statics.o\" \"stickerframe/statics.cc\""
    if [ ! -d "stickerframe/o" ] ; then mkdir "stickerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "stickerframe/o/statics.o" "stickerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (159/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/readrow.o\" \"csvreader/readrow.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/readrow.o" "csvreader/readrow.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (160/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"csvreader/o/read.o\" \"csvreader/read.cc\""
    if [ ! -d "csvreader/o" ] ; then mkdir "csvreader/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "csvreader/o/read.o" "csvreader/read.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (161/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"o/main.o\" \"main.cc\""
    if [ ! -d "o" ] ; then mkdir "o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "o/main.o" "main.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (162/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"headerframe/o/statics.o\" \"headerframe/statics.cc\""
    if [ ! -d "headerframe/o" ] ; then mkdir "headerframe/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "headerframe/o/statics.o" "headerframe/statics.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (163/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"desktopdatabase/o/init.o\" \"desktopdatabase/init.cc\""
    if [ ! -d "desktopdatabase/o" ] ; then mkdir "desktopdatabase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "desktopdatabase/o/init.o" "desktopdatabase/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (164/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"reactionlist/o/setauthor.o\" \"reactionlist/setauthor.cc\""
    if [ ! -d "reactionlist/o" ] ; then mkdir "reactionlist/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "reactionlist/o/setauthor.o" "reactionlist/setauthor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (165/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"jsondatabase/o/jsondatabase.o\" \"jsondatabase/jsondatabase.cc\""
    if [ ! -d "jsondatabase/o" ] ; then mkdir "jsondatabase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "jsondatabase/o/jsondatabase.o" "jsondatabase/jsondatabase.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (166/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/init.o\" \"fileencryptor/init.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/init.o" "fileencryptor/init.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (167/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptframe.o\" \"fileencryptor/encryptframe.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptframe.o" "fileencryptor/encryptframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (168/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/fileencryptor.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/fileencryptor.o" "fileencryptor/fileencryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (169/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"fileencryptor/o/encryptattachment.o\" \"fileencryptor/encryptattachment.cc\""
    if [ ! -d "fileencryptor/o" ] ; then mkdir "fileencryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "fileencryptor/o/encryptattachment.o" "fileencryptor/encryptattachment.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (170/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframe.o\" \"filedecryptor/getframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframe.o" "filedecryptor/getframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (171/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/getframebrute.o\" \"filedecryptor/getframebrute.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/getframebrute.o" "filedecryptor/getframebrute.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (172/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/filedecryptor.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/filedecryptor.o" "filedecryptor/filedecryptor.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (173/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/customs.o\" \"filedecryptor/customs.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/customs.o" "filedecryptor/customs.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (174/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"filedecryptor/o/initbackupframe.o\" \"filedecryptor/initbackupframe.cc\""
    if [ ! -d "filedecryptor/o" ] ; then mkdir "filedecryptor/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "filedecryptor/o/initbackupframe.o" "filedecryptor/initbackupframe.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (175/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/usage.o\" \"arg/usage.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/usage.o" "arg/usage.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (176/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"arg/o/arg.o\" \"arg/arg.cc\""
    if [ ! -d "arg/o" ] ; then mkdir "arg/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "arg/o/arg.o" "arg/arg.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (177/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getbackupkey.o\" \"cryptbase/getbackupkey.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getbackupkey.o" "cryptbase/getbackupkey.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "BUILDING (178/178): $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o \"cryptbase/o/getcipherandmac.o\" \"cryptbase/getcipherandmac.cc\""
    if [ ! -d "cryptbase/o" ] ; then mkdir "cryptbase/o" ; fi
    $COMPILER -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -std=c++2a -O3 -march=native -flto $EXTRAOPTIONS -o "cryptbase/o/getcipherandmac.o" "cryptbase/getcipherandmac.cc"
    if [ $? -ne 0 ] ; then exit 1 ; fi

    echo "LINKING: $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto \"keyvalueframe/o/statics.o\" \"signalbackup/o/tgmapcontacts.o\" \"signalbackup/o/tgbuildbody.o\" \"signalbackup/o/checkdbintegrity.o\" \"signalbackup/o/mergegroups.o\" \"signalbackup/o/writeencryptedframe.o\" \"signalbackup/o/scanself.o\" \"signalbackup/o/applyranges.o\" \"signalbackup/o/prepareoutputdirectory.o\" \"signalbackup/o/scramble.o\" \"signalbackup/o/htmlescapestring.o\" \"signalbackup/o/cleandatabasebymessages.o\" \"signalbackup/o/setminimumid.o\" \"signalbackup/o/htmlprepbody.o\" \"signalbackup/o/croptodates.o\" \"signalbackup/o/datetomsecssinceepoch.o\" \"signalbackup/o/updategroupmembers.o\" \"signalbackup/o/dtimportlongtext.o\" \"signalbackup/o/exporttofile.o\" \"signalbackup/o/setcolumnnames.o\" \"signalbackup/o/handledtgroupchangemessage.o\" \"signalbackup/o/tgimportmessages.o\" \"signalbackup/o/htmlwritefullcontacts.o\" \"signalbackup/o/dtupdateprofile.o\" \"signalbackup/o/getrecipientinfofrommap.o\" \"signalbackup/o/insertrow.o\" \"signalbackup/o/getgroupv1migrationrecipients.o\" \"signalbackup/o/htmlwriteblockedlist.o\" \"signalbackup/o/listrecipients.o\" \"signalbackup/o/importwachat.o\" \"signalbackup/o/dumpmedia.o\" \"signalbackup/o/getattachmentmetadata.o\" \"signalbackup/o/makeidsunique.o\" \"signalbackup/o/dtimportstickerpacks.o\" \"signalbackup/o/getallthreadrecipients.o\" \"signalbackup/o/croptothread.o\" \"signalbackup/o/initfromdir.o\" \"signalbackup/o/fillthreadtablefrommessages.o\" \"signalbackup/o/dtsetavatar.o\" \"signalbackup/o/dtinsertattachments.o\" \"signalbackup/o/getrecipientidfrom.o\" \"signalbackup/o/compactids.o\" \"signalbackup/o/importfromdesktop.o\" \"signalbackup/o/htmlwritecalllog.o\" \"signalbackup/o/exporttxt.o\" \"signalbackup/o/getdtreactions.o\" \"signalbackup/o/decodestatusmessage.o\" \"signalbackup/o/htmlwriteavatar.o\" \"signalbackup/o/scanmissingattachments.o\" \"signalbackup/o/statics_html.o\" \"signalbackup/o/exporttodir.o\" \"signalbackup/o/getfreedateformessage.o\" \"signalbackup/o/getrecipientidfrommapped.o\" \"signalbackup/o/setfiletimestamp.o\" \"signalbackup/o/exportcsv.o\" \"signalbackup/o/handledtgroupv1migration.o\" \"signalbackup/o/migratedatabase.o\" \"signalbackup/o/listthreads.o\" \"signalbackup/o/htmlgetemojipos.o\" \"signalbackup/o/updatethreadsentries.o\" \"signalbackup/o/getcustomcolor.o\" \"signalbackup/o/updaterecipientid.o\" \"signalbackup/o/updatesnippetextrasrecipient.o\" \"signalbackup/o/getgroupinfo.o\" \"signalbackup/o/deleteattachments.o\" \"signalbackup/o/getgroupmembers.o\" \"signalbackup/o/customs.o\" \"signalbackup/o/getminmaxusedid.o\" \"signalbackup/o/reordermmssmsids.o\" \"signalbackup/o/importcsv.o\" \"signalbackup/o/handledtcalltypemessage.o\" \"signalbackup/o/htmlwriterevision.o\" \"signalbackup/o/statics.o\" \"signalbackup/o/getnamefromrecipientid.o\" \"signalbackup/o/htmlwritemsgreceiptinfo.o\" \"signalbackup/o/updaterows.o\" \"signalbackup/o/importthread.o\" \"signalbackup/o/htmlwritesettings.o\" \"signalbackup/o/decodeprofilechangemessage.o\" \"signalbackup/o/remaprecipients.o\" \"signalbackup/o/htmlwriteindex.o\" \"signalbackup/o/dumpavatars.o\" \"signalbackup/o/summarize.o\" \"signalbackup/o/removedoubles.o\" \"signalbackup/o/getthreadidfromrecipient.o\" \"signalbackup/o/cleanattachments.o\" \"signalbackup/o/exporthtml.o\" \"signalbackup/o/buildsqlstatementframe.o\" \"signalbackup/o/htmlescapeurl.o\" \"signalbackup/o/gettranslatedname.o\" \"signalbackup/o/importtelegramjson.o\" \"signalbackup/o/escapexmlstring.o\" \"signalbackup/o/tgsetquote.o\" \"signalbackup/o/handlewamessage.o\" \"signalbackup/o/addsmsmessage.o\" \"signalbackup/o/tgsetbodyranges.o\" \"signalbackup/o/handledtexpirationchangemessage.o\" \"signalbackup/o/htmlwritestickerpacks.o\" \"signalbackup/o/dtsetsharedcontactsjsonstring.o\" \"signalbackup/o/htmlwrite.o\" \"signalbackup/o/initfromfile.o\" \"signalbackup/o/makefilenameunique.o\" \"signalbackup/o/sanitizefilename.o\" \"signalbackup/o/mergerecipients.o\" \"signalbackup/o/htmlwritesearchpage.o\" \"signalbackup/o/missingattachmentexpected.o\" \"signalbackup/o/dtcreaterecipient.o\" \"signalbackup/o/getgroupupdaterecipients.o\" \"signalbackup/o/insertreactions.o\" \"signalbackup/o/dumpinfoonbadframe.o\" \"signalbackup/o/dropbadframes.o\" \"signalbackup/o/dtsetcolumnnames.o\" \"signalbackup/o/tgsetattachment.o\" \"signalbackup/o/exportxml.o\" \"signalbackup/o/statics_emoji.o\" \"signalbackup/o/updategv1migrationmessage.o\" \"signalbackup/o/updatereactionauthors.o\" \"signalbackup/o/setrecipientinfo.o\" \"signalbackup/o/findrecipient.o\" \"signalbackup/o/updateavatars.o\" \"signalbackup/o/htmlwriteattachment.o\" \"signalbackup/o/dtsetmessagedeliveryreceipts.o\" \"attachmentframe/o/statics.o\" \"logger/o/isterminal.o\" \"logger/o/supportsansi.o\" \"logger/o/statics.o\" \"logger/o/outputhead.o\" \"databaseversionframe/o/statics.o\" \"endframe/o/statics.o\" \"sqlcipherdecryptor/o/getencryptedkey.o\" \"sqlcipherdecryptor/o/getkey.o\" \"sqlcipherdecryptor/o/destructor.o\" \"sqlcipherdecryptor/o/gethmackey.o\" \"sqlcipherdecryptor/o/sqlcipherdecryptor.o\" \"sqlcipherdecryptor/o/decryptdata.o\" \"framewithattachment/o/setattachmentdata.o\" \"sharedprefframe/o/statics.o\" \"avatarframe/o/statics.o\" \"sqlstatementframe/o/statics.o\" \"sqlstatementframe/o/buildstatement.o\" \"backupframe/o/init.o\" \"desktopattachmentreader/o/getencryptedattachment.o\" \"memfiledb/o/statics.o\" \"sqlitedb/o/valueasstring.o\" \"sqlitedb/o/prettyprint.o\" \"sqlitedb/o/renamecolumn.o\" \"sqlitedb/o/availablewidth.o\" \"sqlitedb/o/removecolumn.o\" \"sqlitedb/o/valueasint.o\" \"sqlitedb/o/copydb.o\" \"sqlitedb/o/print.o\" \"sqlitedb/o/printlinemode.o\" \"stickerframe/o/statics.o\" \"csvreader/o/readrow.o\" \"csvreader/o/read.o\" \"o/main.o\" \"headerframe/o/statics.o\" \"desktopdatabase/o/init.o\" \"reactionlist/o/setauthor.o\" \"jsondatabase/o/jsondatabase.o\" \"fileencryptor/o/init.o\" \"fileencryptor/o/encryptframe.o\" \"fileencryptor/o/fileencryptor.o\" \"fileencryptor/o/encryptattachment.o\" \"filedecryptor/o/getframe.o\" \"filedecryptor/o/getframebrute.o\" \"filedecryptor/o/filedecryptor.o\" \"filedecryptor/o/customs.o\" \"filedecryptor/o/initbackupframe.o\" \"arg/o/usage.o\" \"arg/o/arg.o\" \"cryptbase/o/getbackupkey.o\" \"cryptbase/o/getcipherandmac.o\" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o \"signalbackup-tools\""
    $COMPILER -Wall -Wextra -Wl,-z,now -Wl,--as-needed -O3 -s -flto=auto "keyvalueframe/o/statics.o" "signalbackup/o/tgmapcontacts.o" "signalbackup/o/tgbuildbody.o" "signalbackup/o/checkdbintegrity.o" "signalbackup/o/mergegroups.o" "signalbackup/o/writeencryptedframe.o" "signalbackup/o/scanself.o" "signalbackup/o/applyranges.o" "signalbackup/o/prepareoutputdirectory.o" "signalbackup/o/scramble.o" "signalbackup/o/htmlescapestring.o" "signalbackup/o/cleandatabasebymessages.o" "signalbackup/o/setminimumid.o" "signalbackup/o/htmlprepbody.o" "signalbackup/o/croptodates.o" "signalbackup/o/datetomsecssinceepoch.o" "signalbackup/o/updategroupmembers.o" "signalbackup/o/dtimportlongtext.o" "signalbackup/o/exporttofile.o" "signalbackup/o/setcolumnnames.o" "signalbackup/o/handledtgroupchangemessage.o" "signalbackup/o/tgimportmessages.o" "signalbackup/o/htmlwritefullcontacts.o" "signalbackup/o/dtupdateprofile.o" "signalbackup/o/getrecipientinfofrommap.o" "signalbackup/o/insertrow.o" "signalbackup/o/getgroupv1migrationrecipients.o" "signalbackup/o/htmlwriteblockedlist.o" "signalbackup/o/listrecipients.o" "signalbackup/o/importwachat.o" "signalbackup/o/dumpmedia.o" "signalbackup/o/getattachmentmetadata.o" "signalbackup/o/makeidsunique.o" "signalbackup/o/dtimportstickerpacks.o" "signalbackup/o/getallthreadrecipients.o" "signalbackup/o/croptothread.o" "signalbackup/o/initfromdir.o" "signalbackup/o/fillthreadtablefrommessages.o" "signalbackup/o/dtsetavatar.o" "signalbackup/o/dtinsertattachments.o" "signalbackup/o/getrecipientidfrom.o" "signalbackup/o/compactids.o" "signalbackup/o/importfromdesktop.o" "signalbackup/o/htmlwritecalllog.o" "signalbackup/o/exporttxt.o" "signalbackup/o/getdtreactions.o" "signalbackup/o/decodestatusmessage.o" "signalbackup/o/htmlwriteavatar.o" "signalbackup/o/scanmissingattachments.o" "signalbackup/o/statics_html.o" "signalbackup/o/exporttodir.o" "signalbackup/o/getfreedateformessage.o" "signalbackup/o/getrecipientidfrommapped.o" "signalbackup/o/setfiletimestamp.o" "signalbackup/o/exportcsv.o" "signalbackup/o/handledtgroupv1migration.o" "signalbackup/o/migratedatabase.o" "signalbackup/o/listthreads.o" "signalbackup/o/htmlgetemojipos.o" "signalbackup/o/updatethreadsentries.o" "signalbackup/o/getcustomcolor.o" "signalbackup/o/updaterecipientid.o" "signalbackup/o/updatesnippetextrasrecipient.o" "signalbackup/o/getgroupinfo.o" "signalbackup/o/deleteattachments.o" "signalbackup/o/getgroupmembers.o" "signalbackup/o/customs.o" "signalbackup/o/getminmaxusedid.o" "signalbackup/o/reordermmssmsids.o" "signalbackup/o/importcsv.o" "signalbackup/o/handledtcalltypemessage.o" "signalbackup/o/htmlwriterevision.o" "signalbackup/o/statics.o" "signalbackup/o/getnamefromrecipientid.o" "signalbackup/o/htmlwritemsgreceiptinfo.o" "signalbackup/o/updaterows.o" "signalbackup/o/importthread.o" "signalbackup/o/htmlwritesettings.o" "signalbackup/o/decodeprofilechangemessage.o" "signalbackup/o/remaprecipients.o" "signalbackup/o/htmlwriteindex.o" "signalbackup/o/dumpavatars.o" "signalbackup/o/summarize.o" "signalbackup/o/removedoubles.o" "signalbackup/o/getthreadidfromrecipient.o" "signalbackup/o/cleanattachments.o" "signalbackup/o/exporthtml.o" "signalbackup/o/buildsqlstatementframe.o" "signalbackup/o/htmlescapeurl.o" "signalbackup/o/gettranslatedname.o" "signalbackup/o/importtelegramjson.o" "signalbackup/o/escapexmlstring.o" "signalbackup/o/tgsetquote.o" "signalbackup/o/handlewamessage.o" "signalbackup/o/addsmsmessage.o" "signalbackup/o/tgsetbodyranges.o" "signalbackup/o/handledtexpirationchangemessage.o" "signalbackup/o/htmlwritestickerpacks.o" "signalbackup/o/dtsetsharedcontactsjsonstring.o" "signalbackup/o/htmlwrite.o" "signalbackup/o/initfromfile.o" "signalbackup/o/makefilenameunique.o" "signalbackup/o/sanitizefilename.o" "signalbackup/o/mergerecipients.o" "signalbackup/o/htmlwritesearchpage.o" "signalbackup/o/missingattachmentexpected.o" "signalbackup/o/dtcreaterecipient.o" "signalbackup/o/getgroupupdaterecipients.o" "signalbackup/o/insertreactions.o" "signalbackup/o/dumpinfoonbadframe.o" "signalbackup/o/dropbadframes.o" "signalbackup/o/dtsetcolumnnames.o" "signalbackup/o/tgsetattachment.o" "signalbackup/o/exportxml.o" "signalbackup/o/statics_emoji.o" "signalbackup/o/updategv1migrationmessage.o" "signalbackup/o/updatereactionauthors.o" "signalbackup/o/setrecipientinfo.o" "signalbackup/o/findrecipient.o" "signalbackup/o/updateavatars.o" "signalbackup/o/htmlwriteattachment.o" "signalbackup/o/dtsetmessagedeliveryreceipts.o" "attachmentframe/o/statics.o" "logger/o/isterminal.o" "logger/o/supportsansi.o" "logger/o/statics.o" "logger/o/outputhead.o" "databaseversionframe/o/statics.o" "endframe/o/statics.o" "sqlcipherdecryptor/o/getencryptedkey.o" "sqlcipherdecryptor/o/getkey.o" "sqlcipherdecryptor/o/destructor.o" "sqlcipherdecryptor/o/gethmackey.o" "sqlcipherdecryptor/o/sqlcipherdecryptor.o" "sqlcipherdecryptor/o/decryptdata.o" "framewithattachment/o/setattachmentdata.o" "sharedprefframe/o/statics.o" "avatarframe/o/statics.o" "sqlstatementframe/o/statics.o" "sqlstatementframe/o/buildstatement.o" "backupframe/o/init.o" "desktopattachmentreader/o/getencryptedattachment.o" "memfiledb/o/statics.o" "sqlitedb/o/valueasstring.o" "sqlitedb/o/prettyprint.o" "sqlitedb/o/renamecolumn.o" "sqlitedb/o/availablewidth.o" "sqlitedb/o/removecolumn.o" "sqlitedb/o/valueasint.o" "sqlitedb/o/copydb.o" "sqlitedb/o/print.o" "sqlitedb/o/printlinemode.o" "stickerframe/o/statics.o" "csvreader/o/readrow.o" "csvreader/o/read.o" "o/main.o" "headerframe/o/statics.o" "desktopdatabase/o/init.o" "reactionlist/o/setauthor.o" "jsondatabase/o/jsondatabase.o" "fileencryptor/o/init.o" "fileencryptor/o/encryptframe.o" "fileencryptor/o/fileencryptor.o" "fileencryptor/o/encryptattachment.o" "filedecryptor/o/getframe.o" "filedecryptor/o/getframebrute.o" "filedecryptor/o/filedecryptor.o" "filedecryptor/o/customs.o" "filedecryptor/o/initbackupframe.o" "arg/o/usage.o" "arg/o/arg.o" "cryptbase/o/getbackupkey.o" "cryptbase/o/getcipherandmac.o" -lcrypto -lsqlite3 $EXTRALINKOPTIONS -o "signalbackup-tools"
    if [ $? -ne 0 ] ; then exit 1 ; fi
fi

