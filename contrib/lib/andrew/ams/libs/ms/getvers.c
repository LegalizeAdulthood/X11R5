/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getvers_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getvers.c,v 2.4 1991/09/12 15:44:29 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getvers.c,v 2.4 1991/09/12 15:44:29 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getvers.c,v $
*/
#include <ms.h>

extern char MS_VersionString[];

MS_GetVersion(Buf, lim) 
char *Buf;
int lim;
{
    debug(1, ("MS_GetVersion\n"));
    debug(4, ("Version number is %s\n", MS_VersionString));
    strncpy(Buf, MS_VersionString, lim);
    Buf[lim-1] = '\0';
    return(0);
}
