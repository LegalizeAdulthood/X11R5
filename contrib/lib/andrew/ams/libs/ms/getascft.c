/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getascft_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getascft.c,v 2.4 1991/09/12 15:43:38 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getascft.c,v 2.4 1991/09/12 15:43:38 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getascft.c,v $
*/
#include <ms.h>

MS_GetAssociatedFileTime(FullName, fdate)
char *FullName;
long *fdate;
{
    debug(1, ("MS_GetAssociatedFileTime %s\n", FullName));
    return(GetAssocFileTime(FullName, fdate));
}
