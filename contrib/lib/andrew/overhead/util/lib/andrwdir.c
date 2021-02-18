/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *andrwdir_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/andrwdir.c,v 2.7 1991/09/12 17:23:19 bobg Exp $";

/* andydir.c */

#include <stdio.h>
#include <andrdir.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char *getenv();

/* Return a string with the current value for ANDREWDIR imbedded in it. */
char *AndrewDir(str)
    char *str;
{
    char *p = NULL;
    int addLen;
    static int andyLen = 0;
    static int bufSize = -1;
    static char *buffer;

    if (str != NULL)  {
	addLen = strlen(str);
    }
    else  {
	addLen = 0;
    }

    if (bufSize == -1) {
        if (((p = getenv("ANDREWDIR")) == NULL || *p == '\0' ) && ((p = (char *) GetConfiguration("AndrewDir")) == NULL || *p == '\0'))  {
/*	    p = "/usr/andrew"; */
	    p = QUOTED_DEFAULT_ANDREWDIR_ENV;
	}
	andyLen = strlen(p);

	bufSize = addLen + andyLen + 1;
	if ((buffer = (char *) malloc(bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}
	strcpy(buffer, p);
    }

    if (bufSize < andyLen + addLen + 1)  {
	bufSize = addLen + andyLen + 1;
	if ((buffer = (char *) realloc(buffer, bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}
    }

    if (str != NULL)  {
	strcpy(&(buffer[andyLen]), str);
    }
    else  {
	buffer[andyLen] = '\0';
    }

    return buffer;
}
