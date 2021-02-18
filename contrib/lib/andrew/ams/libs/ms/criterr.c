/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/criterr.c,v 2.5 1991/09/12 15:42:28 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/criterr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/criterr.c,v 2.5 1991/09/12 15:42:28 bobg Exp $ ";
#endif /* lint */

#include <stdio.h>
#include <errprntf.h>

/* This is in a separate file to make it easy for a no-snap client to override it if it so desires. */

CriticalBizarreError(text)
char *text;
{
    BizarreError(text, ERR_CRITICAL);
}
