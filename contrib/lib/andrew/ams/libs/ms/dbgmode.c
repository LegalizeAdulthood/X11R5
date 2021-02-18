/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dbgmode_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgmode.c,v 2.5 1991/09/12 15:42:38 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgmode.c,v 2.5 1991/09/12 15:42:38 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgmode.c,v $
*/
#include <ms.h>
#include <andyenv.h>

extern int SNAP_debugmask;

MS_DebugMode(level, snap, malloc)
int level, snap, malloc;
{
    debug(1, ("MS_DebugMode %d %d %d\n", level, snap, malloc));
    MSDebugging = level;
    SNAP_debugmask = snap;
#ifdef ANDREW_MALLOC_ENV
    SetMallocCheckLevel(malloc);
#endif /* #ifdef ANDREW_MALLOC_ENV */
    return(0);
}
