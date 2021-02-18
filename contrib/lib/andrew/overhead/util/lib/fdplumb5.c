/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb5.c,v 2.5 1991/09/12 17:25:26 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb5.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <fdplumbi.h>

dbg_t2open(name, argv, r, w)
char *name, *argv[];
FILE **r, **w;
{
    int code;

    code = t2open(name, argv, r, w);
    if (!code) {
	RegisterOpenFile(fileno(*w), "t2-r", FDLEAK_OPENCODE_T2OPEN);
	RegisterOpenFile(fileno(*r), "t2-w", FDLEAK_OPENCODE_T2OPEN);
    }
    return(code);
}


dbg_t2close(fp, seconds, timedout)
FILE *fp;
int seconds, *timedout;
{
    RegisterCloseFile(fileno(fp));
    return(t2close(fp, seconds, timedout));
}


