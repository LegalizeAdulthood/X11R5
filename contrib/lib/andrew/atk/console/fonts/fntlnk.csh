#!/bin/csh -f
## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

#
# This script is only needed for files that are used by WM.  
# The fonts.alias file is used on X11 systems
#

set FONTDIR=$1
set LN="$2"

cd ${FONTDIR}
set nonomatch
rm -f console*.fwm

set NAMELIST=con*.fwm
foreach NAME ($NAMELIST)
	set LNAME=`echo $NAME | sed -e s+con+console+g`
	rm -f ${FONTDIR}/${LNAME}
	(set echo;${LN} ${NAME} ${FONTDIR}/${LNAME})
end

