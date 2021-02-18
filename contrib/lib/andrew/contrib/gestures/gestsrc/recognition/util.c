/***********************************************************************

util.c - memory allocation, error reporting, and other mundane stuff

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
/*
 * Mundane utility routines
 *	see util.h
 */

/*LINTLIBRARY*/

#include "util.h"
#include <stdio.h>
#include <ctype.h>

/*
 * Function used by allocation macro
 */

char *
myalloc(nitems, itemsize, typename)
char *typename;
{
	char	*malloc();
	register unsigned int bytes = nitems * itemsize;
	register char *p = malloc(bytes);
	if(p == NULL)
	     error("Can't get mem for %d %s's (each %d bytes, %d total bytes)",
		nitems, typename, itemsize, bytes);
	return p;
}

/*
 * Return a copy of a string
 */

char *
scopy(s)
char *s;
{
	register char *p = allocate(strlen(s) + 1, char);
	char *strcpy();
	(void) strcpy(p, s);
	return p;
}

/*
 * Print an error message, a newline, and then exit
 */

/*VARARGS1*/
void
error(a, b, c, d, e, f, g)
char *a;
{
	fprintf(stderr, a, b, c, d, e, f, g);
	fprintf(stderr, "\n");
	exit(1);
}

/*
 * print a message if DebugFlag is non-zero
 */

int	DebugFlag = 1;

void
debug(a, b, c, d, e, f, g)
char *a;
{
	if(DebugFlag)
		fprintf(stderr, a, b, c, d, e, f, g);
}

#define	upper(c)	(islower(c) ? toupper(c) : (c))

int
ucstrcmp(s1, s2)
register char *s1, *s2;
{
	register i;

	for(; *s1 && *s2; s1++, s2++)
		if( (i = (upper(*s1) - upper(*s2))) != 0)
			return i;
	return (upper(*s1) - upper(*s2));
}

#define NSTRINGS 3

char *
tempstring()
{
	static char strings[NSTRINGS][100];
	static int index;
	if(index >= NSTRINGS) index = 0;
	return strings[index++];
}
