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
 * File: linkl.c
 *
 * Description: Functions implementing general linked lists
 */

#include "common.h"
#include "linkl.h"

/* function prototypes: */
/* linkl.c: */
static void linkl_panic P((char *s ));

/* end function prototypes */

Global void
initList(list)
     LinkList	*list;
{
    list->top = list->bottom = list->current = (LinkLeaf *)list;
}

Global void
prependToList(list, contents)
     LinkList	*list;
     Pointer	    contents;
{
    LinkLeaf	    *leaf;

    if ((leaf = Tmalloc(LinkLeaf)) == NULL)
	linkl_panic("prependList: malloc failed\n");
    else
    {
	leaf->prev = (LinkLeaf *)list;
	leaf->next = list->top;
	list->top = list->top->prev = leaf;
	leaf->contents = contents;
    }
}

Global void
appendToList(list, contents)
     LinkList	*list;
     Pointer	    contents;
{
    LinkLeaf	    *leaf;

    if ((leaf = Tmalloc(LinkLeaf)) == NULL)
	linkl_panic("appendToList: malloc failed\n");
    else
    {
	leaf->next = (LinkLeaf *)list;
	leaf->prev = list->bottom;
	list->bottom = list->bottom->next = leaf;
	leaf->contents = contents;
    }
}

Global Pointer
deleteFirst(list)
     LinkList	*list;
{
    LinkLeaf	    *leaf = list->top;
    Pointer	    contents;

    if (leaf == (LinkLeaf *)list)
	return ((Pointer)NULL);
    list->top = leaf->next;
    leaf->next->prev = (LinkLeaf *)list;
    if (list->current == leaf)
	list->current = leaf->next;
    contents = leaf->contents;
    Tfree(leaf);
    return (contents);
}

Global Pointer
deleteLast(list)
     LinkList	*list;
{
    LinkLeaf	    *leaf = list->bottom;
    Pointer	    contents;

    if (leaf == (LinkLeaf *)list)
	return ((Pointer)NULL);
    list->bottom = leaf->prev;
    leaf->prev->next = (LinkLeaf *)list;
    if (list->current == leaf)
	list->current = leaf->prev;
    contents = leaf->contents;
    Tfree(leaf);
    return(contents);
}

Global Pointer
deleteCurrent(list)
     LinkList	*list;
{
    LinkLeaf	    *leaf = list->current;
    Pointer	    contents;

    if (leaf == (LinkLeaf *)list)
	return ((Pointer)NULL);
    list->current = leaf->prev->next = leaf->next;
    leaf->next->prev = leaf->prev;
    contents = leaf->contents;
    Tfree(leaf);
    return(contents);
}

Global void
AppendToCurrent(list, contents)
     LinkList	*list;
     Pointer	    contents;
{
    LinkLeaf	    *leaf;

    if ((leaf = Tmalloc(LinkLeaf)) == NULL)
	linkl_panic("AppendToCurrent: malloc failed\n");
    else
    {
	leaf->prev = list->current;
	leaf->next = list->current->next;
	leaf->contents = contents;
	list->current->next = list->current->next->prev = leaf;
    }
}

Global void
PrependToCurrent(list, contents)
     LinkList	*list;
     Pointer	    contents;
{
    LinkLeaf	    *leaf;

    if ((leaf = Tmalloc(LinkLeaf)) == NULL)
	linkl_panic("PrependToCurrent: malloc failed\n");
    else
    {
	leaf->next = list->current;
	leaf->prev = list->current->prev;
	leaf->contents = contents;
	list->current->prev = list->current->prev->next = leaf;
    }
}

Global void
deleteList(list)
    LinkList  *list;
{
    while (!ListIsEmpty(list))
	deleteFirst(list);
}

Global void
freeList(list)
    LinkList  *list;
{
    while (!ListIsEmpty(list))
	Tfree(deleteFirst(list));
}

Global void
freeCurrent(list)
    LinkList  *list;
{
    Tfree(deleteCurrent(list));
}

Global Bool
freeMatchingLeaf(list, contents)
    LinkList	    *list;
    Pointer	    contents;
{
    LinkLeaf	    *leaf;

    for(leaf = list->top; leaf != (LinkLeaf *)list; leaf = leaf->next)
	if (leaf->contents == contents)
	{
	    freeLeaf(list, leaf);
	    return True;
	}
    return False;
}

Global LinkLeaf*
findMatchingLeaf(list, contents)
    LinkList	    *list;
    Pointer	    contents;
{
    LinkLeaf	    *leaf;

    for(leaf = list->top; leaf != (LinkLeaf *)list; leaf = leaf->next)
	if (leaf->contents == contents)
	    return leaf;
    return (LinkLeaf *)NULL;
}

Global int
indexOfContents(list, contents)
    LinkList	    *list;
    Pointer	    contents;
{
    LinkLeaf	    *leaf;
    int		    index = 0;

    for(leaf = list->top; leaf != (LinkLeaf *)list; leaf = leaf->next, index++)
	if (leaf->contents == contents)
	    return index;
    return -1;
}

Global Pointer
getIndexedContents(list, index)
    LinkList	    *list;
    int		    index;
{
    LinkLeaf	    *leaf;
    int		    count = 0;

    for(leaf = list->top; leaf != (LinkLeaf *)list; leaf = leaf->next, count++)
	if (count == index)
	    return leaf->contents;
    return (Pointer)-1;
}

Global void
freeLeaf(list, leaf)
    LinkList	    *list;
    LinkLeaf	    *leaf;
{
    Tfree(deleteLeaf(list, leaf));
}

Global Pointer
deleteLeaf(list, leaf)
    LinkList	    *list;
    LinkLeaf	    *leaf;
{
    Pointer	    contents;

    if (leaf == (LinkLeaf *)list)
	return ((Pointer)NULL);
    else if (leaf == list->current)
	return deleteCurrent(list);
    else
    {
	leaf->prev->next = leaf->next;
	leaf->next->prev = leaf->prev;
	contents = leaf->contents;
	Tfree(leaf);
	return(contents);
    }
}

static void
linkl_panic(s)
    char   *s;
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}
