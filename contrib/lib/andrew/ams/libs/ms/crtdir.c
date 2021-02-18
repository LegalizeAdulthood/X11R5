/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *crtdir_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/crtdir.c,v 2.4 1991/09/12 15:42:31 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/crtdir.c,v 2.4 1991/09/12 15:42:31 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/crtdir.c,v $
*/
#include <ms.h>

MS_CreateNewMessageDirectory(DirName, Overwrite, obsolete)
char *DirName;
int Overwrite;
char *obsolete;
{
    struct MS_Directory *Dir;

    debug(1, ("MS_CreateNewMessageDirectory %s %d\n", DirName, Overwrite));
    if (*DirName != '/') {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CREATENEWMESSAGEDIRECTORY);
    }
    if (obsolete && strcmp(DirName, obsolete)) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CREATENEWMESSAGEDIRECTORY);
    }
    return(CreateNewMSDirectory(DirName, &Dir, Overwrite));
}
