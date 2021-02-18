/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *errops_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errops.c,v 2.6 1991/09/12 17:10:32 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errops.c,v 2.6 1991/09/12 17:10:32 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errops.c,v $ 
 */

#include <errops.h>

/* These are front-end routines to errnode operations */

int             EliErr_ErrP(st)	/* Has an error occurred? */
EliState_t     *st;
{
    return (st->g_errflag);
}

int             EliErr_ErrCode(st)
EliState_t     *st;
{
    return (eliErr_GetCode(EliErrNode(st)));
}

int EliErr_UnixErr(st)
EliState_t *st;
{
    return (eliErr_GetUnixErr(EliErrNode(st)));
}

EliSexp_t      *EliErr_BadSexp(st)
EliState_t     *st;
{
    return (eliErr_GetNode(EliErrNode(st)));
}

int             EliErr_BadSexpP(st)
EliState_t     *st;
{
    return (eliErr_GetNode(EliErrNode(st)) != NULL);
}

char           *EliErr_ErrLoc(st)
EliState_t     *st;
{
    return (eliErr_GetLoc(EliErrNode(st)));
}

int EliErr_BacktraceP(st)
EliState_t *st;
{
    return (eliErr_GetBacktrace(EliErrNode(st)) != NULL);
}

EliCons_t *EliErr_Backtrace(st)
EliState_t *st;
{
    return (eliErr_GetBacktrace(EliErrNode(st)));
}
