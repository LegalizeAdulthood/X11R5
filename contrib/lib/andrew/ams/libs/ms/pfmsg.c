/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/pfmsg.c,v 2.8 1991/09/12 15:46:40 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/pfmsg.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/pfmsg.c,v 2.8 1991/09/12 15:46:40 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <ms.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <sys/ioctl.h>
#include <afs/errors.h>
#include <afs/prs_fs.h>
#include <afs/venus.h>
#endif /* AFS_ENV */

MS_PrefetchMessage(DirName, id, GetNext) /* prefetch a vice file */
char *DirName, *id;
int GetNext;
{
#ifdef AFS_ENV
    struct MS_Directory *Dir;
    struct ViceIoctl blob;
    int msgnum;
    char SnapshotDum[AMS_SNAPSHOTSIZE+1], FileName[1+MAXPATHLEN];

    if (AMS_ViceIsRunning) {
    blob.out_size = 0;
    blob.in_size = 0;

    if (!id || !*id) {
	/* Just prefetch the folder */
	sprintf(FileName, "%s/%s", DirName, MS_DIRNAME);
    } else {
	if (ReadOrFindMSDir(DirName, &Dir, MD_READ) != 0) {
	    return(mserrcode);
	}
	if (GetSnapshotByID(Dir, id, &msgnum, SnapshotDum)) {
	    CloseMSDir(Dir, MD_READ);
	    return(mserrcode);
	}
	if (GetNext && GetSnapshotByNumber(Dir, ++msgnum, SnapshotDum)) {
	    CloseMSDir(Dir, MD_READ);
	    return(mserrcode);
	}
	CloseMSDir(Dir, MD_READ);
	GetBodyFileName(Dir->UNIXDir, AMS_ID(SnapshotDum), FileName);
    }
    if (pioctl(FileName, VIOCPREFETCH, &blob)) {
	AMS_RETURN_ERRCODE(errno, EIN_PIOCTL, EVIA_PREFETCHMSG);
    }
    }
#endif /* AFS_ENV */
    return(0);
}

