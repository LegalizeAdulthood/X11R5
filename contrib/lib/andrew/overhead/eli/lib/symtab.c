/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *symtab_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/symtab.c,v 2.4 1991/09/12 17:12:33 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/symtab.c,v 2.4 1991/09/12 17:12:33 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/symtab.c,v $ 
 */

#include  <symtab.h>

/*
 * Functions to handle manipulation of the global symbol table (basically,
 * front-end to ht.c) MOST OF THESE ASSUME THAT SYM'S ARE IN THE TABLE 
 */

EliSym_t       *eliSymTab_Find(symtab, name)
eliHashTable_t *symtab;
char           *name;
{
    EliSexp_t      *tmp = eliHT_Find(symtab, name);

    return (tmp ? tmp->data.datum.symval : NULL);
}


/*
  * Like eliSymTab_Find, but if it doesn't already exist, it is created,
  * with NULL as its value.  This is as opposed to symtab_getcheck, which
  * binds a value even to an existing node. 
  */

EliSym_t       *eliSymTab_FindOrMake(st, symtab, name)
EliState_t     *st;
eliHashTable_t *symtab;
char           *name;
{
    EliSym_t       *result;

    if (!(result = eliSymTab_Find(symtab, name))) {
	result = eliSymTab_Make(st, symtab, name);
	if (EliErr_ErrP(st))
	    return (NULL);
    }
    return (result);
}

EliSym_t *eliSymTab_Make(st, tab, name)
EliState_t *st;
eliHashTable_t *tab;
char *name;
{
    EliSym_t *tmp;
    EliStr_t *strtmp;
    EliSexp_t *nodetmp;

    strtmp = eliStringTable_FindOrMake(st, EliStringTable(st), name);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!(tmp = eliSym_GetNew_trace(st, EliTraceStk(st), strtmp)))
	return (NULL);
    eliSym_SetScope(tmp, e_sym_known);
    if (!(nodetmp = eliSexp_GetNew_trace(st, EliTraceStk(st))))
	return (NULL);
    EliSexp_SetSym(st, nodetmp, tmp);
    eliHT_Insert(st, tab, nodetmp, name);
    if (EliErr_ErrP(st))
	return (NULL);
    return (tmp);
}

/*
  * Check if node with name "name" is in symtab.  If so, bind the value to it.
      * If not, make a new node and bind the value, then place it in symtab.
      * Return the symnode to which the value was bound (probably rarely used). 
	*/
EliSym_t       *eliSymTab_FindOrMakeAndBind(st, symtab, name, val)
EliState_t     *st;
eliHashTable_t *symtab;
char           *name;
EliSexp_t      *val;
{
    EliSym_t       *tmp;

    if (tmp = eliSymTab_Find(symtab, name))
	EliSym_BindSexp(st, tmp, val);
    else {
	tmp = eliSymTab_MakeAndBind(st, symtab, name, val);
	if (EliErr_ErrP(st))
	    return (NULL);
    }
    return (tmp);
}

/*
  * Like eliSymTab_FindOrMakeAndBind, but always creates a new node (doesn't check for
      * preexistence) 
*/

EliSym_t       *eliSymTab_MakeAndBind(st, symtab, name, val)
EliState_t     *st;
eliHashTable_t *symtab;
char           *name;
EliSexp_t      *val;
{
    EliSym_t       *tmp;
    EliSexp_t      *nodetmp;
    EliStr_t       *strtmp;

    strtmp = eliStringTable_FindOrMake(st, EliStringTable(st), name);
    if (EliErr_ErrP(st))
	return (NULL);
    if (!(tmp = eliSym_GetNew_trace(st, EliTraceStk(st), strtmp)))
	return (NULL);
    eliSym_SetScope(tmp, e_sym_known);
    EliSym_BindSexp(st, tmp, val);
    if (!(nodetmp = eliSexp_GetNew_trace(st, EliTraceStk(st))))
	return (NULL);
    EliSexp_SetSym(st, nodetmp, tmp);
    eliHT_Insert(st, symtab, nodetmp, name);
    if (EliErr_ErrP(st))
	return (NULL);
    return (tmp);
}
