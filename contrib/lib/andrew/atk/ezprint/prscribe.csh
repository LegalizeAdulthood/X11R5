#!/bin/csh -f
## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##
# $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ezprint/RCS/prscribe.csh,v 2.4 1991/09/13 16:28:57 bobg Exp $ 
# $ACIS:printscribe 1.2$ 
# $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ezprint/RCS/prscribe.csh,v $ 

#
# Backward compatibility -- a program that acts like the old printscribe,
#   but calls ezprint to do the real work.
# Ezprint understands most of the same arguments as printscribe, but the
#   -d changes to a -z, sigh...

set DELETE=""
set PASSTHRU=""

while ($#argv > 0)
    switch($1)
	case -d:
	    set DELETE="-z"
	    breaksw
	default:
	    set PASSTHRU="$PASSTHRU $1"
	    breaksw
    endsw
    shift
end

exec ezprint $DELETE $PASSTHRU
