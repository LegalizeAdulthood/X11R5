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
	token.ch
	Delcarations for token.c

*/

#ifndef TOKEN__H
#define TOKEN__H


#include <X11/X.h>
#include "subscriber.h"

typedef struct _TokenNode {
	BYTE			SubID;
	struct _TokenNode	*Next;
} TokenNode;


void AddToTokenList();
Subscriber *GetNextToken();
Subscriber *FindNextToken();
void RemoveFromOneTokenQ();
void RemoveFromAllTokenQs();
void GiveTokenTo();
void SwitchToken();

XtCallbackProc   GetToken();
XtCallbackProc   SnatchToken();
XtCallbackProc   DropToken();
XtTimerCallbackProc FloorTimeOut(); 

#define	HOST_ID	0		/* (The subscriber ID of the host) */

#define MINFLOORTIME  30000             /* Minimum floor holding time befor naging message */ 
#endif /* TOKEN__H */
