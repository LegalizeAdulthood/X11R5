/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getbody_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getbody.c,v 2.6 1991/09/12 15:43:45 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getbody.c,v 2.6 1991/09/12 15:43:45 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getbody.c,v $
*/
#include <andrewos.h>
#include <ms.h>

MS_GetPartialBody(DirName, id, Buf, BufLim, offset, remaining, ct)
char *DirName, *id, *Buf;
int BufLim, offset, *remaining, *ct;
{
    struct MS_Directory *Dir;
    char FileName[MAXPATHLEN+1];

    debug(1, ("MS_GetPartialBody %s %s\n", DirName, id));
    *ct = *remaining = 0;
    if (ReadOrFindMSDir(DirName, &Dir, MD_OK) != 0) {
	return(mserrcode);
    }
    GetBodyFileName(Dir->UNIXDir, id, FileName);
    if (!offset) {
	ConsiderLoggingRead(FileName);
    }
    return(MS_GetPartialFile(FileName, Buf, BufLim, offset, remaining, ct));
}
