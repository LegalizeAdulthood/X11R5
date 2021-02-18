/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *stack_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/stack.c,v 2.5 1991/09/12 17:12:10 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/stack.c,v 2.5 1991/09/12 17:12:10 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/stack.c,v $ 
 */

#include  <stack.h>

/*
 * Functions to handle manipulation of the global stack 
 */

/*
 * Evaluates the items in arglist and binds them to the symbols named in
 * varlist.  Bound symbols are placed on stack s.  Note: Each var name gives
 * rise to a new symbol with the same name, just in case the original points
 * to a global symbol (which we don't want to bind to). 
 */

int             eliEvalAndBind(st, s, varlist, arglist)
EliState_t     *st;
eliEvalStack_t *s;
EliCons_t      *varlist, *arglist;
{
    int             pushed = 0, varlen = EliListLen(varlist), argsempty = !EliListLen(arglist), i;
    EliCons_t      *varptr = varlist, *argptr = arglist;
    EliSexp_t      *curvar, *curarg = NULL, *resbuf, *tmp;
    EliSym_t       *symtmp;

    if (argsempty) {
	if (!(curarg = eliSexp_GetNew_trace(st, EliTraceStk(st))))
	    return (pushed);
	EliSexp_SetSym(st, curarg, EliNilSym(st));
    }
    for (i = 0; i < varlen; ++i) {
	curvar = EliCons_GetCar(varptr);
	if (EliSexp_GetType(curvar) != e_data_symbol) {
	    EliError(st, ELI_ERR_BAD_PARAM, curvar, "INTERNAL [eliEvalAndBind (non-symbol in var list)]", 0);
	    return (pushed);
	}
	if (!argsempty)
	    curarg = EliCons_GetCar(argptr);
	if (!(resbuf = eliSexp_GetNew_trace(st, EliTraceStk(st))))
	    return (pushed);
	eliEval(st, curarg, resbuf);
	if (EliErr_ErrP(st))
	    return (pushed);
	if (!(symtmp = eliSym_GetNew_trace(st, EliTraceStk(st), EliSym_GetName(EliSexp_GetSym(curvar)))))
	    return (pushed);
	EliSym_BindSexp(st, symtmp, resbuf);
        if (!eliEvalStk_Push(st, s, symtmp)) {
            EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliEvalAndBind (pushing symbol)]", 0);
            return (pushed);
        }
	++pushed;
	if (i < varlen - 1) {
	    tmp = EliCons_GetCdr(varptr);
	    varptr = EliSexp_GetCons(tmp);
	    if (!argsempty) {
		tmp = EliCons_GetCdr(argptr);
		if (EliSexp_GetType(tmp) == e_data_none) {
		    argsempty = TRUE;
		    if (!(curarg = eliSexp_GetNew_trace(st, EliTraceStk(st))))
			return (pushed);
		    EliSexp_SetSym(st, curarg, EliNilSym(st));
		}
		else
		    argptr = EliSexp_GetCons(tmp);
	    }
	}
    }
    return (pushed);
}

int             eliBind(st, s, varlist, arglist)
EliState_t     *st;
eliEvalStack_t *s;
EliCons_t      *varlist, *arglist;
{
    int             pushed = 0, varlen = EliListLen(varlist), argsempty = !EliListLen(arglist), i;
    EliCons_t      *varptr = varlist, *argptr = arglist;
    EliSexp_t      *curvar, *curarg = NULL, *tmp;
    EliSym_t       *symtmp;

    if (argsempty) {
	if (!(curarg = eliSexp_GetNew_trace(st, EliTraceStk(st))))
	    return (pushed);
	EliSexp_SetSym(st, curarg, EliNilSym(st));
    }
    for (i = 0; i < varlen; ++i) {
	curvar = EliCons_GetCar(varptr);
	if (EliSexp_GetType(curvar) != e_data_symbol) {
	    EliError(st, ELI_ERR_BAD_PARAM, curvar, "INTERNAL [eliBind (non-symbol in var list)]", 0);
	    return (pushed);
	}
	if (!argsempty)
	    curarg = EliCons_GetCar(argptr);
	if (!(symtmp = eliSym_GetNew_trace(st, EliTraceStk(st), EliSym_GetName(EliSexp_GetSym(curvar)))))
	    return (pushed);
	EliSym_BindSexp(st, symtmp, curarg);
        if (!eliEvalStk_Push(st, s, symtmp)) {
            EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliBind (pushing symbol)]", 0);
            return (pushed);
        }
	++pushed;
	if (i < varlen - 1) {
	    tmp = EliCons_GetCdr(varptr);
	    varptr = EliSexp_GetCons(tmp);
	    if (!argsempty) {
		tmp = EliCons_GetCdr(argptr);
		if (EliSexp_GetType(tmp) == e_data_none) {
		    argsempty = TRUE;
		    if (!(curarg = eliSexp_GetNew_trace(st, EliTraceStk(st))))
			return (pushed);
		    EliSexp_SetSym(st, curarg, EliNilSym(st));
		}
		else
		    argptr = EliSexp_GetCons(tmp);
	    }
	}
    }
    return (pushed);
}

/* pop n items off the stack */

void            eliEvalStk_PopN(st, s, n)
EliState_t     *st;
eliEvalStack_t *s;
int             n;
{
    int             i;

    for (i = 0; i < n; ++i)
	eliEvalStk_Pop(st, s);
}
