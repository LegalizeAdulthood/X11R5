/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *substr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/substr.c,v 2.4 1991/09/12 15:49:18 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/substr.c,v 2.4 1991/09/12 15:49:18 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/substr.c,v $
*/
#include <ms.h>

char   *
        FindSubString (sub, full)
char   *sub,
       *full;
{
    int     slen,
            tries;

    debug(1,("FindSubString %s %s\n", sub, full));
    slen = strlen(sub);
    tries = strlen(full) - slen;
    while (tries-- >= 0) {
	if (*full == *sub && !strncmp(full, sub, slen))
	    return(full);
	++full;
    }
    return(NULL);
}
