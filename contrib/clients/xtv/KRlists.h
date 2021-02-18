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
 * KRlists.h
 *
 * K+R style definitions of the basic list functions.
 */

void newList();
void Insert();
void Remove();
void AddTail();
void AddHead();
NODE *RemHead();
NODE *RemTail();
void NewList();
NODE *Next();
NODE *Prev();
NODE *Head();
NODE *Tail();

LIST *listNew();


void Enqueue();
