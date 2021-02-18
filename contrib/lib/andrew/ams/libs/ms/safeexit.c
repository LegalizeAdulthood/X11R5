/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/safeexit.c,v 2.7 1991/09/12 15:48:12 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/safeexit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/safeexit.c,v 2.7 1991/09/12 15:48:12 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <ms.h>
#include <mailconf.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <sys/ioctl.h>
#include <afs/errors.h>
#include <afs/prs_fs.h>
#include <afs/venus.h>
#endif /* AFS_ENV */

extern int MS_SnapIsRunning;

safeexit(code)
int code;
{
    freepag();
    exit(code);
}

freepag() {
#ifdef AFS_ENV
    struct ViceIoctl blob;

    if (AMS_ViceIsRunning) {
    if (MS_SnapIsRunning) {
	/* Unset the pag */
	blob.in_size = 0;
	blob.out_size = 0;
	pioctl(ViceFile, VIOCUNPAG, &blob, 1);
    }
    }
#endif /* AFS_ENV */
}
