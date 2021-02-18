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
	memory.c
	Memory routines

*/

#include <stdio.h>
#include <malloc.h>
#include "memory.h"
#include <errno.h>
#include "misc.h"

#define DEBUGGetMem	0

#ifdef dec5000
#define DEC
#endif

/*
 *	GetMem()
 *
 *	Get memory and bomb out if unable to.
 *
 *	RETURN VALUE:	Pointer to allocated space
 */
char *GetMem( HowMuch )
unsigned int HowMuch;
{
	char	*Space;

#       if DEBUGGetMem
	 fprintf( stderr, "GetMem(): requesting %d  Bytes\n",  HowMuch);
#       endif
/* IBM RISC/6000 core dump when HowMuch is 0 */
        if (HowMuch == 0) HowMuch++;

/* Seems that DEC 3100 likes multiples of 8 */
#ifdef DEC
	if ( HowMuch%8 ){
		HowMuch += (8 - (HowMuch%8));
	}
#endif

#	if DEBUGGetMem
	 fprintf( stderr, "GetMem(): %d Bytes actually allocated\n",HowMuch);
#	endif

	if ( (Space= (char *)malloc(HowMuch)) != NULL ) {
		return Space;
	}
	perror ("Calling malloc()\n");
	FatalError( "Unable to allocate memory" );
}
