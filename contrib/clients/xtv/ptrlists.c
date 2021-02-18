/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
 *
 * Based on the C++ Ptrlist as used by MFA and SGT
 * in their COMP 240 project in the spring of 1991.
 */

#include <stdio.h>
#include "ptrlists.h"

#ifdef __ANSILIST__
void
newPtrList(PTRLIST *ptrlist)
#else
void newPtrList(ptrlist)
PTRLIST *ptrlist;
#endif
{
  newList((LIST *) ptrlist);
  ptrlist->nitems = 0;
  ptrlist->cursor = (LISTITEM *) Head((LIST *) ptrlist);
  ptrlist->sav_cursor = 0;
}

#ifdef __ANSILIST__
int items(PTRLIST *ptrlist)
#else
int items(ptrlist)
PTRLIST *ptrlist;
#endif
{
  return ptrlist->nitems;
}

#ifdef __ANSILIST__
void addItem(PTRLIST *ptrlist, void *newItem)
#else
void addItem(ptrlist, newItem)
PTRLIST *ptrlist;
void *newItem;
#endif
{
  
  LISTITEM *lp = (LISTITEM *) malloc (sizeof(LISTITEM));
  lp->ln_Val = newItem;

  Insert((LIST *) ptrlist, (NODE *) lp, (NODE *) ptrlist->cursor);
  ptrlist->cursor = lp;
  ptrlist->nitems++;
}

#ifdef __ANSILIST__
void *getItem(PTRLIST *ptrlist)
#else
void *getItem(ptrlist)
PTRLIST *ptrlist;
#endif
{
  if (offright(ptrlist) || offleft(ptrlist))
    return 0;
  else
    return ptrlist->cursor->ln_Val;
}

#ifdef __ANSILIST__
void deleteItem(PTRLIST *ptrlist)
#else
void deleteItem(ptrlist)
PTRLIST *ptrlist;
#endif
{
  LISTITEM *t;
  if (offright(ptrlist) || offleft(ptrlist))
    fprintf(stderr, "ptrlists deleteItem: ERROR: attempt delete when offright or offleft.\n");
  else
    {
      t = (LISTITEM *) Next((NODE *) ptrlist->cursor);
      if (ptrlist->sav_cursor == ptrlist->cursor)
	ptrlist->sav_cursor = 0;
      Remove((NODE *) ptrlist->cursor);
      free(ptrlist->cursor);
      ptrlist->cursor = t;
      ptrlist->nitems--;
      ptrlist->sav_cursor = 0;
    }
}

#ifdef __ANSILIST__
void unshift(PTRLIST *ptrlist, void *newItem)
#else
void unshift(ptrlist, newItem)
PTRLIST *ptrlist;
void *newItem;
#endif
{
  LISTITEM *lp = (LISTITEM *) malloc(sizeof(LISTITEM));
  lp->ln_Val = newItem;

  AddHead((LIST *) ptrlist, (NODE *) lp);
  ptrlist->nitems++;
}

#ifdef __ANSILIST__
void *shift(PTRLIST *ptrlist)
#else
void *shift(ptrlist)
PTRLIST *ptrlist;
#endif
{
  LISTITEM *lp;
  void *p;

  if (offright(ptrlist) && offleft(ptrlist))
    return 0;
  else
    {
      lp = (LISTITEM *) RemHead((LIST *) ptrlist);
      if (ptrlist->sav_cursor == lp)
	ptrlist->sav_cursor = 0;
      if (ptrlist->cursor == lp)
	forth(ptrlist);
      p = lp->ln_Val;
      free(lp);
      ptrlist->nitems--;
      return p;
    }
}

#ifdef __ANSILIST__
void push(PTRLIST *ptrlist, void *newItem)
#else
void push(ptrlist, newItem)
PTRLIST *ptrlist;
void *newItem;
#endif
{
  LISTITEM *lp = (LISTITEM *) malloc(sizeof(LISTITEM));
  lp->ln_Val = newItem;

  AddTail((LIST *) ptrlist, (NODE *) lp);
  ptrlist->nitems++;
}

#ifdef __ANSILIST__
void *pop(PTRLIST *ptrlist)
#else
void *pop(ptrlist)
PTRLIST *ptrlist;
#endif
{
  LISTITEM *lp;
  void *p;

  if (offright(ptrlist) && offleft(ptrlist))
    return 0;
  else
    {
      lp = (LISTITEM *) RemTail((LIST *) ptrlist);
      if (ptrlist->sav_cursor == lp)
	ptrlist->sav_cursor = 0;
      if (ptrlist->cursor == lp)
	back(ptrlist);
      p = lp->ln_Val;
      free(lp);
      ptrlist->nitems--;
      return p;
    }
}

#ifdef __ANSILIST__
void start(PTRLIST *ptrlist)
#else
void start(ptrlist)
PTRLIST *ptrlist;
#endif
{
  ptrlist->cursor = (LISTITEM *) Head((LIST *) ptrlist);
}

#ifdef __ANSILIST__
void finish(PTRLIST *ptrlist)
#else
void finish(ptrlist)
PTRLIST *ptrlist;
#endif
{
  ptrlist->cursor = (LISTITEM *) Tail((LIST *) ptrlist);
}

#ifdef __ANSILIST__
void forth(PTRLIST *ptrlist)
#else
void forth(ptrlist)
PTRLIST *ptrlist;
#endif
{
  if (!offright(ptrlist))
    ptrlist->cursor = (LISTITEM *) Next((NODE *) ptrlist->cursor);
}

#ifdef __ANSILIST__
void back(PTRLIST *ptrlist)
#else
void back(ptrlist)
PTRLIST *ptrlist;
#endif
{
  if (!offleft(ptrlist))
    ptrlist->cursor = (LISTITEM *) ((NODE *) ptrlist->cursor)->ln_Pred;
}

#ifdef __ANSILIST__
void *first(PTRLIST *ptrlist)
#else
void *first(ptrlist)
PTRLIST *ptrlist;
#endif
{
  LISTITEM *lp;

  lp = (LISTITEM *) Head((LIST *) ptrlist);
  if (Next((NODE *) lp) == 0)
    return 0;
  else
    return lp->ln_Val;
}

#ifdef __ANSILIST__
void *last(PTRLIST *ptrlist)
#else
void *last(ptrlist)
PTRLIST *ptrlist;
#endif
{
  LISTITEM *lp;

  lp = (LISTITEM *) Tail((LIST *) ptrlist);
  if (Prev((NODE *) lp) == 0)
    return 0;
  else
    return lp->ln_Val;
}

#ifdef __ANSILIST__
int offright(PTRLIST *ptrlist)
#else
int offright(ptrlist)
PTRLIST *ptrlist;
#endif
{
  int result;
  
  return (((NODE *) (ptrlist->cursor))->ln_Succ == 0);
}

#ifdef __ANSILIST__
int offleft(PTRLIST *ptrlist)
#else
int offleft(ptrlist)
PTRLIST *ptrlist;
#endif
{
  return ((((NODE *) (ptrlist->cursor))->ln_Pred == 0)
      ||  (((LIST *)ptrlist)->lh_Head->ln_Succ == 0));
}

#ifdef __ANSILIST__
void saveCursor(PTRLIST *ptrlist)
#else
void saveCursor(ptrlist)
PTRLIST *ptrlist;
#endif
{
  ptrlist->sav_cursor = ptrlist->cursor;
}

#ifdef __ANSILIST__
void restoreCursor(PTRLIST *ptrlist)
#else
void restoreCursor(ptrlist)
PTRLIST *ptrlist;
#endif
{
  if (ptrlist->sav_cursor)
    ptrlist->cursor = ptrlist->sav_cursor;
}

#ifdef __ANSILIST__
int atSaved(PTRLIST *ptrlist)
#else
int atSaved(ptrlist)
PTRLIST *ptrlist;
#endif
{
  return (ptrlist->cursor == ptrlist->sav_cursor);
}

#ifdef __ANSILIST__
PTRLIST *
ptrNew(void)
#else
PTRLIST *
ptrNew()
#endif
{
  PTRLIST *p;

  p = (PTRLIST *) malloc(sizeof(PTRLIST));
  newPtrList(p);
  return p;
}

#ifdef __ANSILIST__
int haveCursor(PTRLIST *list)
#else
int
haveCursor(list)
PTRLIST *list;
#endif
{
  return (list->sav_cursor != 0);
}

#ifdef __ANSILIST__
void
addHead(PTRLIST *ptrlist, void *newItem)
#else
void
addHead(ptrlist, newItem)
PTRLIST *ptrlist;
void *newItem;
#endif
{
  start(ptrlist);
  if (!offleft(ptrlist))
    back(ptrlist);
  addItem(ptrlist, newItem);
}

#ifdef __ANSILIST__
void
addTail(PTRLIST *ptrlist, void *newItem)
#else
void
addTail(ptrlist, newItem)
PTRLIST *ptrlist;
void *newItem;
#endif
{
  finish(ptrlist);
  addItem(ptrlist, newItem);
}

#ifdef __ANSILIST__
void
enqueue(PTRLIST *ptrlist, long priority, void *newItem)
#else
void
enqueue(ptrlist, priority, newItem)
PTRLIST *ptrlist;
long priority;
void *newItem;
#endif
{
  for (start(ptrlist);
       !offright(ptrlist) &&
       ((NODE *)ptrlist->cursor)->ln_Pri >= priority;
       forth(ptrlist))
    ;
  if (offright(ptrlist))
    {
      addTail(ptrlist, newItem);
      ((NODE *)ptrlist->cursor)->ln_Pri = priority;
    }
  else
    {
      back(ptrlist);
      addItem(ptrlist, newItem);
      ((NODE *)ptrlist->cursor)->ln_Pri = priority;
    }
}
