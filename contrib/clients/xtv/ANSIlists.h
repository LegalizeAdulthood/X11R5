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

/*
 * ANSIlists.h
 *
 * ANSI style definitions of the basic list functions.
 */

void newList(LIST *newL);
void Insert(LIST *h, NODE *n, NODE *pred);
void Remove(NODE *n);
void AddTail(LIST *list, NODE *node);
void AddHead(LIST *list, NODE *node);
NODE *RemHead(LIST *list);
NODE *RemTail(LIST *list);
void NewList(LIST *list);
NODE *Next(NODE *node);
NODE *Prev(NODE *node);
NODE *Head(LIST *list);
NODE *Tail(LIST *list);

LIST *listNew(void);

void Enqueue(LIST *list, NODE *node);
