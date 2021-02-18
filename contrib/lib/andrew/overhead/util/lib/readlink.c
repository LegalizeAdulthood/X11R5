/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: */
/* $Source: */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <errno.h>
#include <andrewos.h>

extern int errno;

/* 
 * osi_readlink is the portability layer equivelent of readlink.  On 
 * systems without symbolic links, it always returns -1 and sets errno 
 * to EINVAL.
 */

#if !OSI_HAS_SYMLINKS

int
osi_readlink(path,buffer,bufsize)
	char *path;
	char *buffer;
	int bufsize;
{
	errno = EINVAL;
	return -1;
}

#endif

