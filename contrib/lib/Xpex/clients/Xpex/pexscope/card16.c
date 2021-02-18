/* $Header: card16.c,v 2.2 91/09/11 15:52:03 sinyaw Exp $ */
#include <stdio.h>
#include <X11/X.h>
#include "PEX.h"
#include "PEXprotost.h"

void
pexpr_array_of_card16( str, count, pArray, bytes_read)
	int count;
	CARD16 *pArray;
	int  *bytes_read;
{
	*bytes_read = count * sizeof(CARD16);

	(void) fprintf( stderr, "%s:", str);

	while( count--) {
		(void) fprintf( stderr, "%u ", *pArray);
		pArray++;
	}
	(void) fprintf( stderr, "\n");
}
