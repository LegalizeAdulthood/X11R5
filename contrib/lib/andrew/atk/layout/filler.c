/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/filler.c,v 1.2 1991/09/12 16:23:40 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/filler.c,v $ */

#ifndef lint
char *filler_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/filler.c,v 1.2 1991/09/12 16:23:40 bobg Exp $";
#endif /* lint */

#include <class.h>

#include <filler.eh>

/* return corresponding view name */

char *				    /* returns "fillerview" */
filler__ViewName(self)
struct filler *self;
{
    return "fillerview";
}
