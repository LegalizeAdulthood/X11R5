/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/netmon.c,v 2.5 1991/09/12 16:07:03 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/netmon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/netmon.c,v 2.5 1991/09/12 16:07:03 bobg Exp $";
#endif /* lint */

/*
	Network monitoring routines for Instrument Console.

 */

#include <stdio.h>
#include <console.h>

CheckNet()
{
    short i;

    mydbg(("entering: CheckNet\n"));
    i = NETRESPONSES;
    if (Numbers[i].IsDisplaying) {
	NewValue(&Numbers[i], 5, NULL, FALSE);
    }
}

InitNet()
{
    mydbg(("entering: InitNet\n"));
    Numbers[NETRESPONSES].RawText = (char *)malloc(20);
    strcpy(Numbers[NETRESPONSES].RawText, "Foobar");
}
