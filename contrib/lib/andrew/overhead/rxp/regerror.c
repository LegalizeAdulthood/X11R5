/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *regerror_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/rxp/RCS/regerror.c,v 2.6 1991/09/12 17:18:15 bobg Exp $";


/* Modified 14-Nov-89 by bobg@andrew.cmu.edu */

#include <stdio.h>

static void (*errfn)() = ((void (*) ()) 0);

void (*regerrhandler(fn))()
void (*fn)();
{
    void (*oldfn)() = errfn;

    errfn = fn;
    return (oldfn);
}

void            regerror(s)
char           *s;
{
    if (errfn == ((void (*) ()) 0))
        fprintf(stderr, "regexp: %s\n", s);
    else
        (*errfn)(s);
}
