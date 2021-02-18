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
 * KRptrlists.h
 *
 * Define K+R style headers for the ptrlist functions.
 */

void newPtrList();

int items();

void addItem();
void *getItem();
void deleteItem();

void unshift();
void *shift();

void push();
void *pop();

void start();
void finish();
void forth();
void back();

void *first();
void *last();

int offright();
int offleft();

void saveCursor();
void restoreCursor();

int atSaved();

PTRLIST* ptrNew();

int haveCursor();

void addHead();
void addTail();

void enqueue();
