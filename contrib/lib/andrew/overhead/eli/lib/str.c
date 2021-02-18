/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *str_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/str.c,v 2.5 1991/09/12 17:12:21 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/str.c,v 2.5 1991/09/12 17:12:21 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/lib/RCS/str.c,v $ 
 */

#include  <str.h>

/*
 * Returns the first strnode in the free area.  If one does not exist, a new
 * block of strnodes is allocated and linked into the freelist, and a strnode
 * is returned. If the allocation fails, an eli internal error occurs. 
 *
 * NOTE: Initialization is performed on the returned node. 
 */

EliStr_t       *eliStr_GetNew(st, string)
EliState_t     *st;
char           *string;
{
    EliStr_t       *tmp = NULL;
    EliSexp_t      *nodetmp;

    switch (st->whichScheme) {
	case e_mem_malloc:
	    tmp = (EliStr_t *) malloc(sizeof(EliStr_t));
	    break;
	case e_mem_pool:
	    if (!(st->g_str_freelist)) {
		if (!(st->g_str_freelist = eliStr_GetNewBlock(STR_BLOCKSIZE))) {
		    EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliStr_GetNew (allocating block)]", 0);
		    return (NULL);
		}
		else
		    st->numTotalStrNodes += STR_BLOCKSIZE;
	    }
	    tmp = st->g_str_freelist;
	    st->g_str_freelist = tmp->freelink;
	    break;
    }
    if (tmp) {
	tmp->data.refcount = 0;
	if (!(tmp->data.string = (char *) malloc(1 + strlen(string)))) {
	    EliError(st, ELI_ERR_OUT_OF_MEM, NULL, "INTERNAL [eliStr_GetNew (allocating string buffer)]", 0);
	    switch (st->whichScheme) {
		case e_mem_malloc:
		    free(tmp);
		    break;
		case e_mem_pool:
		    tmp->freelink = st->g_str_freelist;	/* Return the new
							 * node... */
		    st->g_str_freelist = tmp;	/* ...to the pool */
		    break;
	    }
	    return (NULL);
	}
	strcpy(tmp->data.string, string);
	if (!(nodetmp = eliSexp_GetNew_trace(st, EliTraceStk(st)))) {
	    free(tmp->data.string);
	    switch (st->whichScheme) {
		case e_mem_malloc:
		    free(tmp);
		    break;
		case e_mem_pool:
		    tmp->freelink = st->g_str_freelist;	/* Return the new
							 * node... */
		    st->g_str_freelist = tmp;	/* ...to the pool */
		    break;
	    }
	    return (NULL);
	}
	/* Add the strnode to the global string table */
	EliSexp_SetStr(st, nodetmp, tmp);
	eliHT_Insert(st, EliStringTable(st), nodetmp, string);
	if (EliErr_ErrP(st))
	    return (NULL);	/* BOGUS AS ALL HELL! */
	++(st->numStrNodes);
    }
    return (tmp);
}

/*
 * Allocates a block of strnodes (the number of nodes to allocate is given as
 * the argument).  Sets up freelinks within the block and returns a pointer
 * to the first cell in the block, which should also be the first node in
 * this block's freelist. 
 */

EliStr_t       *eliStr_GetNewBlock(numnodes)
int             numnodes;
{
    EliStr_t       *result;
    int             i;

    result = (EliStr_t *) malloc(numnodes * sizeof(EliStr_t));
    if (result) {
	for (i = 0; i < (numnodes - 1); ++i)
	    (result + i)->freelink = result + i + 1;
	(result + numnodes - 1)->freelink = NULL;
    }
    return (result);
}

int             eliStr_DecrRefcount(st, node)
EliState_t     *st;
EliStr_t       *node;
{
    int             result;

    if (1 > (result = (--(node->data.refcount)))) {
	--(st->numStrNodes);
	free(node->data.string);
	switch (st->whichScheme) {
	    case e_mem_malloc:
		free(node);
		break;
	    case e_mem_pool:
		node->freelink = st->g_str_freelist;
		st->g_str_freelist = node;
		break;
	}
    }
    else if (result == 2)	/* Make sure this stays right */
	eliHT_Delete(st, EliStringTable(st), node->data.string);
    return (result);
}

void            eliStr_IncrRefcount(node)
EliStr_t       *node;
{
    ++(node->data.refcount);
}

char           *EliStr_GetString(node)
EliStr_t       *node;
{
    return (node->data.string);
}
