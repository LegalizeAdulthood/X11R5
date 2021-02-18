/***********************************************************************

util.h - memory allocation, error reporting, and other mundane stuff

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

/*
 * General utility functionss
 *
 * Mostly for dealing with mundane issues such as:
 *	Memory allocation
 *	Error handling
 */

/*
 * General allocation macro
 *
 * Example:
 *	struct list *s; s = allocate(4, struct list)
 * returns space for an array of 4 list structures.
 * Allocate will die if there is no more memory
 */

#define	allocate(n, type)	\
	((type *) myalloc(n, sizeof(type), "type"))

/*
 * Functions
 */

#define	STREQ(a,b)	( ! strcmp(a,b) )

char	*myalloc();	/* Do not call this function directly */
char	*scopy();	/* allocates memory for a string */
void	debug();	/* printf on stderr -
			   setting DebugFlag = 0 turns off debugging */
void	error();	/* printf on stderr, then dies */
int	ucstrcmp();	/* strcmp, upper case = lower case */
char	*tempstring();	/* returns a pointer to space that will reused soon */

/*
   this is the wrong place for all of this, but got chosen since
   every file includes this one
 */

#ifdef unix
#	define GRAPHICS		/* only GDEV on unix machines */
#endif

#ifndef unix

/* various BSD to lattice C name changes */

#define	bcopy	movmem
#define index	strchr
#define	rindex	strrchr

#endif
