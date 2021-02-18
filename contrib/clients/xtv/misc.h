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
	misc.h
	Declaraions for miscellaneous routines in misc.c

*/

#ifndef MISC__H
#define MISC__H

#include <X11/X.h>	/* For LSBFirst & MSBFirst */
#include <X11/Xmd.h>	/* For type definitions */

#define	INITIAL_MSG	"Welcome to XTV -- X TerminalView --\nA Framework for Sharing X Window Clients\n in Remote Synchronous Collaboration"

#define	MSB_First	(BYTE)0x42
#define	LSB_First	(BYTE)0x6c

/*
#define	FROMLOCAL(x)	( LocalByteOrder() == (x) ? (x) : ( (x)==MSB_First ? LSB_First : MSB_First) )
*/
#define FROMLOCAL(x) (x)

void ErrorMessage();
void FatalError();
BYTE LocalByteOrder();
void LGetArgs();

/*
 * Begin Murray Addition
 */
void dumpMessageBuffer();
void freeMessageBuffer();
void lockMessages();
void unlockMessages();
/*
 * End Murray Addition
 */
void UpdateMessage();

#define NEEDSWAP 1
#if NEEDSWAP
CARD16 SwapInt();
CARD32 SwapLong();
#else
#define SwapInt(Value, SwapMode)	Value
#define SwapLong(Value, SwapMode)	Value
#endif

#endif /* MISC__H */
