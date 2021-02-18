/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) mem.c 50.3 90/12/12 Crucible";

/*
 * Safe memory allocation/free routines - front-ends the C library functions
 *
 */

#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>

void *
MemAlloc(sz)
unsigned int sz;
{
#ifdef __STDC__
	void *p;
#else
	char *p;
#endif

	if ((p = malloc(sz)) == NULL)
#ifdef OW_I18N
		ErrorGeneral(gettext("Memory allocation failure."));
#else
		ErrorGeneral("Memory allocation failure.");
#endif
	memset((char *)p, 0, (int)sz);
	return p;
}

void *
MemCalloc(num,sz)
unsigned int num;
unsigned int sz;
{
#ifdef __STDC__
	void *p;
#else
	char *p;
	char *calloc();
#endif

	if ((p = calloc(num,sz)) == NULL)
#ifdef OW_I18N
		ErrorGeneral(gettext("Memory array allocation failure."));
#else
		ErrorGeneral("Memory array allocation failure.");
#endif
	memset((char *)p, 0, (int)sz*(int)num);
	return p;
}

void
MemFree(p)
void *p;
{
	if (p != NULL)
		free(p);
}


