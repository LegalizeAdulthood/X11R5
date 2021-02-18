/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/lwpstubs.c,v 2.4 1991/09/12 15:59:13 bobg Exp $ */
/* $ACIS:lwpstubs.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/lwpstubs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/lwpstubs.c,v 2.4 1991/09/12 15:59:13 bobg Exp $";
#endif /* lint */

/* Stub routines incase we are not linked against the LWP library. */

#ifdef LWP

/* this is a dummy iomgr_select.  It is linked in only if IOMGR is not being used */

IOMGR_Select(num, rfs, wfs, xfs, timeout)
    long num, rfs, wfs, xfs;
    struct timeval *timeout;
{
    return select(num, rfs, wfs, xfs, timeout);
}

LWP_CurrentProcess()
{
}

IOMGR_SoftSig()
{
}

IOMGR_Cancel()
{
}
#endif /* LWP */
