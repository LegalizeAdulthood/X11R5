#!/bin/csh -f
## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##


#
# %%%%% this file is used for AIX on RTs because
# the AIX implementation does such a bizzare job
# of handling font names.  Please remove this as soon
# possible.
#

#
# This script is used to map the short file names used to hold 
# font descriptions to reasonable names.  A mechanism in X
# is used.  The various aliases are placed in "fonts.alias" in 
# the directory that holds the font descriptions.
#

set FONTDIR=$1
set EXTENSION=$2
set TMP=/tmp/tmp.a.$$
set OUTFILE=fonts.alias



echo "indexing fonts in $FONTDIR using extension $EXTENSION"

pushd ${FONTDIR}
echo >! ${TMP}

#
# First do the adobe fonts from overhead/fonts/fonts/adobe.
# We alias the old, long names too just in case.
#
foreach REAL (cou hel symb tim)
	switch (${REAL})
		case "cou":
			set LINK1="andytype"
			set LINK2="courier"
			breaksw
		case "hel":
			set LINK1="andysans"
			set LINK2="helvetica"
			breaksw
		case "symb":
			set LINK1="andysymbol"
			set LINK2="symbol"
			breaksw
		case "tim":
			set LINK1="andy"
			set LINK2="times"
			breaksw
		default:
			echo "ERROR LINKING FONTS (${REAL})"
			exit(1)
	endsw
	set NAMELIST=`echo ${REAL}*${EXTENSION}`
	foreach NAME ($NAMELIST)
		set FNAME=`basename $NAME $EXTENSION`
		set NAME1=`echo ${FNAME} | sed -e s+$REAL+$LINK2+g`
		set NAME2=`echo ${FNAME} | sed -e s+$REAL+$LINK1+g`
		echo $NAME1	$NAME | tee -a ${TMP}
		echo $NAME2	$NAME | tee -a ${TMP}
	end
end

#
# Now do all the ATK program fonts from atk/...
#
foreach REAL (msgs con)
	switch (${REAL})
		case "msgs":
			set LINK="messages"
			breaksw
		case "con":
			set LINK="console"
			breaksw
		default:
			echo "ERROR LINKING FONTS (${REAL})"
			exit(1)
	endsw
	set NAMELIST=`echo ${REAL}*${EXTENSION}`
	foreach NAME ($NAMELIST)
		set FNAME=`basename $NAME $EXTENSION`
		set LNAME=`echo ${FNAME} | sed -e s+$REAL+$LINK+g`
		echo "${LNAME}	${NAME}" | tee -a ${TMP}
	end
end


# We don't really need this alias for ALL the andy fonts...
echo "andrew120	times120.rtx" | tee -a ${TMP}

cat ${TMP} >> ${OUTFILE}
rm -f ${TMP}
popd

