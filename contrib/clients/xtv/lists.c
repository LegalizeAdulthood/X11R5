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
 * Based on C++ list implementation by Steve Tell and Murray Anderegg.
 * Based on C list implementation in Amiga Kernel. 
 *   However, this one works and the Amiga one did not.
 */

#include <stdio.h>
#include "lists.h"

/* explanation of the list header.
 * lh_Head points to the first node in the list
 * lh_Tail is always zero
 * lh_TailPred points to the last node in the list.
 *
 * The list header (struct List) is actually two list nodes:
 * "The head and tail portions of the header actually overlap.
 * This is best understood if you think of the head and tail as two seperate
 * nodes.  The lh_Head field is the ln_Succ field if the first node
 * in the list, and the lh_Tail field is its ln_Pred.  The lh_Tail is set
 * permanently to zero to indicate that this node is the first on the list--
 * that is, it has no predecesors.  A similar method is used for the tail
 * node.  The lh_Tail field is the lh_Succ of the last node in the list
 * and the lh_TailPred field is its ln_Pred.  In this case, the zero lh_Tail
 * indicates that this node is the last on the list--that is, it has
 * no predecessors."  --From the Rom Kernel Reference Manual:EXEC,
 * Addison Wesley, 1986  (...with some corrections by Steve Tell) 
 *
 * This sounds horribly contorted until one realizes that things are really
 * only wierd in the constructor List(); most everywhere else the
 * special cases actually go away.
 */

#ifdef __ANSILIST__
void
newList(LIST *newL)
#else
void
newList(newL)
LIST *newL;
#endif
{
  newL->lh_Head = (NODE *)&(newL->lh_Tail);
  newL->lh_TailPred = (NODE *)&(newL->lh_Head);
  newL->lh_Tail = 0;
}

#ifdef __ANSILIST__
void
AddHead(LIST *list, NODE *node)
#else
void
AddHead(list, node)
LIST *list;
NODE *node;
#endif
{
  node->ln_Succ = list->lh_Head;
  node->ln_Pred = list->lh_Head->ln_Pred;
  list->lh_Head->ln_Pred = node;
  list->lh_Head = node;
}

#ifdef __ANSILIST__
void
AddTail(LIST *list, NODE *node)
#else
void
AddTail(list, node)
LIST *list;
NODE *node;
#endif
{
  node->ln_Succ = list->lh_TailPred->ln_Succ;
  node->ln_Pred = list->lh_TailPred;
  list->lh_TailPred->ln_Succ = node;
  list->lh_TailPred = node;
}

#ifdef __ANSILIST__
NODE *
RemHead(LIST *list)
#else
NODE *
RemHead(list)
LIST *list;
#endif
{
  NODE *n = list->lh_Head;
  
  n->ln_Succ->ln_Pred = n->ln_Pred;
  n->ln_Pred->ln_Succ = n->ln_Succ;
  
  n->ln_Succ = n->ln_Pred = 0;
  return n;
}

#ifdef __ANSILIST__
NODE *
RemTail(LIST *list)
#else
NODE *
RemTail(list)
LIST *list;
#endif
{
  NODE *n = list->lh_TailPred;
  
  n->ln_Succ->ln_Pred = n->ln_Pred;
  n->ln_Pred->ln_Succ = n->ln_Succ;
  
  n->ln_Succ = n->ln_Pred = 0;
  return n;
}

#ifdef __ANSILIST__
void
Remove(NODE *n)
#else
void
Remove(n)
NODE *n;
#endif
{
  n->ln_Succ->ln_Pred = n->ln_Pred;
  n->ln_Pred->ln_Succ = n->ln_Succ;
  
  n->ln_Succ = n->ln_Pred = 0;
}

#ifdef __ANSILIST__
void
Insert(LIST *list, NODE *n, NODE *pred)
#else
void
Insert(list, n, pred)
LIST *list;
NODE *n;
NODE *pred;
#endif
{
  if(pred == 0 || pred == (NODE *)&(list->lh_Head))
    {
      AddHead(list, n);
    }
  else if(pred == (NODE *)&(list->lh_Tail))
    {
      AddTail(list, n);
    }
  else
    {
      n->ln_Succ = pred->ln_Succ;
      n->ln_Pred = pred->ln_Succ->ln_Pred;
      pred->ln_Succ->ln_Pred = n;
      pred->ln_Succ = n;
    }
}

#ifdef __ANSILIST__
NODE *
Next(NODE *n)
#else
NODE *
Next(n)
NODE *n;
#endif
{
  return n->ln_Succ;
}

#ifdef __ANSILIST__
NODE *
Prev(NODE *n)
#else
NODE *
Prev(n)
NODE *n;
#endif
{
  return n->ln_Pred;
}

#ifdef __ANSILIST__
NODE *
Head(LIST *list)
#else
NODE *
Head(list)
LIST *list;
#endif
{
  return list->lh_Head;
}

#ifdef __ANSILIST__
NODE *
Tail(LIST *list)
#else
NODE *
Tail(list)
LIST *list;
#endif
{
  return list->lh_TailPred;
}

#ifdef __ANSILIST__
LIST *
listNew(void)
#else
LIST *
listNew()
#endif
{
  LIST *newL = (LIST *) malloc(sizeof(LIST));

  newList(newL);
  return newL;
}

#ifdef __ANSILIST__
void
Enqueue(LIST *list, NODE *node)
#else
void
Enqueue(list, node)
LIST *list;
NODE *node;
#endif
{
  NODE *np;
  long pri = node->ln_Pri;

  for (np=Head(list); (np) && (pri >= np->ln_Pri); np = np->ln_Succ)
    ;
  
  if (!np)
    AddTail(list, node);
  else
    Insert(list, node, Prev(np));
}
