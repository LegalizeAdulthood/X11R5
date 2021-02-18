/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * File: linkl.h
 *
 */

#ifndef LINKL_H
#define LINKL_H

#include    "common.h"

typedef struct _LinkLeaf
{
    struct  _LinkLeaf*	next;
    struct  _LinkLeaf*	prev;
    Pointer		    contents;
}
    LinkLeaf;

typedef struct
{
    LinkLeaf		    *top;
    LinkLeaf		    *bottom;
    LinkLeaf		    *current;
}
    LinkList;

#define ListIsEmpty(list)		    ((list)->top == (LinkLeaf *)(list))
#define TopOfList(list)			    (list)->top->contents
#define CurrentContentsOfList(list)	    (list)->current->contents

#define ForAllInList(list)  for						    \
			    (						    \
				((list)->current) = ((list)->top);	    \
				((list)->current) != (LinkLeaf *)(list);    \
				((list)->current) = ((list)->current)->next \
			    )


/* function prototypes: */
/* linkl.c: */
Global void initList P((LinkList *list ));
Global void prependToList P((LinkList *list , Pointer contents ));
Global void appendToList P((LinkList *list , Pointer contents ));
Global Pointer deleteFirst P((LinkList *list ));
Global Pointer deleteLast P((LinkList *list ));
Global Pointer deleteCurrent P((LinkList *list ));
Global void AppendToCurrent P((LinkList *list , Pointer contents ));
Global void PrependToCurrent P((LinkList *list , Pointer contents ));
Global void deleteList P((LinkList *list ));
Global void freeList P((LinkList *list ));
Global void freeCurrent P((LinkList *list ));
Global Bool freeMatchingLeaf P((LinkList *list , Pointer contents ));
Global LinkLeaf *findMatchingLeaf P((LinkList *list , Pointer contents ));
Global int indexOfContents P((LinkList *list , Pointer contents ));
Global Pointer getIndexedContents P((LinkList *list , int index ));
Global void freeLeaf P((LinkList *list , LinkLeaf *leaf ));
Global Pointer deleteLeaf P((LinkList *list , LinkLeaf *leaf ));

/* end function prototypes */

#endif	/* LINKL_H */
