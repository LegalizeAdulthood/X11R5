/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *errnode_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errnode.c,v 2.6 1991/09/12 17:10:28 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errnode.c,v 2.6 1991/09/12 17:10:28 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errnode.c,v $ 
 */

#include <errnode.h>

void            eliErr_Set(st, e, code, node, loc, unixerr)
EliState_t     *st;
eliErrStuff_t  *e;
int             code;
EliSexp_t      *node;
char           *loc;
{
    EliSexp_t      *tmp = e->badnode;

    e->errnum = code;
    e->badnode = node;
    e->errloc = loc;
    e->unixerr = unixerr;
    if (node)
	eliSexp_IncrRefcount(node);
    if (tmp)
	eliSexp_DecrRefcount(st, tmp);
}

int eliErr_GetUnixErr(e)
eliErrStuff_t *e;
{
    return (e->unixerr);
}

int             eliErr_GetCode(e)
eliErrStuff_t  *e;
{
    return (e->errnum);
}

EliSexp_t      *eliErr_GetNode(e)
eliErrStuff_t  *e;
{
    return (e->badnode);
}

EliCons_t *eliErr_GetBacktrace(e)
eliErrStuff_t *e;
{
    return (e->backtrace);
}

char           *eliErr_GetLoc(e)
eliErrStuff_t  *e;
{
    return (e->errloc);
}

int             eliErr_SexpP(e)
eliErrStuff_t  *e;
{
    return ((e->badnode) != NULL);
}

void            eliErr_Init(e)
eliErrStuff_t  *e;
{
    e->badnode = NULL;
    e->backtrace = NULL;
}
