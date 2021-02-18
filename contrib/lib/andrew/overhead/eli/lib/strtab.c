/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *strtab_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/strtab.c,v 2.4 1991/09/12 17:12:24 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/strtab.c,v 2.4 1991/09/12 17:12:24 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/strtab.c,v $ 
 */

#include  <strtab.h>

/*
 * Functions to handle manipulation of the global string table 
 */

/*
 * This function checks to see if the string is in the table.  If so, its
 * node is returned.  If not, a new one is created, placed in the table, and
 * returned. 
 */

EliStr_t       *eliStringTable_FindOrMake(st, strtab, string)
EliState_t     *st;
eliHashTable_t *strtab;
char           *string;
{
    EliStr_t       *tmp;

    if (!(tmp = eliStringTable_Find(strtab, string))) {
	tmp = eliStringTable_Make(st, strtab, string);
	if (EliErr_ErrP(st))
	    return (NULL);
    }
    return (tmp);
}


/*
 * Like eliStringTable_FindOrMake, but always creates a new node. 
 */

EliStr_t       *eliStringTable_Make(st, strtab, string)
EliState_t     *st;
eliHashTable_t *strtab;
char           *string;
{
    EliStr_t       *tmp;

    if (!(tmp = eliStr_GetNew_trace(st, EliTraceStk(st), string)))
	return (NULL);		/* Observe that this test is completely
				 * unnecessary */
    return (tmp);
}


/*
 * Find the strnode whose name is name, in the global table WARNING: Assumes
 * all data in ht are strnodes! 
 */

EliStr_t       *eliStringTable_Find(strtab, name)
eliHashTable_t *strtab;
char           *name;
{
    EliSexp_t      *tmp = eliHT_Find(strtab, name);

    return (tmp ? tmp->data.datum.strval : NULL);
}
