/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *gentname_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gentname.c,v 2.6 1991/09/12 15:43:34 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gentname.c,v 2.6 1991/09/12 15:43:34 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gentname.c,v $
*/
#include <stdio.h>
#include <mail.h>

extern char *getprofile();

MS_GenTempFileName(Buf)
char *Buf; /* Out */
{
    GenTempName(Buf);
    return(0);
}

GenTempName(Buf)
char   *Buf;
{
    static char *TempFileLocation = NULL;
    if (!TempFileLocation) {
	TempFileLocation = getprofile("AMStempfilelocation");
	if (!TempFileLocation) TempFileLocation = "/tmp";
    }
    sprintf(Buf, "%s/%s", TempFileLocation, ams_genid(1));
}
