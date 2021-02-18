/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fpacheck.c,v 2.4 1991/09/12 17:25:38 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fpacheck.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

fpacheck()
{
    float f;

    f = 1000.0 * 0.5;
    f /= 2;
    f /= 2.0;
    return ((f == 125) ? 0 : 1);
}
