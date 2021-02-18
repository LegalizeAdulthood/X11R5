/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *eerror_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/eerror.c,v 2.8 1991/09/12 17:10:23 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/eerror.c,v 2.8 1991/09/12 17:10:23 bobg Exp $
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/eerror.c,v $
 */

#include  <eerror.h>

static char    *eliErrStrs[] =
{
    "Error from ELI client",
    "Out of Memory",
    "Bad Syntax",
    "Unbound Atom",
    "Undefined Function",
    "Bad Function Parameter",
    "Bad Argument",
    "Symbol Does Not Exist",
    "Wrong Number of Arguments to Function",
    "System Error",
    "User Error"
};

/* Definition of EliError, the all-purpose error-handler */

void            EliError(st, errtype, node, loc, unixerr)
EliState_t     *st;
int             errtype;
EliSexp_t      *node;
char           *loc;
int unixerr;
{
    void (*fn)();

    eliErr_Set(st, EliErrNode(st), errtype, node, loc, unixerr);
    st->g_errflag = TRUE;
    if (errtype & EliCatchMask(st)) {
        fn = EliCatchFn(st);
        (*fn)(st);
    }
}

char           *EliErrStr(code)        /* If code is a power of two <= 128,
                                        * this will return a string from
                                        * eliErrStrs. */
int             code;
{
    int             i, j;

    for (i = code, j = 0; i; (i = (i >> 1)), ++j);
    return (eliErrStrs[j]);
}

void            eliyyerror(s)
char           *s;
{
}

int             eliyywrap()
{
    EliProcessInfo.u_wrap = TRUE;
    return (1);
}
