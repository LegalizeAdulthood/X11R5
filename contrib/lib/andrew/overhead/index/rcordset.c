/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/rcordset.c,v 2.5 1991/09/12 17:13:39 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/rcordset.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/rcordset.c,v 2.5 1991/09/12 17:13:39 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h> /* sys/types.h sys/file.h */
#include <sys/dir.h>
#include <stdio.h>

#include "index.h"

/*
 * Internal routine: return a new, empty hash list.  Empty hash lists are not represented
 * by null pointers, but rather a hash list with a zero entry count.
 */
struct hashList *index_NewHL()
{
    register struct hashList *th;
    th = (struct hashList *) malloc (sizeof (struct hashList));
    th->next = (struct hashList *) 0;
    th->nentries = 0;
    return th;
}

/*
  * Internal routine: given a hash list and a hash value, return true if the value is
      * contained in the list, and false otherwise.
      */
index_HashPresent(alist, ahash)
register struct hashList *alist;
register long ahash;
{
    register int i;
    for(;alist;alist=alist->next) {
	for(i=0;i<alist->nentries;i++) {
	    if (ahash == alist->entries[i]) return 1;
	}
    }
    return 0;
}

/*
  * Internal routine: given a hash list and a hash value, delete the value from
  * the hash list.
  */
index_HashRemove(alist, ahash)
register struct hashList *alist;
register long ahash;
{
    register int i;
    for(;alist;alist=alist->next) {
	for(i=0;i<alist->nentries;i++) {
	    if (ahash == alist->entries[i]) alist->entries[i] = 0xffff;
	}
    }
    return 0;
}

/*
  * Internal routine: given a hash list and a hash value, add the value to the list.
  */
index_HashAdd(alist, ahash)
register struct hashList *alist;
register long ahash;
{
    register struct hashList *tlist;
    for(tlist=alist;tlist;tlist=tlist->next) {
	if (tlist->nentries < MAXHL) {
	    tlist->entries[tlist->nentries++] = ahash;
	    return;
	}
    }
    tlist = (struct hashList *) malloc(sizeof(struct hashList *));
    tlist->nentries = 1;
    tlist->next = alist->next;
    alist->next = tlist;
    tlist->entries[0] = ahash;
}

/*
  * Internal routine: given a hash list, free it.
  */
index_FreeHL(alist)
register struct hashList *alist;
{
    register struct hashList *next;
    for(;alist;alist=next) {
	next = alist->next;
	free(alist);
    }
}

/*
  * Create a new, empty record set.
  */
struct recordSet *recordset_New(asize)
{
    register struct recordSet *tr;
    if (asize <= 0) asize = 1;
    tr = (struct recordSet *) malloc(sizeof(struct recordSet));
    tr->count = 0;
    tr->acount = asize;
    tr->data = (struct recordID *) malloc(asize * sizeof(struct recordID));
    return tr;
}

/*
  * Free a record set.
  */
recordset_Free(aset)
register struct recordSet *aset;
{
    free(aset->data);
    free(aset);
}

/*
  * Add a record id to a record set.
  */
recordset_Add(aset, arid)
register struct recordSet *aset;
register struct recordID *arid;
{
    register long c;
    register struct recordID *tid;
    long newSize;
    for(tid=aset->data,c=0; c<aset->count; tid++,c++) {
	if (req(*tid, *arid)) return;
    }
    c = aset->count;
    if (c>= aset->acount) {
	newSize = 2*c;
	aset->data = (struct recordID *) realloc(aset->data, newSize*sizeof(struct recordID));
	aset->acount = newSize;
    }
    rset(aset->data[c], *arid);
    aset->count = c+1;
}

/* more recordset operations to come as they become necessary */
