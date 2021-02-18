/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *freedir_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/freedir.c,v 2.4 1991/09/12 15:43:27 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/freedir.c,v 2.4 1991/09/12 15:43:27 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/freedir.c,v $
*/
#include <ms.h>

/* This routine frees a directory and the things it points to.  If the
	directory is in the cache, it should be removed from the cache
	BEFORE this routine is called, or you'll regret it. */

/* This routine is not currently used, because nobody ever frees a directory
any more -- they are in a permanent cache, and freeing them will cause a
core dump.  However, this documents what they allocate, and might be useful
in future versions. */

FreeDirectory(Dir)
struct MS_Directory *Dir;
{
    debug(1, ("FreeDirectory\n"));
    if (Dir) {
	int i;

	if (Dir->UNIXDir) free (Dir->UNIXDir);
	if (Dir->AttrCount && Dir->AttrNames) {
	    for (i=0; i<Dir->AttrCount; ++i) {
		free(Dir->AttrNames[i]);
	    }
	    free(Dir->AttrNames);
	}
	free(Dir);
    }
}
