/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getnth_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getnth.c,v 2.4 1991/09/12 15:44:11 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getnth.c,v 2.4 1991/09/12 15:44:11 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getnth.c,v $
*/
#include <ms.h>

MS_GetNthSnapshot(DirName, n, SnapshotBuf)
char *DirName, *SnapshotBuf;
int n;
{
    struct MS_Directory *Dir;
    long errsave;

    if (ReadOrFindMSDir(DirName, &Dir, MD_READ)) {
	return(mserrcode);
    }
    if (n == -1) {
	n = Dir->MessageCount - 1;
    }
    errsave = GetSnapshotByNumber(Dir, n, SnapshotBuf);
    CloseMSDir(Dir, MD_READ);
    return(errsave);
}
