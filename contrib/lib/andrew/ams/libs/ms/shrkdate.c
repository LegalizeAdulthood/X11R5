/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *shrkdate_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/shrkdate.c,v 2.5 1991/09/12 15:48:31 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/shrkdate.c,v 2.5 1991/09/12 15:48:31 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/shrkdate.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/time.h */


char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
	"Sep", "Oct", "Nov", "Dec", 0};


ParseAndShrinkDate(bigdate, littledate) 
char *bigdate, *littledate;
{
    struct tm TmBuf;

    debug(1, ("ParseAndShrinkDate %s\n", bigdate));
    if (parsedateheader(bigdate, &TmBuf, 1, 1, 1, 0)) {
	strcpy(littledate, "Bad Date");
	return(-1);
    } else {
	sprintf(littledate, "%2d-%s-%02d", TmBuf.tm_mday,
	    months[TmBuf.tm_mon], TmBuf.tm_year);
	return(0);
    }
}

