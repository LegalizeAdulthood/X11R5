/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *unlnfile_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/unlnfile.c,v 2.4 1991/09/12 15:49:29 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/unlnfile.c,v 2.4 1991/09/12 15:49:29 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/unlnfile.c,v $
*/
#include <ms.h>

long    MS_UnlinkFile (FileName)
char   *FileName;
{
    debug(1, ("Entering MS_UnlinkFile %s\n", FileName));
    if (unlink(FileName)) {
	AMS_RETURN_ERRCODE(errno, EIN_UNLINK, EVIA_UNLINKFILE);
    }
    return(0);
}
