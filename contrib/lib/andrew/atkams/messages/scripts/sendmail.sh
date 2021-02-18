#!/bin/sh
## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

FILENAME=/tmp/sm$$
TO=""
SUB=""
if [ $# -gt 0 -a "$1" != . ]; then
    TO="$1"
fi
if [ $# -gt 1 -a "$2" != . ]; then
    SUB="$2"
fi
echo To: $TO > $FILENAME
echo Subject: $SUB >> $FILENAME
echo "CC: " >> $FILENAME
if [ $# -gt 2 -a "$3" != . ]; then
    echo "" >> $FILENAME
    cat $3 >> $FILENAME
fi
exec messages -s -f $FILENAME >/dev/console 2>&1
