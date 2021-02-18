#!/bin/csh
## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

# Usage:  openwin.csh dirname
#
echo "converting fonts in $1 to OpenWindows format"
cd $1
if ($#argv == 2) then
	set builder = $2
else
	if ( $?OPENWINHOME ) then
		set builder = $(OPENWINHOME)/bin/bldfamily
	else
		if ( -e /usr/openwin/bin ) then
			set builder = /usr/openwin/bin/bldfamily
		else
			set builder = bldfamily
		endif
	endif
endif

if ( -e ../X11fonts/fonts.alias ) then
	awk '{printf("/%s /%s _FontDirectorySYN\n",$1,$2)}' <../X11fonts/fonts.alias >Synonyms.list
endif

${builder} -f 200
echo "Done converting to OpenWindows font format."
exit(0)
