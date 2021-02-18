/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *errstkop_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errstkop.c,v 2.6 1991/09/12 17:10:39 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errstkop.c,v 2.6 1991/09/12 17:10:39 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/errstkop.c,v $ 
 */

#include <errstkop.h>

void            eliTraceStk_Purge(st, s)	/* Pop stuff off until empty */
EliState_t     *st;
eliTraceStack_t *s;
{
    while (eliTraceStk_Top(s))
	eliTraceStk_Pop(st, s);
}

int             eliTraceStk_PurgeN(st, s, n)	/* Pop n items off stack,
						 * returns # popped */
EliState_t     *st;
eliTraceStack_t *s;
int n;
{
    int             popped = 0, i = n;

    while (eliTraceStk_Top(s) && i--) {
	eliTraceStk_Pop(st, s);
	++popped;
    }
    return (popped);
}

EliStr_t       *eliStr_GetNew_trace(st, s, string)
EliState_t     *st;
eliTraceStack_t *s;
char           *string;
{
    EliStr_t       *tmp;

    tmp = eliStr_GetNew(st, string);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushStr(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliStr_GetNew_trace (tracing allocation)]", 0);
        eliStr_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

EliSym_t       *eliSym_GetNew_trace(st, s, strnode)
EliState_t     *st;
eliTraceStack_t *s;
EliStr_t       *strnode;
{
    EliSym_t       *tmp;

    tmp = eliSym_GetNew(st, strnode);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushSym(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliSym_GetNew_trace (tracing allocation)]", 0);
        eliSym_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

EliCons_t      *eliCons_GetNew_trace(st, s)
EliState_t     *st;
eliTraceStack_t *s;
{
    EliCons_t      *tmp;

    tmp = eliCons_GetNew(st);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushCons(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliCons_GetNew_trace (tracing allocation)]", 0);
        eliCons_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

EliSexp_t      *eliSexp_GetNew_trace(st, s)
EliState_t     *st;
eliTraceStack_t *s;
{
    EliSexp_t      *tmp;

    tmp = eliSexp_GetNew(st);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushSexp(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliSexp_GetNew_trace (tracing allocation)]", 0);
        eliSexp_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

EliFn_t        *eliFn_GetNew_trace(st, s)
EliState_t     *st;
eliTraceStack_t *s;
{
    EliFn_t        *tmp;

    tmp = eliFn_GetNew(st);
    if (EliErr_ErrP(st))
        return (NULL);
    if (!eliTraceStk_PushFn(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliFn_GetNew_trace (tracing allocation)]", 0);
        eliFn_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

eliBucketNode_t *eliBucketNode_GetNew_trace(st, s, datum, key)
EliState_t     *st;
eliTraceStack_t *s;
EliSexp_t      *datum;
char           *key;
{
    eliBucketNode_t *tmp;

    tmp = eliBucketNode_GetNew(st, datum, key);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushBucketNode(st, s, tmp)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliBucketNode_GetNew_trace (tracing allocation)]", 0);
        eliBucketNode_DecrRefcount(st, tmp);
        return (NULL);
    }
    return (tmp);
}

EliSexp_t      *eliGetSexp_trace(st, s)
EliState_t     *st;
eliTraceStack_t *s;
{
    EliSexp_t      *result;

    result = eliGetSexp(st);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushSexp(st, s, result)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliGetSexp_trace (tracing allocation)]", 0);
        eliSexp_DecrRefcount(st, result);
        return (NULL);
    }
    return (result);
}

EliSexp_t      *eliFGetSexp_trace(st, s, fp)
EliState_t     *st;
eliTraceStack_t *s;
FILE           *fp;
{
    EliSexp_t      *result;

    result = eliFGetSexp(st, fp);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushSexp(st, s, result)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliFGetSexp_trace (tracing allocation)]", 0);
        eliSexp_DecrRefcount(st, result);
        return (NULL);
    }
    return (result);
}

EliSexp_t      *eliSGetSexp_trace(st, s, string)
EliState_t     *st;
eliTraceStack_t *s;
char           *string;
{
    EliSexp_t      *result;

    result = eliSGetSexp(st, string);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!eliTraceStk_PushSexp(st, s, result)) {
        EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliSGetSexp_trace (tracing allocation)]", 0);
        eliSexp_DecrRefcount(st, result);
        return (NULL);
    }
    return (result);
}
