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
 * ANSIptrlist.h
 *
 * Define ANSI-C style headers for the ptrlist functions.
 */

void newPtrList(PTRLIST *ptrlist);

int items(PTRLIST *ptrlist);

void addItem(PTRLIST *ptrlist, void *newItem);
void *getItem(PTRLIST *ptrlist);
void deleteItem(PTRLIST *ptrlist);

void unshift(PTRLIST *ptrlist, void *newItem);
void *shift(PTRLIST *ptrlist);

void push(PTRLIST *ptrlist, void *newItem);
void *pop(PTRLIST *ptrlist);

void start(PTRLIST *ptrlist);
void finish(PTRLIST *ptrlist);
void forth(PTRLIST *ptrlist);
void back(PTRLIST *ptrlist);

void *first(PTRLIST *ptrlist);
void *last(PTRLIST *ptrlist);

int offright(PTRLIST *ptrlist);
int offleft(PTRLIST *ptrlist);

void saveCursor(PTRLIST *ptrlist);
void restoreCursor(PTRLIST *ptrlist);

int atSaved(PTRLIST *ptrlist);

int haveCursor(PTRLIST *list);

void addHead(PTRLIST *ptrlist, void *newItem);
void addTail(PTRLIST *ptrlist, void *newItem);

PTRLIST *ptrNew(void);

void enqueue(PTRLIST *ptrlist, long priority, void *newItem);
