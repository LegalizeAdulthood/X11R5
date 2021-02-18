/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb6.c,v 2.6 1991/09/12 17:25:30 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb6.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */


#include <andrewos.h>    /* types */
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#include <fdplumbi.h>

DIR *dbg_opendir(name)
char *name;
{
    DIR *d;

    d = opendir(name);
    if (d) {
	RegisterOpenFile(d->dd_fd, name, FDLEAK_OPENCODE_OPENDIR);
    }
    return(d);
}


void dbg_closedir(d)
DIR *d;
{
    RegisterCloseFile(d->dd_fd);
    closedir(d);
}


