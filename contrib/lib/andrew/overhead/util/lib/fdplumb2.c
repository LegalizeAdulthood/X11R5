/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb2.c,v 2.5 1991/09/12 17:25:14 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fdplumb2.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <fdplumbi.h>

dbg_vclose(fd)
int fd;
{
    RegisterCloseFile(fd);
    return(vclose(fd));
}

dbg_vfclose(fp)
FILE *fp;
{
    RegisterCloseFile(fileno(fp));
    return(vfclose(fp));
}

