/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dfltdir_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dfltdir.c,v 2.4 1991/09/12 15:42:44 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dfltdir.c,v 2.4 1991/09/12 15:42:44 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dfltdir.c,v $
*/
#include <ms.h>

FindDefaultDir(Root, Name)
char *Root, *Name;
{
    int NumGood, NumBad;
    struct MS_Directory *Dir;

    sprintf(Name, "%s/%s", Root, AMS_DEFAULTMAILDIR);
    mserrcode = ReadOrFindMSDir(Name, &Dir, MD_OK);
    if (mserrcode == 0) {
	return(0);
    }
    if (AMS_ERRNO == EMSBADDIRFORMAT) {
	NonfatalBizarreError("Your mail directory was corrupted and is being automatically reconstructed.  Please wait...");
	return(MS_ReconstructDirectory(Name, &NumGood, &NumBad, TRUE));
    }
    if (AMS_ERRNO != ENOENT) {
	return(mserrcode);
    }
    sprintf(Name, "%s/misc", Root);
    mserrcode = ReadOrFindMSDir(Name, &Dir, MD_OK);
    if (mserrcode == 0) {
	return(0);
    }
    if (AMS_ERRNO == EMSBADDIRFORMAT) {
	NonfatalBizarreError("Your misc directory was corrupted and is being automatically reconstructed.  Please wait...");
	return(MS_ReconstructDirectory(Name, &NumGood, &NumBad, TRUE));
    }
    if (AMS_ERRNO != ENOENT) {
	return(mserrcode);
    }
    /* Could try other things here eventually */
    return(mserrcode);
}
