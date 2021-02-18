/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/brkdown.c,v 2.7 1991/09/12 15:50:01 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/brkdown.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/brkdown.c,v 2.7 1991/09/12 15:50:01 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char *Next822LPart(), *index();

static char * Useful_Next822LPart(s, e, thing, size)
char *s, *e, *thing;
int size;
{
    while (1) {
	s = Next822LPart(s, e, thing, size);
	if (!s || (s == e) || (*thing != ';')) return(s);
    }
}

BreakDownContentTypeField(HeadBuf, fmt, fmtsz, vers, verssz, resources, resourcessz)
char *HeadBuf, *fmt, *vers, *resources;
int fmtsz, verssz, resourcessz;
{
    char *s, *end, *ptr;
    int len;

    *fmt = *vers = *resources = '\0';
    s = HeadBuf;
    end = HeadBuf + strlen(HeadBuf);
    s = Useful_Next822LPart(s, end, fmt, fmtsz);
    if (s) s = Useful_Next822LPart(s, end, vers, verssz);
    if (s) {
	ptr = resources;
	s = Useful_Next822LPart(s, end, ptr, resourcessz);
	do {
	    len = strlen(ptr);
	    ptr += len;
	    resourcessz -= len;
	    s = Next822LPart(s, end, ptr, resourcessz);
	} while (s && (s != end) && resourcessz > 0);
    }
}

/* This routine chops up a resource list into separate resources, allocating an array of pointers to the separate resources.  The separate resources themselves are not allocated, but are obtained via destructive operations on the original list. */

char **BreakDownResourcesIntoArray(reslist)
char *reslist;
{
    int ct=0;
    char *s=reslist;
    char **rval;
    
    while (s=index(s, ',')) {
	++ct;
	++s;
    }
    rval = (char **) malloc((1+ct) * sizeof(char *));
    if (rval) {
	rval[0] = reslist;
	ct = 0;
	while (s=index(rval[ct++], ',')) {
	    *s++ = '\0';
	    rval[ct] = s;
	}
	rval[ct] = 0;
    }
    return(rval);
}
